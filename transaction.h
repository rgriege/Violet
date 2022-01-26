#ifndef VIOLET_TRANSACTION_H
#define VIOLET_TRANSACTION_H

#define PAYLOAD_LIMIT_PRIMITIVE 8  // bytes

typedef enum command_kind {
	COMMAND_KIND_MODIFY_PRIMITIVE,  // a single field
	COMMAND_KIND_MODIFY_BUFFER,     // a single statically allocated buffer
	COMMAND_KIND_REPLACE,           // an entire struct, whose size is compile time constant
} command_kind_e;

typedef struct payload_primitive {
	union {
		b32 b32;
		u32 u32;
		s32 s32;
		r32 r32;
		unsigned char bytes[PAYLOAD_LIMIT_PRIMITIVE];
	} bytes;
	u32 size;
} payload_primitive_t;

typedef struct payload_dynamic {
	array(unsigned char) bytes;
} payload_dynamic_t;

typedef union payload {
	payload_primitive_t primitive;
	payload_dynamic_t   dynamic;
} payload_t;

payload_dynamic_t payload_dynamic_create(allocator_t *alc);
void payload_dynamic_destroy(payload_dynamic_t *payload);

typedef struct command {
	command_kind_e kind;
	store_kind_e store_kind;
	u32 offset;
	u32 transaction_id;
	payload_t payload;
} command_t;

typedef struct revertable_command {
	command_kind_e kind;
	store_kind_e store_kind;
	u32 offset;
	u32 transaction_id;
	/* should only be used for comparing multi-frame transactions,
	   to know if back-to-back interactions target the same mutation */
	void *dst;
	array(unsigned char) bytes_before;
	array(unsigned char) bytes_after;
} revertable_command_t;

typedef struct transaction_system {
	u32 current_id;
	b32 multi_frame;
	b32 pending;
	megastore_t mega;
	array(command_t) command_queue;
	array(revertable_command_t) undo_stack;
	array(revertable_command_t) redo_stack;
	array(store_kind_e) store_kind_stack;
} transaction_system_t;

transaction_system_t transaction_system_create(array(store_t *) stores, allocator_t *alc);
void transaction_system_destroy(transaction_system_t *sys);
void transaction_system_set_active(transaction_system_t *sys);
void transaction_system_on_update();
void transaction_begin();
void transaction_begin_multi_frame();
b32  transaction_commit();
b32  transaction_undo();
b32  transaction_redo();
void transaction_store_kind_push(store_kind_e kind);
void transaction_store_kind_pop();
void transaction_enqueue_mutation_primitive(payload_primitive_t *payload, u32 offset);
void transaction_enqueue_mutation_buffer(payload_dynamic_t *payload, u32 offset);

// TODO(luke): store_from_kind probably shouldn't be exposed as an API function
store_t *store_from_kind(store_kind_e kind);
void    *store_instance_from_kind(store_kind_e kind);

#define payload_primitive(value, type, payload) \
	assert(sizeof(type) <= PAYLOAD_LIMIT_PRIMITIVE); \
	payload_primitive_t (payload) = { \
		.bytes.type = value, \
		.size = sizeof(type), \
	};

#define payload_dynamic_from_buffer(buf, alc, payload) \
	payload_dynamic_t (payload) = payload_dynamic_create(alc); \
	array_appendn(payload.bytes, (unsigned char *)buf, sizeof(buf));

#endif // VIOLET_TRANSACTION_H

/* Implementation */

#ifdef TRANSACTION_IMPLEMENTATION

transaction_system_t *g_active_transaction_system = NULL;

payload_dynamic_t payload_dynamic_create(allocator_t *alc)
{
	return (payload_dynamic_t) {
		.bytes = array_create_ex(alc),
	};
}

void payload_dynamic_destroy(payload_dynamic_t *payload)
{
	array_destroy(payload->bytes);
}

static
void revertable_command__init(revertable_command_t *cmd, const command_t *command,
                              allocator_t *alc)
{
	*cmd = (revertable_command_t){
		.kind = command->kind,
		.offset = command->offset,
		.transaction_id = command->transaction_id,
	};
	cmd->bytes_before = array_create_ex(alc);
	cmd->bytes_after = array_create_ex(alc);
}

static
void revertable_command__destroy(revertable_command_t *cmd)
{
	array_destroy(cmd->bytes_before);
	array_destroy(cmd->bytes_after);
}

static
void revertable_command__copy_bytes(revertable_command_t *cmd, void *dst, const void *src, u32 n)
{
	cmd->dst = dst;
	array_appendn(cmd->bytes_before, dst, n);
	memcpy(dst, src, n);
	array_appendn(cmd->bytes_after, dst, n);
}

store_t *store_from_kind(store_kind_e kind)
{
	array_foreach(g_active_transaction_system->mega.d, store_t *, store_ptr) {
		if (store_get_kind(*store_ptr) == kind)
			return *store_ptr;
	}
	assert(false);
	return NULL;
}

void *store_instance_from_kind(store_kind_e kind)
{
	store_t *store = store_from_kind(kind);
	return store->instance;
}

// TODO(luke): this function needs to be responsible for undoing any changes if they prove invalid
//             as well as cleaning up any dynamic memory allocacted with the payload generation
static
b32 transaction__execute_command(const store_t *store, u32 command_idx, b32 replace_prev)
{
	revertable_command_t cmd;
	transaction_system_t *sys = g_active_transaction_system;
	command_t *command = &sys->command_queue[command_idx];

	if (!sys->multi_frame)
		assert(!replace_prev);

	switch (command->kind) {
	case COMMAND_KIND_MODIFY_PRIMITIVE:
	{
		const payload_primitive_t *payload = &command->payload.primitive;
		store_offset_bytes(store, command, dst);

		if (replace_prev) {
			// CLEANUP: de-dup with other COMMAND_KINDs
			memcpy(dst, &payload->bytes, payload->size);
			/* get the previously issued command that aligns to the current idx
			   in the current queue */
			revertable_command_t *prev_cmd = &sys->undo_stack[  array_sz(sys->undo_stack)
			                                                - array_sz(sys->command_queue)
			                                                + command_idx];
			array_clear(prev_cmd->bytes_after);
			array_appendn(prev_cmd->bytes_after, dst, payload->size);
		} else {
			allocator_t *alc = array__allocator(sys->undo_stack);
			revertable_command__init(&cmd, command, alc);
			revertable_command__copy_bytes(&cmd, dst, &payload->bytes, payload->size);
		}
	}
	break;
	case COMMAND_KIND_MODIFY_BUFFER:
	{
		payload_dynamic_t *payload = &command->payload.dynamic;
		store_offset_bytes(store, command, dst);
		const u32 size = array_sz(payload->bytes);

		if (replace_prev) {
			// CLEANUP: de-dup with other COMMAND_KINDs
			memcpy(dst, payload->bytes, size);
			/* get the previously issued command that aligns to the current idx
			   in the current queue */
			revertable_command_t *prev_cmd = &sys->undo_stack[  array_sz(sys->undo_stack)
			                                                - array_sz(sys->command_queue)
			                                                + command_idx];
			array_clear(prev_cmd->bytes_after);
			array_appendn(prev_cmd->bytes_after, dst, size);
		} else {
			allocator_t *alc = array__allocator(sys->undo_stack);
			revertable_command__init(&cmd, command, alc);
			revertable_command__copy_bytes(&cmd, dst, payload->bytes, size);
		}

		payload_dynamic_destroy(payload);
	}
	break;
	case COMMAND_KIND_REPLACE:
		NOOP;
	break;
	default:
		NOOP;
	break;
	}

	if (!replace_prev)
		array_append(sys->undo_stack, cmd);

	return true;

// err:
// 	return false;

}

transaction_system_t transaction_system_create(array(store_t *) stores, allocator_t *alc)
{
	return (transaction_system_t) {
		.mega = megastore_create(stores, alc),
		.command_queue    = array_create_ex(alc),
		.undo_stack       = array_create_ex(alc),
		.redo_stack       = array_create_ex(alc),
		.store_kind_stack = array_create_ex(alc),
	};
}

void transaction_system_destroy(transaction_system_t *sys)
{
	megastore_destroy(sys->mega);
	array_destroy(sys->command_queue);
	array_destroy(sys->undo_stack);
	array_destroy(sys->redo_stack);
}

void transaction_system_set_active(transaction_system_t *sys)
{
	g_active_transaction_system = sys;
}

/* called once per frame */
void transaction_system_on_update()
{
	/* helps avoid the footgun of pushing store_kind_e's to the stack without popping them */
	assert(array_sz(g_active_transaction_system->store_kind_stack) == 0);
}

static
void transaction_begin__validate(transaction_system_t *sys)
{
	assert(array_sz(sys->command_queue) == 0);
}

void transaction_begin()
{
	transaction_system_t *sys = g_active_transaction_system;
	transaction_begin__validate(sys);
	sys->multi_frame = false;
	sys->pending = true;
}

/* Addresses the use case when some data mutation might be repeated with a
   different payload. If the desired undo behavior is such that it reverts
   to the state before any of the repeated actions happened, then instead
   of creating a new undo point with every transaction, the previous
   transaction's undo point is modified. */
void transaction_begin_multi_frame()
{
	transaction_system_t *sys = g_active_transaction_system;
	transaction_begin__validate(sys);
	sys->multi_frame = true;
	sys->pending = true;
}

static
u32 transaction_prev_n_commands()
{
	transaction_system_t *sys = g_active_transaction_system;

	if (array_sz(sys->undo_stack) == 0)
		return 0;

	u32 result = 1;
	u32 prev_frame_trans_id = array_last(sys->undo_stack).transaction_id;
	for (s32 i = array_sz(sys->undo_stack)-2; i >= 0 ;--i) {
		if (prev_frame_trans_id != sys->undo_stack[i].transaction_id)
			break;
		++result;
	}
	return result;
}

static
b32 transaction__revert_command(megastore_t *mega, revertable_command_t *cmd)
{
	store_t *store = store_from_kind(cmd->store_kind);

	switch (cmd->kind) {
	case COMMAND_KIND_MODIFY_PRIMITIVE:
	case COMMAND_KIND_MODIFY_BUFFER:
		;
		store_offset_bytes(store, cmd, dst);
		assert(cmd->dst == dst);
		memcpy(dst, cmd->bytes_before, array_sz(cmd->bytes_before));
	break;
	case COMMAND_KIND_REPLACE:
		NOOP;
	break;
	default:
		NOOP;
	break;
	}
	return true;
}

static
b32 transaction__should_replace_prev(megastore_t *mega)
{
	transaction_system_t *sys = g_active_transaction_system;

	if (!sys->multi_frame || array_sz(sys->undo_stack) == 0)
		return false;

	u32 prev_trans_n_cmds = transaction_prev_n_commands();
	u32 curr_trans_n_cmds = array_sz(sys->command_queue);

	if (prev_trans_n_cmds != curr_trans_n_cmds)
		return false;

	for (u32 i = 0; i < curr_trans_n_cmds; ++i) {
		const command_t *command = &sys->command_queue[i];
		store_t *store = store_from_kind(command->kind);
		store_offset_bytes(store, command, dst);
		if (sys->undo_stack[array_sz(sys->undo_stack) - prev_trans_n_cmds + i].dst != dst)
			return false;
	}

	return true;
}

static
b32 transaction__unwind_history(u32 unwind_count)
{
	transaction_system_t *sys = g_active_transaction_system;

	for (u32 i = 0, start_idx = array_sz(sys->undo_stack)-1; i < unwind_count; ++i)
		if (transaction__revert_command(&sys->mega, &sys->undo_stack[start_idx-i]))
			array_pop(sys->undo_stack);
		else
			return false;
	return true;
}

static
void transaction__clear_redo_history()
{
	transaction_system_t *sys = g_active_transaction_system;

	array_foreach(sys->redo_stack, revertable_command_t, cmd)
		revertable_command__destroy(cmd);
	array_clear(sys->redo_stack);
}

b32 transaction_commit()
{
	u32 success_count;
	transaction_system_t *sys = g_active_transaction_system;
	const b32 replace_prev = transaction__should_replace_prev(&sys->mega);

	for (u32 i = 0, n = array_sz(sys->command_queue); i < n; ++i) {
		command_t *command = &sys->command_queue[i];
		const store_t *store = store_from_kind(command->store_kind);
		if (transaction__execute_command(store, i, replace_prev)) {
			success_count++;
		} else {
			if (!transaction__unwind_history(success_count))
				/* something went very haywire - expect undefined behavior */
				assert(false);
			return false;
		}
	}

	array_clear(sys->command_queue);
	transaction__clear_redo_history();

	if (!replace_prev)
		sys->current_id++;

	sys->pending = false;
	return true;
}

// CLEANUP: de-dup with store_redo
b32 transaction_undo()
{
	transaction_system_t *sys = g_active_transaction_system;

	if (array_sz(sys->undo_stack) == 0)
		return false;

	do {
		revertable_command_t *cmd = &array_last(sys->undo_stack);
		if (!transaction__revert_command(&sys->mega, cmd))
			/* something went very haywire - expect undefined behavior */
			assert(false);

		array(unsigned char) swap = cmd->bytes_before;
		cmd->bytes_before = cmd->bytes_after;
		cmd->bytes_after = swap;

		array_append(sys->redo_stack, array_last(sys->undo_stack));
		array_pop(sys->undo_stack);
	} while (   array_sz(sys->undo_stack) > 0
	         && array_last(sys->undo_stack).transaction_id
	         == array_last(sys->redo_stack).transaction_id);

	--sys->current_id;

	return true;
}

// CLEANUP: de-dup with store_undo
b32 transaction_redo()
{
	transaction_system_t *sys = g_active_transaction_system;

	if (array_sz(sys->redo_stack) == 0)
		return false;

	do {
		revertable_command_t *cmd = &array_last(sys->redo_stack);
		if (!transaction__revert_command(&sys->mega, cmd))
			/* something went very haywire - expect undefined behavior */
			assert(false);

		array(unsigned char) swap = cmd->bytes_before;
		cmd->bytes_before = cmd->bytes_after;
		cmd->bytes_after = swap;

		array_append(sys->undo_stack, array_last(sys->redo_stack));
		array_pop(sys->redo_stack);
	} while (   array_sz(sys->redo_stack) > 0
	         && array_last(sys->undo_stack).transaction_id
	         == array_last(sys->redo_stack).transaction_id);

	++sys->current_id;

	return true;
}


void transaction_store_kind_push(store_kind_e kind)
{
	array_append(g_active_transaction_system->store_kind_stack, kind);
}

void transaction_store_kind_pop()
{
	assert(array_sz(g_active_transaction_system->store_kind_stack) > 0);
	array_pop(g_active_transaction_system->store_kind_stack);
}

static
void transaction__enqueue_validate(transaction_system_t *sys)
{
	assert(sys->pending);
}

// CLEANUP: de-dup with transaction_enqueue_mutation_buffer()
void transaction_enqueue_mutation_primitive(payload_primitive_t *payload, u32 offset)
{
	transaction_system_t *sys = g_active_transaction_system;
	transaction__enqueue_validate(sys);

	const command_t command = {
		.kind = COMMAND_KIND_MODIFY_PRIMITIVE,
		.store_kind = array_last(sys->store_kind_stack),
		.payload.primitive = *payload,
		.offset = offset,
		.transaction_id = sys->current_id,
	};
	array_append(sys->command_queue, command);
}

void transaction_enqueue_mutation_buffer(payload_dynamic_t *payload, u32 offset)
{
	transaction_system_t *sys = g_active_transaction_system;
	transaction__enqueue_validate(sys);

	const command_t command = {
		.kind = COMMAND_KIND_MODIFY_BUFFER,
		.store_kind = array_last(sys->store_kind_stack),
		.payload.dynamic = *payload,    // NOTE(luke): copies ptr from dynamic allocation
		.offset = offset,
		.transaction_id = sys->current_id,
	};
	array_append(sys->command_queue, command);
}

#undef TRANSACTION_IMPLEMENTATION
#endif // TRANSACTION_IMPLEMENTATION

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
	payload_t payload;
} command_t;

typedef struct revertable_command {
	command_kind_e kind;
	store_kind_e store_kind;
	u32 offset;
	/* should only be used for comparing multi-frame transactions,
	   to know if back-to-back interactions target the same mutation */
	void *dst;
	array(unsigned char) bytes_before;
	array(unsigned char) bytes_after;
} revertable_command_t;

typedef struct revertable_command_bundle {
	array(revertable_command_t) d;
	u32 transaction_id;
} revertable_command_bundle_t;

typedef enum trigger_kind {
	TRIGGEr_KIND_BASIC,
} trigger_kind_e;

/* Triggers mutate state that is not strictly part of the transaction system;
   that is, the trigger handle (function) is expected to alter state that has no
   dependencies except for those in subsequent triggers, within the same transaction.
   Use triggers when there is no direct correlation between GUI interaction
   (widgets, editor canvas, etc) and the state that requires updating. */
typedef struct trigger {
	trigger_kind_e kind;
	// TODO(undo): research if there's a way to keep the metadata from a trigger
	//             instead of strictly relying on pointers, so that if the transaction
	//             history is saved to file, the "triggers" part of that is meaningful
	union {
		/* function signatures here */
		void (* basic)();
		// TODO(undo): some functions could have stack allocated args
		// TODO(undo): some functions could have heap alloacted args
	} handle;
} trigger_t;

typedef struct trigger_bundle {
	array(trigger_t) d;
	u32 transaction_id;
} trigger_bundle_t;

typedef struct transaction_system {
	u32 current_id;
	b32 multi_frame;
	b32 pending;
	allocator_t *alc;
	array(store_t *) stores;
	array(command_t) command_queue;
	array(trigger_t) trigger_queue;
	array(revertable_command_bundle_t) undo_stack;
	array(revertable_command_bundle_t) redo_stack;
	array(trigger_bundle_t) undo_triggers;
	array(trigger_bundle_t) redo_triggers;
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
// TODO(undo): delete from API once call is removed from testfit/gui.c redo()
void transaction_clear_redo_history();

// TODO(luke): store_from_kind probably shouldn't be exposed as an API function
store_t *store_from_kind(store_kind_e kind);
void    *store_instance_from_kind(store_kind_e kind);
void    *store_data_from_kind(store_kind_e kind);

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
		.store_kind = command->store_kind,
		.offset = command->offset,
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

static
void trigger__destroy(trigger_t *trigger)
{
	// TODO(undo): implementation
	NOOP;
}

static
revertable_command_bundle_t revertable_command_bundle__create(u32 transaction_id, allocator_t *alc)
{
	return (revertable_command_bundle_t) {
		.transaction_id = transaction_id,
		.d = array_create_ex(alc),
	};
}

static
void revertable_command_bundle__destroy(revertable_command_bundle_t *commands)
{
	array_foreach(commands->d, revertable_command_t, cmd) {
		revertable_command__destroy(cmd);
	}
	array_destroy(commands->d);
}

static
trigger_bundle_t trigger_bundle__create(u32 transaction_id, allocator_t *alc)
{
	return (trigger_bundle_t) {
		.transaction_id = transaction_id,
		.d = array_create_ex(alc),
	};
}

static
void trigger_bundle__destroy(trigger_bundle_t *triggers)
{
	array_foreach(triggers->d, trigger_t, trigger) {
		trigger__destroy(trigger);
	}
	array_destroy(triggers->d);
}

store_t *store_from_kind(store_kind_e kind)
{
	array_foreach(g_active_transaction_system->stores, store_t *, store_ptr) {
		if (store_get_kind(*store_ptr) == kind)
			return *store_ptr;
	}
	assert(false);
	return NULL;
}

void *store_instance_from_kind(store_kind_e kind)
{
	const store_t *store = store_from_kind(kind);
	return store->instance;
}

void *store_data_from_kind(store_kind_e kind)
{
	const store_t *store = store_from_kind(kind);
	return store_get_data(store);
}

// TODO(luke): this function needs to be responsible for undoing any changes if they prove invalid
//             as well as cleaning up any dynamic memory allocacted with the payload generation
static
b32 transaction__execute_command(const store_t *store, u32 command_idx, b32 replace_prev)
{
	transaction_system_t *sys = g_active_transaction_system;

	if (!sys->multi_frame)
		assert(!replace_prev);

	revertable_command_t cmd;
	command_t *command = &sys->command_queue[command_idx];
	revertable_command_bundle_t *commands = &array_last(sys->undo_stack);

	switch (command->kind) {
	case COMMAND_KIND_MODIFY_PRIMITIVE:
	{
		const payload_primitive_t *payload = &command->payload.primitive;
		store_offset_bytes(store, command, dst);

		if (replace_prev) {
			// CLEANUP: de-dup with other COMMAND_KINDs
			memcpy(dst, &payload->bytes, payload->size);
			revertable_command_t *prev_cmd = &commands->d[command_idx];
			array_clear(prev_cmd->bytes_after);
			array_appendn(prev_cmd->bytes_after, dst, payload->size);
		} else {
			revertable_command__init(&cmd, command, sys->alc);
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
			revertable_command_t *prev_cmd = &commands->d[command_idx];
			array_clear(prev_cmd->bytes_after);
			array_appendn(prev_cmd->bytes_after, dst, size);
		} else {
			revertable_command__init(&cmd, command, sys->alc);
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
		array_append(commands->d, cmd);

	return true;

// err:
// 	return false;

}

static
void transaction__execute_trigger_op(trigger_t *trigger)
{
	switch((trigger_kind_e)trigger->kind) {
	case TRIGGEr_KIND_BASIC:
		trigger->handle.basic();
	break;
	default:
		NOOP;
	break;
	}
}

static
void transaction__execute_trigger(u32 trigger_idx, b32 replace_prev)
{
	transaction_system_t *sys = g_active_transaction_system;

	if (!sys->multi_frame)
		assert(!replace_prev);

	trigger_t *trigger = &sys->trigger_queue[trigger_idx];
	transaction__execute_trigger_op(trigger);

	trigger_bundle_t *triggers = &array_last(sys->undo_triggers);

	if (!replace_prev)
		array_append(triggers->d, *trigger);
	else
		triggers->d[trigger_idx] = *trigger;
}

transaction_system_t transaction_system_create(array(store_t *) stores, allocator_t *alc)
{
	/* having two or more store_t with the same store_kind causes undefined behavior */
	for (u32 i = 0, n = array_sz(stores); i < n; ++i) {
		const store_kind_e kind_i = store_get_kind(stores[i]);
		for (u32 j = 0; j < n; ++j) {
			if (i == j)
				continue;
			const store_kind_e kind_j = store_get_kind(stores[j]);
			assert(kind_i != kind_j);
		}
	}

	assert(array__allocator(stores) == alc);

	return (transaction_system_t) {
		.alc = alc,
		.stores = stores,
		.command_queue    = array_create_ex(alc),
		.trigger_queue    = array_create_ex(alc),
		.store_kind_stack = array_create_ex(alc),
		.undo_stack       = array_create_ex(alc),
		.redo_stack       = array_create_ex(alc),
		.undo_triggers    = array_create_ex(alc),
		.redo_triggers    = array_create_ex(alc),
	};
}

void transaction_system_destroy(transaction_system_t *sys)
{
	array_destroy(sys->command_queue);
	array_destroy(sys->trigger_queue);
	array_destroy(sys->store_kind_stack);

	array_foreach(sys->stores, store_t *, store_ptr) {
		store_destroy(*store_ptr, sys->alc);
	}
	array_destroy(sys->stores);

	array_foreach(sys->undo_stack, revertable_command_bundle_t, commands) {
		revertable_command_bundle__destroy(commands);
	}
	array_destroy(sys->undo_stack);

	array_foreach(sys->redo_stack, revertable_command_bundle_t, commands) {
		revertable_command_bundle__destroy(commands);
	}
	array_destroy(sys->redo_stack);

	array_foreach(sys->undo_triggers, trigger_bundle_t, triggers) {
		trigger_bundle__destroy(triggers);
	}
	array_destroy(sys->undo_triggers);

	array_foreach(sys->redo_triggers, trigger_bundle_t, triggers) {
		trigger_bundle__destroy(triggers);
	}
	array_destroy(sys->redo_triggers);
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
void transaction__begin(transaction_system_t *sys)
{
	assert(array_sz(sys->command_queue) == 0);
	sys->pending = true;

	array_append(sys->undo_stack,
	             revertable_command_bundle__create(sys->current_id, sys->alc));;
	array_append(sys->undo_triggers,
	             trigger_bundle__create(sys->current_id, sys->alc));
}

void transaction_begin()
{
	transaction_system_t *sys = g_active_transaction_system;
	transaction__begin(sys);
	sys->multi_frame = false;
}

/* Addresses the use case when some data mutation might be repeated with a
   different payload. If the desired undo behavior is such that it reverts
   to the state before any of the repeated actions happened, then instead
   of creating a new undo point with every transaction, the previous
   transaction's undo point is modified. */
void transaction_begin_multi_frame()
{
	transaction_system_t *sys = g_active_transaction_system;
	transaction__begin(sys);
	sys->multi_frame = true;
}

static
b32 transaction__revert_command(revertable_command_t *cmd)
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
b32 transaction__should_replace_prev()
{
	const transaction_system_t *sys = g_active_transaction_system;

	if (!sys->multi_frame || array_sz(sys->undo_stack) <= 1)
		return false;

	const revertable_command_bundle_t *commands = &array_last(sys->undo_stack)-1;

	const u32 prev_trans_n_cmds = array_sz(commands->d);
	const u32 curr_trans_n_cmds = array_sz(sys->command_queue);

	if (prev_trans_n_cmds != curr_trans_n_cmds)
		return false;

	for (u32 i = 0; i < curr_trans_n_cmds; ++i) {
		const command_t *command = &sys->command_queue[i];
		const store_t *store = store_from_kind(command->store_kind);
		store_offset_bytes(store, command, dst);
		if (commands->d[i].dst != dst)
			return false;
	}

	// TODO(undo): do we care about triggers?

	return true;
}

static
b32 transaction__unwind_bundle(revertable_command_bundle_t *commands)
{
	for (s32 i = array_sz(commands->d)-1; i >= 0; --i)
		if (!transaction__revert_command(&commands->d[i]))
			return false;

	revertable_command_bundle__destroy(commands);
	return true;
}

// TODO(undo): make this static once call is removed from testfit/gui.c redo()
void transaction_clear_redo_history()
{
	transaction_system_t *sys = g_active_transaction_system;

	array_foreach(sys->redo_stack, revertable_command_bundle_t, commands)
		revertable_command_bundle__destroy(commands);
	array_clear(sys->redo_stack);

	array_foreach(sys->redo_triggers, trigger_bundle_t, triggers)
		trigger_bundle__destroy(triggers);
	array_clear(sys->redo_triggers);
}

static
void transaction_system__cleanup_commit(transaction_system_t *sys)
{
	array_clear(sys->command_queue);
	array_clear(sys->trigger_queue);
	sys->pending = false;
}

b32 transaction_commit()
{
	transaction_system_t *sys = g_active_transaction_system;

	if (array_sz(sys->command_queue) == 0) {
		/* we almost certainly want to queue at least 1 command during a transaction */
		assert(false);
		return false;
	}

	const b32 replace_prev = transaction__should_replace_prev();

	if (replace_prev) {
		/* free and remove the bundles that were added during transaction__begin() */
		revertable_command_bundle__destroy(&array_last(sys->undo_stack));
		trigger_bundle__destroy(&array_last(sys->undo_triggers));
		array_pop(sys->undo_stack);
		array_pop(sys->undo_triggers);
	}

	array_iterate(sys->command_queue, i, n) {
		const store_t *store = store_from_kind(sys->command_queue[i].store_kind);
		if (!transaction__execute_command(store, i, replace_prev))
			goto err;
	}

	// TODO(undo): execute triggers (how should they interact if one or more commands get unwound??)
	array_iterate(sys->trigger_queue, i, n)
		transaction__execute_trigger(i, replace_prev);

	if (!replace_prev)
		sys->current_id++;

	transaction_clear_redo_history();

	transaction_system__cleanup_commit(sys);
	return true;

err:
	if (!transaction__unwind_bundle(&array_last(sys->undo_stack)))
		/* something went very haywire - expect undefined behavior */
		assert(false);

	transaction_system__cleanup_commit(sys);
	return false;
}

static
void transaction__revert_and_swap_commands(revertable_command_bundle_t *from,
                                           revertable_command_bundle_t *to)
{
	do {
		revertable_command_t *cmd = &array_last(from->d);
		if (!transaction__revert_command(cmd))
			/* something went very haywire - expect undefined behavior */
			assert(false);

		array(unsigned char) swap = cmd->bytes_before;
		cmd->bytes_before = cmd->bytes_after;
		cmd->bytes_after = swap;

		array_append(to->d, array_last(from->d));
		array_pop(from->d);

	} while (array_sz(from->d) > 0);

	revertable_command_bundle__destroy(from);
}

static
void transaction__execute_and_swap_triggers(trigger_bundle_t *from, trigger_bundle_t *to)
{
	array_foreach(from->d, trigger_t, trigger) {
		transaction__execute_trigger_op(trigger);
		array_append(to->d, *trigger);
	}

	trigger_bundle__destroy(from);
}

static
void transaction__undo_commands(transaction_system_t *sys)
{
	revertable_command_bundle_t *undo_commands = &array_last(sys->undo_stack);
	array_append(sys->redo_stack,
	             revertable_command_bundle__create(undo_commands->transaction_id, sys->alc));
	revertable_command_bundle_t *redo_commands = &array_last(sys->redo_stack);

	transaction__revert_and_swap_commands(undo_commands, redo_commands);
	array_pop(sys->undo_stack);
}

static
void transaction__undo_triggers(transaction_system_t *sys)
{
	trigger_bundle_t *undo_triggers = &array_last(sys->undo_triggers);
	array_append(sys->redo_triggers,
	             trigger_bundle__create(undo_triggers->transaction_id, sys->alc));
	trigger_bundle_t *redo_triggers = &array_last(sys->redo_triggers);

	transaction__execute_and_swap_triggers(undo_triggers, redo_triggers);
	array_pop(sys->undo_triggers);
}

b32 transaction_undo()
{
	transaction_system_t *sys = g_active_transaction_system;

	if (array_sz(sys->undo_stack) == 0)
		return false;

	transaction__undo_commands(sys);
	transaction__undo_triggers(sys);

	--sys->current_id;

	return true;
}

static
void transaction__redo_commands(transaction_system_t *sys)
{
	revertable_command_bundle_t *redo_commands = &array_last(sys->redo_stack);
	array_append(sys->undo_stack,
	             revertable_command_bundle__create(redo_commands->transaction_id, sys->alc));
	revertable_command_bundle_t *undo_commands = &array_last(sys->undo_stack);

	transaction__revert_and_swap_commands(redo_commands, undo_commands);
	array_pop(sys->redo_stack);
}

static
void transaction__redo_triggers(transaction_system_t *sys)
{
	trigger_bundle_t *redo_triggers = &array_last(sys->redo_triggers);
	array_append(sys->undo_triggers,
	             trigger_bundle__create(redo_triggers->transaction_id, sys->alc));
	trigger_bundle_t *undo_triggers = &array_last(sys->undo_triggers);

	transaction__execute_and_swap_triggers(redo_triggers, undo_triggers);
	array_pop(sys->redo_triggers);
}

b32 transaction_redo()
{
	transaction_system_t *sys = g_active_transaction_system;

	if (array_sz(sys->redo_stack) == 0)
		return false;

	transaction__redo_commands(sys);
	transaction__redo_triggers(sys);

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

static
store_kind_e transaction__active_store_kind(transaction_system_t *sys)
{
	return array_last(sys->store_kind_stack);
}

// CLEANUP: de-dup with transaction_enqueue_mutation_buffer()
void transaction_enqueue_mutation_primitive(payload_primitive_t *payload, u32 offset)
{
	transaction_system_t *sys = g_active_transaction_system;
	transaction__enqueue_validate(sys);

	const command_t command = {
		.kind = COMMAND_KIND_MODIFY_PRIMITIVE,
		.store_kind = transaction__active_store_kind(sys),
		.payload.primitive = *payload,
		.offset = offset,
	};
	array_append(sys->command_queue, command);
}

void transaction_enqueue_mutation_buffer(payload_dynamic_t *payload, u32 offset)
{
	transaction_system_t *sys = g_active_transaction_system;
	transaction__enqueue_validate(sys);

	const command_t command = {
		.kind = COMMAND_KIND_MODIFY_BUFFER,
		.store_kind = transaction__active_store_kind(sys),
		.payload.dynamic = *payload,    // NOTE(luke): copies ptr from dynamic allocation
		.offset = offset,
	};
	array_append(sys->command_queue, command);
}

void transaction_enqueue_trigger_basic(void (* basic)())
{
	transaction_system_t *sys = g_active_transaction_system;

	const trigger_t trigger = {
		.kind = TRIGGEr_KIND_BASIC,
		.handle.basic = basic,
	};
	array_append(sys->trigger_queue, trigger);
}

#undef TRANSACTION_IMPLEMENTATION
#endif // TRANSACTION_IMPLEMENTATION

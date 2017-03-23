#ifndef UUID_H
#define UUID_H

#include "violet/core.h"

#ifdef _WIN32
#include "Rpc.h"
typedef UUID uuid;
#elif
#include <uuid/uuid.h>
typedef uuid_t uuid;
#endif

void uuid_gen(uuid *id);
void uuid_to_string(const uuid *id, char *buf);

#endif // UUID_H


/* Implementation */

#ifdef UUID_IMPLEMENTATION

#ifndef _WIN32

void uuid_gen(uuid *id)
{
	uuid_generate(*id);
}

void uuid_to_string(const uuid *id, char *buf)
{
	uuid_unparse(*id, buf);
}

#else

void uuid_generate(uuid *id)
{
	const RPC_STATUS result = UuidCreate(id);
	assert(result == RPC_S_OK);
	UNUSED(result);
}

void uuid_unparse(const uuid *id, char *buf)
{
	RPC_CSTR str = NULL;
	const RPC_STATUS result = UuidToString(id, &str);
	assert(result == RPC_S_OK);
	buf[0] = '{';
	strcpy(buf + 1, (char*)str);
	buf[37] = '}';
	buf[38] = '\0';
	RpcStringFree(&str);
	UNUSED(result);
}

#undef UUID_IMPLEMENTATION
#endif // UUID_IMPLEMENTATION

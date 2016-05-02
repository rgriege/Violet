#include <assert.h>

#include "violet/utility/uuid.h"

#ifdef _WIN32

void vlt_uuid_generate(uuid * id)
{
	const RPC_STATUS result = UuidCreate(id);
	assert(result == RPC_S_OK);
	UNUSED(result);
}

void vlt_uuid_to_string(const uuid * id, char * buf)
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

#elif defined(__unix__)

void vlt_uuid_generate(uuid * id)
{
	uuid_generate(id);
}

void vlt_uuid_to_string(const uuid * id, char * buf)
{
	uuid_unparse(id, buf);
}

#endif

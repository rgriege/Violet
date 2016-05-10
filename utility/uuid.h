#ifndef VIOLET_UUID_H
#define VIOLET_UUID_H

#include "violet/core/types.h"

#ifdef _WIN32

#include "Rpc.h"

typedef UUID uuid;

#elif defined(__unix__)

#include <uuid/uuid.h>

typedef struct uuid_t uuid*;

#endif

void vlt_uuid_generate(uuid * id);
void vlt_uuid_to_string(const uuid * id, char * buf);

#endif
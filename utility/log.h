#ifndef VIOLET_LOG_H
#define VIOLET_LOG_H

typedef struct ostream ostream;

void log_init();
void log_destroy();

void log_add_stream(ostream * s);
b8 log_remove_stream(ostream * s);

void log_write(const char * format, ...);

#endif


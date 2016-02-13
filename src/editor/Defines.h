#ifndef EDITOR_DEFINES_H
#define EDITOR_DEFINES_H

#ifdef WIN32
#ifdef EDITOR_EXPORT
#define EDITOR_API __declspec(dllexport)
#else
#define EDITOR_API __declspec(dllimport)
#endif
#else
#define EDITOR_API
#endif

#endif
#ifndef SERIALIZATION_CONFIG_H
#define SERIALIZATION_CONFIG_H

#ifdef WIN32
#ifdef VIOLETEXTRAS_EXPORT
#define VIOLET_EXTRAS_API __declspec(dllexport)
#else
#define VIOLET_EXTRAS_API __declspec(dllimport)
#endif
#else
#define VIOLET_API
#endif

#endif
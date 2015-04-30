#ifndef CONFIG_H
#define CONFIG_H

#ifdef WIN32
#ifdef VIOLETGAME_EXPORT
#define VIOLET_GAME_API __declspec(dllexport)
#else
#define VIOLET_GAME_API __declspec(dllimport)
#endif
#else
#define VIOLET_API
#endif

#endif
#ifndef VIOLET_EditorConfig_H
#define VIOLET_EditorConfig_H

#ifdef WIN32
#ifdef VIOLETEDITOR_EXPORT
#define VIOLET_EDITOR_API __declspec(dllexport)
#else
#define VIOLET_EDITOR_API __declspec(dllimport)
#endif
#else
#define VIOLET_EDITOR_API
#endif

#endif
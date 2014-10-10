#ifdef VIOLET_GRAPHICS_EXPORTS
    #define VIOLET_GRAPHICS_API __declspec(dllexport)
    #define VIOLET_GRAPHICS_TEMPLATE_EXTERN
#else
    #define VIOLET_GRAPHICS_API __declspec(dllimport)
    #define VIOLET_GRAPHICS_TEMPLATE_EXTERN extern
#endif
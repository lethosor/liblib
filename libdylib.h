#ifndef LIBDYLIB_H
#define LIBDYLIB_H

#define LIBDYLIB_VERSION 0x010200
#define LIBDYLIB_VERSION_STR "1.2"

#include <stdarg.h>

#if !defined(LIBDYLIB_UNIX) && (defined(__APPLE__) || defined(__linux__) || defined(__UNIX__))
    #define LIBDYLIB_UNIX
#elif !defined(LIBDYLIB_WINDOWS) && defined(WIN32)
    #define LIBDYLIB_WINDOWS
#else
    #error "Could not detect platform - try defining LIBDYLIB_UNIX or LIBDYLIB_WINDOWS appropriately"
#endif

#ifdef LIBDYLIB_UNIX
    #define LIBDYLIB_EXPORT __attribute__((visibility("default")))
#else
    #define LIBDYLIB_EXPORT __declspec(dllexport)
#endif

#ifdef __cplusplus
    #ifndef LIBDYLIB_C
        #define LIBDYLIB_CXX
    #endif
#else
    #ifdef LIBDYLIB_CXX
        #error "Can't build as C++ with a C compiler"
    #else
        #define LIBDYLIB_C
    #endif
#endif

#ifdef LIBDYLIB_C
    #define LIBDYLIB_NAME(name) libdylib_##name
    #define LIBDYLIB_DECLARE(type, name) LIBDYLIB_EXPORT type libdylib_##name
    #define LIBDYLIB_DEFINE(type, name) type libdylib_##name
#else
    #define LIBDYLIB_NAME(name) libdylib::name
    #define LIBDYLIB_DECLARE(type, name) LIBDYLIB_EXPORT type name
    #define LIBDYLIB_DEFINE(type, name) type libdylib::name
#endif

#ifdef LIBDYLIB_CXX
namespace libdylib {
#endif

    // Empty struct
    typedef struct dylib_ref_ {} *dylib_ref;

    // attempt to load a dynamic library from a path
    // return a library handle or NULL
    LIBDYLIB_DECLARE(dylib_ref, open)(const char *path);

    // return a handle to the current executable
    LIBDYLIB_DECLARE(dylib_ref, open_self)();

    // close the specified dynamic library
    // returns 1 on success, 0 on failure
    LIBDYLIB_DECLARE(short, close)(dylib_ref lib);

    // attempt to load a dynamic library from all paths given
    // return a library handle of the first successfully-loaded library, or NULL if none were successfully loaded
    // NOTE: the last argument must be NULL
    LIBDYLIB_DECLARE(dylib_ref, open_list)(const char *path, ...);
    LIBDYLIB_DECLARE(dylib_ref, va_open_list)(const char *path, va_list args);

    // return the address of a symbol in a library, or NULL if the symbol does not exist
    LIBDYLIB_DECLARE(void*, lookup)(dylib_ref lib, const char *symbol);

    // set the contents of dest to the result of lookup(lib, symbol) and returns 1,
    // or set dest to NULL and returns 0 if the symbol was not found
    LIBDYLIB_DECLARE(short, bind)(dylib_ref lib, const char *symbol, void **dest);
    // helper macros - note that dest is a simple pointer, NOT a pointer to a pointer
    #define LIBDYLIB_BIND(lib, symbol, dest) LIBDYLIB_NAME(bind)(lib, symbol, (void**)&dest)
    #define LIBDYLIB_BINDNAME(lib, name) LIBDYLIB_BIND(lib, #name, name)

    // check for the existence of a symbol in a library
    LIBDYLIB_DECLARE(short, find)(dylib_ref lib, const char *symbol);

    // check for the existence of any or all specified symbols in a library, respectively
    // NOTE: the last argument must be NULL (0 should not be relied on)
    LIBDYLIB_DECLARE(short, find_any)(dylib_ref lib, ...);
    LIBDYLIB_DECLARE(short, va_find_any)(dylib_ref lib, va_list args);
    LIBDYLIB_DECLARE(short, find_all)(dylib_ref lib, ...);
    LIBDYLIB_DECLARE(short, va_find_all)(dylib_ref lib, va_list args);

    // returns the last error message set by libdylib functions, or NULL
    LIBDYLIB_DECLARE(const char*, last_error)();

    // return compiled version information
    LIBDYLIB_DECLARE(int, get_version)();
    LIBDYLIB_DECLARE(const char*, get_version_str)();

#ifdef LIBDYLIB_CXX
}
#endif

#endif /* LIBDYLIB_H */

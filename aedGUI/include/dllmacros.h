#ifndef DLLMACROS_H
#define DLLMACROS_H

#if defined(_MSC_VER)
#pragma warning( disable : 4231 )
#pragma warning( disable : 4251 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4311 )
#endif

#if !defined(DLLEXPORT) && defined(WIN32)
#if !defined(AEDGUI_H)
#define DLLEXPORT __declspec( dllexport )
#define DLLEXTERN
#else
#define DLLEXPORT __declspec( dllimport )
#define DLLEXTERN extern
#endif
#else
#define DLLEXPORT
#define DLLEXTERN
#endif

#endif /* DLLMACROS_H */

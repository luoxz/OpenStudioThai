#ifndef BEC_BECAPI_HPP
#define BEC_BECAPI_HPP

#if _WIN32 || _MSC_VER

#ifdef openstudio_bec_EXPORTS
#define BEC_API __declspec(dllexport)
#else
#define BEC_API __declspec(dllimport)
#endif
#else
#define BEC_API
#endif

#endif

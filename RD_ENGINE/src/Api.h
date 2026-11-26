#pragma once

#ifdef RD_EXPORTS
#define RD_API __declspec(dllexport)
#else
#define RD_API __declspec(dllimport)
#endif

#pragma warning(disable: 4251 4275)

#pragma once
#include "core/Log.h"

#ifdef _DEBUG
#define RD_ASSERT(condition) \
	do \
	{ \
		if (!(condition)) \
		{ \
			rd::Log::Error("Assert: {} ({}:{})", #condition, __FILE__, __LINE__); \
			__debugbreak(); \
		} \
	} while (0)
#else
#define RD_ASSERT(condition) ((void)0)
#endif

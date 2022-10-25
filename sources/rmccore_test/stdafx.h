#pragma once

#include <cstdint>
#include <string>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#else
	#ifdef __APPLE__
		#include "TargetConditionals.h"
		#ifdef TARGET_OS_MAC
			// Mac OS
		#elif TARGET_OS_IPHONE
			// iOS
		#elif TARGET_IPHONE_SIMULATOR
			// iOS Simulator
		#else
			// Unsupported platform
			#error "Unsupported platform"
		#endif
	#else
		#ifdef __ANDROID__
			// Android NDK
			#include <android/api-level.h>
		#else
			#ifdef __linux__
				// Linux
			#else
				#error "Unsupported platform"
			#endif
		#endif
	#endif
#endif

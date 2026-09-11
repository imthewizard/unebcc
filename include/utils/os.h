#ifndef UNEBCC_UTILS_OS_H
#define UNEBCC_UTILS_OS_H

#if defined(_WIN32)
	#define UNEBCC_WINDOWS
#elif defined(__linux__)
	#define UNEBCC_LINUX
#else
	#error "Couldn't determine operating system"
#endif

#endif // UNEBCC_UTILS_OS_H

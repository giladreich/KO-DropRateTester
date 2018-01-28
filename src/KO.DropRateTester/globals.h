#pragma once

#include <QObject>
#include <windows.h>
#include <random>

#ifdef _WIN32
#define INLINE __forceinline
#else
#define INLINE inline //__attribute__((always_inline))
#endif

class Globals : public QObject
{
	Q_OBJECT

public:
	Globals(QObject *parent);
	~Globals();
};


int32_t myrand(int32_t min, int32_t max);
void reset();

INLINE time_t getMSTime()
{
#ifdef _WIN32
#if WINVER >= 0x0600
	typedef ULONGLONG(WINAPI *GetTickCount64_t)(void);
	static GetTickCount64_t pGetTickCount64 = nullptr;

	if (!pGetTickCount64)
	{
		HMODULE hModule = LoadLibraryA("KERNEL32.DLL");
		pGetTickCount64 = (GetTickCount64_t)GetProcAddress(hModule, "GetTickCount64");
		if (!pGetTickCount64)
			pGetTickCount64 = (GetTickCount64_t)GetTickCount;
		FreeLibrary(hModule);
	}

	return pGetTickCount64();
#else
	return GetTickCount();
#endif
#else
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec * SECOND) + (tv.tv_usec / SECOND);
#endif
}

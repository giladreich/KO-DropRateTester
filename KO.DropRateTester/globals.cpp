#include "stdafx.h"
#include "globals.h"


static std::mt19937 s_randomNumberGenerator;
static bool s_rngSeeded = false;

Globals::Globals(QObject *parent)
	: QObject(parent)
{ }

Globals::~Globals()
{ }


void reset(void)
{
	s_rngSeeded = false;
}


INLINE void SeedRNG()
{
	if (!s_rngSeeded)
	{
		s_randomNumberGenerator.seed(static_cast<uint32_t>(getMSTime()));
		s_rngSeeded = true;
	}
}


int32_t myrand(int32_t min, int32_t max)
{
	SeedRNG();
	if (min > max) std::swap(min, max);
	std::uniform_int_distribution<int32_t> dist(min, max);
	return dist(s_randomNumberGenerator);
}

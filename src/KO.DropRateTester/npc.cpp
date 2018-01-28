#include "stdafx.h"
#include "npc.h"
#include "globals.h"

CNpc::CNpc(QObject *parent)
	: QObject(parent)
{
}

CNpc::~CNpc()
{
}

bool CNpc::GiveNpcHaveItem(uint16_t sPercent)
{
	int iRandom = myrand(1, 10000);

	if (iRandom > sPercent)
		return false;

	return true;
}
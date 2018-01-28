#pragma once

#include <QObject>

class CNpc : public QObject
{
	Q_OBJECT

public:
	CNpc(QObject* parent);
	~CNpc();

	bool GiveNpcHaveItem(uint16_t sPercent);
	bool isItemDroped(uint16_t sPercent) { return GiveNpcHaveItem(sPercent); };
};

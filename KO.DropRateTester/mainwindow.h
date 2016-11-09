#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "npc.h"
#include "helpwindow.h"
#include "aboutwindow.h"

enum Layout
{
	vLayoutPercent,
	gLayoutResults,
	hLayoutLoop,
	vLayoutFrequency,
	vLayoutItemDroped,
	LAYOUTS_COUNT
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);
	
	void showLayout(bool visible, QLayout* pLayout, Layout type);
	void displayResults(uint16_t iPercent);

private:
	Ui::MainWindow ui;
	
	CNpc* m_pNpc;

	uint32_t m_monKillCounter = 0;
	uint32_t m_dropsCounter = 0;

	bool m_vLayoutPercentVisible;
	bool m_gLayoutResultsVisible;
	bool m_hLayoutLoopVisible;
	bool m_vLayoutFrequencyVisible;
	bool m_vLayoutItemDropedVisible;


public slots:
	void onReset();
	void onCheckDrop();

	void showAbout();
	void showHelp();


};

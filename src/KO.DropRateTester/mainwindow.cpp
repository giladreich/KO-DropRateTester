#include "stdafx.h"
#include "mainwindow.h"
#include "globals.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//showLayout(false, ui.vLayoutFrequency, vLayoutFrequency);
	showLayout(false, ui.hLayoutLoop, hLayoutLoop);
	this->setFixedSize(this->size());
	this->statusBar()->setSizeGripEnabled(false);


	connect(ui.actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
	connect(ui.actionHelp, &QAction::triggered, this, &MainWindow::showHelp);
	connect(ui.btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
	connect(ui.btnCheckDrop, &QPushButton::clicked, this, &MainWindow::onCheckDrop);
	connect(ui.chbLoop, &QCheckBox::clicked, this, [=]{
		//showLayout(!m_vLayoutFrequencyVisible, ui.vLayoutFrequency, vLayoutFrequency);
		showLayout(!m_hLayoutLoopVisible, ui.hLayoutLoop, hLayoutLoop);
	});

}

void MainWindow::onReset()
{
	m_monKillCounter = 0;
	m_dropsCounter = 0;
	reset();

	// UI
	ui.lblDropCount->setText("0");
	ui.lblMonCount->setText("0");
	ui.tbxResult->setText("");
	ui.tbxLoopCount->setText("");
	ui.chbLoop->setCheckState(Qt::Unchecked);
	ui.cbxDrops->clear();
	//showLayout(false, ui.vLayoutFrequency, vLayoutFrequency);
	showLayout(false, ui.hLayoutLoop, hLayoutLoop);
}


void MainWindow::onCheckDrop()
{
	float fPercent = ui.tbxPercent->displayText().toFloat();
	uint16_t iPercent = fPercent * 100; // In the database we read 100% as 10000.
	if (iPercent <= 0 || iPercent > 10000)
	{
		ui.tbxResult->setStyleSheet("");
		ui.tbxResult->setText("Invalid Input");
		return;
	}

	if (ui.chbLoop->checkState())
	 	for (size_t i = 0; i < ui.tbxLoopCount->displayText().toUInt(); i++)
	 		displayResults(iPercent);
	else displayResults(iPercent);
}

void MainWindow::displayResults(uint16_t sPercent)
{
	ui.lblMonCount->setText(std::to_string(++m_monKillCounter).c_str());

	if (m_pNpc->isItemDroped(sPercent))
	{
		ui.tbxResult->setStyleSheet("color: #53A36E");
		ui.tbxResult->setText("Yup");
		ui.lblDropCount->setText(std::to_string(++m_dropsCounter).c_str());
		ui.cbxDrops->addItem(std::to_string(m_monKillCounter).c_str());
	}
	else
	{
		ui.tbxResult->setStyleSheet("color: #FF0000");
		ui.tbxResult->setText("Nope");
	}

}


void MainWindow::showLayout(bool visible, QLayout* pLayout, Layout type)
{
	for (size_t i = 0; i < pLayout->count(); ++i)
	{
		QWidget* widget = pLayout->itemAt(i)->widget();
		if (widget == nullptr) continue;
		widget->setVisible(visible);
	}

	switch (type)
	{
	case vLayoutPercent:
		m_vLayoutPercentVisible = visible;
		break;
	case gLayoutResults:
		m_gLayoutResultsVisible = visible;
		break;
	case hLayoutLoop:
		m_hLayoutLoopVisible = visible;
		break;
	case vLayoutFrequency: 
		m_vLayoutFrequencyVisible = visible; 
		break;
	case vLayoutItemDroped:
		m_vLayoutItemDropedVisible = visible;
		break;
	}
}


void MainWindow::showAbout()
{
	AboutWindow* pAboutWindow = new AboutWindow(this);
	pAboutWindow->exec();
}

void MainWindow::showHelp()
{
	HelpWindow* pHelpWindow = new HelpWindow(this);
	pHelpWindow->show();;
}



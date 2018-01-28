#include "stdafx.h"
#include "helpwindow.h"

HelpWindow::HelpWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	this->setFixedSize(this->size());

}

HelpWindow::~HelpWindow()
{
}

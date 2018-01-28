#include "stdafx.h"
#include "aboutwindow.h"

AboutWindow::AboutWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	this->setFixedSize(this->size());
	ui.lblGitHubLink->setText("<a href=\"https://github.com/GiladRe\">https://github.com/GiladRe</a>");
}

AboutWindow::~AboutWindow()
{
}

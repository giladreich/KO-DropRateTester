#pragma once

#include <QWidget>
#include "ui_helpwindow.h"

class HelpWindow : public QWidget
{
	Q_OBJECT

public:
	HelpWindow(QWidget *parent = Q_NULLPTR);
	~HelpWindow();

private:
	Ui::HelpWindow ui;
};

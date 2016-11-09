#pragma once

#include <QWidget>
#include "ui_aboutwindow.h"

class AboutWindow : public QDialog
{
	Q_OBJECT

public:
	AboutWindow(QWidget *parent = Q_NULLPTR);
	~AboutWindow();

private:
	Ui::AboutWindow ui;
};

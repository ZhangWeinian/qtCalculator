#pragma once
#include "Calc.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	Calc myCalc;
	myCalc.show();

	return a.exec();
}

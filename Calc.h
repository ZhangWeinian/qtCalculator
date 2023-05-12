#ifndef __CALC__
#define __CALC__

#pragma once

#include <QtWidgets/QWidget>
#include <qbuttongroup.h>
#include <qvector.h>
#include <qpushbutton.h>
#include "ui_Calc.h"
#include "getComputedResult.h"

class Calc: public QWidget
{
	Q_OBJECT

public:
	Calc(QWidget* parent = nullptr);

	~Calc();

public slots:

	//行为控制
	void onButtonGroupCliked(QAbstractButton* btn);

	//准备动作
	void iniUI(void);

private:

	//运算标识符：表示是否完成了一次完整运算（包括完整显示）。定义：0--未完成，1--完成
	int calcFlag;

	//+/-标识符：0--调整中，1--调整结束，-1 —— 跳过当前显示
	int asFlag;

	//操作标识符：0--操作中，1--操作结束
	int operFlag;

	//括号标识符：给左括号：a++，给右括号：a--
	unsigned short bkFlag;

	//存储按钮的组
	QButtonGroup* BG;

	//存储表达式
	QString _str;
	QVector<QString> strs;

	//缓存上一次的数字类数据
	QString curData;

	//缓存上一次的操作符数据
	QString curOper;

	//缓存上一次的全类型数据
	QString lastName;

	//基操类的实例
	Ui::CalcClass ui;

	//计算类的实例
	getComputedResult ans;

	//操作符表，控制符表
	const QVector<QString> _oper;
	const QVector<QString> _ctrl;

	//判断是否是操作符、数字、控制符。返回 bool 类型的判断结果：是--true、否--false
	bool is_oper(const QString& qStr);
	bool is_num(const QString& qStr);
	bool is_ctrl(const QString& qStr);

	//可变的三个按钮，其显示内容只有三种可能。定义：
	// 0 is : "( )", "C", "CE"
	//-1 is : "("  , ")", "CE"
	// 1 is : "("  , ")", "C"
	//当需要改变其状态时，按所传数字改变为对应的文本
	void changeBtn(const int _case);

	//获取当前表达式
	QString getStr(void);

	QString getStr(const QString& _str);

	//获取当前表达式（无括号）
	QString getStrNoBk(void);

	//在exp上实时显示表达式
	void showStrToExp(const QString& _str, const QString& _cur);

	//在输入栏实时显示数据
	void showStrToLine(const QString& _str);

	//记录已经确定的数据
	void takeData(QString& _str);

	//清除所有数据
	void clearAll(void);

	//清除exp
	void clearExp(void);

	//清除输入栏
	void clearLine(void);
};

#endif //__CALC__

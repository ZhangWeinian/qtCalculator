#include "Calc.h"

Calc::Calc(QWidget* parent):
QWidget(parent),
_oper({ "+", "-", "×", "/", "( )", "(", ")" }),
_ctrl({ "CE", "C", "DEL", "=", "1/x", "x²", "√x", "+/-" })
{
	//构造窗口
	ui.setupUi(this);

	//设置窗口图标
	setWindowIcon(QIcon(":/assest/Calc.ico"));

	//设置窗口标题
	setWindowTitle("计算器");

	//存储按钮
	BG = new QButtonGroup(this);

	//其他数据初始化
	_str = "";
	strs = {};

	curData = "";

	calcFlag = 0;
	asFlag = 1;
	operFlag = 1;
	bkFlag = 0;

	curOper = "";
	lastName = "";

	//初始时，btnLbk显示完整的括号；btnRbk显示“C”；btnCE显示“CE”
	ui.btnLbk->setText("( )");
	ui.btnRbk->setText("C");
	ui.btnCE->setText("CE");

	//开始执行
	iniUI();
}

Calc::~Calc()
{
	delete BG;
}

void Calc::iniUI(void)
{
	//1，找到所有按钮，放到一个组里
	auto btnList = findChildren<QPushButton*>();
	for (auto& btn : btnList)
	{
		BG->addButton(btn);
	}

	//2，关联按钮的点击信号
	connect(BG, &QButtonGroup::buttonClicked, this, &Calc::onButtonGroupCliked);
}

//行为控制
void Calc::onButtonGroupCliked(QAbstractButton* btn)
{
	//获取所点击的按钮的字符（串）
	auto name = btn->text();

	//部分状态标识符重置需要在此进行
	if (lastName == "=")
	{
		//1，重置运算标识符
		calcFlag = 0;

		//2，视情况是否清楚数据
		if (name != "=")
		{
			clearAll();
		}
	}
	if (lastName == "+/-" && name != "+/-")
	{
		asFlag = 1;
	}
	if (lastName == "error")
	{
		clearExp();
		clearLine();

		lastName.clear();
	}

	//如果是数字
	if (is_num(name))
	{
		//1，调整并记录
		operFlag = 1;
		takeData(curOper);

		//2，临时缓存。区分是否为小数
		if (name == ".")
		{
			if (curData == "" || curData.isNull())
			{
				curData = "0.";
			}
			else
			{
				curData += name;
			}
		}
		else
		{
			if (curData == "0")
			{
				curData = name;
			}
			else
			{
				curData += name;
			}
		}

		//3，显示
		if (operFlag == 1)
		{
			showStrToLine(curData);
		}
	}

	//如果是操作符
	else if (is_oper(name))
	{
		//是否使用上次的计算结果作为此次的操作数
		if (curData.isNull() && lastName == "=")
		{
			curData = ui.LineEdit->text();
		}
		if (strs.empty())
		{
			curData = "0";
		}

		//1，调整
		operFlag = 0;
		if (curData.size() > 1 && curData.front() == '-')
		{
			curData = '(' + curData + ')';
		}

		takeData(curData);

		if (name != lastName && (name == "(" || name == ")"))
		{
			//2，临时缓存，区分是否为括号；调整文本按钮
			if (name == "( )")
			{
				name = "(";
			}
			if (name == "×")
			{
				name = "*";
			}

			if (is_oper(lastName) && (lastName != "("))
			{
				takeData(curOper);
			}

			curOper = name;

			//记录bk次数；设置按钮文本
			if (name == "(")
			{
				bkFlag++;
				changeBtn(-1);
			}
			if (name == ")")
			{
				bkFlag--;

				if (bkFlag == 0)
				{
					changeBtn(0);
				}
				else
				{
					changeBtn(-1);
				}
			}

			//左、右括号没有调整余地，如遇到，直接记录
			if (name == "(" || name == ")")
			{
				takeData(curOper);
			}
		}
		else
		{
			curOper = name;
		}

		//3，显示
		showStrToExp(getStr(), curOper);
	}

	//如果是控制符
	else if (is_ctrl(name))
	{
		//不同的控制符对应不同的处理办法
		//如下的三个在未完成一次完整运算时不改变其他
		if (name == "=")
		{
			//1，调整
			calcFlag = 0;

			//2，记录
			if (is_num(lastName) || asFlag == 1)
			{
				if (curData.size() > 1 && curData.front() == '-')
				{
					curData = '(' + curData + ')';
				}

				if (strs.empty() && (curData.isNull() || curData == ""))
				{
					curData = "0";
				}

				takeData(curData);
			}
			if (lastName != ")" && is_oper(lastName))
			{
				auto _a = curData + ui.LineEdit->text();
				curData.clear();
				takeData(_a);
			}

			//3，判断括号是否匹配
			if (bkFlag != 0)
			{
				showStrToExp(getStr(), "");
				showStrToLine("括号不完整");
				lastName = "error";
			}
			else
			{
				//4，计算并显示
				auto thisAns = ans._get(getStr().toStdString());
				showStrToLine(QString::number(thisAns));
				showStrToExp(getStr(), "=");
			}

			//5，清空所有数据；重置按钮文本
			//为了防止重复按=时的意外显示，清除工作放在最开始
			changeBtn(0);
		}

		if (name == "C")
		{
			clearExp();
			clearLine();

			changeBtn(0);
		}

		if (name == "CE")
		{
			clearLine();

			if (bkFlag != 0)
			{
				changeBtn(1);
			}
		}

		if (name == "DEL")
		{
			//在完成一次运算的基础上del，则只删除输入栏，保留exp栏
			if (calcFlag == 1)
			{
				clearLine();
			}
			else if (ui.LineEdit->text() != "0")
			{
				//光标向前移动一位，光标之后的内容清除
				ui.LineEdit->setCursorPosition(ui.LineEdit->cursorPosition() - 1);
				ui.LineEdit->del();

				//如果删除了仅有的一位数字，则置零，并按需更新btnCE文本
				if (ui.LineEdit->text() == "")
				{
					ui.LineEdit->setText("0");

					if (bkFlag != 0 && ui.btnRbk->text() != "C")
					{
						changeBtn(1);
					}
				}
			}

			//输入栏数据与curData应保持一致
			curData = ui.LineEdit->text();
		}

		//如下的四个需要在按下的同时显示对应答案
		if (name == "1/x")
		{
			//1，调整
			asFlag = 1;

			//2，临时缓存
			if (curData.isNull() || curData == "")
			{
				curData = ui.LineEdit->text();
			}
			if (curData == "0")
			{
				showStrToExp(getStr(), "1/(0)");
				showStrToLine("除数不能为零");
				lastName = "error";
			}
			else
			{
				QString _a = {};

				if (strs.isEmpty())
				{
					_a = "1/(" + curData + ")";
				}
				else
				{
					_a = "( 1/(" + curData + ") )";
				}

				curData = QString::number(ans._get(_a.toStdString()));

				//3，记录
				takeData(_a);

				//4，显示
				showStrToExp(getStr(), "");
				showStrToLine(curData);
			}

			//5，清理
			curData.clear();
		}

		if (name == "√x")
		{
			//1，调整
			asFlag = 1;

			//2，临时缓存
			if (curData.isNull() || curData == "")
			{
				curData = ui.LineEdit->text();
			}
			if (curData.front() == '-')
			{
				showStrToExp(getStr(), "√(" + curData + ")");
				showStrToLine("无效输入");
				lastName = "error";
			}
			else
			{
				QString _a = "√(" + curData + ")";
				curData = QString::number(ans._get(_a.toStdString()));

				//3，记录
				takeData(_a);

				//4，显示
				showStrToExp(getStr(), "");
				showStrToLine(curData);
			}

			//5，清理
			curData.clear();
		}

		if (name == "x²")
		{
			//1，调整
			asFlag = 1;

			//2，临时缓存
			if (curData.isNull() || curData == "")
			{
				curData = ui.LineEdit->text();
			}

			QString _a = "(" + curData + ")^2";
			curData = QString::number(ans._get(_a.toStdString()));

			//3，记录
			takeData(_a);

			//4，显示
			showStrToExp(getStr(), "");
			showStrToLine(curData);

			//5，清理
			curData.clear();
		}

		if (name == "+/-")
		{
			//1，调整
			asFlag = 0;

			//2，临时缓存
			if (curData.isNull() || curData == "")
			{
				curData = ui.LineEdit->text();
			}

			QString _a = {};
			if (!curData.isNull())
			{
				if (curData == "0" || curData == "")
				{
					_a = "0";
				}
				else
				{
					if (curData.front() == '-')
					{
						for (size_t i = 1; i < curData.size(); i++)
						{
							_a += curData.at(i);
						}
					}
					else
					{
						_a = '-' + curData;
					}
				}

				curData = _a;
			}

			//3，显示
			showStrToLine(curData);

			//4，由于数字的正负调整时可以反复进行的，故此处不记录
			//直到asFlag==1，再记录
		}
	}

	//记录上一次的全类型数据
	lastName = (lastName == "error") ? "error" : name;
}

//判断是否是操作符、数字、控制符。返回 bool 类型的判断结果：是--true、否--false
inline bool Calc::is_oper(const QString& qStr)
{
	for (auto& i : _oper)
	{
		if (i == qStr)
		{
			return true;
		}
	}

	return false;
}

inline bool Calc::is_num(const QString& qStr)
{
	QString cur = "";

	if (qStr.size() > 1 && qStr.front() == '-')
	{
		for (auto i = 1; i < qStr.size(); i++)
		{
			cur += qStr.at(i);
		}
	}
	else
	{
		cur = qStr;
	}

	for (auto& i : cur)
	{
		if ((i > '9' || i < '0') && i != '.')
		{
			return false;
		}
	}

	return true;
}

inline bool Calc::is_ctrl(const QString& qStr)
{
	for (auto& i : _ctrl)
	{
		if (i == qStr)
		{
			return true;
		}
	}

	return false;
}

//可变的三个按钮，其显示内容只有三种可能。定义：
// 0 is : "( )", "C", "CE"
//-1 is : "("  , ")", "CE"
// 1 is : "("  , ")", "C"
//当需要改变其状态时，按所传数字改变为对应的文本
void Calc::changeBtn(const int _case)
{
	switch (_case)
	{
		case 0:
			{
				ui.btnLbk->setText("( )");
				ui.btnRbk->setText("C");
				ui.btnCE->setText("CE");

				break;
			}
		case -1:
			{
				ui.btnLbk->setText("(");
				ui.btnRbk->setText(")");
				ui.btnCE->setText("CE");

				break;
			}
		case 1:
			{
				ui.btnLbk->setText("(");
				ui.btnRbk->setText(")");
				ui.btnCE->setText("C");
			}
	}
}

//获取当前表达式
QString Calc::getStr(const QString& _str)
{
	this->_str = _str;

	for (size_t i = 0; i < _str.size(); i++)
	{
		if (i % 1 == 0)
		{
			this->_str += ' ';
		}
	}

	if (this->_str.size() != 0 && this->_str.back() != ' ')
	{
		this->_str += ' ';
	}

	return this->_str;
}

QString Calc::getStr(void)
{
	_str.clear();

	for (size_t i = 0; i < strs.size(); i++)
	{
		_str += strs.at(i);

		if (i % 1 == 0)
		{
			_str += ' ';
		}
	}

	if (_str.size() != 0 && _str.back() != ' ')
	{
		_str += ' ';
	}

	return _str;
}

//获取当前表达式（无括号）
QString Calc::getStrNoBk(void)
{
	_str.clear();

	for (size_t i = 0; i < strs.size(); i++)
	{
		if (strs.at(i) != "(" && strs.at(i) != ")")
		{
			_str += strs.at(i);

			if (i % 1 == 0)
			{
				_str += ' ';
			}
		}
	}

	if (_str.size() != 0 && _str.back() != ' ')
	{
		_str += ' ';
	}

	return _str;
}

//在exp上实时显示表达式
void Calc::showStrToExp(const QString& _str, const QString& _cur)
{
	ui.expLineEdit->setText(_str + _cur);
}

//在输入栏实时显示数据
void Calc::showStrToLine(const QString& _str)
{
	ui.LineEdit->setText(_str);
}

//记录已经确定的数据
void Calc::takeData(QString& _str)
{
	if (!_str.isNull() && _str != "")
	{
		strs.emplaceBack(_str);
		_str.clear();
	}
}

//清除所有数据
void Calc::clearAll(void)
{
	_str.clear();
	strs.clear();
	curData.clear();
	curOper.clear();

	if (lastName != "=")
	{
		lastName.clear();
	}
}

//清除exp
void Calc::clearExp(void)
{
	ui.expLineEdit->clear();
	clearAll();
}

//清除输入栏
void Calc::clearLine(void)
{
	ui.LineEdit->setText("0");
	curData.clear();
}

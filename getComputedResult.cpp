#include "getComputedResult.h"

//表达式自定义标准格式化
void getComputedResult::getFormat(const std::string& infix)
{
	//处理根号
	stdInfix = changeOperOfSqrt(infix);

	//实现正负数
	for (size_t i = 0; i < stdInfix.size(); i++)
	{
		if (stdInfix.at(i) == '-' || stdInfix.at(i) == '+')
		{
			//-x转换为0-x，+x转化为0+x
			if (i == 0)
			{
				stdInfix.insert(0, 1, '0');
			}
			else if (stdInfix.at(i - 1) == '(')
			{
				stdInfix.insert(i, 1, '0');
			}
		}
	}
}

//处理给定文本中的根号
std::string getComputedResult::changeOperOfSqrt(const std::string& str)
{
	auto cmp = [&str]()
	{
		for (auto& i : str)
		{
			if (i < 0)
			{
				return false;
			}
		}

		return true;
	};

	if (cmp())
	{
		return str;
	}

	//将string转为wstring，以便于查找√
	std::wstring _str = toWstring(str);

	//找出字符串中第一个√
	size_t i = findOperOfSqrt(_str);

	//i之间的文本，保持不动
	std::wstring cur(_str.begin(), _str.begin() + i);

	//循环查找源字符串中的√，将其替换
	while (i != SIZE_MAX && i < str.size())
	{
		size_t j = 0;
		int bkFlagNum = 0;

		for (j = i + 3; j < str.size(); j++)
		{
			if (str.at(j) == '(')
			{
				bkFlagNum++;
			}
			if (str.at(j) == ')')
			{
				bkFlagNum--;
			}

			cur += str.at(j);

			if (bkFlagNum == 0)
			{
				cur += L"^(0.5)";
				break;
			}
		}
		cur += std::wstring(str.begin() + j + 1, str.end());

		i = findOperOfSqrt(cur);
	}

	return toString(cur);
}

//获取算术符号优先级
int getComputedResult::getPrior(char c)
{
	if (c == '+' || c == '-')
	{
		return PRIO_LV1;
	}
	else if (c == '*' || c == '/')
	{
		return PRIO_LV2;
	}
	else if (c == '^')
	{
		return PRIO_LV3;
	}
	else if (c == '！')
	{
		return PRIO_LV4;
	}
	else
	{
		return PRIO_LV0;
	}
}

//后缀表达式转换
void getComputedResult::getPostfix()
{
	//绝对值符号个数的奇偶性
	int absNumeber = ABS_ODD;

	std::string tmp;

	for (size_t i = 0; i < stdInfix.size(); i++)
	{
		tmp = "";

		switch (stdInfix.at(i))
		{
			case '+':
			case '-':
			case '*':
			case '/':

			case '^':
			case '!':
				{
					if (symStack.empty() || symStack.top() == '(' || symStack.top() == '[' || symStack.top() == '{' ||
						(symStack.top() == '|' && absNumeber == ABS_ODD))
					{
						symStack.push(stdInfix.at(i));
					}
					else
					{
						while (!symStack.empty() && (getPrior(symStack.top()) >= getPrior(stdInfix.at(i))))
						{
							tmp += symStack.top();
							postfix.emplace_back(tmp);
							symStack.pop();

							tmp = "";
						}

						symStack.push(stdInfix.at(i));
					}

					break;
				}

			case '|':
				{
					if (absNumeber == ABS_ODD)
					{
						symStack.push(stdInfix.at(i));

						absNumeber = ABS_EVEN;
					}
					else
					{
						while (!symStack.empty() && symStack.top() != '|')
						{
							tmp += symStack.top();
							postfix.emplace_back(tmp);
							symStack.pop();

							tmp = "";
						}

						if (!symStack.empty() && symStack.top() == '|')
						{
							tmp += symStack.top();

							//左绝对值符号'|'加入后缀表达式，用于绝对值的检测计算
							postfix.emplace_back(tmp);
							symStack.pop();

							absNumeber = ABS_ODD;
						}
					}

					break;
				}

			case '(':
			case '[':
			case '{':
				{
					symStack.push(stdInfix.at(i));

					break;
				}

			case ')':
				{
					while (!symStack.empty() && symStack.top() != '(')
					{
						tmp += symStack.top();
						postfix.emplace_back(tmp);
						symStack.pop();

						tmp = "";
					}

					if (!symStack.empty() && symStack.top() == '(')
					{
						//将左括号出栈丢弃
						symStack.pop();
					}

					break;
				}

			case ']':
				{
					while (!symStack.empty() && symStack.top() != '[')
					{
						tmp += symStack.top();
						postfix.emplace_back(tmp);
						symStack.pop();

						tmp = "";
					}
					if (!symStack.empty() && symStack.top() == '[')
					{
						//将左括号出栈丢弃
						symStack.pop();
					}

					break;
				}

			case '}':
				{
					while (!symStack.empty() && symStack.top() != '{')
					{
						tmp += symStack.top();
						postfix.emplace_back(tmp);
						symStack.pop();
						tmp = "";
					}

					if (!symStack.empty() && symStack.top() == '{')
					{
						//将左括号出栈丢弃
						symStack.pop();
					}

					break;
				}

			default:
				{
					if ((stdInfix.at(i) >= '0' && stdInfix.at(i) <= '9'))
					{
						tmp += stdInfix.at(i);

						while (i + 1 < stdInfix.length() &&
							   (stdInfix.at(i + 1) >= '0' && stdInfix.at(i + 1) <= '9' || stdInfix.at(i + 1) == '.'))
						{
							//小数处理
							tmp += stdInfix[i + 1];

							//是连续的数字，则追加
							i++;
						}
						if (tmp.at(tmp.length() - 1) == '.')
						{
							//将x.做x.0处理
							tmp += '0';
						}

						postfix.emplace_back(tmp);
					}

					break;
				}
		} //end switch
	}	  //end for

	while (!symStack.empty())
	{ //将栈中剩余符号加入后缀表达式
		tmp = "";
		tmp += symStack.top();
		postfix.emplace_back(tmp);
		symStack.pop();
	}
}

//计算后缀表达式
void getComputedResult::calResult()
{
	std::string tmp;

	double number = 0;
	double op1 = 0;
	double op2 = 0;

	for (int i = 0; i < postfix.size(); i++)
	{
		tmp = postfix.at(i);

		if (tmp.at(0) >= '0' && tmp.at(0) <= '9')
		{
			number = atof(tmp.c_str());
			figStack.push(number);
		}
		else if (postfix.at(i) == "+")
		{
			if (!figStack.empty())
			{
				op2 = figStack.top();
				figStack.pop();
			}
			if (!figStack.empty())
			{
				op1 = figStack.top();
				figStack.pop();
			}

			figStack.push(op1 + op2);
		}
		else if (postfix.at(i) == "-")
		{
			if (!figStack.empty())
			{
				op2 = figStack.top();
				figStack.pop();
			}
			if (!figStack.empty())
			{
				op1 = figStack.top();
				figStack.pop();
			}

			figStack.push(op1 - op2);
		}
		else if (postfix.at(i) == "*")
		{
			if (!figStack.empty())
			{
				op2 = figStack.top();
				figStack.pop();
			}
			if (!figStack.empty())
			{
				op1 = figStack.top();
				figStack.pop();
			}

			figStack.push(op1 * op2);
		}
		else if (postfix.at(i) == "/")
		{
			if (!figStack.empty())
			{
				op2 = figStack.top();
				figStack.pop();
			}
			if (!figStack.empty())
			{
				op1 = figStack.top();
				figStack.pop();
			}

			if (op2 != 0)
			{
				//除数不为0，未做处理，默认
			}

			figStack.push(op1 / op2);
		}

		else if (postfix.at(i) == "^")
		{
			if (!figStack.empty())
			{
				op2 = figStack.top();
				figStack.pop();
			}
			if (!figStack.empty())
			{
				op1 = figStack.top();
				figStack.pop();
			}

			figStack.push(pow(op1, op2));
		}
		else if (postfix.at(i) == "|")
		{
			if (!figStack.empty())
			{
				op1 = figStack.top();
				figStack.pop();
			}
			figStack.push(abs(op1));
		}
		else if (postfix.at(i) == "!")
		{
			if (!figStack.empty())
			{
				op1 = figStack.top();
				figStack.pop();
			}
			if (op1 > 0)
			{
				//阶乘数为小数时(转化为整数求阶)
				double factorial = 1;

				for (int i = 1; i <= op1; ++i)
				{
					factorial *= i;
				}
				op1 = factorial;
			}

			figStack.push(op1);
		}
	} //end for

	if (!figStack.empty())
	{
		result = figStack.top();
	}
}

//计算方法
void getComputedResult::calculate(const std::string& infix)
{
	//表达式自定义标准格式化
	getFormat(infix);

	//后缀表达式转换
	getPostfix();

	//获取算术结果
	calResult();
}

//获取结果
double getComputedResult::_get(const std::string& infix)
{
	//开始计算
	calculate(infix);

	//清空数据，为下次计算做准备
	std::stack<char>().swap(symStack);
	std::stack<double>().swap(figStack);

	postfix.clear();
	stdInfix.clear();

	//返回结果
	return result;
}

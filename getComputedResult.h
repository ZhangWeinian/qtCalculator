/*
* 2023/05/02
*
* 1、实现功能：
* 支持运算符：+、 - 、 ×、 /、 x%（百分号）、 x^y（幂次方）、 |x|绝对值）、 x!（阶乘）。其他符号：()、 []、 {}。
* 支持 √() 表达式
* 支持数据类型：正负数、小数。
*
* 2、实现方法
* 符号优先级处理方法：符号等级制
* 运算表达式处理方法：后缀表达式法
* 后缀表达式转换及计算辅助方法：符号栈、数字栈
*
*/

#ifndef __GETANS__
#define __GETANS__

#pragma once

#include <stack>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <codecvt>

//绝对值符号个数的奇偶性
enum ABS_ODEVITY
{
	ABS_ODD = 1,
	ABS_EVEN = 2,
};

//算术符号优先权等级
enum PRIO_LV
{
	PRIO_LV0 = 0,
	PRIO_LV1 = 1,
	PRIO_LV2 = 2,
	PRIO_LV3 = 3,
	PRIO_LV4 = 4,
};

//计算器类
class getComputedResult
{
public:
	getComputedResult(void)
	{
		result = 0;
		stdInfix = "";
		postfix = {};
	}

	//获取结果
	double _get(const std::string& infix);

private:

	//最终计算结果
	double result;

	//自定义标准格式化表达式
	std::string stdInfix;

	//后缀表达式向量
	std::vector<std::string> postfix;

	//符号栈、数字栈
	std::stack<char> symStack;
	std::stack<double> figStack;

	//表达式自定义标准格式化
	void getFormat(const std::string& infix);

	//获取算术符号优先级
	int getPrior(char c);

	//后缀表达式转换
	void getPostfix();

	//计算后缀表达式
	void calResult();

	//计算方法
	void calculate(const std::string& infix);

	//处理给定文本中的根号
	std::string changeOperOfSqrt(const std::string& str);

	//找寻√的位置
	inline size_t findOperOfSqrt(const std::wstring& str)
	{
		size_t i = 0;
		for (; i < str.size(); i++)
		{
			if (str.at(i) == L'√')
			{
				return i;
			}
		}

		return SIZE_MAX;
	}

	//string to wstring
	inline std::wstring toWstring(const std::string& input)
	{
		std::wstring cur = {};
		std::string _cur = {};

		auto changeStr = [](const std::string& str)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			return converter.from_bytes(str);
		};

		for (size_t i = 0; i < input.size(); i++)
		{
			if (input.at(i) >= 0)
			{
				_cur += input.at(i);

				//std::cout << input.at(i) << std::endl;
			}
			else
			{
				cur += (changeStr(_cur) + L'√');
				i += 2;

				//std::wcout << cur << std::endl;
			}
		}

		return cur;
	}

	//wstring to string
	inline std::string toString(const std::wstring& input)
	{
		//wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.to_bytes(input);
	}
};

#endif

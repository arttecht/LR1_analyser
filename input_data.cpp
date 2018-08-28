#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "common.h"
#include "lr_grammar.h"

using namespace std;

void fillDataFromFile(ifstream &ifs, LrGrammar	*grammar)
{
	int cntLine = 0;
	while (!ifs.eof())
	{
		string inputStr, startWord;
		getline(ifs, inputStr);
		cntLine++;

		stringstream ss(inputStr);
		ss >> startWord;
		if (startWord.c_str()[0] == '#' || startWord.empty())
			continue;

		string tmp;
		LrGrammar::grRule element;
		element.left = startWord;
		ss >> tmp;
		if (tmp.find("->") == std::string::npos)
		{
			cout << "Неверный формат записи файла с грамматикой, строка: " << cntLine << endl;
		}

		bool isAction = true;
		while (ss >> tmp)
		{
			if (tmp != "|")
			{
				if (tmp[0] == '<' && tmp[tmp.size() - 1] == '>')
				{
					element.action.push_back(tmp);
					isAction = true;
				}
				else
				{
					if (isAction == false)
					{
						element.action.push_back(string(""));
					}
					element.right.push_back(tmp);
					isAction = false;
				}
			}
			else
			{
				if (isAction == false)
				{
					element.action.push_back(string(""));
				}

				if (!element.right.empty())
				{
					grammar->setRule(element);
					element.right.clear();
					element.action.clear();
					isAction = true;
				}
			}
		}
		if (isAction == false)
		{
			element.action.push_back(string(""));
		}
		grammar->setRule(element);
	}
}

void fillRawDataFromFile(ifstream &ifs, string &text)
{
	while (!ifs.eof())
	{
		string inputStr, startWord;
		getline(ifs, inputStr);

		if (inputStr.size() > 0)
		{
			text.append(inputStr + "\n");
		}
		else
		{
			text.append("\n");
		}
	}
	text += '$';
}
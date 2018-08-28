#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include "lr_det_automate.h"

using namespace std;

const vector<string> LrDetAutoStoreMem::reservedWords = { "void", "int", "char", "bool", "float", "double", "short", "long", "...", "namespace" };

LrDetAutoStoreMem::LrDetAutoStoreMem(shared_ptr<LrGrammar>& gr, shared_ptr<LrTable>& tb)
	: gram(gr), table(tb)
{
	implStore.clear();
}

LrDetAutoStoreMem::~LrDetAutoStoreMem()
{}

bool LrDetAutoStoreMem::isSymbolInSet(string& strSet, const string& inpStr)
{
	int size = strSet.size();
	if (strSet.size() > inpStr.size() && inpStr.size() == 1 )
	{
		char ch = inpStr[0];
		char setStart = strSet[0];
		char setEnd = strSet[strSet.size() - 1];

		if (strSet == "\\s" && (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'))
		{
			return true;
		}
		else if ((size > 1 && strSet[1] == '-') && setEnd >= setStart && ch >= setStart && ch <= setEnd)
		{
			return true;
		}
	}
	return false;
}

bool LrDetAutoStoreMem::isAllowSymbol(char ch)
{
	if (ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))
	{
		return true;
	}
	return false;
}

int LrDetAutoStoreMem::determReserveWord(string& str, int position)
{
	string formNewString(str, position);

	for (const auto &word : reservedWords)
	{
		if (formNewString.compare(0, word.size(), word) == 0)
		{
			if (position > 0 &&
				(isAllowSymbol(str[position - 1]) || isAllowSymbol(str[position + word.size()])))
			{
				volatile int a = 1;
				break;
			}
			return word.size();
		}
	}
	return 0;
}

int LrDetAutoStoreMem::getElementPositionInLine(int linePosition, const string& inputText, LrTable::tableCell &cell)
{
	for (unsigned k = 0; k < table->getLine(linePosition).size(); ++k)
	{
		LrTable::tableCell &tbCell = table->getLine(linePosition)[k];
		if (tbCell.lableElem == inputText || isSymbolInSet(tbCell.lableElem, inputText) == true)
		{
			cell = tbCell;
			return 0;
		}
	}
	return 1;
}

int LrDetAutoStoreMem::stringParsing(string& str)
{
	int linePos = 1; // Позиция строки
	int itemPos = 0; // Позиция курсора в строке
	string parseStr(str), tmp;
	LrTable::tableCell elStore{ 'S', 0 };
	store.push(elStore);

	int len, i = 0;
	if ((len = determReserveWord(parseStr, i)) > 0)
	{
		tmp = string(parseStr, i, len);
		i += len;
	}
	else
	{
		tmp = string(1, parseStr[i++]);
	}
	itemPos = i;
	if (tmp == "\n")
	{
		linePos++;
		itemPos = 0;
	}

	int linePositionInTable = 0;
	ParseErrorStatus status = PARSE_ERROR_TABLE;

	while (true)
	{
		if (getElementPositionInLine(linePositionInTable, tmp, elStore) != 0)
			break;
		if (implAction(elStore, tmp) != 0)
		{
			status = PARSE_ERROR_ACTION;
			break;
		}

		/* Осуществляем свёртку R -> P */
		if (elStore.stateSymb == 'R')
		{
			int numLine = elStore.numState;
			/* Подсчитываем кол-во элементов в правой части Pn правила грамматики и выталкиваем сохранённые элементы из стека
			столько же, сколько элементов в правой части правила. */
			for (const auto &k : gram->grammar[numLine].right)
			{
				store.pop();
			}
			/* Заменяем текущее состояние предыдущим, но с новым символом грамматики */
			elStore = store.top();
			elStore.lableElem = gram->grammar[numLine].left;
			linePositionInTable = elStore.numState;
			/* Находим в строке (elStore.numState) таблицы для символа elStore.lableElem новое состояние и сохраняем в стеке*/
			if (getElementPositionInLine(linePositionInTable, elStore.lableElem, elStore) != 0)
				break;
			if (implAction(elStore, elStore.lableElem) != 0)
			{
				status = PARSE_ERROR_ACTION;
				break;
			}
			store.push(elStore);
			linePositionInTable = elStore.numState;
		}
		else if (elStore.stateSymb == 'H')
		{
			status = PARSE_SUCCESS;
			break;
		}
		else
		{	/* elStore.stateSymb == 'S' */
			store.push(elStore);
			if ((len = determReserveWord(parseStr, i)) > 0)
			{
				tmp = string(parseStr, i, len);
				i += len;
				itemPos += len;
			}
			else
			{
				tmp = string(1, parseStr[i++]);
				itemPos++;
			}
			if (tmp == "\n")
			{
				itemPos = 0;
				linePos++;
			}
			linePositionInTable = elStore.numState;
		}
	}
	if (status == PARSE_SUCCESS)
	{
		cout << "Разбор выражения успешно завершён" << endl;
	}
	else if (status == PARSE_ERROR_TABLE)
	{
		cout << "Ошибка в " << linePos << " строке, " << itemPos << " позиции" << endl;
	}
	else if (status == PARSE_ERROR_ACTION)
	{
		cout << "Ошибка в " << linePos << " строке" << endl;
	}
	else
	{
		cout << "Неизвестная ошибка в " << linePos << " строке, " << endl;
	}
	return status;
}

void LrDetAutoStoreMem::printImplActionsData()
{
	for (auto &itSt : implStore)
	{
		for (auto &itVar : itSt.var)
		{
			cout << "Type_name=" << itVar.essence << " " << itVar.typeName << " " << itVar.varName << endl;
		}
		cout << "=====================" << endl;
	}
}

int LrDetAutoStoreMem::implAction(LrTable::tableCell &cell, const string &lex)
{
	static int level = 0;
	static string type; // тип данных идентификатора
	static string varName; // тип данных идентификатора

	for (auto &ch : cell.action)
	{
		if (ch == '<' || ch == '>')
		{
			/* Заменяем служебные символы '<' или '>' пробелами, чтоб иметь возможность считать 
			последовательность внедрённых действий (если их больше одного) последовательно используя
			std::stringstream поток */
			ch = ' ';
		}
	}
	/* Инициализируем поток строчкой из внедрённого(ых) действия(ий) */
	std::stringstream ss(cell.action);
	std::string	sa;
	static SemanticType st;

	/* Вычитываем последовательно внедрённые действия из потока, помещая их в строковую переменную */
	while (ss >> sa)
	{
//		cout << "2." << sa << " " << lex << " ";
//		cout << endl;
		if (sa == "TYPE" || sa == "NAMESPACE")
		{
			type = lex;
		}
		else if (sa == "ADDPARAM" || sa == "ADDFUNCNAME" || sa == "ADDNAMESPACE")
		{
			SemanticType::defVar dv;
			if (sa == "ADDFUNCNAME")		dv.essence = SemanticType::FUNC_NAME;
			else if (sa == "ADDPARAM")		dv.essence = SemanticType::ARG_NAME;
			else if (sa == "ADDNAMESPACE")	dv.essence = SemanticType::SPACE_NAME;
			else							dv.essence = SemanticType::NO_NAME;

			dv.typeName = type;
			dv.varName = varName;
			st.var.push_back(dv);
			type.clear();
			varName.clear();
		}
		else if (sa == "LEVEL+")
		{
			level++;
		}
		else if (sa == "LEVEL-")
		{
			level = (level > 0) ? (level--) : (0);
		}
		else if (sa == "LIT")
		{
			varName += lex;
		}
		else if (sa == "CHECKARGLIST")
		{
			vector<SemanticType::defVar> &v = st.var;
			/* Если список параметров функции более двух, тогда делаем их сравнение на наличие конфликта одинаковых имён */
			if (v.size() > 2)
			{
				for (unsigned i = 1; i < v.size(); ++i)
				{
					for (unsigned j = 2; j < v.size(); ++j)
					{
						if (i == j || v[i].essence != SemanticType::ARG_NAME || v[j].essence != SemanticType::ARG_NAME)	break;
						/* Если параметр явл. указателем или ссылкой, то устраняем данный символ из дальнейшей проверки */
						string var1((v[i].varName[0] == '&' || v[i].varName[0] == '*') ? (string(&v[i].varName[1], v[i].varName.size() - 1)) : (v[i].varName));
						string var2((v[j].varName[0] == '&' || v[j].varName[0] == '*') ? (string(&v[j].varName[1], v[j].varName.size() - 1)) : (v[j].varName));
						//cout << var1 << " " << var2 << endl;
						if (var1 == var2)
						{
							cout << "Конфликт имён параметров функции!\n";
							return -1;
						}
					}
				}
			}
		}
		else if (sa == "CHECKPROTO")
		{
			st.level = level;
			implStore.push_back(st);
			st.clear();

/*			for (unsigned i = 0; i < implStore.size(); ++i)
			{
				for (unsigned j = 1; j < implStore.size(); ++j)
				{
					if ((i == j) || (implStore[i].var[0].essence != implStore[j].var[0].essence))
					{
						break;
					}
					vector<SemanticType::defVar> &v = implStore[i].var;
					vector<SemanticType::defVar> &v2 = implStore[j].var;
				}
			}
*/		}
	}
	return 0;
}
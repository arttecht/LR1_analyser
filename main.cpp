#include <iostream>
#include <fstream>
#include <memory>
#include "common.h"
#include "lr_grammar.h"
#include "lr_graph.h"
#include "lr_table.h"
#include "lr_det_automate.h"

using namespace std;

ifstream	ifs("input.txt");
ifstream	ifsGram("grammar.txt");
ofstream	ofs("output.txt");
string		textForParsing;

auto grammar  = make_shared<LrGrammar>();
auto graph	  = make_shared<LrGraph>(grammar);
auto table	  = make_shared<LrTable>(grammar, graph);
auto automate = make_shared<LrDetAutoStoreMem>(grammar, table);

int main()
{
	setlocale(LC_ALL, "Russian");
	if (!ifsGram.is_open())
	{
		printf("\nFile: grammar.txt not found\n");
		return 1;
	}

	if (!ifs.is_open())
	{
		printf("\nFile: input.txt not found\n");
		return 1;
	}
	/*  Вычитываем грамматику и помещаем его в объект grammar */
	fillDataFromFile(ifsGram, grammar.get());
	grammar->print();

	/* Размещаем входные данные в объект std::string в конце важно не забыть поставить символ '$' */
	fillRawDataFromFile(ifs, textForParsing);
	//cout << textForParsing << endl;

	/* Строим граф состояний */
	graph->defMultipleStates();
	//graph->printGraphStates(ofs);

	/* СОздаём таблицу разбора */
	table->generateParseTable();
	//table->printParseTable(ofs);

	/* Согласно таблицы разбора производим проверку (парсинг) данных из входного файла */
	automate->stringParsing(textForParsing);
	//automate->printImplActionsData();

	std::cin.get();
	return 0;
}

#ifndef _LR_TABLE_H_
#define _LR_TABLE_H_

#include <string>
#include <vector>
#include <memory>
#include "lr_graph.h"

class LrGraph;

class LrTable
{
public:
	struct tableCell
	{
	/*	tableCell() : stateSymb('S'), numState(0)
		{}
		~tableCell() = default;

		tableCell& operator=(const tableCell& tc)
		{
			stateSymb = tc.stateSymb;
			numState = tc.numState;
			lableElem = tc.lableElem;
			return *this;
		}*/

		char		stateSymb;
		int			numState;
		std::string lableElem;
		std::string action;
	};

	LrTable(std::shared_ptr<LrGrammar>& gram, std::shared_ptr<LrGraph>& gr);
	~LrTable();

	void generateParseTable();
	void printParseTable(std::ofstream& ofs);
	std::vector<std::vector<tableCell>>& get();
	std::vector<tableCell>& getLine(int ind);
	LrTable::tableCell& LrTable::getElement(int numOfLine, int numOfColumn);
	size_t	size();

private:
	std::shared_ptr<LrGraph>			graph;
	std::shared_ptr<LrGrammar>			gram;
	std::vector<std::vector<tableCell>> table;
};


#endif
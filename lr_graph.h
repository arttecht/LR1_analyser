#ifndef _LR_GRAPH_H_
#define _LR_GRAPH_H_

#include <string>
#include <vector>
#include <memory>
#include "lr_grammar.h"
#include "lr_table.h"

class LrGraph;

class LrGraph : public GrammarSymbolControl
{
public:
	struct lrSituation
	{
		lrSituation() : curIndex(0), pointPosition(0), rule(0), tail("$")
		{}
		~lrSituation()
		{}
		lrSituation(const lrSituation& lrs) :
			left(lrs.left), right(lrs.right), tail(lrs.tail), curIndex(lrs.curIndex), pointPosition(lrs.pointPosition), rule(lrs.rule)
		{}
		lrSituation& operator=(const lrSituation& lrs)
		{
			left = lrs.left;
			right = lrs.right; 
			tail = lrs.tail;
			curIndex = lrs.curIndex;
			pointPosition = lrs.pointPosition;
			rule = lrs.rule;
			return *this;
		}
		bool operator==(const lrSituation& lrs)
		{
			return left == lrs.left && right == lrs.right && tail == lrs.tail && pointPosition == lrs.pointPosition;
		}
		bool operator!=(const lrSituation& lrs)
		{
			return left != lrs.left || right != lrs.right || tail != lrs.tail || pointPosition != lrs.pointPosition;
		}
		std::string left;
		std::vector<std::string> right;
		std::string tail;
		int curIndex;
		int pointPosition;
		int rule;
	};

	struct graphState
	{
		int			parentPosition;
		int			childPosition;
		std::string labelTransition;
	};

	explicit LrGraph(std::shared_ptr<LrGrammar>& gr);
	~LrGraph();

	void defMultipleStates();

	void first(const lrSituation& item, std::set<std::string>& setSymb);
	void closure(std::vector<lrSituation>& vec);

	void setOfStates(std::vector<lrSituation>& lrVec, std::set<std::string>& setStr);
	void printGraphStates(std::ofstream& ofs);
	void compareAndStore(std::vector<lrSituation>& vec, graphState& grs);

private:
	friend class LrTable;

	std::shared_ptr<LrGrammar>				grammar;
	std::vector<std::vector<lrSituation>>	listVecs;		//Вектор состояний графа S0, S1,...
	std::vector<graphState>					grStates;		//Вектор переходов (S0,S1,'+'), (S1,S3,'E') и т.п.
	std::set<std::string>					setElements;	//Множество элементов грамматики (терминалы и нетерминалы)
};

#endif
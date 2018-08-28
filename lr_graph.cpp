#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include "lr_grammar.h"
#include "lr_graph.h"

using namespace std;

LrGraph::LrGraph(std::shared_ptr<LrGrammar>& gr) : grammar(gr)
{}

LrGraph::~LrGraph()
{}

/* Определение набора (set) символов предшественников, записываем в  set<string>& setSymb*/
void LrGraph::first(const lrSituation& item, set<string>& setSymb)
{
	if (item.curIndex == item.right.size() - 1)
	{
		setSymb.insert(item.tail);
	}
	else
	{
		/* Если вдруг элемент правила оказался терминалом, то заносим его в множество */
		int size = item.right[item.curIndex + 1].size();
		/* Терминалы, заданные заглавными символами задаются диапазоном A-Z и определяются по символу "-" */
		if (isTerminal(item.right[item.curIndex + 1][0]) || (size > 1 && item.right[item.curIndex + 1][1] == '-'))
		{
			setSymb.insert(item.right[item.curIndex + 1]);
			return;
		}
		for (int i = 1; i < grammar->size(); ++i)
		{
			LrGrammar::grRule &gr = (*grammar)[i];
			if (item.right[item.curIndex + 1] == gr.left)
			{
				/* Если S(ba) терминал, то добавляем его, иначе спускаемся по грамматике и пытаемся найти 
				тразитивное замыкание для нетерминала пока не упрёмся в терминал (либо вообще останемся ни с чем) */
				int size2 = gr.right[0].size();
				/* Терминалы, заданные заглавными символами задаются диапазоном A-Z и определяются по символу "-" */
				if (isTerminal(gr.right[0][0]) || (size2 > 1 && gr.right[0][1] == '-'))
				{
					setSymb.insert(gr.right[0]);
				}
				else
				{
					for (int j = 1; j < grammar->size(); ++j)
					{
						LrGrammar::grRule &gr2 = (*grammar)[j];
						if (gr.right[0] == gr2.left)
						{
							size2 = gr2.right[0].size();
							/* Терминалы, заданные заглавными символами задаются диапазоном A-Z и определяются по символу "-" */
							if (isTerminal(gr2.right[0][0]) || (size2 > 1 && gr2.right[0][1] == '-'))
							{
								setSymb.insert(gr2.right[0]);
							}
						}
					}// for (...)
				}
			}
		}// for (int i = 1;...)
		if (setSymb.empty())
		{
			setSymb.insert(item.tail);
		}
	}
}

/** Вычисление замыкания для LR ситуаций. Входной вектор lrVec должен содержать минимум 
  * одну LR ситуацию с которой начинается разбор правила. */
void LrGraph::closure(vector<lrSituation>& lrVec)
{
	for (auto i = 0; i < lrVec.size(); ++i)
	{
		auto lrSit = lrVec[i];

		set<string> symbPrev;
		first(lrSit, symbPrev);

		for (auto &symbItem : symbPrev)
		{
			for (int j = 0 /*1*/; j < grammar->size(); ++j)
			{
				LrGrammar::grRule &gr = (*grammar)[j];
				if (lrSit.pointPosition <= lrSit.curIndex)
				{
					if (lrSit.right[lrSit.curIndex] == gr.left)
					{
						lrSituation sit;
						sit.left = gr.left;
						sit.right = gr.right;
						sit.tail = symbItem;
						sit.rule = j;

						bool isNewSituation = false;
						for (int k = 0; k < lrVec.size(); ++k)
						{
							if (lrVec[k] == sit)
							{
								isNewSituation = true;
								break;
							}
						}
						if (isNewSituation == false)
						{
							lrVec.push_back(sit);
						}
					}
				}
			}
		}//for (auto &symbItem ...)
	}
}

/** Определение множества состояний, определяемых размещением "точки" перед определённым 
 *  состоянием и помещение их в std::set объект */
void LrGraph::setOfStates(vector<lrSituation>& lrVec, set<string>& setStr)
{
	for (auto &elem : lrVec)
	{
		if (elem.right[elem.curIndex].size() > 0)
		{
			setStr.insert(elem.right[elem.curIndex]);
			setElements.insert(elem.right[elem.curIndex]);
		}
	}
	setElements.insert("$");
}

/* Определение множества состояний */
void LrGraph::defMultipleStates()
{
	/* 1. Вычисляем состояние S0 */
	vector<lrSituation> lrVec;
	lrSituation sit;
	sit.left = (*grammar)[0].left;
	sit.right = (*grammar)[0].right;
	lrVec.push_back(sit);
	
	closure(lrVec);
	listVecs.push_back(lrVec);

	int childPosition = 1;

	/* 2. Определяем множество состояний в которые явл. переходом из состояния S[i] */
	for (unsigned i = 0; i < listVecs.size(); ++i)
	{
		set<string> lrSet;
		/* Из текущего состояния определяем набор из правил в правой части перед которыми стоит точка и записываем в lrSet.
		   Применяем структуру set, чтоб не проверять программно на наличие уже существующего элемента. */
		setOfStates(listVecs[i], lrSet);
		/* Инициализируем состояние перехода (Sx, Sy) по символу "xxx" (ещё не определён), кот. определяется в процессе работы алгоритма */
		graphState grs = { i, 0, "" };
		/* Проходим по всем словам/символам из множества */
		for (auto &symb : lrSet)
		{
			grs.labelTransition = symb;
			vector<lrSituation> subVec;
			for (auto &elem : listVecs[i])
			{
				if (elem.right[elem.curIndex] == symb)
				{
					lrSituation subSit = elem;
					/* Определяем новую позицию точки и соответственно элемента в правой части.
					   Позиция точки и элемента не совпадают на последнем элементе. */
					if (subSit.pointPosition == elem.right.size())	continue;
					else
					{
						subSit.pointPosition++;
						if (subSit.curIndex < elem.right.size() - 1)
						{
							subSit.curIndex++;
						}
						subVec.push_back(subSit);
					}
				}
			}
			if (!subVec.empty())
			{
				closure(subVec);
				compareAndStore(subVec, grs);
				grStates.push_back(grs);
				//std::cout << "(" << grs.parentPosition << ", " << grs.childPosition << ", '" << grs.labelTransition << "'), ";
			}
		}
		//std::cout << endl;
	}
}

/* Сравниваем совпадающие состояния и записываем, если они уникальны */
void LrGraph::compareAndStore(vector<lrSituation>& vec, graphState& grs)
{
	for (auto i = 1; i < listVecs.size(); ++i)
	{
		bool isExcessElem = true;
		for (auto j = 0; j < vec.size(); ++j)
		{
			if (vec[j] != listVecs[i][j] || vec.size() != listVecs[i].size())
			{
				isExcessElem = false;
				break;
			}
		}
		if (isExcessElem == true)
		{
			grs.childPosition = i;
			return;
		}
	}
	grs.childPosition = listVecs.size();
	listVecs.push_back(vec);
}

void LrGraph::printGraphStates(ofstream& ofs)
{
	int cntState = 0;
	for (auto &state : listVecs)
	{
		cout << "\n====  State: " << cntState << " ====" << endl;
		ofs  << "\n====  State: " << cntState++   << " ====" << endl;
		for (auto &elem : state)
		{
			string tmp;
			for (int i = 0; i < elem.right.size(); ++i)
			{
				if (i == elem.pointPosition)
				{
					tmp.append(" . ");
				}
				tmp.append(elem.right[i]);
				if (i != elem.right.size() - 1)
				{
					tmp.append(" ");
				}
			}
			if (elem.pointPosition == elem.right.size())
			{
				tmp.append(".");
			}
			cout << elem.left << " -> " << tmp << "|" << elem.tail << endl;
			ofs << elem.left << " -> " << tmp << "|" << elem.tail << endl;
		}
	}
#ifdef DEBUG
	cout << "\nX = (" ;
	for (auto elem : setElements)
	{
		cout << elem << ", ";
	}
	cout << ")"<< endl;
#endif
}
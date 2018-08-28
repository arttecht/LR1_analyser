#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include "lr_graph.h"
#include "lr_table.h"

using namespace std;

LrTable::LrTable(shared_ptr<LrGrammar>& gram, shared_ptr<LrGraph>& gr) : gram(gram), graph(gr)
{}

LrTable::~LrTable()
{}

void LrTable::generateParseTable()
{
	int mState = 0;
	bool isError = false;
	/* ������ ������ ������� */
	for (auto &state : graph->listVecs)
	{
		vector<tableCell> vec;
		for (auto &elem : graph->setElements)
		{
			tableCell t = { ' ', -1 };
			vec.push_back(t);
		}
		table.push_back(vec);
	}

	/* �������� �� ������� ��������� ����� S0, S1,... */
	for (unsigned i = 0; i < graph->listVecs.size(); ++i)
	{
		/*  ��� ��������������� ������ ���������, ����� ��� ���������� (���� ��� � �� �����) */
		mState = i;
		/* ������������� ������ ������� ��������� ����������-������������ + $ (����� �������) */
		unsigned j = 0;
		for (auto &elem : graph->setElements)
		{
			/* ���������� ������ �� �������������� ������ ������� ��� ����������*/
			auto &t = table[i][j];
			/* ��������� � ������� ��������� R1-Rn (������). ������������� ������ ���������
			�����������, �.�. ��������������� ������ ������� �� ������� ������� ������ ��������� (�������� S1) */
			for (auto &situation : graph->listVecs[i])
			{
				if (situation.pointPosition == situation.right.size() && situation.tail == elem)
				{
					/* ������������ ������� ������������ ���������� Pn � ���������� ���������� ���������
					� ��������� �������� ��������� Rn. H0 ������ �� HALT */
					bool isHalt = (situation.right == gram->grammar[0].right) ? ( true ) : ( false );
					if (t.numState != -1 && (t.stateSymb == 'R' || t.stateSymb == 'H'))
					{
						cout << "\n 1 ������ � ����������!  ���������: S" << mState << ", �������: " << t.lableElem
							<< ", ��������: " << t.stateSymb << t.numState << "/" << (isHalt ? 'H' : 'R') << situation.rule << "\n";
						isError = true;
					}
					t = { (isHalt ? 'H' : 'R'), situation.rule, elem };
					//t.action = gram->grammar[situation.rule].action[situation.right.size() - 1];
				}

				if (situation.pointPosition < situation.right.size())
				{
					int edge = 0;
					/* ������������ ������ �� ������������ ���������� � ���� ����������� ��������� 'situation' � ��������������� �������.
					   ������ ����� ��� ��������� ��� ���������� ���� ������ 'action' � ������� ���������. */
					for (auto &g : gram->grammar)
					{
						if (g.left == situation.left && g.right == situation.right && elem == g.right[situation.curIndex])
						{
							for (auto &gs : graph->grStates)
							{
								if (gs.parentPosition == mState && elem == gs.labelTransition)
								{
									if (t.numState != -1 && t.stateSymb == 'S' && t.numState != gs.childPosition) // ���� ������ ��� �������� �����, �� ��� ��������
									{
										cout << "\n 2 ������ � ����������!  ���������: S" << mState << ", �������: " << t.lableElem
											<< ", ��������: S" << t.numState << "/S" << gs.childPosition << "\n";
										isError = true;
									}
									t = { 'S', gs.childPosition, elem };
									t.action = gram->grammar[situation.rule].action[situation.curIndex];
									//cout << t.stateSymb << t.numState << " " << t.action << " ==>> " << gram->grammar[situation.rule].right[0] << " sit_rule=" << situation.rule << endl;
									break;
								}
							}
						}
					}
				} // if (situation.pointPosition...)
			}
			j++;
		}
	}
	if (isError)
	{
		std::cin.get();
		exit(1);
	}
}

vector<vector<LrTable::tableCell>>& LrTable::get()
{
	return table;
}

vector<LrTable::tableCell>& LrTable::getLine(int ind)
{
	return table[ind];
}

LrTable::tableCell& LrTable::getElement(int numOfLine, int numOfColumn)
{
	return table[numOfLine][numOfColumn];
}

size_t LrTable::size()
{
	return table.size();
}

void LrTable::printParseTable(ofstream& ofs)
{
	const int indentWidth = 5;
	const int tableElemWidth = 8;//4;
	vector<int> tbWidth(graph->setElements.size(), tableElemWidth);

	int lineSize = 0;
	int cnt = 0;
	for (auto &state : graph->setElements)
	{
		if (state.size() > tableElemWidth)
		{
			tbWidth[cnt] = state.size();
			lineSize += tbWidth[cnt] + 1;
		}
		else
		{
			lineSize += tableElemWidth + 1;
		}
		cnt++;
	}
	string line(lineSize + indentWidth, '-');

	cout << "\n" << line << endl;
	ofs  << "\n" << line << endl;
	cout << setw(indentWidth) << "|";
	ofs << setw(indentWidth) << "|";

	cnt = 0;
	for (auto &state : graph->setElements)
	{
		cout << setw(tbWidth[cnt]) << std::right << state << "|";
		ofs << setw(tbWidth[cnt]) << std::right << state << "|";
		cnt++;
	}
	cout << endl << line << endl;
	ofs  << endl << line << endl;

	int mState = 0;
	for (auto &line : table)
	{
		string tmp("S");
		tmp += to_string(mState);
		tmp += "|";
		cout << setw(indentWidth) << std::right << tmp;
		ofs << setw(indentWidth) << std::right << tmp;

		cnt = 0;
		for (auto &state : line)
		{
			if (state.numState != -1)
			{
				string tmp;
				tmp += state.stateSymb;
				tmp += to_string(state.numState);
				tmp += " " + state.action;
				cout << setw(tbWidth[cnt]) << std::right << tmp << "|";
				ofs << setw(tbWidth[cnt]) << std::right << tmp << "|";
			}
			else
			{
				cout << setfill(' ') << setw(tbWidth[cnt]) << std::right << " " << "|";
				ofs << setfill(' ') << setw(tbWidth[cnt]) << std::right << " " << "|";
			}
			cnt++;
		}
		cout << endl;
		ofs  << endl;
		mState++;
	}
}
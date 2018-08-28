#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include "common.h"
#include "lr_grammar.h"

using namespace std;

//std::string  GrammarSymbolControl::terminals = { "abcdefghijklmnopqrstuvwxyz1234567890_.,;+-*(){}[]:=_\\ " };
std::string  GrammarSymbolControl::terminals = { "abcdefghijklmnopqrstuvwxyz_.,;+-*(){}[]:=_\\& " };
std::string  GrammarSymbolControl::nonTerminals = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };

GrammarSymbolControl::GrammarSymbolControl()
{
	term.insert(terminals.begin(), terminals.end());
	nonTerm.insert(nonTerminals.begin(), nonTerminals.end());
}

GrammarSymbolControl::~GrammarSymbolControl()
{}

bool GrammarSymbolControl::isTerminal(char ch)
{
	auto it = term.find(ch);
	if (it != term.end())
	{
		return true;
	}
	return false;
}
bool GrammarSymbolControl::isNonTerminal(char ch)
{
	auto it = nonTerm.find(ch);
	if (it != nonTerm.end())
	{
		return true;
	}
	return false;
}

LrGrammar::LrGrammar()
{}

LrGrammar::~LrGrammar()
{}

bool isGetTerminal(char ch)
{
	return true;
}

void LrGrammar::print() const
{
	for (int i = 0; i < grammar.size(); ++i)
	{
		cout << grammar[i].left << " -> ";
		for (int k = 0; k < grammar[i].right.size(); ++k)
		{
			cout << grammar[i].right[k] << " ";
		}
		cout << endl;
	}
}

void LrGrammar::setRule(grRule& rule)
{
	grammar.push_back(rule);
}

LrGrammar::grRule& LrGrammar::getRule(int pos)
{
	int position = pos;
	if (pos >= grammar.size())
	{
		cout << "ƒанна€ позици€ превышает размер массива!\n";
		position = grammar.size() - 1;
	}
	return grammar[position];
}

LrGrammar::grRule& LrGrammar::operator[](int pos)
{
	return getRule(pos);
}

size_t	LrGrammar::size() const
{
	return grammar.size();
}
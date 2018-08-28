#ifndef _LR_GRAMMAR_H_
#define _LR_GRAMMAR_H_

#include <string>
#include <vector>
#include <set>

class GrammarSymbolControl
{
public:
	GrammarSymbolControl();
	~GrammarSymbolControl();

	bool isTerminal(char ch);
	bool isNonTerminal(char ch);
protected:
	std::set<char> term;
	std::set<char> nonTerm;

	static std::string  terminals;
	static std::string  nonTerminals;
};

class LrTable;
class LrDetAutoStoreMem;

class LrGrammar : public GrammarSymbolControl
{
public:
	struct grRule
	{
		std::string left;
		std::vector<std::string> right;
		std::vector<std::string> action;
	};

	LrGrammar();
	~LrGrammar();

	void setRule(grRule& rule);
	grRule& getRule(int pos);
	grRule& operator[](int pos);
	size_t	size() const;
	void print() const;

	bool isGetTerminal(char ch);

private:
	friend class LrTable;
	friend class LrDetAutoStoreMem;

	std::vector<grRule>	grammar;
};

#endif
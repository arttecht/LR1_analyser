#ifndef _LR_DET_AUTOMATE_H_
#define _LR_DET_AUTOMATE_H_

#include <string>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <memory>
#include "lr_grammar.h"
#include "lr_table.h"

struct SemanticType
{
	SemanticType() : level(0) {}
	~SemanticType(){}
	void clear()
	{
		level = 0;
		var.clear();
	}
	enum secName
	{
		FUNC_NAME,
		ARG_NAME,
		SPACE_NAME,
		NO_NAME
	};
	struct defVar
	{
		defVar() : essence(NO_NAME) {}
		std::string typeName;
		std::string varName;
		secName		essence;
	};
	std::vector<defVar> var;
	int	level;
};

class LrDetAutoStoreMem
{
public:
	enum ParseErrorStatus
	{
		PARSE_SUCCESS,
		PARSE_ERROR_TABLE,
		PARSE_ERROR_ACTION
	};

	LrDetAutoStoreMem(std::shared_ptr<LrGrammar>& gr, std::shared_ptr<LrTable>& tb);
	~LrDetAutoStoreMem();

	bool isAllowSymbol(char ch);
	bool isSymbolInSet(std::string& strSet, const std::string& inpStr);

	int determReserveWord(std::string& str, int position);
	int stringParsing(std::string& str);
	int getElementPositionInLine(int linePosition, const std::string& inputText, LrTable::tableCell &cell );
	int implAction(LrTable::tableCell &cell, const std::string &lex);
	void printImplActionsData();

private:
	std::shared_ptr<LrGrammar>	gram;
	std::shared_ptr<LrTable>	table;

	std::stack<LrTable::tableCell>	store;
	std::vector<SemanticType>		implStore;

	static const std::vector<std::string>	reservedWords;
	//static std::vector<char>		allowIdSymbols;
};


#endif
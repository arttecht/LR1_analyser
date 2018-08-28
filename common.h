#ifndef _COMMON_H_
#define _COMMON_H_

#include <fstream>
#include "lr_grammar.h"

void fillDataFromFile(std::ifstream &ifs, LrGrammar	*grammar);
void fillRawDataFromFile(std::ifstream &ifs, std::string &text);

#endif
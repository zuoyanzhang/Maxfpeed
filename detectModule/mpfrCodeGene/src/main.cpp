//
// Created by 99065 on 2023/10/20.
//
#include "mpfrCodeGene/include/basic.h"
#include "mpfrCodeGene/include/color.h"
#include "mpfrCodeGene/include/laxerAST.h"
#include "mpfrCodeGene/include/parserAST.h"
#include "mpfrCodeGene/include/geneCode.h"
#include <fstream>
#include <chrono>
#include <iomanip>

using std::string;
using std::cin;
using std::cout;
using std::vector;
using std::endl;
using std::regex;

int main(int argc, char *argv[]) {
    installOperatorsForStr();
    string inputStr = "";
    inputStr = string(argv[1]);
    auto originExpr = ParseExpressionFromString(inputStr);
    vector<string> vars;
    getVariablesFromExpr(originExpr, vars);

    if (containOnlySimpleX(inputStr)) {
        auto funcNameMpfr = geneMpfrCode(inputStr, vars);
        auto funcNameMpfr2 = geneMultiMpfrCode(inputStr, vars);
    } else {
        auto funcNameMpfr = geneMultiMpfrCode(inputStr, vars);
    }

    return 0;
}
//
// Created by 99065 on 2023/10/19.
//

#ifndef MAXFPEED_GENECODE_H
#define MAXFPEED_GENECODE_H

#endif //MAXFPEED_GENECODE_H
#ifndef _GENECODE
#define _GENECODE

#include "mpfrCodeGene/include/basic.h"

#include<stdio.h>
#include <fstream>
#include<iostream>
#include<vector>
using namespace std;
using std::cout;
using std::cin;
using std::vector;
using std::endl;
using std::string;

bool getVariablesFromExpr(const ast_ptr &expr, vector<string> &vars);

int geneMpfrCode(const string exprStr, vector<string> vars);

int geneMultiMpfrCode(const string exprStr, vector<string> vars);

#endif
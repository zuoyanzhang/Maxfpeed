//
// Created by 99065 on 2023/10/19.
//

#ifndef MAXFPEED_PARSERAST_H
#define MAXFPEED_PARSERAST_H

#endif //MAXFPEED_PARSERAST_H
#ifndef _PARSERAST
#define _PARSERAST

#include "mpfrCodeGene/include/basic.h"

#include <fstream>
#include <sstream>
#include <iostream>

using std::string;

//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//

ast_ptr ParseExpressionForStr();

/// numberexpr ::= number
ast_ptr ParseNumberExprForStr();

/// parenexpr ::= '(' expression ')'
ast_ptr ParseParenExprForStr();

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'
ast_ptr ParseIdentifierExprForStr();

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
ast_ptr ParsePrimaryForStr();

/// binoprhs
///   ::= ('+' primary)*
ast_ptr ParseBinOpRHSForStr(int ExprPrec, ast_ptr LHS);

/// expression
///   ::= primary binoprhs
///
ast_ptr ParseExpressionForStr();

string readFileIntoString(const char * filename);

ast_ptr ParseExpressionFromString();
ast_ptr ParseExpressionFromString(string str);

#endif
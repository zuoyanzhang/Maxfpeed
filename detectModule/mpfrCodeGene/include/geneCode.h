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

/**
 * This function extracts the variables used in a given mathematical expression.
 *
 * @param expr The mathematical expression represented as an abstract syntax tree (AST).
 * @param vars A reference to a vector of strings where the extracted variable names will be stored.
 * @return A boolean value indicating whether the extraction was successful.
 *
 * The function traverses the AST of the given expression and collects the variable names.
 * The collected variable names are stored in the provided vector 'vars'.
 *
 * Note: This function assumes that the AST 'expr' is properly constructed and valid.
 */
bool getVariablesFromExpr(const ast_ptr &expr, vector<string> &vars);

/**
 * This function generates MPFR (Multiple Precision Floating-Point Reliable) code for a given mathematical expression.
 *
 * @param exprStr The mathematical expression represented as a string.
 * @param vars A vector of strings containing the variables used in the expression.
 * @return An integer value representing the success of the code generation process.
 *         A return value of 0 indicates successful code generation.
 *         A non-zero return value indicates an error during code generation.
 *
 * The function takes a mathematical expression represented as a string and a vector of variables.
 * It generates MPFR code for the given expression and stores it in a file.
 * The generated code includes the necessary includes, function prototypes, and the main function.
 * The main function initializes the variables, calls the generated MPFR code, and prints the result.
 *
 * Note: This function assumes that the input expression is valid and well-formed.
 *       It does not perform any error checking or validation on the input expression.
 *       It is the responsibility of the caller to ensure the input expression is valid.
 */
int geneMpfrCode(const string exprStr, vector<string> vars);

/**
 * This function generates MPFR (Multiple Precision Floating-Point Reliable) code for a given mathematical expression.
 * It also handles multiple expressions and generates a single file with multiple functions.
 *
 * @param exprStr The mathematical expression represented as a string.
 * @param vars A vector of strings containing the variables used in the expression.
 * @return An integer value representing the success of the code generation process.
 *         A return value of 0 indicates successful code generation.
 *         A non-zero return value indicates an error during code generation.
 *
 * The function takes a mathematical expression represented as a string and a vector of variables.
 * It generates MPFR code for the given expression and stores it in a file.
 * The generated code includes the necessary includes, function prototypes, and the main function.
 * The main function initializes the variables, calls the generated MPFR code, and prints the result.
 *
 * Note: This function assumes that the input expression is valid and well-formed.
 *       It does not perform any error checking or validation on the input expression.
 *       It is the responsibility of the caller to ensure the input expression is valid.
 *       This function is designed to handle multiple expressions and generate a single file with multiple functions.
 */
int geneMultiMpfrCode(const string exprStr, vector<string> vars);

#endif
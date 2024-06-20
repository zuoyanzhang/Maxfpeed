//
// Created by 99065 on 2023/10/19.
//
#include "mpfrCodeGene/include/basic.h"

#include <sstream>
#include <iomanip>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::to_string;
using std::ofstream;
using std::ios;
using std::regex;
//===----------------------------------------------------------------------===//
// basic operation
//===----------------------------------------------------------------------===//
/**
 * @brief check if two abstract syntax tree(AST) expressions are equal
 * 
 * @param expr1 the first AST expression
 * @param expr2 the second AST expression
 * @return true 
 * @return false 
 */
bool isEqual(const ast_ptr &expr1, const ast_ptr &expr2)
{
    if(expr1 == nullptr || expr2 == nullptr)
    {
        fprintf(stderr, "empty\n");
        return false;
    }

    const string expr1Type = expr1->type();
    const string expr2Type = expr2->type();

    if(expr1Type == expr2Type)
    {
        if(expr1->type() == "Number")
        {
            NumberExprAST *numberExpr1 = dynamic_cast<NumberExprAST *>(expr1.get());
            double number1 = (numberExpr1->getNumber());
            NumberExprAST *numberExpr2 = dynamic_cast<NumberExprAST *>(expr2.get());
            double number2 = (numberExpr2->getNumber());

            return (number1 == number2);
        }
        else if(expr1->type() == "Variable")
        {
            VariableExprAST *variableExpr1 = dynamic_cast<VariableExprAST *>(expr1.get());
            string variable1 = (variableExpr1->getVariable());
            VariableExprAST *variableExpr2 = dynamic_cast<VariableExprAST *>(expr2.get());
            string variable2 = (variableExpr2->getVariable());

            return (variable1 == variable2);
        }
        else if(expr1->type() == "Call")
        {
            CallExprAST *callExpr1 = dynamic_cast<CallExprAST *>(expr1.get());
            string callee1 = (callExpr1->getCallee());
            CallExprAST *callExpr2 = dynamic_cast<CallExprAST *>(expr2.get());
            string callee2 = (callExpr2->getCallee());

            if(callee1 == callee2)
            {
                vector<ast_ptr> &args1 = callExpr1->getArgs();
                vector<ast_ptr> &args2 = callExpr2->getArgs();

                if(args1.size() == args2.size())
                {
                    for(long unsigned int i = 0; i < args1.size(); ++i)
                    {
                        if(!isEqual(args1.at(i), args2.at(i)))
                        {
                            return false;
                        }
                    }
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else if(expr1Type == "Binary")
        {
            BinaryExprAST *binOp1 = dynamic_cast<BinaryExprAST *>(expr1.get());
            char op1 = binOp1->getOp();
            string opStr1(1, op1);
            BinaryExprAST *binOp2 = dynamic_cast<BinaryExprAST *>(expr2.get());
            char op2 = binOp2->getOp();
            string opStr2(1, op2);
            if(opStr1 == opStr2)
            {
                ast_ptr &lhs1 = binOp1->getLHS();
                ast_ptr &rhs1 = binOp1->getRHS();
                ast_ptr &lhs2 = binOp2->getLHS();
                ast_ptr &rhs2 = binOp2->getRHS();

                return (isEqual(lhs1, lhs2) && isEqual(rhs1, rhs2));
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;  // TODO: handle the other cases
        }
    }
    else
    {
        return false;
    }
}

/**
 * @brief Checks if the given abstract syntax tree (AST) expression represents a fraction.
 * This function determines if the provided AST expression is a binary operation
 * with the division operator ('/'). If the expression is a fraction, it returns true.
 * @param expr The AST expression to check.
 * @return true 
 * @return false 
 */
bool isFraction(const ast_ptr &expr)
{
    if (expr == nullptr)
    {
        fprintf(stderr, "\tERROR: isFraction: the input expr is nullptr!\n");
        return false;
    }
    if (expr->type() == "Binary")
    {
        BinaryExprAST *binOpPtr = dynamic_cast<BinaryExprAST *>(expr.get());
        char op = binOpPtr->getOp();
        if (op == '/')
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Checks if the given vector of abstract syntax tree (AST) expressions contains a single constant number.
 * 
 * This function determines if the provided vector of AST expressions contains exactly one element,
 * and if that element is of type "Number".
 * 
 * @param exprs A vector of AST expressions to check.
 * @return true if the vector contains exactly one element and that element is a number.
 * @return false otherwise.
 */
bool isConstant(const vector<ast_ptr> &exprs)
{
    size_t size = exprs.size();
    if (size != 1)
    {
        return false;
    }
    const ast_ptr &expr = exprs.at(0);
    if(expr->type() == "Number")
    {
        return true;
    }
    return false;
}

/**
 * @brief Extracts the numerator from a binary expression representing a fraction.
 * 
 * This function checks if the given abstract syntax tree (AST) expression is a binary operation
 * with the division operator ('/'). If it is, the function returns the left-hand side (numerator)
 * of the division. If the expression is not a fraction, it returns a clone of the original expression.
 * 
 * @param expr The AST expression to extract the numerator from.
 * @return ast_ptr The numerator of the fraction if the expression is a fraction, otherwise a clone of the original expression.
 */
ast_ptr getNumerator(const ast_ptr &expr)
{
    if (expr == nullptr)
    {
        fprintf(stderr, "\tERROR: getNumerator: the input expr is nullptr!\n");
        return nullptr;
    }
    if (expr->type() == "Binary")
    {
        BinaryExprAST *binOpPtr = dynamic_cast<BinaryExprAST *>(expr.get());
        char op = binOpPtr->getOp();
        if (op == '/')
        {
            return std::move(binOpPtr->getLHS());
        }
    }
    fprintf(stderr, "\tWARNING: getNumerator: the input expr is not match for fraction\n");
    return expr->Clone();
}

/**
 * @brief Extracts the denominator from a binary expression representing a fraction.
 * 
 * This function checks if the given abstract syntax tree (AST) expression is a binary operation
 * with the division operator ('/'). If it is, the function returns the right-hand side (denominator)
 * of the division. If the expression is not a fraction, it returns a clone of the original expression.
 * 
 * @param expr The AST expression to extract the denominator from.
 * @return ast_ptr The denominator of the fraction if the expression is a fraction, otherwise a clone of the original expression.
 */
ast_ptr getDenominator(const ast_ptr &expr)
{
    if (expr == nullptr)
    {
        fprintf(stderr, "\tERROR: getDenominator: the input expr is nullptr!\n");
        return nullptr;
    }
    if (expr->type() == "Binary")
    {
        BinaryExprAST *binOpPtr = dynamic_cast<BinaryExprAST *>(expr.get());
        char op = binOpPtr->getOp();
        if (op == '/')
        {
            return std::move(binOpPtr->getRHS());
        }
    }
    fprintf(stderr, "\tWARNING: getDenominator: the input expr is not match for fraction\n");
    return expr->Clone();
}

/**
 * @brief Creates a binary expression AST node from two given expressions and an operator.
 * 
 * This function takes two abstract syntax tree (AST) expressions and a binary operator,
 * and creates a new binary expression AST node that represents the operation.
 * If either of the input expressions is nullptr, it returns a clone of the other expression.
 * If both input expressions are nullptr, it returns nullptr and logs a warning.
 * 
 * @param expr1 The left-hand side expression of the binary operation.
 * @param expr2 The right-hand side expression of the binary operation.
 * @param op The binary operator to be applied to the expressions.
 * @return ast_ptr A pointer to the newly created binary expression AST node.
 */
ast_ptr createBinaryExpr(const ast_ptr &expr1, const ast_ptr &expr2, const char op)
{
    if (expr1 == nullptr && expr2 == nullptr)
    {
        fprintf(stderr, "\tWARNING: createBinaryExpr: all the inputs are nullptr!\n");
        return nullptr;
    }
    if (expr1 == nullptr)
    {
        return expr2->Clone();
    }
    if (expr2 == nullptr)
    {
        return expr1->Clone();
    }
    auto lhs = expr1->Clone();
    auto rhs = expr2->Clone();
    return makePtr<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
}

ast_ptr addExpr(const ast_ptr &expr1, const ast_ptr &expr2) { return createBinaryExpr(expr1, expr2, '+'); }

ast_ptr mulExpr(const ast_ptr &expr1, const ast_ptr &expr2) { return createBinaryExpr(expr1, expr2, '*'); }

ast_ptr divExpr(const ast_ptr &expr1, const ast_ptr &expr2) { return createBinaryExpr(expr1, expr2, '/'); }

void mineAppend(vector<ast_ptr> &dest, vector<ast_ptr> &origin)
{
    dest.insert(dest.end(), std::make_move_iterator(origin.begin()), std::make_move_iterator(origin.end()));
}

//===----------------------------------------------------------------------===//
// print information
//===----------------------------------------------------------------------===//
/**
 * @brief Converts an abstract syntax tree (AST) expression to its string representation.
 * 
 * This function takes an AST expression and converts it to a string representation
 * with a specified precision for numerical values. It handles different types of expressions
 * such as numbers, variables, function calls, and binary operations.
 * 
 * @param expr The AST expression to convert to a string.
 * @param PRECISION The precision to use for numerical values.
 * @return string The string representation of the AST expression.
 */
string PrintExpression(const ast_ptr &expr, const size_t PRECISION)
{
    if(expr == nullptr)
    {
        fprintf(stderr, "this is a nullptr.\n");
    }
// const string exprType = expr->type();
#ifdef DEBUG
    fprintf(stderr, "expr type: %s;\t", exprType.c_str());
#endif
    string exprStr = "";
    if(expr->type() == "Number")
    {
        NumberExprAST *numberExpr = dynamic_cast<NumberExprAST *>(expr.get());
        // std::unique_ptr<NumberExprAST> numberExpr;
        // if (tmp != nullptr)
        // {
        //     expr.release();
        //     numberExpr.reset(tmp);
        // }
        double number = (numberExpr->getNumber());
#ifdef DEBUG
        fprintf(stderr, "number: %f\n", number);
#endif
        std::stringstream ss;
        ss << std::setprecision(PRECISION) << number;
        return ss.str(); // better than to_string: 1 outputs as 1, 1.23456789 outputs as 1.23456789
        // return std::to_string(number); // really not a good choice: 1 outputs as 1.000000, 1.23456789 outputs as 1.234567
    }
    else if(expr->type() == "Variable")
    {
        VariableExprAST *variableExpr = dynamic_cast<VariableExprAST *>(expr.get());
        string variable = (variableExpr->getVariable());
#ifdef DEBUG
        fprintf(stderr, "variable: %s\n", variable.c_str());
#endif

        return variable;
    }
    else if(expr->type() == "Call")
    {
        CallExprAST *callExpr = dynamic_cast<CallExprAST *>(expr.get());
        string callee = (callExpr->getCallee());
#ifdef DEBUG
        fprintf(stderr, "call: %s\n", callee.c_str());
#endif
        vector<ast_ptr> &args = callExpr->getArgs();

        vector<string> argsStr;
        for(long unsigned int i = 0; i < args.size(); ++i)
        {
            string strTmp = PrintExpression(args.at(i), PRECISION);  // ast_ptr& exprTmp = args.at(i);
            argsStr.push_back(strTmp);
        }
        callee += "(";
        for(long unsigned int i = 0; i < argsStr.size() - 1; ++i)
        {
            callee += argsStr.at(i) + ", ";
        }
        callee += argsStr.back() + ")";
        return callee;
    }
    else if(expr->type() == "Binary")
    {
        BinaryExprAST *binOp = dynamic_cast<BinaryExprAST *>(expr.get());
        // std::unique_ptr<BinaryExprAST> binOp = makePtr<BinaryExprAST>(expr); // a old wrong method
        char op = binOp->getOp();
        string opStr(1, op);
#ifdef DEBUG
        fprintf(stderr, "op: %s\n", opStr.c_str());
#endif

        ast_ptr &lhs = binOp->getLHS();
        string lhsStr = PrintExpression(lhs, PRECISION);
        ast_ptr &rhs = binOp->getRHS();
        string rhsStr = PrintExpression(rhs, PRECISION);

        exprStr += "(" + lhsStr + " " + opStr + " " + rhsStr + ")";
    }
    else
    {
        exprStr = "unknown expression";
    }
    return exprStr;
}

/**
 * @brief Prints the details of a given function AST (Abstract Syntax Tree).
 * 
 * This function prints the name, arguments, and body of a function represented by the given FunctionAST object.
 * It outputs the information to the standard error stream.
 * 
 * @param fun A unique pointer to the FunctionAST object representing the function to be printed.
 */
void PrintFunction(std::unique_ptr<FunctionAST> &fun)
{
    if(fun == nullptr)
    {
        fprintf(stderr, "this is a nullptr.\n");
    }
    else
    {
        string funcNameStr = fun->getFuncName();
        vector<string> funcArgsStr = fun->getFuncArgs();
        ast_ptr &expr = fun->getFuncBody();

        fprintf(stderr, "funcName: %s\n", funcNameStr.c_str());
        fprintf(stderr, "Args list:\n");
        if(funcArgsStr.size() == 0)
        {
            fprintf(stderr, "\tempty args");
        }
        else
        {
            for(auto it = funcArgsStr.begin(); it != funcArgsStr.end(); ++it)
            {
                fprintf(stderr, "%s ", (*it).c_str());
            }
        }
        fprintf(stderr, "\nFunc Body:\n");
        // expr->printExpr();
        string funcBodyStr = PrintExpression(expr);
        fprintf(stderr, "\t%s\n", funcBodyStr.c_str());
    }
}

/**
 * @brief Prints an abstract syntax tree (AST) expression with a specified prefix and precision.
 * 
 * This function prints the given AST expression to the standard error stream, 
 * with a specified prefix and precision for numerical values. If an index is provided, 
 * it includes the index in the printed output.
 * 
 * @param expr The AST expression to print.
 * @param prefix A string prefix to prepend to the printed expression.
 * @param PRECISION The precision to use for numerical values in the expression.
 * @param index The index of the expression. If index is -1, it is not included in the output.
 */
void printExpr(const ast_ptr &expr, string prefix, const size_t PRECISION, int index)
{
    if(index == -1)
        fprintf(stderr, "%s%s\n", prefix.c_str(), PrintExpression(expr, PRECISION).c_str());
    else
        fprintf(stderr, "%sNo.%d: %s\n", prefix.c_str(), index, PrintExpression(expr, PRECISION).c_str());
}

/**
 * @brief Prints a list of abstract syntax tree (AST) expressions with a specified prefix and precision.
 * 
 * This function iterates through a vector of AST expressions and prints each expression to the standard error stream.
 * Each expression is prefixed with a specified string and its index in the vector. If the showTree flag is set to true,
 * it also prints the tree representation of each expression.
 * 
 * @param exprs A vector of AST expressions to print.
 * @param prefix A string prefix to prepend to each printed expression.
 * @param showTree A boolean flag indicating whether to print the tree representation of each expression.
 * @param PRECISION The precision to use for numerical values in the expressions.
 */
void printExprs(const vector<ast_ptr> &exprs, string prefix, bool showTree, const size_t PRECISION)
{
    for(size_t i = 0; i < exprs.size(); i++)
    {
        auto &expr = exprs.at(i);
        fprintf(stderr, "%sNo.%ld: %s\n", prefix.c_str(), i, PrintExpression(expr, PRECISION).c_str());
        if(showTree)
        {
            printAST(expr);
        }
    }
}

const char blankChar(' ');
const string blankStr(2, blankChar);

/**
 * @brief Updates the given string by appending blank characters if the position is greater than the current rightmost position.
 * 
 * This function ensures that the string is extended with blank characters to reach the specified position.
 * It appends the necessary number of blank characters to the string if the given position is greater than the current rightmost position.
 * 
 * @param str The string to be updated.
 * @param posit The target position to reach in the string.
 * @param rightest The current rightmost position in the string.
 */
void updateStr(string &str, const int posit, const int rightest)
{
    if(posit > rightest)
    {
        string tmpStr(posit - rightest, blankChar);
        str.append(tmpStr);
    }
}

/**
 * @brief Recursively prints the abstract syntax tree (AST) expression into a visual tree representation.
 * 
 * This function traverses the AST expression and generates a visual representation of the tree structure.
 * It handles different types of expressions such as numbers, variables, function calls, and binary operations.
 * The visual representation is stored in the treePics vector, and the rightmost positions of each level are tracked in the rightests vector.
 * 
 * @param expr The AST expression to be printed.
 * @param posit The current position in the visual representation.
 * @param treePics A vector of strings representing the visual tree structure at each level.
 * @param rightests A vector of integers representing the rightmost positions at each level.
 * @param PRECISION The precision to use for numerical values in the expression.
 * @return int The rightmost position after processing the current expression.
 */
int printASTKernel(const ast_ptr &expr, const int posit, vector<string> &treePics, vector<int> &rightests, const size_t PRECISION = DOUBLE_PRECISION)
{
    if (expr == nullptr)
    {
        fprintf(stderr, "ERROR: printASTKernel: this is a nullptr.\n");
        exit(EXIT_FAILURE);
    }
    if (treePics.size() != rightests.size())
    {
        cerr << "ERROR: printASTKernel: size not equal" << endl;
        exit(EXIT_FAILURE);
    }

    static int level = -1;
    level++;
    if (int(treePics.size()) < level + 1) // lhs
    {
        if (int(treePics.size()) != level)
        {
            cerr << "ERROR: printASTKernel: size small" << endl;
            exit(EXIT_FAILURE);
        }
        string tmpStr(posit, blankChar);
        int tmpSize = tmpStr.size();
        treePics.push_back(tmpStr);
        rightests.push_back(tmpSize);
    }

    auto currentPic = treePics.at(level);
    int rightest = currentPic.size(); // int rightest = rightests.at(level); // NOTE: can not change 'auto rightest' to 'auto &rightest' !
    const string exprType = expr->type();
    if(exprType == "Number")
    {
        NumberExprAST *numberExpr = dynamic_cast<NumberExprAST *>(expr.get());
        double number = (numberExpr->getNumber());

        updateStr(currentPic, posit, rightest);

        std::stringstream ss;
        ss << std::setprecision(PRECISION) << number;
        currentPic.append(ss.str());
        currentPic.append(blankStr);
        rightest = currentPic.size();
    }
    else if(exprType == "Variable")
    {
        VariableExprAST *variableExpr = dynamic_cast<VariableExprAST *>(expr.get());
        string variable = (variableExpr->getVariable());

        updateStr(currentPic, posit, rightest);

        currentPic.append(variable);
        currentPic.append(blankStr);
        rightest = currentPic.size();
    }
    else if(exprType == "Call")
    {
        CallExprAST *callExpr = dynamic_cast<CallExprAST *>(expr.get());
        string callee = (callExpr->getCallee());
        vector<ast_ptr> &args = callExpr->getArgs();

        updateStr(currentPic, posit, rightest);

        currentPic.append(callee + "()");
        currentPic.append(blankStr);
        rightest = currentPic.size();

        if(args.size() == 0)
        {
            cerr << "ERROR: printASTKernel: func has no args" << endl;
            exit(EXIT_FAILURE);
        }
        int childRightest = std::max(0, posit);
        for(auto &arg : args)
        {
            childRightest = printASTKernel(arg, childRightest, treePics, rightests, PRECISION);
        }
        rightest = std::max(rightest, childRightest);
    }
    else if(exprType == "Binary")
    {
        BinaryExprAST *binOp = dynamic_cast<BinaryExprAST *>(expr.get());
        char op = binOp->getOp();
        string opStr(1, op);
        ast_ptr &lhs = binOp->getLHS();
        ast_ptr &rhs = binOp->getRHS();

        updateStr(currentPic, posit, rightest);
        currentPic.append(opStr);
        currentPic.append(blankStr);

        int childRightest = std::max(0, posit);
        childRightest = printASTKernel(lhs, childRightest, treePics, rightests, PRECISION);
        childRightest = printASTKernel(rhs, childRightest, treePics, rightests, PRECISION);

        rightest = currentPic.size();
        rightest = std::max(rightest, childRightest);

        // TODO: draw lines
        // auto positL = drawLeftLine(posit, currentPic);
        // auto positR = drawRightLine(posit, rightest, currentPic);
    }
    else
    {
        cerr << "ERROR: printASTKernel: unknown expression" << endl;
        exit(EXIT_FAILURE);
    }
    rightests.at(level) = rightest;
    treePics.at(level) = currentPic;
    level--;
    return rightest;
}

/**
 * @brief Prints the abstract syntax tree (AST) expression into a visual tree representation.
 * 
 * This function traverses the AST expression and generates a visual representation of the tree structure.
 * It handles different types of expressions such as numbers, variables, function calls, and binary operations.
 * The visual representation is printed to the standard output.
 * 
 * @param expr The AST expression to be printed.
 * @param PRECISION The precision to use for numerical values in the expression.
 */
void printAST(const ast_ptr &expr, const size_t PRECISION)
{
    vector<string> treePics;
    vector<int> rightests;
    printASTKernel(expr, 0, treePics, rightests, PRECISION);
    for (auto &treePic : treePics)
    {
        cout << std::setprecision(PRECISION) << treePic << endl;
    }
}

/**
 * @brief Prints the abstract syntax tree (AST) expression into a visual tree representation and stores it in a string.
 * 
 * This function traverses the AST expression and generates a visual representation of the tree structure.
 * It handles different types of expressions such as numbers, variables, function calls, and binary operations.
 * The visual representation is stored in the provided result string.
 * 
 * @param expr The AST expression to be printed.
 * @param result A string to store the visual representation of the AST.
 * @param PRECISION The precision to use for numerical values in the expression.
 */
void printAST(const ast_ptr &expr, string &result, const size_t PRECISION)
{
    vector<string> treePics;
    vector<int> rightests;
    printASTKernel(expr, 0, treePics, rightests, PRECISION);
    for (auto &treePic : treePics)
    {
        result.append(treePic + "\n");
    }
}

/**
 * @brief Retrieves the MPFR parameter number representation of an AST expression.
 * 
 * This function traverses an abstract syntax tree (AST) expression and generates a string
 * representation of the expression suitable for MPFR (Multiple Precision Floating-Point
 * Reliable) library usage. It handles different types of expressions such as numbers,
 * variables, function calls, and binary operations.
 * 
 * @param expr The AST expression to be converted.
 * @param mpfr_variables A reference to a size_t variable that keeps track of the number of MPFR variables.
 * @return string The MPFR parameter number representation of the AST expression.
 */
string getMpfrParameterNumber(const ast_ptr &expr, size_t &mpfr_variables) {
    if(expr == nullptr)
    {
        fprintf(stderr, "this is a nullptr.\n");
    }
    string exprStr = "";
    if(expr->type() == "Number")
    {
        ++mpfr_variables;
        NumberExprAST *numberExpr = dynamic_cast<NumberExprAST *>(expr.get());
        double number = (numberExpr->getNumber());
        return std::to_string(number);
    }
    else if(expr->type() == "Variable")
    {
        VariableExprAST *variableExpr = dynamic_cast<VariableExprAST *>(expr.get());
        string variable = (variableExpr->getVariable());
        ++mpfr_variables;
        return variable;
    }
    else if(expr->type() == "Call")
    {
        CallExprAST *callExpr = dynamic_cast<CallExprAST *>(expr.get());
        string callee = (callExpr->getCallee());
        vector<ast_ptr> &args = callExpr->getArgs();
        vector<string> argsStr;
        for(long unsigned int i = 0; i < args.size(); ++i)
        {
            string strTmp = getMpfrParameterNumber(args.at(i), mpfr_variables);  // ast_ptr& exprTmp = args.at(i);
            argsStr.push_back(strTmp);
        }
        ++mpfr_variables;
        callee += "(";
        for(long unsigned int i = 0; i < argsStr.size() - 1; ++i)
        {
            callee += argsStr.at(i) + ", ";
        }
        callee += argsStr.back() + ")";
        return callee;
    }
    else if(expr->type() == "Binary")
    {
        BinaryExprAST *binOp = dynamic_cast<BinaryExprAST *>(expr.get());
        char op = binOp->getOp();
        string opStr(1, op);
        ast_ptr &lhs = binOp->getLHS();
        string lhsStr = getMpfrParameterNumber(lhs, mpfr_variables);
        ast_ptr &rhs = binOp->getRHS();
        string rhsStr = getMpfrParameterNumber(rhs, mpfr_variables);
        ++mpfr_variables;
        exprStr += "(" + lhsStr + " " + opStr + " " + rhsStr + ")";
    }
    else
    {
        exprStr = "unknown expression";
    }
    return exprStr;
}

/**
 * @brief Generates MPFR (Multiple Precision Floating-Point Reliable) code for a given AST expression.
 * 
 * This function traverses an abstract syntax tree (AST) expression and generates MPFR code
 * for the expression. It handles different types of expressions such as numbers, variables,
 * function calls, and binary operations. The generated code is written to the provided output file stream.
 * 
 * @param expr The AST expression to be converted to MPFR code.
 * @param mpfr_variables A reference to a size_t variable that keeps track of the number of MPFR variables.
 * @param map A map that associates function names and operators with their corresponding MPFR function names.
 * @param ofs The output file stream where the generated MPFR code will be written.
 * @param variable_tmp A temporary string used to store variable names.
 * @return string The name of the MPFR variable that holds the result of the expression.
 */
string mpfrCodeGenerator(const ast_ptr &expr, size_t &mpfr_variables, const std::map<string, string> &map, ofstream &ofs, string &variable_tmp) {
    string number_str, variable_str, call_str, binary_str, l, r;
    if (expr == nullptr) {
        fprintf(stderr, "this is a nullptr.\n");
    }
    string exprStr = "";
    if (expr->type() == "Number") {
        ++mpfr_variables;
        NumberExprAST *numberExpr = dynamic_cast<NumberExprAST *>(expr.get());
        double number = (numberExpr->getNumber());
        std::stringstream ss;
        ss << std::setprecision(DOUBLE_PRECISION) << number;
        number_str = "mpfr_set_d(mp" + to_string(mpfr_variables) + ", " + ss.str() + ", MPFR_RNDN);";
        // cout << number_str << endl;
        ofs << "\t" << number_str << endl;
        return "mp" + to_string(mpfr_variables);
    } else if (expr->type() == "Variable") {
        VariableExprAST *variableExpr = dynamic_cast<VariableExprAST *>(expr.get());
        string variable = (variableExpr->getVariable());
        // if (variable_tmp != variable) {
        //     variable_tmp = variable;
        //     ofs << "\tdouble " << variable_tmp << ";" << endl;
        // }
        // ofs << "\tdouble " << variable << ";" << endl;
        ++mpfr_variables;
        variable_str = "mpfr_set_d(mp" + to_string(mpfr_variables) + ", " + variable + ", MPFR_RNDN);";
        // cout << variable_str << endl;
        ofs << "\t" << variable_str << endl;
        return "mp" + to_string(mpfr_variables);
    } else if (expr->type() == "Call") {
        CallExprAST *callExpr = dynamic_cast<CallExprAST *>(expr.get());
        string callee = (callExpr->getCallee());
        vector<ast_ptr> &args = callExpr->getArgs();
        vector<string> argsStr;
        for (long unsigned int i = 0; i < args.size(); ++i) {
            string strTmp = mpfrCodeGenerator(args.at(i), mpfr_variables, map, ofs, variable_tmp);
            argsStr.push_back(strTmp);
        }
        ++mpfr_variables;
        auto it = map.find(callee);
        call_str = it->second;
        string callee_str;
        if (call_str == "mpfr_pow") {
            string str1 = argsStr.at(0), str2 = argsStr.at(1);
            callee_str = call_str + "(mp" + to_string(mpfr_variables) + ", " + str1 + ", " + str2 + ", MPFR_RNDN);";
        } else {
            string str1 = argsStr.at(0);
            callee_str = call_str + "(mp" + to_string(mpfr_variables) + ", " + str1 + ", MPFR_RNDN);";
        }
        // cout << callee_str << endl;
        ofs << "\t" << callee_str << endl;
        return "mp" + to_string(mpfr_variables);
    } else if (expr->type() == "Binary") {
        BinaryExprAST *binOp = dynamic_cast<BinaryExprAST *>(expr.get());
        char op = binOp->getOp();
        string opStr(1, op);
        auto it = map.find(opStr);
        string map_str = it->second;
        ast_ptr &lhs = binOp->getLHS();
        string lhsStr = mpfrCodeGenerator(lhs, mpfr_variables, map, ofs, variable_tmp);
        l = lhsStr;
        ast_ptr &rhs = binOp->getRHS();
        string rhsStr = mpfrCodeGenerator(rhs, mpfr_variables, map, ofs, variable_tmp);
        r = rhsStr;
        ++mpfr_variables;
        binary_str = map_str + "(mp" + to_string(mpfr_variables) + ", " + l + ", " + r + ", MPFR_RNDN);";
        // cout << binary_str << endl;
        ofs << "\t" << binary_str << endl;
        return "mp" + to_string(mpfr_variables);
    } else {
        exprStr = "unknown expression";
    }
    return exprStr;
}

/**
 * @brief Checks if the given string contains only the simple variable 'x'.
 * 
 * This function uses regular expressions to determine if the input string contains
 * only the variable 'x' without any digits attached to it. It first checks if there
 * is an 'x' that is not part of a larger word or number, and then ensures that there
 * are no occurrences of 'x' followed by digits.
 * 
 * @param str The input string to be checked.
 * @return true if the string contains only the simple variable 'x', false otherwise.
 */
bool containOnlySimpleX(const string &str) {
    regex pattern("(^|[^x\\d])x([^\\d]|$)");
    if (regex_search(str, pattern)) {
        regex xWithNumbersPattern("x\\d+");
        return !regex_search(str, xWithNumbersPattern);
    }
    return false;
}
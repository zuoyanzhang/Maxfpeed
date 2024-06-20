//
// Created by 99065 on 2023/10/19.
//
#include "mpfrCodeGene/include/basic.h"
#include "mpfrCodeGene/include/geneCode.h"
#include "mpfrCodeGene/include/parserAST.h"

#include <algorithm>

using std::cout;
using std::endl;
using std::ios;
using std::ios_base;
using std::ofstream;
using std::string;
using std::to_string;

/**
 * @brief Extracts variable names from an abstract syntax tree (AST) expression.
 *
 * This function traverses the given AST expression and collects all variable names
 * into the provided vector. It handles different types of AST nodes such as Number,
 * Variable, Call, and Binary. The collected variable names are sorted and made unique.
 *
 * @param expr The AST expression to traverse.
 * @param vars A vector to store the extracted variable names.
 * @return true if the extraction is successful, false otherwise.
 */
bool getVariablesFromExpr(const ast_ptr &expr, vector<string> &vars)
{
    if (expr == nullptr)
    {
        fprintf(stderr, "ERROR: getVariablesFromExpr: the input is nullptr\n");
        exit(EXIT_FAILURE);
    }
    auto type = expr->type();
    if (type == "Number")
    {
        return true;
    }
    else if (type == "Variable")
    {
        VariableExprAST *varPtr = dynamic_cast<VariableExprAST *>(expr.get());
        auto var = varPtr->getVariable();
        vars.push_back(var);
        return true;
    }
    else if (type == "Call")
    {
        CallExprAST *callPtr = dynamic_cast<CallExprAST *>(expr.get());
        auto &args = callPtr->getArgs();
        for (auto &arg : args)
        {
            if (!getVariablesFromExpr(arg, vars))
            {
                fprintf(stderr, "ERROR: getVariablesFromExpr: run failed\n");
                exit(EXIT_FAILURE);
            }
        }
        std::sort(vars.begin(), vars.end()); // sort to unique the parameters queue
        vars.erase(std::unique(vars.begin(), vars.end()), vars.end());
        return true;
    }
    if (type != "Binary")
    {
        fprintf(stderr, "ERROR: getVariablesFromExpr: type %s is wrong\n", type.c_str());
        exit(EXIT_FAILURE);
    }
    BinaryExprAST *binaryPtr = dynamic_cast<BinaryExprAST *>(expr.get());
    auto &lhs = binaryPtr->getLHS();
    auto &rhs = binaryPtr->getRHS();
    getVariablesFromExpr(lhs, vars);
    getVariablesFromExpr(rhs, vars);
    std::sort(vars.begin(), vars.end()); // sort to unique the parameters queue
    vars.erase(std::unique(vars.begin(), vars.end()), vars.end());
    return true;
}

/**
 * @brief Generates MPFR code for high-precision arithmetic operations.
 *
 * This function generates C++ code that uses the MPFR library to perform
 * high-precision arithmetic operations based on the given expression string.
 * The generated code is written to a file located at "./detectModule/HSED/src/getresult.cpp".
 *
 * @param exprStr The expression string to be parsed and converted into MPFR code.
 * @param vars A vector of variable names used in the expression.
 * @return true if the code generation is successful, false otherwise.
 */
int geneMpfrCode(const string exprStr, vector<string> vars)
{
    ofstream file_clean("./detectModule/HSED/src/getresult.cpp", ios_base::out);
    ofstream ofs("./detectModule/HSED/src/getresult.cpp", ios::app);

    std::map<string, string> mpfr_map = {
            {"+", "mpfr_add"},
            {"-", "mpfr_sub"},
            {"*", "mpfr_mul"},
            {"/", "mpfr_div"},
            {"exp", "mpfr_exp"},
            {"pow", "mpfr_pow"},
            {"sqrt", "mpfr_sqrt"},
            {"sin", "mpfr_sin"},
            {"log", "mpfr_log"},
            {"cos", "mpfr_cos"},
            {"atan", "mpfr_atan"},
            {"tan", "mpfr_tan"},
            {"cbrt", "mpfr_cbrt"}};

    std::unique_ptr<ExprAST> exprAst = ParseExpressionFromString(exprStr);
    // vector<string> vars;
    // getVariablesFromExpr(exprAst, vars);
    size_t mpfr_variables = 0;
    getMpfrParameterNumber(exprAst, mpfr_variables);

    ofs << "#include \"HSED/include/getresult.h\"\n"
        << "double getULP" << "(";
    for (auto &var : vars)
    {
        ofs << "double" << " " << var << ", ";
    }
    ofs << "double origin) {\n"
        << "\tmpfr_t mpfr_origin, mpfr_oracle, ";
    for (size_t i = 0; i < mpfr_variables; ++i)
    {
        if (i != mpfr_variables - 1)
            ofs << "mp" + to_string(i + 1) + ", ";
        else
            ofs << "mp" + to_string(i + 1) + ";\n";
    }
    ofs << "\tmpfr_inits2(128, mpfr_origin, mpfr_oracle, ";
    for (size_t i = 0; i < mpfr_variables; ++i) {
        ofs << "mp" + to_string(i + 1) + ", ";
    }
    ofs << "(mpfr_ptr) 0);\n";
    mpfr_variables = 0;
    string variable_tmp = "";
    mpfrCodeGenerator(exprAst, mpfr_variables, mpfr_map, ofs, variable_tmp);
    ofs << "\n\tmpfr_set(mpfr_oracle, mp" << mpfr_variables << ", MPFR_RNDN);\n"
        << "\tmpfr_set_d(mpfr_origin, origin, MPFR_RNDN);\n"
        << "\tdouble ulp = computeULPDiff(mpfr_origin, mpfr_oracle);\n";
    ofs << "\tmpfr_clears(mpfr_origin, mpfr_oracle, ";
    for (size_t i = 0; i < mpfr_variables; ++i) {
        ofs << "mp" << to_string(i + 1) << ", ";
    }
    ofs << "(mpfr_ptr) 0);\n";
    ofs << "\tmpfr_free_cache();\n";
    ofs << "\treturn ulp;\n"
        << "}\n";

    ofs << "double getRelativeError(";
    for (auto &var : vars)
    {
        ofs << "double" << " " << var << ", ";
    }
    ofs << "double origin) {\n"
        << "\tmpfr_t mpfr_origin, mpfr_oracle, mpfr_relative, mpfr_absolute, ";
    for (size_t i = 0; i < mpfr_variables; ++i)
    {
        if (i != mpfr_variables - 1)
            ofs << "mp" + to_string(i + 1) + ", ";
        else
            ofs << "mp" + to_string(i + 1) + ";\n";
    }
    ofs << "\tmpfr_inits2(128, mpfr_origin, mpfr_oracle, mpfr_relative, mpfr_absolute, ";
    for (size_t i = 0; i < mpfr_variables; ++i) {
        ofs << "mp" + to_string(i + 1) + ", ";
    }
    ofs << "(mpfr_ptr) 0);\n";
    mpfr_variables = 0;
    mpfrCodeGenerator(exprAst, mpfr_variables, mpfr_map, ofs, variable_tmp);
    ofs << "\n\tmpfr_set(mpfr_oracle, mp" << mpfr_variables << ", MPFR_RNDN);\n"
        << "\tmpfr_set_d(mpfr_origin, origin, MPFR_RNDN);\n"
        << "\tmpfr_sub(mpfr_absolute, mpfr_oracle, mpfr_origin, MPFR_RNDN);\n"
        << "\tmpfr_div(mpfr_relative, mpfr_absolute, mpfr_oracle, MPFR_RNDN);\n"
        << "\tdouble relative = mpfr_get_d(mpfr_relative, MPFR_RNDN);\n"
        << "\trelative = abs(relative);\n";
    ofs << "\tmpfr_clears(mpfr_origin, mpfr_oracle, mpfr_relative, mpfr_absolute, ";
    for (size_t i = 0; i < mpfr_variables; ++i) {
        ofs << "mp" << to_string(i + 1) << ", ";
    }
    ofs << "(mpfr_ptr) 0);\n";
    ofs << "\tmpfr_free_cache();\n";
    ofs << "\treturn relative;\n"
        << "}\n";

    ofs << "double getFloatToDoubleOfOrigin(const float &inputx) {\n"
        << "\tdouble " << vars[0] << " = (double)inputx;\n";
    for (size_t i = 1; i < vars.size(); ++i) {
        ofs << "\tdouble " << vars[i] << " = " << 0.0 << ";\n";
    }
    ofs << "\treturn " << exprStr << ";\n"
        << "}\n";
    ofs << "double getDoubleOfOrigin(const double &inputx) {\n"
        << "\tdouble " << vars[0] << " = inputx;\n";
    for (size_t i = 1; i < vars.size(); ++i) {
        ofs << "\tdouble " << vars[i] << " = " << 0.0 << ";\n";
    }
    ofs << "\treturn " << exprStr << ";\n"
        << "}";
    ofs << "\n";
    return true;
}

/**
 * Generates multiple MPFR (Multiple Precision Floating-Point Reliable) code.
 * 
 * @param exprStr The expression string to be parsed and evaluated.
 * @param vars A vector of variable names used in the expression.
 * @return true if the code generation is successful, false otherwise.
 */
int geneMultiMpfrCode(const string exprStr, vector<string> vars) {
    ofstream file_clean("./detectModule/EIFFEL/src/getMultiResult.cpp", ios_base::out);
    ofstream ofs("./detectModule/EIFFEL/src/getMultiResult.cpp", ios::app);
    size_t count = 0;

    std::map<string, string> mpfr_map = {
            {"+", "mpfr_add"},
            {"-", "mpfr_sub"},
            {"*", "mpfr_mul"},
            {"/", "mpfr_div"},
            {"exp", "mpfr_exp"},
            {"pow", "mpfr_pow"},
            {"sqrt", "mpfr_sqrt"},
            {"sin", "mpfr_sin"},
            {"log", "mpfr_log"},
            {"cos", "mpfr_cos"},
            {"atan", "mpfr_atan"},
            {"tan", "mpfr_tan"},
            {"cbrt", "mpfr_cbrt"}};

    std::unique_ptr<ExprAST> exprAst = ParseExpressionFromString(exprStr);
    // vector<string> vars;
    // getVariablesFromExpr(exprAst, vars);
    size_t mpfr_variables = 0;
    getMpfrParameterNumber(exprAst, mpfr_variables);

    ofs << "#include \"HSED/include/common.h\"\n";

    ofs << "\n";
    size_t size = vars.size();

    if (size == 1) {
        ofs << "double getULPE(double x, double origin);" << "\n";

        ofs << "double getULPE2(double x1, double x2, double origin);" << "\n";

        ofs << "double getULPE3(double x1, double x2, double x3, double origin);" << "\n";

        ofs << "double getULPE4(double x1, double x2, double x3, double x4, double origin);" << "\n";

        ofs << "double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin);" << "\n";

        ofs << "double getComputeValueE(const double &inputx);" << "\n";

        ofs << "double getComputeValueE2(const double &inputx1, const double &inputx2);" << "\n";

        ofs << "double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3);" << "\n";

        ofs << "double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4);" << "\n";

        ofs << "double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5);" << "\n";

        ofs << "\n";

        count = 0;
        ofs << "double getComputeValueE" << "(";
        for (auto &var : vars) {
            ++count;
            if (count == size) {
                ofs << "const double &input" << var << ")" << "{\n";
            } else {
                ofs << "const double &input" << var << ", ";
            }
        }

        for (size_t i = 0; i < vars.size(); ++i) {
            ofs << "\tdouble " << vars[i] << " = " << "input" << vars[i] << ";\n";
        }
        ofs << "\treturn " << exprStr << ";\n"
            << "}";
        ofs << "\n";


        // double getComputeValueE2
        ofs << "double getComputeValueE2(const double &inputx1, const double &inputx2) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE3
        ofs << "double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE4
        ofs << "double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE5
        ofs << "double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        ofs << "double getULPE" << "(";
        for (auto &var : vars)
        {
            ofs << "double" << " " << var << ", ";
        }
        ofs << "double origin) {\n"
            << "\tmpfr_t mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i)
        {
            if (i != mpfr_variables - 1)
                ofs << "mp" + to_string(i + 1) + ", ";
            else
                ofs << "mp" + to_string(i + 1) + ";\n";
        }
        ofs << "\tmpfr_inits2(128, mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i) {
            ofs << "mp" + to_string(i + 1) + ", ";
        }
        ofs << "(mpfr_ptr) 0);\n";
        mpfr_variables = 0;
        string variable_tmp = "";
        mpfrCodeGenerator(exprAst, mpfr_variables, mpfr_map, ofs, variable_tmp);
        ofs << "\n\tmpfr_set(mpfr_oracle, mp" << mpfr_variables << ", MPFR_RNDN);\n"
            << "\tmpfr_set_d(mpfr_origin, origin, MPFR_RNDN);\n"
            << "\tdouble ulp = computeULPDiff(mpfr_origin, mpfr_oracle);\n";
        ofs << "\tmpfr_clears(mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i) {
            ofs << "mp" << to_string(i + 1) << ", ";
        }
        ofs << "(mpfr_ptr) 0);\n";
        ofs << "\tmpfr_free_cache();\n";
        ofs << "\treturn ulp;\n"
            << "}\n";

        // double getULPE2
        ofs << "double getULPE2(double x1, double x2, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE3
        ofs << "double getULPE3(double x1, double x2, double x3, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE4
        ofs << "double getULPE4(double x1, double x2, double x3, double x4, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE5
        ofs << "double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

    } else if (size == 2) {
        ofs << "double getULPE(double x, double origin);" << "\n";
        ofs << "double getULPE2(double x1, double x2, double origin);" << "\n";

        ofs << "double getULPE3(double x1, double x2, double x3, double origin);" << "\n";

        ofs << "double getULPE4(double x1, double x2, double x3, double x4, double origin);" << "\n";

        ofs << "double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin);" << "\n";

        ofs << "double getComputeValueE(const double &inputx);" << "\n";

        ofs << "double getComputeValueE2(const double &inputx1, const double &inputx2);" << "\n";

        ofs << "double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3);" << "\n";

        ofs << "double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4);" << "\n";

        ofs << "double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5);" << "\n";

        ofs << "\n";
        // double getULPE
        ofs << "double getULPE(double x, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";
        // double getULPE2
        ofs << "double getULPE2" << "(";
        for (auto &var : vars)
        {
            ofs << "double" << " " << var << ", ";
        }
        ofs << "double origin) {\n"
            << "\tmpfr_t mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i)
        {
            if (i != mpfr_variables - 1)
                ofs << "mp" + to_string(i + 1) + ", ";
            else
                ofs << "mp" + to_string(i + 1) + ";\n";
        }
        ofs << "\tmpfr_inits2(128, mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i) {
            ofs << "mp" + to_string(i + 1) + ", ";
        }
        ofs << "(mpfr_ptr) 0);\n";
        mpfr_variables = 0;
        string variable_tmp = "";
        mpfrCodeGenerator(exprAst, mpfr_variables, mpfr_map, ofs, variable_tmp);
        ofs << "\n\tmpfr_set(mpfr_oracle, mp" << mpfr_variables << ", MPFR_RNDN);\n"
            << "\tmpfr_set_d(mpfr_origin, origin, MPFR_RNDN);\n"
            << "\tdouble ulp = computeULPDiff(mpfr_origin, mpfr_oracle);\n";
        ofs << "\tmpfr_clears(mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i) {
            ofs << "mp" << to_string(i + 1) << ", ";
        }
        ofs << "(mpfr_ptr) 0);\n";
        ofs << "\tmpfr_free_cache();\n";
        ofs << "\treturn ulp;\n"
            << "}\n";

        // double getULPE3
        ofs << "double getULPE3(double x1, double x2, double x3, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE4
        ofs << "double getULPE4(double x1, double x2, double x3, double x4, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE5
        ofs << "double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE
        ofs << "double getComputeValueE(const double &inputx) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputedValueE2
        count = 0;
        ofs << "double getComputeValueE2" << "(";
        for (auto &var : vars) {
            ++count;
            if (count == size) {
                ofs << "const double &input" << var << ")" << "{\n";
            } else {
                ofs << "const double &input" << var << ", ";
            }
        }

        for (size_t i = 0; i < vars.size(); ++i) {
            ofs << "\tdouble " << vars[i] << " = " << "input" << vars[i] << ";\n";
        }
        ofs << "\treturn " << exprStr << ";\n"
            << "}";

        // double getComputeValueE3
        ofs << "double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE4
        ofs << "double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE5
        ofs << "double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        ofs << "\n";
    } else if (size == 3) {
        ofs << "double getULPE(double x, double origin);" << "\n";
        ofs << "double getULPE2(double x1, double x2, double origin);" << "\n";

        ofs << "double getULPE3(double x1, double x2, double x3, double origin);" << "\n";

        ofs << "double getULPE4(double x1, double x2, double x3, double x4, double origin);" << "\n";

        ofs << "double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin);" << "\n";

        ofs << "double getComputeValueE(const double &inputx);" << "\n";

        ofs << "double getComputeValueE2(const double &inputx1, const double &inputx2);" << "\n";

        ofs << "double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3);" << "\n";

        ofs << "double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4);" << "\n";

        ofs << "double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5);" << "\n";

        ofs << "\n";

        // double getULPE
        ofs << "double getULPE(double x, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE2
        ofs << "double getULPE2(double x1, double x2, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        //double getULPE3
        ofs << "double getULPE3" << "(";
        for (auto &var : vars)
        {
            ofs << "double" << " " << var << ", ";
        }
        ofs << "double origin) {\n"
            << "\tmpfr_t mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i)
        {
            if (i != mpfr_variables - 1)
                ofs << "mp" + to_string(i + 1) + ", ";
            else
                ofs << "mp" + to_string(i + 1) + ";\n";
        }
        ofs << "\tmpfr_inits2(128, mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i) {
            ofs << "mp" + to_string(i + 1) + ", ";
        }
        ofs << "(mpfr_ptr) 0);\n";
        mpfr_variables = 0;
        string variable_tmp = "";
        mpfrCodeGenerator(exprAst, mpfr_variables, mpfr_map, ofs, variable_tmp);
        ofs << "\n\tmpfr_set(mpfr_oracle, mp" << mpfr_variables << ", MPFR_RNDN);\n"
            << "\tmpfr_set_d(mpfr_origin, origin, MPFR_RNDN);\n"
            << "\tdouble ulp = computeULPDiff(mpfr_origin, mpfr_oracle);\n";
        ofs << "\tmpfr_clears(mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i) {
            ofs << "mp" << to_string(i + 1) << ", ";
        }
        ofs << "(mpfr_ptr) 0);\n";
        ofs << "\tmpfr_free_cache();\n";
        ofs << "\treturn ulp;\n"
            << "}\n";

        // double getULPE4
        ofs << "double getULPE4(double x1, double x2, double x3, double x4, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE5
        ofs << "double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE
        ofs << "double getComputeValueE(const double &inputx) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE2
        ofs << "double getComputeValueE2(const double &inputx1, const double &inputx2) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE3
        count = 0;
        ofs << "double getComputeValueE3" << "(";
        for (auto &var : vars) {
            ++count;
            if (count == size) {
                ofs << "const double &input" << var << ")" << "{\n";
            } else {
                ofs << "const double &input" << var << ", ";
            }
        }

        for (size_t i = 0; i < vars.size(); ++i) {
            ofs << "\tdouble " << vars[i] << " = " << "input" << vars[i] << ";\n";
        }
        ofs << "\treturn " << exprStr << ";\n"
            << "}";

        // double getComputeValueE4
        ofs << "double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE5
        ofs << "double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        ofs << "\n";

    } else if (size == 4) {
        ofs << "double getULPE(double x, double origin);" << "\n";
        ofs << "double getULPE2(double x1, double x2, double origin);" << "\n";

        ofs << "double getULPE3(double x1, double x2, double x3, double origin);" << "\n";

        ofs << "double getULPE4(double x1, double x2, double x3, double x4, double origin);" << "\n";

        ofs << "double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin);" << "\n";

        ofs << "double getComputeValueE(const double &inputx);" << "\n";

        ofs << "double getComputeValueE2(const double &inputx1, const double &inputx2);" << "\n";

        ofs << "double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3);" << "\n";

        ofs << "double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4);" << "\n";

        ofs << "double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5);" << "\n";

        ofs << "\n";

        // double getULPE
        ofs << "double getULPE(double x, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE2
        ofs << "double getULPE2(double x1, double x2, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE3
        ofs << "double getULPE3(double x1, double x2, double x3, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        //double getULPE4
        ofs << "double getULPE4" << "(";
        for (auto &var : vars)
        {
            ofs << "double" << " " << var << ", ";
        }
        ofs << "double origin) {\n"
            << "\tmpfr_t mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i)
        {
            if (i != mpfr_variables - 1)
                ofs << "mp" + to_string(i + 1) + ", ";
            else
                ofs << "mp" + to_string(i + 1) + ";\n";
        }
        ofs << "\tmpfr_inits2(128, mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i) {
            ofs << "mp" + to_string(i + 1) + ", ";
        }
        ofs << "(mpfr_ptr) 0);\n";
        mpfr_variables = 0;
        string variable_tmp = "";
        mpfrCodeGenerator(exprAst, mpfr_variables, mpfr_map, ofs, variable_tmp);
        ofs << "\n\tmpfr_set(mpfr_oracle, mp" << mpfr_variables << ", MPFR_RNDN);\n"
            << "\tmpfr_set_d(mpfr_origin, origin, MPFR_RNDN);\n"
            << "\tdouble ulp = computeULPDiff(mpfr_origin, mpfr_oracle);\n";
        ofs << "\tmpfr_clears(mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i) {
            ofs << "mp" << to_string(i + 1) << ", ";
        }
        ofs << "(mpfr_ptr) 0);\n";
        ofs << "\tmpfr_free_cache();\n";
        ofs << "\treturn ulp;\n"
            << "}\n";

        // double getULPE5
        ofs << "double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE
        ofs << "double getComputeValueE(const double &inputx) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE2
        ofs << "double getComputeValueE2(const double &inputx1, const double &inputx2) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE3
        ofs << "double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        //double getComputeValueE4
        count = 0;
        ofs << "double getComputeValueE4" << "(";
        for (auto &var : vars) {
            ++count;
            if (count == size) {
                ofs << "const double &input" << var << ")" << "{\n";
            } else {
                ofs << "const double &input" << var << ", ";
            }
        }

        for (size_t i = 0; i < vars.size(); ++i) {
            ofs << "\tdouble " << vars[i] << " = " << "input" << vars[i] << ";\n";
        }
        ofs << "\treturn " << exprStr << ";\n"
            << "}";

        ofs << "\n";

        // double getComputeValueE5
        ofs << "double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

    } else if (size == 5) {
        ofs << "double getULPE(double x, double origin);" << "\n";

        ofs << "double getULPE2(double x1, double x2, double origin);" << "\n";

        ofs << "double getULPE3(double x1, double x2, double x3, double origin);" << "\n";

        ofs << "double getULPE4(double x1, double x2, double x3, double x4, double origin);" << "\n";

        ofs << "double getULPE5(double x1, double x2, double x3, double x4, double x5, double origin);" << "\n";

        ofs << "double getComputeValueE(const double &inputx);" << "\n";

        ofs << "double getComputeValueE2(const double &inputx1, const double &inputx2);" << "\n";

        ofs << "double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3);" << "\n";

        ofs << "double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4);" << "\n";

        ofs << "double getComputeValueE5(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4, const double &inputx5);" << "\n";

        ofs << "\n";

        // double getULPE
        ofs << "double getULPE(double x, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE2
        ofs << "double getULPE2(double x1, double x2, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE3
        ofs << "double getULPE3(double x1, double x2, double x3, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE4
        ofs << "double getULPE4(double x1, double x2, double x3, double x4, double origin) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getULPE5
        ofs << "double getULPE5" << "(";
        for (auto &var : vars)
        {
            ofs << "double" << " " << var << ", ";
        }
        ofs << "double origin) {\n"
            << "\tmpfr_t mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i)
        {
            if (i != mpfr_variables - 1)
                ofs << "mp" + to_string(i + 1) + ", ";
            else
                ofs << "mp" + to_string(i + 1) + ";\n";
        }
        ofs << "\tmpfr_inits2(128, mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i) {
            ofs << "mp" + to_string(i + 1) + ", ";
        }
        ofs << "(mpfr_ptr) 0);\n";
        mpfr_variables = 0;
        string variable_tmp = "";
        mpfrCodeGenerator(exprAst, mpfr_variables, mpfr_map, ofs, variable_tmp);
        ofs << "\n\tmpfr_set(mpfr_oracle, mp" << mpfr_variables << ", MPFR_RNDN);\n"
            << "\tmpfr_set_d(mpfr_origin, origin, MPFR_RNDN);\n"
            << "\tdouble ulp = computeULPDiff(mpfr_origin, mpfr_oracle);\n";
        ofs << "\tmpfr_clears(mpfr_origin, mpfr_oracle, ";
        for (size_t i = 0; i < mpfr_variables; ++i) {
            ofs << "mp" << to_string(i + 1) << ", ";
        }
        ofs << "(mpfr_ptr) 0);\n";
        ofs << "\tmpfr_free_cache();\n";
        ofs << "\treturn ulp;\n"
            << "}\n";

        // double getComputeValueE
        ofs << "double getComputeValueE(const double &inputx) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE2
        ofs << "double getComputeValueE2(const double &inputx1, const double &inputx2) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE3
        ofs << "double getComputeValueE3(const double &inputx1, const double &inputx2, const double &inputx3) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE4
        ofs << "double getComputeValueE4(const double &inputx1, const double &inputx2, const double &inputx3, const double &inputx4) {\n"
            << "\treturn 1.0;\n"
            << "}\n";

        // double getComputeValueE5
        count = 0;
        ofs << "double getComputeValueE5" << "(";
        for (auto &var : vars) {
            ++count;
            if (count == size) {
                ofs << "const double &input" << var << ")" << "{\n";
            } else {
                ofs << "const double &input" << var << ", ";
            }
        }

        for (size_t i = 0; i < vars.size(); ++i) {
            ofs << "\tdouble " << vars[i] << " = " << "input" << vars[i] << ";\n";
        }
        ofs << "\treturn " << exprStr << ";\n"
            << "}";

        ofs << "\n";

    }

    return true;
}


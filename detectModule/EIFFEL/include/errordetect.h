//
// Created by 99065 on 2024/3/15.
//

#ifndef ERRORDETECTION_ERRORDETECT_H
#define ERRORDETECTION_ERRORDETECT_H

#include <vector>
#include <iostream>
#include <fstream>
#include <Python.h>
#include <iomanip>
#include <utility> // in order to use pair
#include <algorithm>
#include <regex>
#include <string>
#include <ctime>
#include <cmath>

// 单参函数
// 单个输入下的误差检测，返回值是二维数组，存储结构为x ulp
const std::vector<std::vector<double>> &oneParaErrorDetect(double x, std::vector<std::vector<double>> &vec);

const std::vector<std::vector<double>> &handleErrorPoint(std::vector<std::vector<double>> &vec, std::vector<std::vector<double>> &vecAfter);

int geneDBSCANFile(std::vector<std::vector<double>> vec);

std::pair<int, double> callDbscanPy();
// 判断是否是单调递增
bool isMonotonousUp(const std::vector<std::vector<double>> &vec);
// 判断是否是单调递减
bool isMonotonousDown(const std::vector<std::vector<double>> &vec);
// 确定拟合函数的参数，分别是第一项和比例大小
std::vector<double> getCurveFittingParas();
// 预测D内的所有显著误差输入区间
std::vector<std::vector<double>> predictInterval(double x1, double q1, double x2, double q2, double left_endpoint, double right_endpoint);

std::vector<double> detectAllIntervalGetMaximumError(std::vector<std::vector<double>> vec_interval);

bool compareByUlp(const std::vector<double> &a, const std::vector<double> &b);
// 返回[left, right]区间内的最大误差
std::vector<double> obtainMaxUlpInterval(double left, double right);
//打印vector<double>存放的最大误差x,ulp的信息到屏幕上
void printMaxError(std::vector<double> &vec);
// 处理检测区间时正半域或者负半域的情况
int detectNoSpan(double left_endpoint, double right_endpoint);

// getNumberOfParameters是为了提取输入给EIFFEL的表达式参数数量
int getNumberOfParameters(const std::string &filePath);

int onePara(double left, double right);

// 双参函数
// 双参输入下的误差检测，返回值应该是三维数组，存储结构为x1, x2, ulp
std::vector<std::vector<double>> twoParaErrorDetect(double x1, double x2);

std::vector<std::vector<double>> getTwoParaErrorAllPoint(double left_x1, double right_x1, double left_x2, double right_x2);

std::vector<std::vector<double>> handleErrorPoint2(std::vector<std::vector<double>> vec);

std::vector<double> obtainMaxUlpInterval2(double left, double right);

bool compareByUlp2(const std::vector<double> &a, const std::vector<double> &b);

void printMaxError2(std::vector<double> vec);
int detectNoSpan2(double left, double right);
std::vector<double> detectAllIntervalGetMaximumError2(std::vector<std::vector<double>> vec_interval, double left, double right, int sign);
void createCurvePointsTwoPara(std::vector<std::vector<double>> vec);
int twoPara(double left, double right);

int detectNoSpan3(double left, double right);
int threePara(double left, double right);
std::vector<double> obtainMaxUlpInterval3(double left, double right);
std::vector<std::vector<double>> getThreeParaErrorAllPoint(double left_x1, double right_x1, double left_x2, double right_x2, double left_x3, double right_x3);
std::vector<std::vector<double>> threeParaErrorDetect(double x1, double x2, double x3);
bool compareByUlp3(const std::vector<double> &a, const std::vector<double> &b);
void printMaxError3(std::vector<double> &vec);
std::vector<std::vector<double>> handleErrorPoint3(std::vector<std::vector<double>> vec);
std::vector<double> detectAllIntervalGetMaximumError3(std::vector<std::vector<double>> vec_interval, double left, double right);

std::vector<double> obtainMaxUlpInterval4(double left, double right);
std::vector<std::vector<double>> getFourParaErrorAllPoint(double left, double right);
std::vector<std::vector<double>> fourParaErrorDetect(double x1, double x2, double x3, double x4);
std::vector<std::vector<double>> handleErrorPoint4(std::vector<std::vector<double>> vec);
std::vector<double> detectAllIntervalGetMaximumError4(std::vector<std::vector<double>> vec_interval, double left, double right);
bool compareByUlp4(const std::vector<double> &a, const std::vector<double> &b);
bool compareByUlp5(const std::vector<double> &a, const std::vector<double> &b);
void printMaxError4(std::vector<double> &vec);
int detectNoSpan4(double left, double right);
int fourPara(double left, double right);

std::vector<double> obtainMaxUlpInterval5(double left, double right);
std::vector<std::vector<double>> getFiveParaErrorAllPoint(double left, double right);
std::vector<std::vector<double>> fiveParaErrorDetect(double x1, double x2, double x3, double x4, double x5);
std::vector<std::vector<double>> handleErrorPoint5(std::vector<std::vector<double>> vec);
std::vector<double> detectAllIntervalGetMaximumError5(std::vector<std::vector<double>> vec_interval, double left, double right);
int detectNoSpan5(double left, double right);
void printMaxError5(std::vector<double> &vec);
int fivePara(double left, double right);
int eiffel(double left, double right);


#endif //ERRORDETECTION_ERRORDETECT_H





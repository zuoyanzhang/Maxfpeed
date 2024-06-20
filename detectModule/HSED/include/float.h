//
// Created by 99065 on 2023/10/20.
//

#ifndef MAXFPEED_FLOAT_H
#define MAXFPEED_FLOAT_H

#include "HSED/include/common.h"

class DoubleFunction {
public:
    //操作double的尾数位，且增加显著误差判断分支决定起始操作的尾数位位数
    static void detectMethod2(const double &start, const double &end);
    static void detectMethod2_1(const double &start, const double &end);
    static void detectMethod2_2(const double &start, const double &end);
    //随机采样算法（目的是于模拟退火算法进行效果对比）
    static vector<double> random_test(const double &start, const double &end);
};


#endif //MAXFPEED_FLOAT_H

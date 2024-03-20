//
// Created by 99065 on 2023/10/20.
//

#ifndef MAXFPEED_GETRESULT_H
#define MAXFPEED_GETRESULT_H

#include "HSED/include/common.h"

double getULP(double x, double origin);
double getRelativeError(double x, double origin);
double getFloatToDoubleOfOrigin(const float &inputx);
double getDoubleOfOrigin(const double &inputx);


#endif //MAXFPEED_GETRESULT_H

//
// Created by 99065 on 2023/10/20.
//

#ifndef MAXFPEED_COMMON_H
#define MAXFPEED_COMMON_H

#include <iostream>
#include <gmp.h>
#include <mpfr.h>
#include <iomanip>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstdlib>
#include <random>


using namespace std;

int jiou(int x);
double computeULP(double y);
float computeULPf(float y);
double computeULPDiff(mpfr_t origin, mpfr_t oracle);
float computeULPFDiff(mpfr_t origin, mpfr_t oracle);


#endif //MAXFPEED_COMMON_H

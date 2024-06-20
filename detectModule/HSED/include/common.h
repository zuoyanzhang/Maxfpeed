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

/**
 * Function to compute the Unit in the Last Place (ULP) of a given double precision floating-point number.
 *
 * Note: This function assumes that the input number is a valid double precision floating-point number.
 */
double computeULP(double y);
float computeULPf(float y);

/**
 * Function to compute the difference in ULP (Unit in the Last Place) between two given double precision floating-point numbers.
 *
 * Note: This function assumes that the input numbers are valid double precision floating-point numbers.
 *       The ULP difference is calculated using the MPFR library for higher precision arithmetic.
 */
double computeULPDiff(mpfr_t origin, mpfr_t oracle);
float computeULPFDiff(mpfr_t origin, mpfr_t oracle);


#endif //MAXFPEED_COMMON_H

//
// Created by 99065 on 2024/3/15.
//
/**
 * create Eiffel's input
 */
#ifndef ERRORDETECTION_CREATEINPUT_H
#define ERRORDETECTION_CREATEINPUT_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
/**
 * generating real number uniformly distributed datasets
 * @param left_endpoint
 * @param right_endpoint
 * @return
 */
const std::vector<double> &uniformDistribution(double left_endpoint, double right_endpoint, int size, std::vector<double> &vec);

/**
 * generating floating point number distributed datasets
 * @param left_endpoint
 * @param right_endpoint
 * @return
 */
std::vector<double> random(double left, double right);

//const std::vector<double> &floatDistribution(double left_endpoint, double right_endpoint, int size, std::vector<double> &vec);

#endif //ERRORDETECTION_CREATEINPUT_H



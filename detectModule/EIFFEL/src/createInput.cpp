//
// Created by 99065 on 2024/3/15.
//
#include "EIFFEL/include/createInput.h"

typedef union {
    double d;
    unsigned long int i;
} DL;

const std::vector<double> &uniformDistribution(double left_endpoint, double right_endpoint, int size, std::vector<double> &vec) {
    double step = (right_endpoint - left_endpoint) / size;
    for (int i = 0; i < size; ++i) {
        vec.push_back(left_endpoint + i * step);
    }
    return vec;
}

std::vector<double> random(double left, double right) {
    std::vector<double> result;
    double x = 0.0;
    for (int i = 0; i < 500000; ++i) {
        x = left + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right - left)));
        result.push_back(x);
    }
    return result;
}

/*
const std::vector<double> &floatDistribution(double left_endpoint, double right_endpoint, int size, std::vector<double> &vec) {
    DL dl1, dl2;
    dl1.d = left_endpoint;
    dl2.d = right_endpoint;
    if (left_endpoint >= 0) {
        unsigned long int step = (dl2.i - dl1.i) / size;
        for (int i = 0; i < size; ++i) {
            DL temp;
            temp.i = dl1.i + i * step;
            vec.push_back(temp.d);
        }
    } else if (right_endpoint <= 0) {
        dl1.d = -left_endpoint;
        dl2.d = right_endpoint;
        unsigned long int step = (dl1.i - dl2.i) / size;
        for (int i = 0; i < size; ++i) {
            DL temp;
            temp.i = dl2.i + i * step;
            vec.push_back(-temp.d);
        }
    }
    return vec;
}*/

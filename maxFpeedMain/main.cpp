//
// Created by 99065 on 2023/10/19.
//
#include "HSED/include/float.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include "EIFFEL/include/errordetect.h"

#define TRUE 0
#define FALSE -1

using std::string;
using std::cin;
using std::cout;
using std::vector;
using std::endl;

typedef union {
    int i;
    float f;
} FL;

typedef union {
    long int i;
    double d;
} DL;

int main(int argc, char *argv[]) {
    if (argc == 4) {
        if (string(argv[3]) == "--HSED") { // HSED method
            cout << "---------------------------------------" << endl;
            cout << "| step2: detect FP errors              |" << endl;
            cout << "| now you choose HSED method to detect |" << endl;
            cout << "---------------------------------------" << endl;
            double start = atof(argv[1]);
            double end = atof(argv[2]);
            if (start > end) {
                cout << "start > end, input range error, you have to input start <= end" << endl;
                return FALSE;
            } else if (end <= 0) {
                DoubleFunction::detectMethod2_1(start, end);
            } else if (start < 0 && end > 0) {
                DoubleFunction::detectMethod2_2(start, end);
            } else {
                DoubleFunction::detectMethod2(start, end);
            }
        } else if (string(argv[3]) == "--EIFFEL") { // EIFFEL method
            cout << "-----------------------------------------" << endl;
            cout << "| step2: detect FP errors                |" << endl;
            cout << "| now you choose EIFFEL method to detect |" << endl;
            cout << "-----------------------------------------" << endl;
            double left_number = atof(argv[1]);
            double right_number = atof(argv[2]);
            eiffel(left_number, right_number);
        } else {
            cout << "please input right argument: --HSED or --EIFFEL" << endl;
            return FALSE;
        }
    } else {
        cout << "please input 4 argument" << "\n"
            << "The first argument is the f(x)" << "\n"
            << "The second argument is the interval of left" << "\n"
            << "The third argument is the interval of right" << "\n"
            << "The fourth argument is the detect method (--HSED or --EIFFEL)" << endl;
        return FALSE;
    }
    return TRUE;
}
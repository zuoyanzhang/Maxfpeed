//
// Created by 99065 on 2024/3/15.
//
#include "EIFFEL/include/errordetect.h"
#include "EIFFEL/src/getMultiResult.cpp"
#include "EIFFEL/include/createInput.h"
#include "EIFFEL/include/DBSCAN.h"

#define true 0
#define false -1
#define SIZE 500000
#define LEFTR 0.01
#define RIGHTR 100
#define LEFTL -100
#define RIGHTL -0.01
#define RADIO 0.7
#define G 500
#define ULP 0.5

/**
 * use this function when the detected expression is a single parameter
 * @param x
 * @param vec
 * @return
 */
const std::vector<std::vector<double>> &oneParaErrorDetect(double x, std::vector<std::vector<double>> &vec) {
    double origin = getComputeValueE(x);
    double ulp = getULPE(x, origin);

    if (std::isnan(ulp) || std::isinf(ulp)) {
        vec.emplace_back(std::vector<double>{x, 0.0});
    } else {
        vec.emplace_back(std::vector<double>{x, ulp});
    }
    return vec;
}
/**
 * return the c/g (g=500) error points, so finally we can get above 1000 error pink points
 * @param vec
 * @param vecAfter
 * @return
 */
const std::vector<std::vector<double>> &handleErrorPoint(std::vector<std::vector<double>> &vec, std::vector<std::vector<double>> &vecAfter) {
    for (size_t i = 0; i < vec.size(); i += G) {
        double maxULP = 0;
        std::vector<double> maxRow = {0.0, 0.0};

        for (size_t j = i; j < i + G && j < vec.size(); ++j) {
            if (vec[j][1] > maxULP) {
                maxULP = vec[j][1];
                maxRow = vec[j];
            }
        }

        if (maxULP >= ULP) {
            vecAfter.push_back(maxRow);
        }
    }
    return vecAfter;
}

int geneDBSCANFile(vector<vector<double>> vec) {
    ofstream file_clean("./detectModule/EIFFEL/src/galaxy.csv", ios_base::out);
    ofstream ofs("./detectModule/EIFFEL/src/galaxy.csv", ios::app);
    ofs << "x,ulp" << "\n";
    for (int i = 0; i < vec.size(); ++i) {
        ofs << vec[i][0] << "," << vec[i][1] << "\n";
    }
    file_clean.close();
    ofs.close();
    return true;
}

/**
 * using the C API interface to call dbscan.py, return Minpts and e parameters
 * @return std::pair<int, double>, the first parameter is Minpts, the second parameter is eps
 */
std::pair<int, double> callDbscanPy() {
    FILE *pipe = popen("python3 ./detectModule/EIFFEL/src/dbscan.py", "r");
    if (!pipe) {
        std::cout << "couldn't start command" << std::endl;
    }
    char buffer[128];
    std::string str = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr) {
            str += buffer;
        }
    }
    pclose(pipe);
    int a;
    double b;
    if (sscanf(str.c_str(), "%d %lf", &a, &b) == 2) {
        return std::make_pair(a, b);
    } else {
        std::cerr << "error parsing output." << std::endl;
    }
    return std::make_pair(false, false);
}
/**
 * determine whether it is monotonically increasing
 * @param vec
 * @return
 */
bool isMonotonousUp(const std::vector<std::vector<double>> &vec) {
    for (int i = 0; i < vec.size() - 1; ++i) {
        if (vec[i][1] < vec[i + 1][1]) {
            return true;
        }
    }
    return false;
}
/**
 * determine whether it is monotonically decreasing
 * @param vec
 * @return
 */
bool isMonotonousDown(const std::vector<std::vector<double>> &vec) {
    for (int i = 0; i < vec.size() - 1; ++i) {
        if (vec[i][1] > vec[i + 1][1]) return true;
    }
    return false;
}
/**
 * fit the function to obtain the x1 and proportional parameters of the function
 * @return vector<double> x1 and the q
 */
std::vector<double> getCurveFittingParas() {
    std::vector<double> result;
    FILE *pipe = popen("python3 ./detectModule/EIFFEL/src/curvefitting.py", "r");
    if (!pipe) {
        std::cerr << "couldn't start command." << std::endl;
        return result;
    }
    char buffer[256];
    std::string str = "";

    while (!feof(pipe)) {
        if (fgets(buffer, 256, pipe) != nullptr) {
            str += buffer;
        }
    }
    pclose(pipe);
    double a, b, c, d;
    if (sscanf(str.c_str(), "%lf %lf %lf %lf", &a, &b, &c, &d) == 4) {
        result.push_back(a);
        result.push_back(b);
        result.push_back(c);
        result.push_back(d);
    } else {
        std::cerr << "error parsing output." << std::endl;
    }
    return result;
}

/**
 * prediction interval
 * @param x1  lower bound's x1
 * @param q1 lower bound's q1
 * @param x2 upper bound's x2
 * @param q2 upper bound's q2
 * @return
 */
std::vector<std::vector<double>> predictInterval(double x1, double q1, double x2, double q2, double left_endpoint, double right_endpoint) {
    std::vector<std::vector<double>> result;
    double q1_prev = std::pow(q1, -31);
    double q2_prev = std::pow(q2, -31);
    const double threshold = 10e-06;

    for (int i = -30; i < 100; ++i) {
        q1_prev *= q1;
        q2_prev *= q2;
        double lower = x1 * q1_prev;
        double upper = x2 * q2_prev;

        if (lower > upper) {
            std::swap(lower, upper);
        }

        if (upper < left_endpoint || lower > right_endpoint) {
            continue;
        }

        lower = std::max(lower, left_endpoint);
        upper = std::min(upper, right_endpoint);

        if (upper - lower >= threshold) {
            result.push_back({lower, upper});
        }
    }

    return result;
}

bool compareByUlp(const std::vector<double> &a, const std::vector<double> &b) {
    return a[1] < b[1];
}
bool compareByUlp2(const std::vector<double> &a, const std::vector<double> &b) {
    return a[2] < b[2];
}
bool compareByUlp3(const std::vector<double> &a, const std::vector<double> &b) {
    return a[3] < b[3];
}
bool compareByUlp4(const std::vector<double> &a, const std::vector<double> &b) {
    return a[4] < b[4];
}
bool compareByUlp5(const std::vector<double> &a, const std::vector<double> &b) {
    return a[5] < b[5];
}
/**
 * detect all predicted cell intervals and return the maximum error
 * @param left
 * @param right
 * @return
 */
std::vector<double> detectAllIntervalGetMaximumError(std::vector<std::vector<double>> vec_interval) {
    std::vector<double> result;
    std::vector<std::vector<double>> temp;
    for (auto currentInterval : vec_interval) {
        std::vector<std::vector<double>> vecUniform;
        double left = currentInterval[0];
        double right = currentInterval[1];
        std::vector<double> inputx;
        inputx = uniformDistribution(left, right, 500000, inputx);

        for (double x : inputx) {
            vecUniform  = oneParaErrorDetect(x, vecUniform);
        }

        // 这次for循环结束之后，vec里存了第一个区间的500000个误差点x ulp。现在需要得到这五十万个最大的那个误差点。存到一个数组里。
        auto maxUlpRow = max_element(vecUniform.begin(), vecUniform.end(), compareByUlp);
        temp.emplace_back(std::vector<double>{(*maxUlpRow)[0], (*maxUlpRow)[1]});
    }

    auto maxULP = max_element(temp.begin(), temp.end(), compareByUlp);
    result.push_back((*maxULP)[0]);
    result.push_back((*maxULP)[1]);
    return result;
}


/**
 * return the [left, right] interval's maximum error
 * @param left
 * @param right
 * @return
 */
std::vector<double> obtainMaxUlpInterval(double left, double right) {
    std::vector<double> result;
    std::vector<double> x_uniform;
    std::vector<std::vector<double>> x_uniform_vec;
    x_uniform = uniformDistribution(left, right, SIZE, x_uniform);
    for (int i = 0; i < x_uniform.size(); ++i) {
        x_uniform_vec = oneParaErrorDetect(x_uniform[i], x_uniform_vec);
    }
    auto uniformMaxError = max_element(x_uniform_vec.begin(), x_uniform_vec.end(), compareByUlp);
    result.push_back((*uniformMaxError)[0]);
    result.push_back((*uniformMaxError)[1]);
    return result;
}

/**
 * print the maximum error to the screen
 * @param vec
 */
void printMaxError(std::vector<double> &vec) {
    if (vec[1] == 0) std::cout << "x = " << vec[0] << ", maximum ULP = NaN or Inf" << std::endl;
    else std::cout << "x = " << setprecision(16) << vec[0] << ", maximum ULP = " << setprecision(5) << vec[1] << std::endl;
}

void printMaxError2(std::vector<double> vec) {
    if (vec[2] == 0) std::cout << "x1 = " << vec[0] << ", x2 = " << vec[1] << ", maximum ULP = NaN or Inf" << std::endl;
    else std::cout << "x1 = " << setprecision(16) << vec[0] << ", x2 = " << setprecision(16) << vec[1] << ", maximum ULP = " << setprecision(5) << vec[2] << std::endl;
}

void printMaxError3(std::vector<double> &vec) {
    if (vec[3] == 0) std::cout << "x1 = " << vec[0] << ", x2 = " << vec[1] << ", x3 = " << vec[2] << ", maximum ULP = NaN or Inf" << std::endl;
    else std::cout << "x1 = " << setprecision(16) << vec[0] << ", x2 = " << setprecision(16) << vec[1] << ", x3 = " << setprecision(16) << vec[2] << ", maximum ULP = " << setprecision(5) << vec[3] << std::endl;
}

void printMaxError4(std::vector<double> &vec) {
    if (vec[4] == 0) std::cout << "x1 = " << vec[0] << ", x2 = " << vec[1] << ", x3 = " << vec[2]
                                << ", x4 = " << vec[3] << ", maximum ULP = NaN or Inf" << std::endl;
    else std::cout << "x1 = " << setprecision(16) << vec[0] << ", x2 = " << setprecision(16) << vec[1]
                << ", x3 = " << setprecision(16) << vec[2] << ", x4 = " << setprecision(16)
                << vec[3] << ", maximum ULP = " << setprecision(5) << vec[4] << std::endl;
}

void printMaxError5(std::vector<double> &vec) {
    if (vec[4] == 0) std::cout << "x1 = " << vec[0] << ", x2 = " << vec[1] << ", x3 = " << vec[2]
                               << ", x4 = " << vec[3] << ", x5 = " << vec[4] << ", maximum ULP = NaN or Inf" << std::endl;
    else std::cout << "x1 = " << setprecision(16) << vec[0] << ", x2 = " << setprecision(16) << vec[1]
                   << ", x3 = " << setprecision(16) << vec[2] << ", x4 = " << setprecision(16)
                   << vec[3] <<
                   ", x5 = " << setprecision(16) << vec[4] << ", maximum ULP = " << setprecision(5) << vec[5] << std::endl;
}
/**
 * address [0,right_endpoint] and [left_endpoint, 0]
 * @param left_endpoint
 * @param right_endpoint
 * @return
 */
int detectNoSpan(double left_endpoint, double right_endpoint) {
    vector<double> vec; // store the input x
    vector<vector<double>> vec2, vec3; // vec2 store the input x and the corresponding ULP, vec3 store after handle error points
    if (left_endpoint >= 0) {
        uniformDistribution(LEFTR, RIGHTR, SIZE, vec);
    }
    if (right_endpoint <= 0) {
        uniformDistribution(LEFTL, RIGHTL, SIZE, vec);
    }
    for (int i = 0; i < vec.size(); ++i) {
        oneParaErrorDetect(vec[i],vec2);
    }
    std::cout << "=====1. Data set construction completed!=====" << std::endl;
    handleErrorPoint(vec2, vec3);
    std::cout << "=====2. Boundary extraction completed!=====" << std::endl;
    // 现在经过误差处理之后已经得到了1000个最大误差点，后面需要执行dbscan.py脚本来确定Minpts和epsilon参数
    geneDBSCANFile(vec3); // 生成dbscan.py的输入文件，用于确定minpts和epsilon参数
    std::cout << "=====3. Automatic determination of MinPts and Eps parameters completed!=====" << std::endl;
    std::pair<int, double> p = callDbscanPy(); // 执行dbscan.py脚本，得到MinPts和epsilon参数
    std::cout << "MinPts: " << p.first << ", Eps: " << p.second << std::endl;
    if (p.first == 0 && p.second == 0) {
        // 直接返回R的最大误差
        std::cout << "=====4. DBSCAN clusters less than 3, return R's maximum error!=====" << std::endl;
        if (right_endpoint > 0) {
            std::vector<double> error = obtainMaxUlpInterval(LEFTR, RIGHTR);
            printMaxError(error);
        }
        if (left_endpoint < 0) {
            std::vector<double> error = obtainMaxUlpInterval(LEFTL, RIGHTL);
            printMaxError(error);
        }
    } else {
        std::cout << "=====4. DBSCAN completed and successfully!=====" << std::endl;
        std::cout << "no." << std::setw(10) << "x" << std::setw(13) << "ULP" << endl;
        std::vector<std::vector<double>> vec_dbscan = DBSCAN(p.second, p.first);
        for (int i = 0; i < vec_dbscan.size(); ++i) {
            std::cout << i + 1 << std::setw(15) << vec_dbscan[i][0] << std::setw(13) << vec_dbscan[i][1] << std::endl;
        }

        if (isMonotonousUp(vec_dbscan)) {
            // 单调递增
            if (right_endpoint > 0) {
                std::vector<double> error = obtainMaxUlpInterval(right_endpoint * RADIO, right_endpoint);
                std::cout << "Function monotonically increasing, directly detecting endpoint intervals" << std::endl;
                printMaxError(error);
            } else {
                std::vector<double> error = obtainMaxUlpInterval(left_endpoint * (1 - RADIO), right_endpoint);
                std::cout << "Function monotonically increasing, directly detecting endpoint intervals" << std::endl;
                printMaxError(error);
            }
        } else if (isMonotonousDown(vec_dbscan)) {
            // 单调递减
            if (right_endpoint > 0) {
                std::vector<double> error = obtainMaxUlpInterval(left_endpoint, right_endpoint * (1 - RADIO));
                std::cout << "Function monotonically increasing, directly detecting endpoint intervals" << std::endl;
                printMaxError(error);
            } else {
                std::vector<double> error = obtainMaxUlpInterval(left_endpoint, right_endpoint * RADIO);
                std::cout << "Function monotonically increasing, directly detecting endpoint intervals" << std::endl;
                printMaxError(error);
            }
        } else {
            // 非单调，执行曲线拟合
            std::vector<double> vec_curve = getCurveFittingParas();
            std::cout << "=====5. Curve function completed!=====" << std::endl;
            std::cout << "The lower bound function is: " << "x_i = " << vec_curve[0] << " * " << "pow(" << vec_curve[1]
                      << ", i - 1),   i >= 1" << std::endl;
            std::cout << "The upper bound function is: " << "x_i = " << vec_curve[2] << " * " << "pow(" << vec_curve[3]
                      << ", i - 1),   i >= 1" << std::endl;
            std::vector<std::vector<double>> vec_interval;
            vec_interval = predictInterval(vec_curve[0], vec_curve[1], vec_curve[2], vec_curve[3], left_endpoint, right_endpoint);
            std::cout << "=====6. Prediction interval completed!=====" << "\n"
                    << "Above are the intervals: " << std::endl;
            for (int i = 0; i < vec_interval.size(); ++i) {
                std::cout << "[" << vec_interval[i][0] << ", " << vec_interval[i][1] << "]" << std::endl;
            }
            std::cout << "Please wait, error detection is currently underway~" << std::endl;
            std::vector<double> maxError = detectAllIntervalGetMaximumError(vec_interval);
            std::cout << "=====7. Error detection completed!=====" << std::endl;
            printMaxError(maxError);
        }
    }
    return 0;
}

int getNumberOfParameters(const std::string &filePath) {
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::regex pattern(R"delim(double\s+(getULPE(\d+)?)[^\)]*\)[^{]*\{[^}]*return ulp;[^}]*\})delim");
    std::smatch matches;
    std::string::const_iterator searchStart(content.cbegin());
    int maxNumber = 0;
    while (std::regex_search(searchStart, content.cend(), matches, pattern)) {
        if (matches.size() >= 3) {
            std::string numberStr = matches[2].str();
            int currentNumber = numberStr.empty() ? 0 : std::stoi(numberStr);
            maxNumber = std::max(maxNumber, currentNumber);
        }
        searchStart = matches.suffix().first;
    }
    return maxNumber;
}

/**
 * the eiffel's interface
 * @param left_endpoint
 * @param right_endpoint
 * @return
 */
int onePara(double left, double right) {
    if (left >= right) {
        std::cout << "ERROR: left endpoint must be smaller than right endpoint" << std::endl;
        return false;
    }

    if ((left < 0 && right > 0) || (right < 0 && left > 0)) {
        std::cout << "ERROR: left endpoint must be >= 0, or right endpoint <= 0" << std::endl;
        return false;
    }

    if ((left >= 0 && right < 100) || (right <= 0 && left > -100) || (std::abs(left - right) < 100)) {
            std::cout << "Detection interval is less than R, directly return the maximum error" << std::endl;
            std::vector<double> maxError = obtainMaxUlpInterval(left, right);
            printMaxError(maxError);
    } else {
        detectNoSpan(left, right);
    }

    return true;
}

// 双参表达式检测函数定义如下
/**
 * return x1, x2, ulp
 * @param x1
 * @param x2
 * @return
 */
std::vector<std::vector<double>> twoParaErrorDetect(double x1, double x2) {
    std::vector<std::vector<double>> result;
    double origin = getComputeValueE2(x1, x2);
    double ulp = getULPE2(x1, x2, origin);
    if (std::isnan(ulp) || std::isinf(ulp)) {
        result.emplace_back(std::vector<double>{x1, x2, 0.0});
    } else {
        result.emplace_back(std::vector<double>{x1, x2, ulp});
    }
    return result;
}
/**
 * return the 500000 [x1, x2, ulp] error points
 * @param left
 * @param right
 * @return
 */
std::vector<std::vector<double>> getTwoParaErrorAllPoint(double left_x1, double right_x1, double left_x2, double right_x2) {
    std::vector<std::vector<double>> result;
    double x1, x2;
    for (int i = 0; i < SIZE; ++i) {
        x1 = left_x1 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right_x1 - left_x1)));
        x2 = left_x2 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right_x2 - left_x2)));
        std::vector<std::vector<double>> vec = twoParaErrorDetect(x1, x2);
        result.insert(result.end(), std::make_move_iterator(vec.begin()), std::make_move_iterator(vec.end()));
    }
    return result;
}
/**
 * return the 1000 points x1 x2
 * @param vec
 * @return
 */
std::vector<std::vector<double>> handleErrorPoint2(std::vector<std::vector<double>> vec) {
    std::vector<std::vector<double>> result;
    for (size_t i = 0; i < vec.size(); i += G) {
        double maxULP = 0;
        std::vector<double> maxRow = {0.0, 0.0, 0.0};
        for (size_t j = i; j < i + G && j < vec.size(); ++j) {
            if (vec[j][2] > maxULP) {
                maxULP = vec[j][2];
                maxRow = vec[j];
            }
        }
        if (maxULP >= ULP) {
            result.push_back(maxRow);
        }
    }
    return result;
}

std::vector<double> obtainMaxUlpInterval2(double left, double right) {
    std::vector<double> result;
    std::vector<std::vector<double>> vec = getTwoParaErrorAllPoint(left, right, left, right);
    auto maxError = max_element(vec.begin(), vec.end(), compareByUlp2);
    result.push_back(maxError->at(0));
    result.push_back(maxError->at(1));
    result.push_back(maxError->at(2));
    return result;
}

std::vector<double> detectAllIntervalGetMaximumError2(std::vector<std::vector<double>> vec_interval, double left, double right, int sign) {
    std::vector<double> result;
    std::vector<std::vector<double>> temp;
    for (auto currentInterval : vec_interval) {
        std::vector<std::vector<double>> vec_random;
        std::vector<double> x1, x2;
        double x_left = currentInterval[0];
        double x_right = currentInterval[1];
        if (sign == 0) {
            x1 = random(x_left, x_right);
            x2 = random(left, right);
        }
        if (sign == 1) {
            x1 = random(left, right);
            x2 = random(x_left, x_right);
        }
        for (int i = 0; i < x1.size(); ++i) {
            vec_random = twoParaErrorDetect(x1[i], x2[i]);
        }
        auto maxUlpRow = max_element(vec_random.begin(), vec_random.end(), compareByUlp2);
        temp.push_back(std::vector<double>{maxUlpRow->at(0), maxUlpRow->at(1), maxUlpRow->at(2)});
    }
    auto maxULP = max_element(temp.begin(), temp.end(), compareByUlp2);
    result.push_back(maxULP->at(0));
    result.push_back(maxULP->at(1));
    result.push_back(maxULP->at(2));
    return result;
}

void createCurvePointsTwoPara(std::vector<std::vector<double>> v) {
    sort(v.begin(), v.end(), compareByUlp);
    for (int i = 0; i < v.size(); ++i) {
        std::cout << "[" << i + 1 << ", " << v[i][1] << "]" << std::endl;
    }
    ofstream file_clean("./detectModule/EIFFEL/src/curvepoint.csv", ios_base::out);
    ofstream ofs("./detectModule/EIFFEL/src/curvepoint.csv", ios::app);
    for (int i = 0; i < v.size(); i++) {
        ofs << i + 1 << " ";
    }
    ofs << "\n";

    if (v[0][1] > 0) {
        for (int i = 0; i < v.size(); i++) {
            if (i == 0) {
                if (v[i][1] < 2.0) {
                    ofs << v[i][1] << " ";
                } else {
                    ofs << v[i][1] - 1 << " ";
                }
            } else {
                ofs << v[i][1] - 1 << " ";
            }
        }

        ofs << "\n";

        for (int i = 0; i < v.size(); ++i) {
            ofs << v[i][1] + 1 << " ";
        }
    } else {
        for (int i = v.size() - 1; i >= 0; --i) {
            if (i == v.size() - 1) {
                if (v[i][1] > -2.0) {
                    ofs << v[i][1] << " ";
                } else {
                    ofs << v[i][1] + 1 << " ";
                }
            } else {
                ofs << v[i][1] + 1 << " ";
            }
        }

        ofs << "\n";
        for (int i = v.size() - 1; i >= 0; --i) {
            ofs << v[i][1] - 1 << " ";
        }
    }
    file_clean.close();
    ofs.close();
}

int detectNoSpan2(double left, double right) {
    std::vector<std::vector<double>> vecErrorDataSet;
    // 先生成x1和x2在[0.01,100]的输入集
    if (left >= 0) {
        // 得到[0.01,100]区间内500000个误差数据点，数据格式是x1 x2 ulp
        vecErrorDataSet = getTwoParaErrorAllPoint(LEFTR, RIGHTR, LEFTR, RIGHTR);
    }
    if (right <= 0) {
        // 得到[-100,-0.01]区间内500000个误差数据点，数据格式是x1 x2 ulp
        vecErrorDataSet = getTwoParaErrorAllPoint(LEFTL, RIGHTL, LEFTL, RIGHTL);
    }
    std::cout << "=====1. Data set construction completed!=====" << std::endl;
    // 目前得到了在起始R区间内的500000个误差数据点，接着要执行数据处理，只保留前1000个误差最大点
    vecErrorDataSet = handleErrorPoint2(vecErrorDataSet);
    std::cout << "=====2. Boundary extraction completed!=====" << std::endl;
    // 现在得到了起始小区间内1000个用于数据聚类的误差点，且vecErrorDataSet中存放的是xy面的数据
    geneDBSCANFile(vecErrorDataSet);
    // 执行dbscan.py脚本，得到MinPts和epsilon参数
    std::pair<int, double> p = callDbscanPy();
    std::cout << "=====3. Automatic determination of MinPts and Eps parameters completed!=====" << std::endl;
    if (p.first == 0 && p.second == 0) {
        std::cout << "=====4. DBSCAN clusters less than 3, return R's maximum error!=====" << std::endl;
        if (left >= 0) {
            std::vector<double> maxerror = obtainMaxUlpInterval2(LEFTR, RIGHTR);
            printMaxError2(maxerror);
            return true;
        }
        if (right <= 0) {
            std::vector<double> maxerror = obtainMaxUlpInterval2(LEFTL, RIGHTL);
            printMaxError2(maxerror);
            return true;
        }
    } else {
        std::vector<std::vector<double>> vec_dbscan = DBSCAN2(p.second, p.first);
        if (vec_dbscan[0][0] == 0 || vec_dbscan[0][1] == 0) {
            std::cout << "=====4. DBSCAN clusters less than 3, return R's maximum error!=====" << std::endl;
            std::cout << "MinPts: " << p.first << ", Eps: " << p.second << std::endl;
            if (left >= 0) {
                std::vector<double> maxError = obtainMaxUlpInterval2(LEFTR, RIGHTR);
                printMaxError2(maxError);
                return true;
            }
            if (right <= 0) {
                std::vector<double> maxError = obtainMaxUlpInterval2(LEFTL, RIGHTL);
                printMaxError2(maxError);
                return true;
            }
        }
        std::cout << "=====4. DBSCAN completed and successfully!=====" << std::endl;
//        for (int i = 0; i < vec_dbscan.size(); ++i) {
//            std::cout << "[" << i + 1 << ", " << vec_dbscan[i][0] << "]" << std::endl;
//        }
        std::cout << "Now fit the function along the x1-axis" << std::endl;
        std::vector<double> vec_curve = getCurveFittingParas();
        std::cout << "=====5. Curve function completed!=====" << std::endl;
        std::cout << "The lower bound function is: " << "x_i = " << vec_curve[0] << " * " << "pow(" << vec_curve[1]
                << ", i - 1),   i >= 1" << std::endl;
        std::cout << "The upper bound function is: " << "x_i = " << vec_curve[2] << " * " << "pow(" << vec_curve[3]
                << ", i - 1),   i >= 1" << std::endl;
        std::vector<std::vector<double>> vec_interval;
        vec_interval = predictInterval(vec_curve[0], vec_curve[1], vec_curve[2], vec_curve[3], left, right);
        std::cout << "=====6. Prediction interval completed!=====" << std::endl;
        for (int i = 0; i < vec_interval.size(); ++i) {
            std::cout << "[" << vec_interval[i][0] << ", " << vec_interval[i][1] << "]" << std::endl;
        }
        std::cout << "Please wait, error detection is currently underway~" << std::endl;
        std::vector<double> maxError = detectAllIntervalGetMaximumError2(vec_interval, left, right, 0);
        // 到此得到了沿着x轴拟合检测的最大误差maxError[2]
        // 执行沿着y轴预测
        std::cout << "Now fit the function along the x2-axis" << std::endl;
        createCurvePointsTwoPara(vec_dbscan);
        std::vector<double> vec_curve_y = getCurveFittingParas();
        std::cout << "=====5. Curve function completed!=====" << std::endl;
        std::cout << "The lower bound function is: " << "x_i = " << vec_curve_y[0] << " * " << "pow(" << vec_curve_y[1]
                  << ", i - 1),   i >= 1" << std::endl;
        std::cout << "The upper bound function is: " << "x_i = " << vec_curve_y[2] << " * " << "pow(" << vec_curve_y[3]
                  << ", i - 1),   i >= 1" << std::endl;
        std::vector<std::vector<double>> vec_interval_y;
        vec_interval_y = predictInterval(vec_curve_y[0], vec_curve_y[1], vec_curve_y[2], vec_curve_y[3], left, right);
        std::cout << "=====6. Prediction interval completed!=====" << std::endl;
        for (int i = 0; i < vec_interval_y.size(); ++i) {
            std::cout << "[" << vec_interval_y[i][0] << ", " << vec_interval_y[i][1] << "]" << std::endl;
        }
        std::cout << "Please wait, error detection is currently underway~" << std::endl;
        std::vector<double> maxError_y = detectAllIntervalGetMaximumError2(vec_interval_y, left, right, 1);
        if (maxError[2] > maxError_y[2]) {
            std::cout << "=====7. Error detection completed!=====" << std::endl;
            std::cout << "The error in the x1 dimension is greater, the maximum error is: " << std::endl;
            printMaxError2(maxError);
        } else {
            std::cout << "=====7. Error detection completed!=====" << std::endl;
            std::cout << "The error in the x2 dimension is greater, the maximum error is: " << std::endl;
            printMaxError2(maxError_y);
        }
    }
    return true;
}

int twoPara(double left, double right) {
    if (left >= right) {
        std::cout << "ERROR: left endpoint must be smaller than right endpoint" << std::endl;
        return false;
    }
    if ((left < 0 && right > 0) || (right < 0 && left > 0)) {
        std::cout << "ERROR: left endpoint must be >= 0, or right endpoint <= 0" << std::endl;
        return false;
    }
    if ((left >= 0 && right < 100) || (right <= 0 && left > -100) || (std::abs(left - right) < 100)) {
        std::cout << "Detection interval is less than R, directly return the maximum error" << std::endl;
        std::vector<double> maxError = obtainMaxUlpInterval2(left, right);
        printMaxError2(maxError);
    } else {
        detectNoSpan2(left, right);
    }
    return true;
}

std::vector<std::vector<double>> getThreeParaErrorAllPoint(double left_x1, double right_x1, double left_x2, double right_x2, double left_x3, double right_x3) {
    std::vector<std::vector<double>> result;
    double x1, x2, x3;
    for (int i = 0; i < SIZE; ++i) {
        x1 = left_x1 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right_x1 - left_x1)));
        x2 = left_x2 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right_x2 - left_x2)));
        x3 = left_x3 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right_x3 - left_x3)));
        std::vector<std::vector<double>> vec = threeParaErrorDetect(x1, x2, x3);
        result.insert(result.end(), std::make_move_iterator(vec.begin()), std::make_move_iterator(vec.end()));
    }
    return result;
}

std::vector<std::vector<double>> threeParaErrorDetect(double x1, double x2, double x3) {
    std::vector<std::vector<double>> result;
    double origin = getComputeValueE3(x1, x2, x3);
    double ulp = getULPE3(x1, x2, x3, origin);
    if (std::isnan(ulp) || std::isinf(ulp)) {
        result.emplace_back(std::vector<double>({x1, x2, x3, 0.0}));
    } else {
        result.emplace_back(std::vector<double>({x1, x2, x3, ulp}));
    }
    return result;
}

std::vector<double> obtainMaxUlpInterval3(double left, double right) {
    std::vector<double> result;
    std::vector<std::vector<double>> vec = getThreeParaErrorAllPoint(left, right, left, right, left, right);
    auto maxError = max_element(vec.begin(), vec.end(), compareByUlp2);
    result.push_back(maxError->at(0));
    result.push_back(maxError->at(1));
    result.push_back(maxError->at(2));
    result.push_back(maxError->at(3));
    return result;
}

std::vector<std::vector<double>> handleErrorPoint3(std::vector<std::vector<double>> vec) {
    std::vector<std::vector<double>> result;
    for (size_t i = 0; i < vec.size(); i += G) {
        double maxULP = 0;
        std::vector<double> maxRow = {0.0, 0.0, 0.0, 0.0};
        for (size_t j = i; j < i + G && j < vec.size(); ++j) {
            if (vec[j][3] > maxULP) {
                maxULP = vec[j][3];
                maxRow = vec[j];
            }
        }
        if (maxULP >= ULP) {
            result.push_back(maxRow);
        }
    }
    return result;
}

std::vector<double> detectAllIntervalGetMaximumError3(std::vector<std::vector<double>> vec_interval, double left, double right) {
    std::vector<double> result;
    std::vector<std::vector<double>> temp;
    for (auto currentInterval : vec_interval) {
        std::vector<std::vector<double>> vec_random;
        std::vector<double> x1, x2, x3;
        double x_left = currentInterval[0];
        double x_right = currentInterval[1];
        x1 = random(x_left, x_right);
        x2 = random(left, right);
        x3 = random(left, right);
        for (int i = 0; i < x1.size(); ++i) {
            vec_random = threeParaErrorDetect(x1[i], x2[i], x3[i]);
        }
        auto maxUlpRow = max_element(vec_random.begin(), vec_random.end(), compareByUlp3);
        temp.push_back(std::vector<double>{maxUlpRow->at(0), maxUlpRow->at(1), maxUlpRow->at(2), maxUlpRow->at(3)});
    }
    auto maxULP = max_element(temp.begin(), temp.end(), compareByUlp3);
    result.push_back(maxULP->at(0));
    result.push_back(maxULP->at(1));
    result.push_back(maxULP->at(2));
    result.push_back(maxULP->at(3));
    return result;
}
int detectNoSpan3(double left, double right) {
    std::vector<std::vector<double>> vecErrorDataSet;
    if (left >= 0) {
        vecErrorDataSet = getThreeParaErrorAllPoint(LEFTR, RIGHTR, LEFTR, RIGHTR, LEFTR, RIGHTR);
    }
    if (right <= 0) {
        vecErrorDataSet = getThreeParaErrorAllPoint(LEFTL, RIGHTL, LEFTL, RIGHTL, LEFTL, RIGHTL);
    }
    std::cout << "=====1. Data set construction completed!=====" << std::endl;
    vecErrorDataSet = handleErrorPoint3(vecErrorDataSet);
    std::cout << "=====2. Boundary extraction completed!=====" << std::endl;
    geneDBSCANFile(vecErrorDataSet);
    std::pair<int, double> p = callDbscanPy();
    std::cout << "=====3. Automatic determination of MinPts and Eps parameters completed!=====" << std::endl;
    if (p.first == 0 && p.second == 0) {
        std::cout << "=====4. DBSCAN clusters less than 3, return R's maximum error!=====" << std::endl;
        if (left >= 0) {
            std::vector<double> maxerror = obtainMaxUlpInterval3(LEFTR, RIGHTR);
            printMaxError3(maxerror);
            return true;
        }
        if (right <= 0) {
            std::vector<double> maxerror = obtainMaxUlpInterval3(LEFTL, RIGHTL);
            printMaxError3(maxerror);
            return true;
        }
    } else {
        std::vector<std::vector<double>> vec_dbscan = DBSCAN(p.second, p.first);
        if (vec_dbscan[0][0] == 0 || vec_dbscan[0][1] == 0) {
            std::cout << "4. DBSCAN clusters less than 3, return R's maximum error!=====" << std::endl;
            std::cout << "MinPts: " << p.first << ", Eps: " << p.second << std::endl;
            if (left >= 0) {
                std::vector<double> maxerror = obtainMaxUlpInterval3(LEFTR, RIGHTR);
                printMaxError3(maxerror);
                return true;
            }
            if (right <= 0) {
                std::vector<double> maxerror = obtainMaxUlpInterval3(LEFTL, RIGHTL);
                printMaxError3(maxerror);
                return true;
            }
        }
        std::cout << "=====4. DBSCAN completed and successfully!=====" << std::endl;
//        for (int i = 0; i < vec_dbscan.size(); ++i) {
//            std::cout << "[" << i + 1 << ", " << vec_dbscan[i][0] << std::endl;
//        }
        std::cout << "Now fit the function" << std::endl;
        std::vector<double> vec_curve = getCurveFittingParas();
        std::cout << "=====5. Curve function completed!=====" << std::endl;
        std::cout << "The lower bound function is: " << "x_i = " << vec_curve[0] << " * " << "pow(" << vec_curve[1]
                << ", i - 1),   i >= 1" << std::endl;
        std::cout << "The upper bound function is: " << "x_i = " << vec_curve[2] << " * " << "pow(" << vec_curve[3]
                << ", i - 1),   i >= 1" << std::endl;
        std::vector<std::vector<double>> vec_interval;
        vec_interval = predictInterval(vec_curve[0], vec_curve[1], vec_curve[2], vec_curve[3], left, right);
        std::cout << "=====6. PredictInterval interval completed!=====" << std::endl;
        for (int i = 0; i < vec_interval.size(); ++i) {
            std::cout << "[" << vec_interval[i][0] << ", " << vec_interval[i][1] << "]" << std::endl;
        }
        std::cout << "Please wait, error detection is currently underway~" << std::endl;
        std::vector<double> maxerror = detectAllIntervalGetMaximumError3(vec_interval, left, right);
        std::cout << "=====7. Error detection completed!=====" << std::endl;
        printMaxError3(maxerror);
    }
    return true;
}

int threePara(double left, double right) {
    if (left >= right) {
        std::cout << "ERROR: left endpoint must be smaller than right endpoint" << std::endl;
        return false;
    }
    if ((left < 0 && right > 0) || (right < 0 && left > 0)) {
        std::cout << "ERROR: left endpoint must be >= 0, or right endpoint <= 0" << std::endl;
        return false;
    }
    if ((left >= 0 && right < 100) || (right <= 0 && left > -100) || (std::abs(left - right) < 100)) {
        std::cout << "Detection interval is less than R, directly return the maximum error" << std::endl;
        std::vector<double> maxError = obtainMaxUlpInterval3(left, right);
        printMaxError3(maxError);
    } else {
        detectNoSpan3(left, right);
    }
    return true;
}
std::vector<std::vector<double>> fourParaErrorDetect(double x1, double x2, double x3, double x4) {
    std::vector<std::vector<double>> result;
    double origin = getComputeValueE4(x1, x2, x3, x4);
    double ulp = getULPE4(x1, x2, x3, x4, origin);
    if (std::isnan(ulp) || std::isinf(ulp)) {
        result.emplace_back(std::vector<double>({x1, x2, x3, x4, 0.0}));
    } else {
        result.emplace_back(std::vector<double>({x1, x2, x3, x4, ulp}));
    }
    return result;
}

std::vector<std::vector<double>> getFourParaErrorAllPoint(double left, double right) {
    std::vector<std::vector<double>> result;
    double x1, x2, x3, x4;
    for (int i = 0; i < SIZE; ++i) {
        x1 = left + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right - left)));
        x2 = left + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right - left)));
        x3 = left + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right - left)));
        x4 = left + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right - left)));
        std::vector<std::vector<double>> vec = fourParaErrorDetect(x1, x2, x3, x4);
        result.insert(result.end(), std::make_move_iterator(vec.begin()), std::make_move_iterator(vec.end()));
    }
    return result;
}

std::vector<double> obtainMaxUlpInterval4(double left, double right) {
    std::vector<double> result;
    std::vector<std::vector<double>> vec = getFourParaErrorAllPoint(left, right);
    auto maxError = max_element(vec.begin(), vec.end(), compareByUlp4);
    result.push_back(maxError->at(0));
    result.push_back(maxError->at(1));
    result.push_back(maxError->at(2));
    result.push_back(maxError->at(3));
    result.push_back(maxError->at(4));
    return result;
}

std::vector<std::vector<double>> handleErrorPoint4(std::vector<std::vector<double>> vec) {
    std::vector<std::vector<double>> result;
    for (size_t i = 0; i < vec.size(); i += G) {
        double maxULP = 0;
        std::vector<double> maxRow = {0.0, 0.0, 0.0, 0.0, 0.0};
        for (size_t j = i; j < i + G && j < vec.size(); ++j) {
            if (vec[j][4] > maxULP) {
                maxULP = vec[j][4];
                maxRow = vec[j];
            }
        }
        if (maxULP >= ULP) {
            result.push_back(maxRow);
        }
    }
    return result;
}

std::vector<double> detectAllIntervalGetMaximumError4(std::vector<std::vector<double>> vec_interval, double left, double right) {
    std::vector<double> result;
    std::vector<std::vector<double>> temp;
    for (auto currentInterval : vec_interval) {
        std::vector<std::vector<double>> vec_random;
        std::vector<double> x1, x2, x3, x4;
        double x_left = currentInterval[0];
        double x_right = currentInterval[1];
        x1 = random(x_left, x_right);
        x2 = random(left, right);
        x3 = random(left, right);
        x4 = random(left, right);
        for (int i = 0; i < x1.size(); ++i) {
            vec_random = fourParaErrorDetect(x1[i], x2[i], x3[i], x4[i]);
        }
        auto maxUlpRow = max_element(vec_random.begin(), vec_random.end(), compareByUlp4);
        temp.push_back(std::vector<double>{maxUlpRow->at(0), maxUlpRow->at(1), maxUlpRow->at(2), maxUlpRow->at(3), maxUlpRow->at(4)});
    }
    auto maxULP = max_element(temp.begin(), temp.end(), compareByUlp4);
    result.push_back(maxULP->at(0));
    result.push_back(maxULP->at(1));
    result.push_back(maxULP->at(2));
    result.push_back(maxULP->at(3));
    result.push_back(maxULP->at(4));
    return result;
}

int detectNoSpan4(double left, double right) {
    std::vector<std::vector<double>> vecErrorDataSet;
    if (left >= 0) {
        vecErrorDataSet = getFourParaErrorAllPoint(LEFTR, RIGHTR);
    }
    if (right <= 0) {
        vecErrorDataSet = getFourParaErrorAllPoint(LEFTL, RIGHTL);
    }
    std::cout << "=====1. Data set construction completed!=====" << std::endl;
    vecErrorDataSet = handleErrorPoint4(vecErrorDataSet);
    std::cout << "=====2. Boundary extraction completed!=====" << std::endl;
    geneDBSCANFile(vecErrorDataSet);
    std::pair<int, double> p = callDbscanPy();
    std::cout << "=====3. Automatic determination of MinPts and Eps parameters completed!=====" << std::endl;
    if (p.first == 0 && p.second == 0) {
        std::cout << "=====4. DBSCAN clusters less than 3, return R's maximum error!=====" << std::endl;
        if (left >= 0) {
            std::vector<double> maxerror = obtainMaxUlpInterval4(LEFTR, RIGHTR);
            printMaxError4(maxerror);
            return true;
        }
        if (right <= 0) {
            std::vector<double> maxerror = obtainMaxUlpInterval4(LEFTL, RIGHTL);
            printMaxError4(maxerror);
            return true;
        }
    } else {
        std::vector<std::vector<double>> vec_dbscan = DBSCAN(p.second, p.first);
        if (vec_dbscan[0][0] == 0 || vec_dbscan[0][1] == 0) {
            std::cout << "4. DBSCAN clusters less than 3, return R's maximum error!=====" << std::endl;
            std::cout << "MinPts: " << p.first << ", Eps: " << p.second << std::endl;
            if (left >= 0) {
                std::vector<double> maxerror = obtainMaxUlpInterval4(LEFTR, RIGHTR);
                printMaxError4(maxerror);
                return true;
            }
            if (right <= 0) {
                std::vector<double> maxerror = obtainMaxUlpInterval4(LEFTL, RIGHTL);
                printMaxError4(maxerror);
                return true;
            }
        }
        std::cout << "=====4. DBSCAN completed and successfully!=====" << std::endl;
//        for (int i = 0; i < vec_dbscan.size(); ++i) {
//            std::cout << "[" << i + 1 << ", " << vec_dbscan[i][0] << std::endl;
//        }
        std::cout << "Now fit the function" << std::endl;
        std::vector<double> vec_curve = getCurveFittingParas();
        std::cout << "=====5. Curve function completed!=====" << std::endl;
        std::cout << "The lower bound function is: " << "x_i = " << vec_curve[0] << " * " << "pow(" << vec_curve[1]
                  << ", i - 1),   i >= 1" << std::endl;
        std::cout << "The upper bound function is: " << "x_i = " << vec_curve[2] << " * " << "pow(" << vec_curve[3]
                  << ", i - 1),   i >= 1" << std::endl;
        std::vector<std::vector<double>> vec_interval;
        vec_interval = predictInterval(vec_curve[0], vec_curve[1], vec_curve[2], vec_curve[3], left, right);
        std::cout << "=====6. PredictInterval interval completed!=====" << std::endl;
        for (int i = 0; i < vec_interval.size(); ++i) {
            std::cout << "[" << vec_interval[i][0] << ", " << vec_interval[i][1] << "]" << std::endl;
        }
        std::cout << "Please wait, error detection is currently underway~" << std::endl;
        std::vector<double> maxerror = detectAllIntervalGetMaximumError4(vec_interval, left, right);
        std::cout << "=====7. Error detection completed!=====" << std::endl;
        printMaxError4(maxerror);
    }
    return true;
}

int fourPara(double left, double right) {
    if (left >= right) {
        std::cout << "ERROR: left endpoint must be smaller than right endpoint" << std::endl;
        return false;
    }
    if ((left < 0 && right > 0) || (right < 0 && left > 0)) {
        std::cout << "ERROR: left endpoint must be >= 0, or right endpoint <= 0" << std::endl;
        return false;
    }
    if ((left >= 0 && right < 100) || (right <= 0 && left > -100) || (std::abs(left - right) < 100)) {
        std::cout << "Detection interval is less than R, directly return the maximum error" << std::endl;
        std::vector<double> maxError = obtainMaxUlpInterval4(left, right);
        printMaxError4(maxError);
    } else {
        detectNoSpan4(left, right);
    }
    return true;
}

std::vector<double> obtainMaxUlpInterval5(double left, double right) {
    std::vector<double> result;
    std::vector<std::vector<double>> vec = getFiveParaErrorAllPoint(left, right);
    auto maxError = max_element(vec.begin(), vec.end(), compareByUlp5);
    result.push_back(maxError->at(0));
    result.push_back(maxError->at(1));
    result.push_back(maxError->at(2));
    result.push_back(maxError->at(3));
    result.push_back(maxError->at(4));
    result.push_back(maxError->at(5));
    return result;
}
std::vector<std::vector<double>> fiveParaErrorDetect(double x1, double x2, double x3, double x4, double x5) {
    std::vector<std::vector<double>> result;
    double origin = getComputeValueE5(x1, x2, x3, x4, x5);
    double ulp = getULPE5(x1, x2, x3, x4, x5, origin);
    if (std::isnan(ulp) || std::isinf(ulp)) {
        result.emplace_back(std::vector<double>({x1, x2, x3, x4, x5, 0.0}));
    } else {
        result.emplace_back(std::vector<double>({x1, x2, x3, x4, x5, ulp}));
    }
    return result;
}
std::vector<std::vector<double>> getFiveParaErrorAllPoint(double left, double right) {
    std::vector<std::vector<double>> result;
    double x1, x2, x3, x4, x5;
    for (int i = 0; i < SIZE; ++i) {
        x1 = left + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right - left)));
        x2 = left + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right - left)));
        x3 = left + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right - left)));
        x4 = left + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right - left)));
        x5 = left + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (right - left)));
        std::vector<std::vector<double>> vec = fiveParaErrorDetect(x1, x2, x3, x4, x5);
        result.insert(result.end(), std::make_move_iterator(vec.begin()), std::make_move_iterator(vec.end()));
    }
    return result;
}

std::vector<std::vector<double>> handleErrorPoint5(std::vector<std::vector<double>> vec) {
    std::vector<std::vector<double>> result;
    for (size_t i = 0; i < vec.size(); i += G) {
        double maxULP = 0;
        std::vector<double> maxRow = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        for (size_t j = i; j < i + G && j < vec.size(); ++j) {
            if (vec[j][5] > maxULP) {
                maxULP = vec[j][5];
                maxRow = vec[j];
            }
        }
        if (maxULP >= ULP) {
            result.push_back(maxRow);
        }
    }
    return result;
}

std::vector<double> detectAllIntervalGetMaximumError5(std::vector<std::vector<double>> vec_interval, double left, double right) {
    std::vector<double> result;
    std::vector<std::vector<double>> temp;
    for (auto currentInterval : vec_interval) {
        std::vector<std::vector<double>> vec_random;
        std::vector<double> x1, x2, x3, x4, x5;
        double x_left = currentInterval[0];
        double x_right = currentInterval[1];
        x1 = random(x_left, x_right);
        x2 = random(left, right);
        x3 = random(left, right);
        x4 = random(left, right);
        x5 = random(left, right);
        for (int i = 0; i < x1.size(); ++i) {
            vec_random = fiveParaErrorDetect(x1[i], x2[i], x3[i], x4[i], x5[i]);
        }
        auto maxUlpRow = max_element(vec_random.begin(), vec_random.end(), compareByUlp5);
        temp.push_back(std::vector<double>{maxUlpRow->at(0), maxUlpRow->at(1), maxUlpRow->at(2), maxUlpRow->at(3), maxUlpRow->at(4), maxUlpRow->at(5)});
    }
    auto maxULP = max_element(temp.begin(), temp.end(), compareByUlp5);
    result.push_back(maxULP->at(0));
    result.push_back(maxULP->at(1));
    result.push_back(maxULP->at(2));
    result.push_back(maxULP->at(3));
    result.push_back(maxULP->at(4));
    result.push_back(maxULP->at(5));
    return result;
}
int detectNoSpan5(double left, double right) {
    std::vector<std::vector<double>> vecErrorDataSet;
    if (left >= 0) {
        vecErrorDataSet = getFiveParaErrorAllPoint(LEFTR, RIGHTR);
    }
    if (right <= 0) {
        vecErrorDataSet = getFiveParaErrorAllPoint(LEFTL, RIGHTL);
    }
    std::cout << "=====1. Data set construction completed!=====" << std::endl;
    vecErrorDataSet = handleErrorPoint5(vecErrorDataSet);
    std::cout << "=====2. Boundary extraction completed!=====" << std::endl;
    geneDBSCANFile(vecErrorDataSet);
    std::pair<int, double> p = callDbscanPy();
    std::cout << "=====3. Automatic determination of MinPts and Eps parameters completed!=====" << std::endl;
    if (p.first == 0 && p.second == 0) {
        std::cout << "=====4. DBSCAN clusters less than 3, return R's maximum error!=====" << std::endl;
        if (left >= 0) {
            std::vector<double> maxerror = obtainMaxUlpInterval5(LEFTR, RIGHTR);
            printMaxError5(maxerror);
            return true;
        }
        if (right <= 0) {
            std::vector<double> maxerror = obtainMaxUlpInterval5(LEFTL, RIGHTL);
            printMaxError5(maxerror);
            return true;
        }
    } else {
        std::vector<std::vector<double>> vec_dbscan = DBSCAN(p.second, p.first);
        if (vec_dbscan[0][0] == 0 || vec_dbscan[0][1] == 0) {
            std::cout << "4. DBSCAN clusters less than 3, return R's maximum error!=====" << std::endl;
            if (left >= 0) {
                std::vector<double> maxerror = obtainMaxUlpInterval5(LEFTR, RIGHTR);
                printMaxError5(maxerror);
                return true;
            }
            if (right <= 0) {
                std::vector<double> maxerror = obtainMaxUlpInterval5(LEFTL, RIGHTL);
                printMaxError5(maxerror);
                return true;
            }
        }
        std::cout << "=====4. DBSCAN completed and successfully!=====" << std::endl;
//        for (int i = 0; i < vec_dbscan.size(); ++i) {
//            std::cout << "[" << i + 1 << ", " << vec_dbscan[i][0] << std::endl;
//        }
        std::cout << "Now fit the function" << std::endl;
        std::vector<double> vec_curve = getCurveFittingParas();
        std::cout << "=====5. Curve function completed!=====" << std::endl;
        std::cout << "The lower bound function is: " << "x_i = " << vec_curve[0] << " * " << "pow(" << vec_curve[1]
                  << ", i - 1),   i >= 1" << std::endl;
        std::cout << "The upper bound function is: " << "x_i = " << vec_curve[2] << " * " << "pow(" << vec_curve[3]
                  << ", i - 1),   i >= 1" << std::endl;
        std::vector<std::vector<double>> vec_interval;
        vec_interval = predictInterval(vec_curve[0], vec_curve[1], vec_curve[2], vec_curve[3], left, right);
        std::cout << "=====6. PredictInterval interval completed!=====" << std::endl;
        for (int i = 0; i < vec_interval.size(); ++i) {
            std::cout << "[" << vec_interval[i][0] << ", " << vec_interval[i][1] << "]" << std::endl;
        }
        std::cout << "Please wait, error detection is currently underway~" << std::endl;
        std::vector<double> maxerror = detectAllIntervalGetMaximumError5(vec_interval, left, right);
        std::cout << "=====7. Error detection completed!=====" << std::endl;
        printMaxError5(maxerror);
    }
    return true;
}
int fivePara(double left, double right) {
    if (left >= right) {
        std::cout << "ERROR: left endpoint must be smaller than right endpoint" << std::endl;
        return false;
    }
    if ((left < 0 && right > 0) || (right < 0 && left > 0)) {
        std::cout << "ERROR: left endpoint must be >= 0, or right endpoint <= 0" << std::endl;
        return false;
    }
    if ((left >= 0 && right < 100) || (right <= 0 && left > -100) || (std::abs(left - right) < 100)) {
        std::cout << "Detection interval is less than R, directly return the maximum error" << std::endl;
        std::vector<double> maxError = obtainMaxUlpInterval5(left, right);
        printMaxError5(maxError);
    } else {
        detectNoSpan5(left, right);
    }
    return true;
}

int eiffel(double left, double right) {
    int paraNum = getNumberOfParameters("./detectModule/EIFFEL/src/getMultiResult.cpp");
    if (paraNum == 0) { // 这里0是表示1个参数，因为单参的时候函数是getULPE，后面没有跟数字
        onePara(left, right);
    } else if (paraNum == 2) {
        twoPara(left, right);
    } else if (paraNum == 3) {
        threePara(left, right);
    } else if (paraNum == 4) {
        fourPara(left, right);
    } else if (paraNum == 5) {
        fivePara(left, right);
    } else {
        std::cout << "Sorry! Temporary automation only supports five parameters expressions!!!" << std::endl;
    }
    return true;
}
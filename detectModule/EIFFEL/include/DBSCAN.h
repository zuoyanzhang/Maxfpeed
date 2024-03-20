//
// Created by 99065 on 2024/3/15.
//

#ifndef ERRORDETECTION_DBSCAN_H
#define ERRORDETECTION_DBSCAN_H

#endif //ERRORDETECTION_DBSCAN_H
// #if !defined(_DBSCAN_H_INCLUDED_)
// #define _DBSCAN_H_INCLUDED_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <cmath>
#include <stack>
#include <map>

using namespace std;

enum//枚举类型默认值为0-3
{
    pointType_UNDO,
    pointType_NOISE,//噪声
    pointType_BORDER,//边界点
    pointType_CORE//核心点

};



class point {
public:
    float x;
    float y;//~x，y坐标
    vector<float> xn;
    //vector<float> yn;
    int cluster;//第几个簇
    int pointType;  //1 noise 2 border 3 core
    int pts;        //每个点周围包含的点数
    int corePointID;//核心点的标号
    vector<int> corepts;
    int  visited;//点是否被遍历，1代表被遍历，0代表没被遍历
    void init();//初始化
    //point();
    //point(float a, float b, int c) {
    //	x = a;
    //	y = b;
    //	cluster = c;
    //};
    point(vector<float>  an, int c) {//three or more dimension
        this->xn = an;
        this->cluster = c;//
    };
};


float stringToFloat(string i);//把字符串转换为浮点数
vector<point> openFile(const char* dataset);//打开文件
float squareDistance(point a, point b);//计算距离
float squareDistanceVect(point a, point b);//多维数据算欧氏距离
vector<vector<double>> DBSCAN(double Eps, int MinPts);
vector<vector<double>> DBSCAN2(double Eps, int MinPts);

// #endif


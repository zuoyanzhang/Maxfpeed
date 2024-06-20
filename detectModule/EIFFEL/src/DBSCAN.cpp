//
// Created by 99065 on 2024/3/15.
//
#include "EIFFEL/include/DBSCAN.h"
#include <string.h>
#include <algorithm>
#include <vector>
//#include "DBSCAN.h"

int clusterID = 0;

void point::init()
{
    cluster = 0;
    pointType = pointType_UNDO;//pointType_NOISE pointType_UNDO
    pts = 0;
    visited = 0;
    corePointID = -1;
}


float stringToFloat(string i) {//读取的是一个字符串，转换为浮点类型计算
    stringstream sf;
    float score = 0;
    sf << i;//~写入字符串i
    sf >> score;//~读取字符串i
    return score;
}

vector<point> openFile(const char* dataset) {
    fstream file;//~读写皆可
    file.open(dataset, ios::in);//~以读的方式打开文件
    if (!file)
    {
        cout << "Open File Failed!" << endl;
        vector<point> a;
        return a;
    }
    vector<point> data;

    int i = 1;
    //~eof（）检查是否到达文件末尾
    while (1) {//!file.eof()
        string temp;
        file >> temp; //读取一行数据
        //int split = temp.find(',', 0);//~在字符串temp中，从下标0开始读，找到','，并返回逗号在字符串中的下标位置
        //二维数据直接用point（float x，float y，int c）
        //point p(stringToFloat(temp.substr(0,split)),stringToFloat(temp.substr(split+1,temp.length()-1)),i++);

        //三维及以上多维数据需要以下处理过程
        int numSpit = 0;
        vector <int> splitN1, splitN2;
        numSpit = count(temp.begin(), temp.end(), ',');//~计算每一行数据里有几个逗号
        if (numSpit > 0) {
            vector<float> xn;
            int m = 0;
            int cPos = 0;
            while (1) {//
                int splitTemp = temp.find_first_of(',', cPos);//~在temp字符串的第cpos位开始查找逗号，返回逗号的位置下标
                //~nops是字符串结束的位置，只要！=，就说明字符串还没读到末尾结束
                m++;//m的作用是记录压入了几维坐标
                splitN1.push_back(cPos);//将cpos加入splitN1 第一次：0
                cPos = splitTemp + 1;//~逗号右边数据的初始位置下标（三维及以上这一步就很有用了，二维不需要）
                splitN2.push_back(splitTemp);//~将逗号下标加入splitN2
                if(m==numSpit+1)//坐标维度等于逗号数+1说明所有的数都压入，跳出循环
                {
                    break;
                }
            }
            for (int m = 0; m <=numSpit; m++) {
                xn.push_back(stringToFloat(temp.substr(splitN1[m], splitN2[m])));//将一行的前n-1个数放进xn里
            }
            //xn.push_back(stringToFloat(temp.substr(splitN2[numSpit - 1] + 1, temp.length() - 1)));//将一行的最后一个数放进xn
            point p(xn, i++);
            data.push_back(p);
            //data[i] = p;
        }
        if (file.eof())
            break;
        //data.push_back(p);
    }
    file.close();
    return data;
}

//~二维数据算欧式距离
float squareDistance(point a, point b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));//计算欧氏距离
}

//多维数据算欧氏距离
float squareDistanceVect(point a, point b) {
    vector <float> xn1 = a.xn;
    vector <float> yn1 = b.xn;

    float sumSqrt = 0;
    for (int i = 0; i < xn1.size(); i++) {
        sumSqrt = sumSqrt + (xn1[i] - yn1[i]) * (xn1[i] - yn1[i]);
    }
    return sqrt(sumSqrt);
}

bool cmp_func(const vector<double> &v1, const vector<double> &v2) {
    return v1[0] < v2[0];
}

bool cmp_func_y(const vector<double> &v1, const vector<double> &v2) {
    return v1[1] < v2[1];
}

vector<vector<double>> DBSCAN(double Eps, int MinPts) {
    std::vector<point> dataset = openFile("./detectModule/EIFFEL/src/galaxy.csv");
    int len = dataset.size();//有多少个点
    //cout << len << endl;
    for (int i = 0; i < len; i++)//参数初始化
    {
        dataset[i].init();
    }
    vector<vector <float>> distP2P(len);
    //vector<vector <float>> distP2P(vector <float>(len));

    //calculate pts计算每个点在eps范围内有几个点
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len; j++) {
            float distance = squareDistanceVect(dataset[i], dataset[j]);//squareDistanceVect squareDistance
            distP2P[i].push_back(distance);//disp for debug
            if (distance <= Eps) {
                dataset[i].pts++;
            }
        }
    }
    //core point 核心点，pts大于minPts的时候，该点为核心点
    vector<point> corePoint;
    for (int i = 0; i < len; i++) {
        int tempPts = dataset[i].pts;
        if (tempPts >= MinPts) {
            dataset[i].pointType = pointType_CORE;
            dataset[i].corePointID = i;
            corePoint.push_back(dataset[i]);
        }
    }

    //joint core point
    int numCorePoint = corePoint.size(); //core point number核心点的数量
    for (int i = 0; i < numCorePoint; i++) {
        for (int j = 0; j < numCorePoint; j++) {
            float distTemp = distP2P[corePoint[i].corePointID][corePoint[j].corePointID];//display for debug  distTemp相当于二维数组，distTemp[i][j]即核心点i和j之间的距离
            if (distTemp <= Eps) {//squareDistance(corePoint[i],corePoint[j])
                corePoint[i].corepts.push_back(j);//other point orderID link to core point
            }//把每一个在核心点领域的核心点放到一起
        }
    }
    for (int i = 0; i < numCorePoint; i++) {//遍历所有的核心点
        stack<point*> ps;//临时存储核心点
        if (corePoint[i].visited == 1) continue;
        clusterID++;
        corePoint[i].cluster = clusterID;//create a new cluster
        ps.push(&corePoint[i]);
        point* v;
        while (!ps.empty()) {
            v = ps.top();
            v->visited = 1;
            ps.pop();
            for (int j = 0; j < v->corepts.size(); j++) {//最开始归类的一簇进行遍历
                if (corePoint[v->corepts[j]].visited == 1) continue;
                corePoint[v->corepts[j]].cluster = corePoint[i].cluster;
                //dataset[v->corepts[j]].cluster= corePoint[i].cluster;
                corePoint[v->corepts[j]].visited = 1;
                ps.push(&corePoint[v->corepts[j]]);
            }
        }
    }

    //border point,joint border point to core point
    int k = 0;//k用来在dataset中统计是第几个核心点
    for (int i = 0; i < len; i++) {
        if (dataset[i].pointType == pointType_CORE)//如果该点是核心点，在上面已经访问过了，就不再访问，因为核心点不可能是边界点，没必要再访问一次
        {
            dataset[i].cluster = corePoint[k++].cluster;//遍历到第k个核心点时，把属于的簇id给原来的dataset
            continue;
        }
        for (int j = 0; j < numCorePoint; j++) {
            float distTemp = distP2P[i][corePoint[j].corePointID];
            if (distTemp <= Eps) {
                dataset[i].pointType = pointType_BORDER;
                dataset[i].cluster = corePoint[j].cluster;
                break;
            }
        }
    }
    fstream clustering;//save .txt
    clustering.open("clustering.txt", ios::out);//save .txt
    char dispInfo[500];
    int dataDim = dataset[0].xn.size();//data dimension
    int cluster_sum = 0;
    for (int i = 1; i < len; i++) {
        sprintf(dispInfo, "第%3d个数据：", i);
        for (int j = 0; j < dataDim; j++) {
            char dataSrc[30];
            if (j == 0)
                sprintf(dataSrc, "%11.4lf", dataset[i].xn[j]);
            else
                sprintf(dataSrc, ",%11.4lf", dataset[i].xn[j]);
            strcat(dispInfo, dataSrc);
        }
        char dataClust[30];
        sprintf(dataClust, ",%4d\n", dataset[i].cluster);
        if (cluster_sum <= dataset[i].cluster) cluster_sum = dataset[i].cluster;
        strcat(dispInfo, dataClust);
        clustering << dispInfo;//save the results in .txt format named clustering.txt

    }
    clustering.close();//save .txt
    double number[cluster_sum][2];
    //二维数组初始化
    for (int i = 0; i < cluster_sum; ++i) {
        for (int j = 0; j < 2; ++j) {
            number[i][j] = {0};
        }
    }
    for (int i = 1; i < len; i++) {
        //cluster_sum = 3
        for (int j = 1; j <= cluster_sum; ++j) {
            if (dataset[i].cluster == j) {
                if (number[j - 1][1] <= dataset[i].xn[1]) {
                    number[j - 1][0] = dataset[i].xn[0];
                    number[j - 1][1] = dataset[i].xn[1];
                }
            }
        }
    }
    vector<vector<double>> v;
    for (int i = 0; i < cluster_sum; ++i) {
        vector<double> v1;
        v1.push_back(number[i][0]);
        v1.push_back(number[i][1]);
        v.push_back(v1);
    }
    sort(v.begin(), v.end(), cmp_func);

    ofstream file_clean("./detectModule/EIFFEL/src/curvepoint.csv", ios_base::out);
    ofstream ofs("./detectModule/EIFFEL/src/curvepoint.csv", ios::app);
    for (int i = 0; i < v.size(); i++) {
        ofs << i + 1 << " ";
    }
    ofs << "\n";

    if (v[0][0] > 0) {
        for (int i = 0; i < v.size(); i++) {
            if (i == 0) {
                if (v[i][0] < 2.0) {
                    ofs << v[i][0] << " ";
                } else {
                    ofs << v[i][0] - 1 << " ";
                }
            } else {
                ofs << v[i][0] - 1 << " ";
            }
        }

        ofs << "\n";

        for (int i = 0; i < v.size(); ++i) {
            ofs << v[i][0] + 1 << " ";
        }
    } else {
        for (int i = v.size() - 1; i >= 0; --i) {
            if (i == v.size() - 1) {
                if (v[i][0] > -2.0) {
                    ofs << v[i][0] << " ";
                } else {
                    ofs << v[i][0] + 1 << " ";
                }
            } else {
                ofs << v[i][0] + 1 << " ";
            }
        }

        ofs << "\n";
        for (int i = v.size() - 1; i >= 0; --i) {
            ofs << v[i][0] - 1 << " ";
        }
    }
    file_clean.close();
    ofs.close();
    return v;
}

vector<vector<double>> DBSCAN2(double Eps, int MinPts) {
    std::vector<point> dataset = openFile("./detectModule/EIFFEL/src/galaxy.csv");
    int len = dataset.size();//有多少个点
    //cout << len << endl;
    for (int i = 0; i < len; i++)//参数初始化
    {
        dataset[i].init();
    }
    vector<vector <float>> distP2P(len);
    //vector<vector <float>> distP2P(vector <float>(len));

    //calculate pts计算每个点在eps范围内有几个点
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len; j++) {
            float distance = squareDistanceVect(dataset[i], dataset[j]);//squareDistanceVect squareDistance
            distP2P[i].push_back(distance);//disp for debug
            if (distance <= Eps) {
                dataset[i].pts++;
            }
        }
    }
    //core point 核心点，pts大于minPts的时候，该点为核心点
    vector<point> corePoint;
    for (int i = 0; i < len; i++) {
        int tempPts = dataset[i].pts;
        if (tempPts >= MinPts) {
            dataset[i].pointType = pointType_CORE;
            dataset[i].corePointID = i;
            corePoint.push_back(dataset[i]);
        }
    }

    //joint core point
    int numCorePoint = corePoint.size(); //core point number核心点的数量
    for (int i = 0; i < numCorePoint; i++) {
        for (int j = 0; j < numCorePoint; j++) {
            float distTemp = distP2P[corePoint[i].corePointID][corePoint[j].corePointID];//display for debug  distTemp相当于二维数组，distTemp[i][j]即核心点i和j之间的距离
            if (distTemp <= Eps) {//squareDistance(corePoint[i],corePoint[j])
                corePoint[i].corepts.push_back(j);//other point orderID link to core point
            }//把每一个在核心点领域的核心点放到一起
        }
    }
    for (int i = 0; i < numCorePoint; i++) {//遍历所有的核心点
        stack<point*> ps;//临时存储核心点
        if (corePoint[i].visited == 1) continue;
        clusterID++;
        corePoint[i].cluster = clusterID;//create a new cluster
        ps.push(&corePoint[i]);
        point* v;
        while (!ps.empty()) {
            v = ps.top();
            v->visited = 1;
            ps.pop();
            for (int j = 0; j < v->corepts.size(); j++) {//最开始归类的一簇进行遍历
                if (corePoint[v->corepts[j]].visited == 1) continue;
                corePoint[v->corepts[j]].cluster = corePoint[i].cluster;
                //dataset[v->corepts[j]].cluster= corePoint[i].cluster;
                corePoint[v->corepts[j]].visited = 1;
                ps.push(&corePoint[v->corepts[j]]);
            }
        }
    }

    //border point,joint border point to core point
    int k = 0;//k用来在dataset中统计是第几个核心点
    for (int i = 0; i < len; i++) {
        if (dataset[i].pointType == pointType_CORE)//如果该点是核心点，在上面已经访问过了，就不再访问，因为核心点不可能是边界点，没必要再访问一次
        {
            dataset[i].cluster = corePoint[k++].cluster;//遍历到第k个核心点时，把属于的簇id给原来的dataset
            continue;
        }
        for (int j = 0; j < numCorePoint; j++) {
            float distTemp = distP2P[i][corePoint[j].corePointID];
            if (distTemp <= Eps) {
                dataset[i].pointType = pointType_BORDER;
                dataset[i].cluster = corePoint[j].cluster;
                break;
            }
        }
    }
    fstream clustering;//save .txt
    clustering.open("clustering.txt", ios::out);//save .txt
    char dispInfo[500];
    int dataDim = dataset[0].xn.size();//data dimension
    int cluster_sum = 0;
    for (int i = 1; i < len; i++) {
        sprintf(dispInfo, "第%3d个数据: ", i);
        for (int j = 0; j < dataDim; j++) {
            char dataSrc[30];
            if (j == 0)
                sprintf(dataSrc, "%11.4lf", dataset[i].xn[j]);
            else
                sprintf(dataSrc, ",%11.4lf", dataset[i].xn[j]);
            strcat(dispInfo, dataSrc);
        }
        char dataClust[30];
        sprintf(dataClust, ",%4d\n", dataset[i].cluster);
        if (cluster_sum <= dataset[i].cluster) cluster_sum = dataset[i].cluster;
        strcat(dispInfo, dataClust);
        clustering << dispInfo;//save the results in .txt format named clustering.txt

    }
    clustering.close();//save .txt
    double number[cluster_sum][2];
    //二维数组初始化
    for (int i = 0; i < cluster_sum; ++i) {
        for (int j = 0; j < 2; ++j) {
            number[i][j] = {0};
        }
    }
    for (int i = 1; i < len; i++) {
        //cluster_sum = 3
        for (int j = 1; j <= cluster_sum; ++j) {
            if (dataset[i].cluster == j) {
                if (number[j - 1][1] <= dataset[i].xn[1]) {
                    number[j - 1][0] = dataset[i].xn[0];
                    number[j - 1][1] = dataset[i].xn[1];
                }
            }
        }
    }
    vector<vector<double>> v;
    for (int i = 0; i < cluster_sum; ++i) {
        vector<double> v1;
        v1.push_back(number[i][0]);
        v1.push_back(number[i][1]);
        v.push_back(v1);
    }
    sort(v.begin(), v.end(), cmp_func);

    ofstream file_clean("./detectModule/EIFFEL/src/curvepoint.csv", ios_base::out);
    ofstream ofs("./detectModule/EIFFEL/src/curvepoint.csv", ios::app);
    for (int i = 0; i < v.size(); i++) {
        ofs << i + 1 << " ";
    }
    ofs << "\n";

    if (v[0][0] > 0) {
        for (int i = 0; i < v.size(); i++) {
            if (i == 0) {
                if (v[i][0] < 2.0) {
                    ofs << v[i][0] << " ";
                } else {
                    ofs << v[i][0] - 1 << " ";
                }
            } else {
                ofs << v[i][0] - 1 << " ";
            }
        }

        ofs << "\n";

        for (int i = 0; i < v.size(); ++i) {
            ofs << v[i][0] + 1 << " ";
        }
    } else {
        for (int i = v.size() - 1; i >= 0; --i) {
            if (i == v.size() - 1) {
                if (v[i][0] > -2.0) {
                    ofs << v[i][0] << " ";
                } else {
                    ofs << v[i][0] + 1 << " ";
                }
            } else {
                ofs << v[i][0] + 1 << " ";
            }
        }

        ofs << "\n";
        for (int i = v.size() - 1; i >= 0; --i) {
            ofs << v[i][0] - 1 << " ";
        }
    }
    file_clean.close();
    ofs.close();
    return v;
}

import math
import copy
import numpy as np
from sklearn.cluster import DBSCAN
import pandas as pd
# import matplotlib.pyplot as plt

#两个样本点之间的欧式距离
def dist(a,b):
    """
    :param a: 样本点
    :param b: 样本点
    :return: 两个样本点之间的欧式距离
    """
    return math.sqrt(math.pow(a[0]-b[0],2) + math.pow(a[1]-b[1],2))

#第k最近距离集合
def returnDk(matrix,k):
    """
    :param matrix: 距离矩阵
    :param k: 第k最近
    :return: 第k最近距离集合
    """
    Dk = []
    for i in range(len(matrix)):
        Dk.append(matrix[i][k])
    return Dk

#第k最近距的平均即eps
def returnDkAverage(Dk):
    """
    :param Dk: k-最近距离集合
    :return: Dk的平均值
    """
    sum = 0
    for i in range(len(Dk)):
        sum = sum + Dk[i]
    return sum/len(Dk)

#传入数据计算距离矩阵
def CalculateDistMatrix(dataset):
    """
    :param dataset: 数据集
    :return: 距离矩阵
    """
    n = len(dataset)
    DistMatrix = [[0 for j in range(n)] for i in range(n)]
    for i in range(n):
        for j in range(n):
            DistMatrix[i][j] = dist(dataset[i], dataset[j])
    return DistMatrix


#传入数据和k输出eps,min_samples以及聚类簇的个数
#n=len(X)，DistMatrix = CalculateDistMatrix(X)
#n是数据长度，DistMatrix距离矩阵
def ems(dataset,k):
    DistMatrix = CalculateDistMatrix(dataset)
    #深度拷贝距离矩阵依此计算出eps
    tmp_matrix = copy.deepcopy(DistMatrix)
    n = len(dataset)
    for i in range(n):
        tmp_matrix[i].sort()
    Dk = returnDk(tmp_matrix,k)
    eps = returnDkAverage(Dk)
    tmp_count = 0
    for i in range(len(DistMatrix)):
        for j in range(len(DistMatrix[i])):
            if DistMatrix[i][j] <= eps:
                tmp_count = tmp_count + 1
    min_samples=tmp_count/n#参考前述公式
    clustering = DBSCAN(eps= eps,min_samples= int(min_samples)).fit(dataset)
    num_clustering = max(clustering.labels_)+1
    return eps,min_samples,num_clustering

def calculateDbscanParameters():
    df = pd.read_csv('./detectModule/EIFFEL/src/galaxy.csv')
    df1=df[["x","ulp"]]
    X=df1[["x","ulp"]].values
    n=len(X)
    DistMatrix = CalculateDistMatrix(X)

    E=[]
    M=[]
    S=[]
    for k in range(1,n):
        e,m,s = ems(X,k)
        if s<3:
            break
        else:
            E.append(e)
            M.append(m)
            S.append(s)
    if not S:
        return (0, 0)

    max_count = 0
    current_count = 1
    last_value = S[0]
    max_count_value = S[0]
    for i in range(1, len(S)):
        if S[i] == last_value:
            current_count += 1
            if current_count > max_count:
                max_count = current_count
                max_count_value = S[i]
        else:
            current_count = 1
        last_value = S[i]

    if max_count < 3:
        return (0, 0)

    last_index = len(S) - 1 - S[::-1].index(max_count_value)
    tup = (int(M[last_index]), E[last_index])
    return tup

if __name__ == '__main__':
    tup = calculateDbscanParameters()
    print(tup[0], tup[1])

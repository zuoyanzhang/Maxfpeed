from scipy.optimize import curve_fit
import numpy as np
# import matplotlib.pyplot as plt
# 等比数列函数拟合
x1data = []
y1data = []
y2data = []

with open("./detectModule/EIFFEL/src/curvepoint.csv", "r") as file:
    lines = file.readlines()
    x1data = [int(item) for item in lines[0].split()]
    y1data = [float(item) for item in lines[1].split()]
    y2data = [float(item) for item in lines[2].split()]

# plt.plot(x1data,y1data,'b-')

### 定义拟合函数, y = a * b^(x-1)###
def target_func(x, a, b):
    return a * (b ** (x - 1))

### 利用拟合函数求特征值 ###
def curve1():
    popt, pcov = curve_fit(target_func, x1data, y1data)

    ### R^2计算 ###
    calc_ydata = [target_func(i, popt[0], popt[1]) for i in x1data]
    res_ydata = np.array(y1data) - np.array(calc_ydata)
    ss_res = np.sum(res_ydata ** 2)
    ss_tot = np.sum((y1data - np.mean(y1data)) ** 2)
    r_squared = 1 - (ss_res / ss_tot)

    # 拟合的值
    y = [target_func(i,popt[0],popt[1]) for i in x1data]
    # plt.plot(xdata,y,'r--')
    # plt.show()
    ### 输出结果 ###
    # print("x1 = %f  q = %f   R2 = %f" % (popt[0], popt[1], r_squared))
    tup = (popt[0], popt[1])
    return tup
    # print(ydata, calc_ydata)

def curve2():
    popt, pcov = curve_fit(target_func, x1data, y2data)

    ### R^2计算 ###
    calc_ydata = [target_func(i, popt[0], popt[1]) for i in x1data]
    res_ydata = np.array(y2data) - np.array(calc_ydata)
    ss_res = np.sum(res_ydata ** 2)
    ss_tot = np.sum((y2data - np.mean(y2data)) ** 2)
    r_squared = 1 - (ss_res / ss_tot)

    # 拟合的值
    y = [target_func(i,popt[0],popt[1]) for i in x1data]
    # plt.plot(xdata,y,'r--')
    # plt.show()
    ### 输出结果 ###
    # print("x1 = %f  q = %f   R2 = %f" % (popt[0], popt[1], r_squared))
    tup = (popt[0], popt[1])
    return tup
    # print(ydata, calc_ydata)

def main():
    a, b = curve1()
    c, d = curve2()
    print(f"{a} {b} {c} {d}")

if __name__ == '__main__':
    main()



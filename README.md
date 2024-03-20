# Maxfpeed

#### Description
Maxfpeed is an error detection tool for floating-point arithmetic expressions, integrating two error methods: high-precision version program generation, HSED (Hierarchical Search Algorithm), and EIFFEL (based on fitting function prediction).

Maxfpeed supports the detection of single-argument and multi-argument expressions, and the high-precision version of the code is automatically generated using the LLVM Tutorial-Chapter 2 as a reference (https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl02.html).

You can refer to the FPBench benchmark test suite for benchmark reference (http://fpbench.org/benchmarks.html).

For example:

* predatorPrey:((4.0 * x) * x) / (1.0 + ((x / 1.11) * (x / 1.11)))
* sqrt_add:(1 / (sqrt((x + 1)) + sqrt(x)))
* NMSEproblem333:((1.0 / (x + 1.0)) - (2.0 / x)) + (1.0 / (x - 1.0))
* hypot: sqrt((x1 * x1) + (x2 * x2))
* nonlin2: (x1 * x2 - 1.0) / ((x1 * x2) * (x1 * x2) - 1.0)
* doppler1: ( − (331.4 + (0.6 ∗ x3)) ∗ x2) / (((331.4 + (0.6 ∗ x3)) + x1) ∗ ((331.4 + (0.6 ∗ x3)) + x1))
* ......

#### Software Architecture
```
.
|—— bin
|—— build
|—— CMakeLists.txt
|—— makefile
|—— README.md
|—— README.en.md
|—— run.sh
|—— detectModule
|    |—— EIFFEL
|    |   |—— include
|    |   |   |—— createInput.h
|    |   |   |—— DBSCAN.h
|    |   |   |__ errordetect.h
|    |   |—— src
|    |       |—— createInput.cpp
|    |       |—— curvefitting.py
|    |       |—— DBSCAN.cpp
|    |       |—— dbscan.py
|    |       |—— errordetect.cpp
|    |       |__ getMultiResult.cpp
|    |—— HSED
|    |   |—— include
|    |   |   |—— common.h
|    |   |   |—— float.h   
|    |   |   |__ getresult.h
|    |   |__ src   
|    |       |—— common.cpp
|    |       |—— float.cpp
|    |       |__ getresult.cpp
|    |—— mpfrCodeGene
|    |   |—— include
|    |   |   |——basic.h
|    |   |   |—— color.h
|    |   |   |—— geneCode.h
|    |   |   |—— laxerAST.h
|    |   |   |__ parserAST.h
|    |   |__ src
|    |       |——basic.cpp
|    |       |—— geneCode.cpp
|    |       |—— laxerAST.cpp
|    |       |—— main.cpp
|    |       |__ parserAST.cpp
|__  |__ CMakeLists.txt
```
#### Installation

* Dependence on environment

MPFR, GMP, python3 (numpy, scikit-learn, pandas), and cmake. You can install the dependencies by executing the following command.

```
$ sudo apt-get update 
$ sudo apt-get upgrade
$ sudo apt-get install libgmp-dev ligmpfr-dev cmake python3
$ pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple numpy pandas scikit-learn 
```

#### Instructions
Maxfpeed is a command-line tool that you can use on Ubuntu.
```
$ git clone https://gitee.com/zhangzuoyan/Maxfpeed.git
$ cd Maxfpeed
$ mkdir bin
```
You can then execute the run.sh script to perform error detection.
```
$ ./run.sh "para1" para2 para3 para4
```
Where para1 is the expression, para2 is the left end of the interval, para3 is the right end of the interval, and para4 is the detection method (--HSED or --EIFFEL).

For example, running ./run.sh "(4.0 * x * x) / (1 + x / 1.11 * x / 1.11)" 0 100 --HSED produces the following output:
```
------------------------------------------------------------
| step1: generate high-precision version code successfully |
------------------------------------------------------------
---------------------------------------
| step2: detect FP errors              |
| now you choose HSED method to detect |
---------------------------------------
Detection interval: [0, 100]
preprocessing: x = 2.001953, maximumULP = 2.34, maximumRelative = 2.754374e-16

---------------No significant error, excute two-layer search------------------
float-precision layer: x = 2.13522115, maximumULP = 2.87, maximumRelative = 3.279387e-16

double-precision layer: x = 2.1352211012717968, maximumULP = 3.19, maximumRelative = 3.655532e-16, BitsError = 2.1
-------------------------------------------------------------------------------
```

For example, running ./run.sh "(4.0 * x * x) / (1 + (x / 1.11) * (x / 1.11))" 0 100 --EIFFE outputs the following results:
```
------------------------------------------------------------
| step1: generate high-precision version code successfully |
------------------------------------------------------------
-----------------------------------------
| step2: detect FP errors                |
| now you choose EIFFEL method to detect |
-----------------------------------------
=====1. Data set construction completed!=====
=====2. Boundary extraction completed!=====
=====3. Automatic determination of MinPts and Eps parameters completed!=====
MinPts: 12, Eps: 0.66636
=====4. DBSCAN completed and successfully!=====
no.         x          ULP
1       0.823919      2.10557
2         2.9681      2.25284
3        4.45136      2.33375
4        9.46925      2.31113
5        17.9898      2.43869
6        37.0861      2.51231
7        72.7679      2.52831
=====5. Curve function completed!=====
The lower bound function is: x_i = 1.00445 * pow(2.03817, i - 1),   i >= 1
The upper bound function is: x_i = 1.41848 * pow(1.93146, i - 1),   i >= 1
=====6. Prediction interval completed!=====
Above are the intervals:
[5.55717e-06, 1.9578e-05]
[1.13264e-05, 3.78141e-05]
[2.30852e-05, 7.30364e-05]
[4.70515e-05, 0.000141067]
[9.58987e-05, 0.000272465]
[0.000195458, 0.000526255]
[0.000398375, 0.00101644]
[0.000811955, 0.00196321]
[0.0016549, 0.00379187]
[0.00337296, 0.00732384]
[0.00687466, 0.0141457]
[0.0140117, 0.0273218]
[0.0285582, 0.052771]
[0.0582064, 0.101925]
[0.118634, 0.196864]
[0.241797, 0.380235]
[0.492822, 0.734409]
[1.00445, 1.41848]
[2.04724, 2.73974]
[4.17263, 5.2917]
[8.50451, 10.2207]
[17.3336, 19.7409]
[35.3288, 38.1287]
[72.006, 73.644]
Please wait, error detection is currently underway~
=====7. Error detection completed!=====
x = 2.254804438626276, maximum ULP = 3.4609
```

Note: HSED currently only supports the detection of single-parameter expressions, so when using --HSED, make sure to target single-parameter expressions. EIFFEL supports the detection of multi-parameter expressions, and when using --EIFFEL, the detection interval must be [left, right] (where left >= 0 or right <= 0).




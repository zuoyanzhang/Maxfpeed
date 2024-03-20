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


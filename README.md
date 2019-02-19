# Mutithread-Sorting-Code

Tested Code with sort methods such as :-

1) QuickSort(with 2 async tasks and without async tasks)

2)  c++17 std::sorting technique with execution policy and vectorization.

Quick Sort with Async and without Async implementation gave results as below for 5mb file on a 2 core machine :-
                  
                          2 threads    4threads     6 threads
                          214 secs	    220 secs	   245 secs
                          80.7 secs	    132 secs	   148 secs


c++17 sorting algorithm with execution policy gave an amazing execution speed of 62 seconds.

With No Sort and just writing the exisiting input file onto output file was executed in 45 seconds.

Used Visual Studio 2017 , with C++17 and CMAKE so its platform independent.




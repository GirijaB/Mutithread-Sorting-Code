/******************************************************************************
FileName   : main.cpp 

Author     : Girija

Date       : 18 feb, 2019

Description: Defines the entry point for the application.

Version No : Date of update:         Updated by:           Changes:
Issues     : None

******************************************************************************/
#include "Main.h"
#include <concurrent_vector.h>
#include <algorithm>
#include <atomic>
#include <vector>
#include <mutex>
#include <string>
#include <list>
#include <future>
#include <fstream>
#include <ostream>
#include "ParallelSort.h"
#include <sstream>



int main(int argc, char ** argv)
{
	std::string inFileName;
	std::string outFileName;
	std::vector<std::string> sortSelect;
	std::cout << "*****************************************************************************************************" << std::endl;
	std::cout << "Please Provide Proper Arguments" << std::endl;
	std::cout << " provide input file name , output file name, sorting techniques" << std::endl;
	std::cout << " provide sorting techniques either NoSort,CPlusPlusSort,PQuickSort, or PAsyncQuickSort" << std::endl;
	std::cout << " for example: ParallelSort.exe C:\\ParallelSort\\Input.txt C:\\ParallelSort\\Output.txt ParallelSort CPlusPlusSort NoSort" << std::endl;
	std::cout << "*****************************************************************************************************" << std::endl;

	
	
	//read in all command line arguments and get the sort type input file name and  output file name;
	std::vector<std::string> args(argv, argv + argc);
	if (args.empty())
	{
		std::cout << "Please Provide Proper Arguments"<<std::endl;
		std::cout << " provide input file name , output file name, sorting techniques" << std::endl;
		std::cout << " for example:C:\ParallelSort\Input.txt C:\ParallelSort\Output.txt ParallelSort CPlusPlusSort NoSort" << std::endl;
			return 0;
	}
	else if (args.size()<4)
	{
		std::cout << "There are no requests for sorting" << std::endl;
		std::cout << " provide sorting techniques either NoSort,CPlusPlusSort,PQuickSort, or PAsyncQuickSort" << std::endl;
		std::cout << " for example: C:\ParallelSort\Input.txt C:\ParallelSort\Output.txt ParallelSort CPlusPlusSort NoSort" << std::endl;
		return 0;
	}
	else
	{
		sortSelect.reserve(2);
		for (size_t i = 1; i < args.size(); ++i)
		{
			if (i == 2 && !(args[1].empty()))
				inFileName = args[i];
			else if ((i == 3) && !(args[2].empty()))
				outFileName = args[i];
			else if ((i >= 4) && !(args[i].empty()))
				sortSelect.push_back(args[i]);
		}
	}

	//read each line of the file will represent an item to process

	//change the number of threads;
	const size_t max_concurrent_threads = 4;
	
	ParallelSort *readFile = new ParallelSort();
	readFile->Run(max_concurrent_threads, inFileName, outFileName, sortSelect);
	return 0;
}

	
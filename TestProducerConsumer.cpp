// TestProducerConsumer.cpp : Defines the entry point for the application.
//

#include "TestProducerConsumer.h"

// ProducerConsumer.cpp : Defines the entry point for the application.
#include <concurrent_vector.h>
#include <thread>
#include <algorithm>
#include <atomic>
#include <vector>
#include <mutex>
#include <string>
#include <list>
#include <future>
#include <fstream>
#include <ostream>
#include "FileHandler.h"
#include <sstream>




int main(int argc, char ** argv)
{
	std::string inFileName;
	std::string outFileName;
	char *line;
	std::vector<std::string> sortSelect;
	sortSelect.reserve(2);
	std::mutex readMtx;
	//read in all command line arguments and get the sort type input file name and  output file name;
	std::vector<std::string> args(argv, argv + argc);
	for (size_t i = 1; i < args.size(); ++i) {
		if (i == 1 && !(args[1].empty()))
			inFileName = args[i];
		else if ((i == 2) && !(args[2].empty()))
			outFileName = args[i];
		else if ((i >= 3) && !(args[i].empty()))
			sortSelect.push_back(args[i]);
	}


	//read each line of the file will represent an item to process
	// Create a packaged_task<> that encapsulated the callback i.e. a function


	//change the number of threads;
	const size_t max_concurrent_threads = 4;

	
	FileHandler *readFile = new FileHandler();
	readFile->Run(max_concurrent_threads, inFileName, outFileName, sortSelect[2]);
	return 0;
}


	
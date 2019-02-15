// ProducerConsumer.cpp : Defines the entry point for the application.
//

#include "ProducerConsumer.h"
#include <concurrent_vector.h>
#include <thread>
#include <algorithm>
#include <atomic>
#include <vector>
#include <mutex>
#include <string>
#include <list>
#include <future>
#include <thread>
#include <fstream>
#include <ostream>


bool do_sort(std::vector<char>& chunk_data)
{
	bool res = false;
	return res;
}
bool parallel_quick_sort(std::string input)
{

	std::vector<char> vec = { 'a','b','c' };
	if (input.empty())
	{
		return false;
	}
	
	return do_sort(vec);
}



int main(int argc, char ** argv) {

	std::string inFileName;
	std::string outFileName;
	std::string line;
	std::vector<std::string> sortSelect;
	sortSelect.reserve(2);

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

	std::ifstream inFile;

	//read each line of the file will represent an item to process
	// Create a packaged_task<> that encapsulated the callback i.e. a function
	if (!inFileName.empty())
	{
		inFile.open(inFileName.c_str());
	}
	else if (inFileName.empty()) //Always test the file open.
	{
		std::cout << "Error opening input file" << std::endl;
		system("pause");
		return -1;
	}

	//change the number of threads;
	const int max_concurrent_threads = 4;

	std::vector<std::thread>  all_threads;

	


	while (std::getline(inFile, line))
	{
		for (int i = 0; i < max_concurrent_threads; i++) {

			std::packaged_task<bool(std::string)> task(parallel_quick_sort);

			parallel_quick_sort(line);
			// Pass the packaged_task to thread to run asynchronously
			all_threads.push_back(std::thread(std::move(task), "Arg"));
			// Fetch the associated future<> from packaged_task<>
			std::future<bool> result = task.get_future();
			// Fetch the result of packaged_task<> i.e. value returned by parallel_quick_sort()
			bool data = result.get();

		}

		while (true) {
			try {
				for (int i = 0; i < max_concurrent_threads; i++)
				{
					all_threads[i].join(); 
				}
			}
			catch (std::exception ex) {
			}
		}
	}
	
	

	
	

	

	return 0;
}
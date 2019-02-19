/******************************************************************************
FileName   : ParallelSort.h

Author     : Girija

Date       : 18 feb, 2019

Description: File Handling and Sorting techniques declarations

Version No : Date of update:         Updated by:           Changes:
Issues     : None

******************************************************************************/

#include "ParallelSort.h"
#include <algorithm>
#include <functional>
#include <string>
#include <memory>
#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <future>
#include <execution> 
#include <iterator> 
#include <chrono>

using namespace std::chrono;

//change the number of threads as per the number of cores in machine
#define Max_Thread_Count 2


ParallelSort::ParallelSort()
{
	Initialize();
	m_maxLineData = 100;
}

ParallelSort::~ParallelSort()
{
 //Destructor;
}

//creates a file from scratch!
ParallelSort::status ParallelSort::Create(const char * pFilename)
{
	ParallelSort::status status = e_NOK;

	m_ptrFile.open(pFilename, std::ios::in | std::ios::out );
	if (!m_ptrFile.good())
	{
		strcpy_s(m_strError,28, "Error :Cannot create file ");
		strcat_s(m_strError, pFilename);
	}
	else
	{
		status = e_OK;
	}

	return status;
}

size_t ParallelSort::GetTotalRecords()
{
	return m_file_index.size();
}

void ParallelSort::Rewind()
{
	m_ptrFile.clear();
	m_ptrFile.seekg(0,std::ios::beg);
	m_bExceededFileLimits = false;
	m_nReadCurrentRecord = -1;
}

void ParallelSort::RewindWriteFile()
{
	m_ptrWriteFile.clear();
	m_ptrWriteFile.seekp(0, std::ios::beg);
	m_bExceededFileLimits = false;
}

//Parallel QSort function using async functions which recursively apply on the partition
//This function works good when range is is above certain threshold, else it is very expensive
//with all the context switches
template<class itr>
inline void ParallelSort::PQuicksort(itr first, itr last)
{
	if (first < last)
	{
		auto p = Partition(first, last);
		auto f1 = std::async(std::launch::async, [&]() {ParallelSort::PQuicksort(first, p); });
		auto f2 = std::async(std::launch::async, [&]() {ParallelSort::PQuicksort(p + 1, last); });
		f1.wait();
		f2.wait();		
	}	
}

//Parallel QSort function without async function  which recursively apply on the partition
template<class itr>
inline void ParallelSort::Quicksort(itr first, itr last)
{
	if (first < last)
	{
		auto p = Partition(first, last);
		ParallelSort::Quicksort(first, p); 
		ParallelSort::Quicksort(p + 1, last);
	}
}

template<class itr>
itr ParallelSort::Partition(itr first, itr last)
{
	auto pivot = *first;
	auto i = first + 1;
	auto j = last - 1;

	while (i <= j)
	{
		while (i <= j && *i <= pivot) i++;
		while (i <= j && *j > pivot) j--;
		if (i < j) std::iter_swap(i, j);
	}

	std::iter_swap(i - 1, first);
	return i - 1;
}

std::vector<char>  ParallelSort::RemoveBlankSpaces(std::string data)
{	
	data.erase(std::remove(data.begin(), data.end(), ' '),data.end());
	std::vector<char> vec;
	std::copy(data.begin(), data.end(), std::back_inserter(vec));
	return vec;
}

int ParallelSort::Next(std::string sortType)
{
	plineData sData = new char[100];
	m_nReadCurrentRecord++;
	if (m_nReadCurrentRecord + 1 >= (int)m_file_index.size())
	{
		return e_EOF;
	}

	const std::ifstream::pos_type pos = m_file_index[m_nReadCurrentRecord];
	m_ptrFile.seekg(pos,std::ios::beg);

	m_ptrFile.getline(sData,100);
	std::string sortString = sData;
	
	std::vector<char> sortVect = RemoveBlankSpaces(sortString);
	

	// sort call with par_unseq:
	if (sortType._Equal("CPlusPlusSort"))
	{		
		std::sort(std::execution::par_unseq, sortVect.begin(), sortVect.end());
		//std::cout << "\n Function running in thread = " << std::this_thread::get_id();
		m_writeMtx.lock();
		m_ptrWriteFile.seekp(pos, std::ios::beg);
		m_ptrWriteFile.write(reinterpret_cast<char*>(sortVect.data()), sortVect.size());
		m_writeMtx.unlock();
	}
	else if (sortType._Equal("PQuickSort"))
	{
		itr startElement = std::begin(sortVect);
		itr endElement = std::end(sortVect);
		Quicksort(startElement, endElement);
		m_writeMtx.lock();
		m_ptrWriteFile.seekp(pos, std::ios::beg);
		m_ptrWriteFile.write(reinterpret_cast<char*>(sortVect.data()), sortVect.size());
		m_writeMtx.unlock();
	}
	else if (sortType._Equal("PAsyncQuickSort"))
	{
		itr startElement = std::begin(sortVect);
		itr endElement = std::end(sortVect);
		PQuicksort(startElement, endElement);
		m_writeMtx.lock();
		m_ptrWriteFile.seekp(pos, std::ios::beg);
		m_ptrWriteFile.write(reinterpret_cast<char*>(sortVect.data()), sortVect.size());
		m_writeMtx.unlock();
	}
	else if (sortType._Equal("NoSort"))
	{
		m_ptrWriteFile.seekp(pos, std::ios::beg);
		m_ptrWriteFile.write(reinterpret_cast<char*>(sortVect.data()), sortVect.size());
	}
	else
	{
		std::cout << "Parameter provided for sorting doesn't match any options" << std::endl;
		std::cout << " provide sorting techniques parameter either NoSort,CPlusPlusSort,PQuickSort, or PAsyncQuickSort" << std::endl;
		return e_NOK;
		
	}
	return e_OK;
}

int ParallelSort::Peek()
{
	if (0 > m_nReadCurrentRecord)
	{
		return e_EOF;
	}

	const std::ifstream::pos_type  pos = m_file_index[m_nReadCurrentRecord];
	m_ptrFile.seekg(pos,std::ios::beg);
	
	if (m_ptrFile.tellg() < 0)
		return e_NOK;
	else
		return e_OK;
}

void ParallelSort::Run(int threadCount, std::string inputFileName, std::string outputFileName, std::vector<std::string> sortType)
{
	//time the execution speed to compare various sorting techniques
	const high_resolution_clock::time_point t1 = high_resolution_clock::now();
	//Open the input file in read mode ,this function also maps the position of lines into m_file_index vector
	OpenReadFile(inputFileName.c_str());
	//Open Output file in write mode
	OpenWriteFile(outputFileName.c_str());
	//just incase the outputfile was already opened and had some data i am just rewinding it so i can overwrite and start from begining
	RewindWriteFile();

	//create vector of threads
	std::vector<std::thread>threads;
	threads.reserve(Max_Thread_Count);

	//get total num of records to read from input file
	const size_t recCount = GetTotalRecords();

	for (auto sort : sortType)
	{
		for (unsigned int j = 0; j < recCount / Max_Thread_Count; j++)
		{
			for (int i = 0; i < Max_Thread_Count; i++) {
				std::thread th(&ParallelSort::Next, this, sort);
				threads.push_back(std::move(th));
			}
			for (auto& thread : threads)
			{
				thread.join();
			}
			threads.clear();
		}
		const high_resolution_clock::time_point t2 = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(t2 - t1).count();
		std::cout << duration;
	}
}


ParallelSort::status ParallelSort::Initialize()
{
	readPosition = m_ptrFile.tellg();

	m_bExceededFileLimits = false;

	m_bReadOnly = false;
	m_strError[0] = '\0';

	m_nReadCurrentRecord = -1;
	m_file_index.clear();

	return e_OK;
}

ParallelSort::status ParallelSort::GenerateIndex()
{
	const ParallelSort::status retVal = e_OK;
	m_file_index.clear();
	char data[100] = "";
	std::ifstream::pos_type position ;
	position = m_ptrFile.tellg();
	m_file_index.push_back(position);
	while (m_ptrFile && !m_ptrFile.eof())
	{
		m_ptrFile.getline(data, 100);
		position = m_ptrFile.tellg();
		m_file_index.push_back(position);
	}

	Rewind();
	m_nReadCurrentRecord = 0;
	return retVal;
}



ParallelSort::status ParallelSort::OpenWriteFile(const char* pFilename)
{
	ParallelSort::status status = e_OK;

	// open specified file for reading
	m_ptrWriteFile.open(pFilename, std::ios::in | std::ios::out | std::ios::binary);

	if (m_ptrWriteFile.good())
	{
			status = e_OK;	
	}
	else {
		strcpy_s(m_strError, 25, "Error: Cannot Open file ");
		strcat_s(m_strError, pFilename);
		status = e_NOK;
	}
	//m_sharedPtr_WriteFile.m_ptrWriteFile;
	return status;
}

ParallelSort::status ParallelSort::OpenReadFile(const char* pFilename)
{
	ParallelSort::status status = e_OK;
	m_ptrFile.imbue(std::locale::classic());
	// open specified file for reading
	m_ptrFile.open(pFilename, std::ios::in  | std::ios::binary);

	if (m_ptrFile.good())
	{	
			status = e_OK;
			status = ParallelSort::status(status | GenerateIndex());
	
		if (status == e_NOK) {
			strcpy_s(m_strError, "Error: Cannot index file ");
			strcat_s(m_strError, pFilename);
		}
	}
	else {
		strcpy_s(m_strError, "Error: Cannot Open file ");
		strcat_s(m_strError, pFilename);
		status = e_NOK;
	}

	return status;
}

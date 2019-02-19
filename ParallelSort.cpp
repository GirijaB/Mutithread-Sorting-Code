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
#define Max_Thread_Count 4

ParallelSort::ParallelSort()
{
	Initialize();
	maxLineData = 100;
}

ParallelSort::~ParallelSort()
{
 //Destructor;
}

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
	return m_log_index.size();
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

template<class itr>
inline void ParallelSort::pquicksort(itr first, itr last)
{
	if (first < last)
	{
		auto p = partition(first, last);
		auto f1 = std::async(std::launch::async, [&]() {ParallelSort::pquicksort(first, p); });
		auto f2 = std::async(std::launch::async, [&]() {ParallelSort::pquicksort(p + 1, last); });
		f1.wait();
		f2.wait();		
	}	
}

template<class itr>
itr ParallelSort::partition(itr first, itr last)
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

void ParallelSort::Next(std::string sortType)
{
	lineData sData = new char[100];
	m_nReadCurrentRecord++;
	if (m_nReadCurrentRecord + 1 >= (int)m_log_index.size())
	{
		return;
	}

	const std::ifstream::pos_type pos = m_log_index[m_nReadCurrentRecord];
	m_ptrFile.seekg(pos,std::ios::beg);

	m_ptrFile.getline(sData,100);
	std::string sortString = sData;
	
	std::vector<char> sortVect = RemoveBlankSpaces(sortString);
	
	//std::sort(sortVect.begin(), sortVect.end());
	// same sort call as above, but with par_unseq:
	if (sortType._Equal("CPlusPlusSort"))
	{		
		std::sort(std::execution::par_unseq, sortVect.begin(), sortVect.end());
		//std::cout << "\n Function running in thread = " << std::this_thread::get_id();
		m_ptrWriteFile.seekp(pos, std::ios::beg);
		m_ptrWriteFile.write(reinterpret_cast<char*>(sortVect.data()), sortVect.size());
		
	}
	else if (sortType._Equal("ParallelSort"))
	{
		itr startElement = std::begin(sortVect);
		itr endElement = std::end(sortVect);
		pquicksort(startElement, endElement);
		writeMtx.lock();
		m_ptrWriteFile.seekp(pos, std::ios::beg);
		m_ptrWriteFile.write(reinterpret_cast<char*>(sortVect.data()), sortVect.size());
		writeMtx.unlock();
	}
	else if (sortType._Equal("NoSort"))
	{
		m_ptrWriteFile.seekp(pos, std::ios::beg);
		m_ptrWriteFile.write(reinterpret_cast<char*>(sortVect.data()), sortVect.size());
	}
}

void ParallelSort::Peek()
{
	if (0 > m_nReadCurrentRecord)
	{
		return;
	}

	const std::ifstream::pos_type  pos = m_log_index[m_nReadCurrentRecord];
	m_ptrFile.seekg(pos,std::ios::beg);
	
	if (m_ptrFile.tellg() < 0)
		return;
	else
		return;
}

void ParallelSort::Run(int threadCount, std::string inputFileName, std::string outputFileName, std::vector<std::string> sortType)
{
	const high_resolution_clock::time_point t1 = high_resolution_clock::now();
	OpenReadFile(inputFileName.c_str());
	OpenWriteFile(outputFileName.c_str());
	RewindWriteFile();
	std::vector<std::thread>threads;
	threads.reserve(Max_Thread_Count);
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
	m_log_index.clear();

	return e_OK;
}

ParallelSort::status ParallelSort::GenerateIndex()
{
	const ParallelSort::status retVal = e_OK;
	m_log_index.clear();
	char data[100] = "";
	std::ifstream::pos_type position ;
	position = m_ptrFile.tellg();
	m_log_index.push_back(position);
	while (m_ptrFile && !m_ptrFile.eof())
	{
		m_ptrFile.getline(data, 100);
		position = m_ptrFile.tellg();
		m_log_index.push_back(position);

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

//  ParallelSort.h: interface for the reading file.

#if !defined(_ParallelSort_H__)
#define _ParallelSort_H__

/******************************************************************************
FileName   : ParallelSort.h

Author     : Girija 

Date       : 16 feb, 2019

Description: File Handling and Sorting techniques implementations

Version No : Date of update:         Updated by:           Changes:
Issues     : None

******************************************************************************/

#include <stdio.h>
#include<iostream>
#include<istream>
#include <vector>
#include <fstream>
#include <mutex>

typedef std::vector<char>::iterator itr;

class ParallelSort  {
public:
	//Enums used as return type while reading or writing file e_OK means function/statement executed as expected
	//e_NOK - function/statement did not execute as expected, e_EOF - Function/statement reached end of file.
	enum status {
		e_OK = 0,
		e_NOK,
		e_EOF,
	};

	//enums to use for accessing file in fwd backward or in any direction.
	typedef enum { e_FWD, e_BCK, e_ANY } enSEARCH_DIR;

	//maximum size of characters a line in a file can hold
	int m_maxLineData;
	
	//constructor
	ParallelSort();

	//destructor
	virtual ~ParallelSort();

	//char * to use for reading line data
	typedef char *plineData;
	plineData lineInfo;

	// ----------------------
	// Operations on new files
	// ----------------------
	status Create(const char* pFilename);
	
	// ----------------------
	// Operations on existing files
	// ----------------------
	//Open Input File as just istream file since we wont be performing any write operations to input file.
	virtual status OpenReadFile(const char* pFilename);

	//Open Output File as ostream file since we will be writing to the file.
	virtual status OpenWriteFile(const char* pFilename);

	//Rewind Input file if reached eof, to get back to beg again
	void Rewind();

	//Rewind Output file if reached eof, to get back to beg again
	void RewindWriteFile();

	//move to next line in the file 
	int Next(std::string sortType);

	//check if data exists in the current position in the file
	int Peek();

	//spawns number of threads to spawn,uses inputfile name and reads inputfile, checks if output file exists, and pass on sorting technique 
	void Run(int threadCount, std::string inputFileName, std::string outputFileName, std::vector<std::string> sortType);

	//Parallel QSort function using async functions which recursively apply on the partition 
	template<class itr> void PQuicksort(itr first, itr last);

	//Parallel QSort function without async function  which recursively apply on the partition 
	template<class itr> void Quicksort(itr first, itr last);

	//Partitions the range to be sorted into two parts one which is less than the pivot and another larger than the pivot
	template<class itr> itr Partition(itr first, itr last);

	//Get total number of records in the read file
	size_t GetTotalRecords(); 

	//removes blank spaces in the line data
	std::vector<char> RemoveBlankSpaces(std::string data);

private:
	
	std::mutex m_readMtx;
	std::mutex m_writeMtx;
	std::ifstream m_ptrFile;
	std::ofstream m_ptrWriteFile;
	std::shared_ptr < std::ofstream> m_sharedPtr_WriteFile;
	std::ifstream::pos_type readPosition;
	std::ifstream::pos_type currentWritePosition;
	bool m_bReadOnly;
	bool m_bExceededFileLimits;
	char m_strError[100];     // Error string

	std::vector<std::ifstream::pos_type> m_file_index;
	int m_nReadCurrentRecord;

	status Initialize();
	status GenerateIndex();
};

#endif //_ParallelSort_H__


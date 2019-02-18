//  FileHandler.h: interface for the reading file.

#if !defined(_FILEHANDLER_H__)
#define _FILEHANDLER_H__

/******************************************************************************
FileName   : FileHandler.h

Author     : Girija 

Date       : 16 feb, 2019

Description: File Handling method implementations

Version No : Date of update:         Updated by:           Changes:
Issues     : None

******************************************************************************/

#include <stdio.h>
#include<iostream>
#include<istream>
#include <vector>
#include <fstream>
#include <mutex>

/*
*/


class FileHandler  {
public:
	enum status {
		e_OK = 0,
		e_NOK,
		e_EOF,
	};
	typedef enum { FWD, BCK, ANY } enSEARCH_DIR;
	int maxLineData;
	
	FileHandler();
	virtual ~FileHandler();
	typedef char *lineData;
	lineData lineInfo;
	// ----------------------
	// Operations on new files
	// ----------------------
	status Create(const char* pFilename);
	
	// ----------------------
	// Operations on existing files
	// ----------------------
	virtual status OpenReadFile(const char* pFilename);	
	virtual status OpenWriteFile(const char* pFilename);

	void Rewind();


	void Next(std::string sortType);
	void Peek();
	void Run(int threadCount, std::string inputFileName, std::string outputFileName, std::string sortType);
	template<class Randomlt> void pquicksort(Randomlt first, Randomlt last);
	template<class Randomlt> Randomlt partition(Randomlt first, Randomlt last);
	unsigned int GetTotalRecords() { return (unsigned int) m_log_index.size(); }
	std::vector<char> RemoveBlankSpaces(std::string data);
private:

	std::mutex readMtx;
	std::mutex writeMtx;
	std::ifstream m_ptrFile;
	std::ofstream m_ptrWriteFile;
	std::shared_ptr < std::ofstream> m_sharedPtr_WriteFile;
	std::ifstream::pos_type readPosition;
	std::ifstream::pos_type currentWritePosition;
	bool m_bReadOnly;
	bool m_bExceededFileLimits;
	char m_strError[100];     // Error string
	std::vector<std::ifstream::pos_type> m_log_index;
	int m_nReadCurrentRecord;

	status Initialize();
	status GenerateIndex();
};

#endif //_FILEHANDLER_H__

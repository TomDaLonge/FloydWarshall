#pragma once

#include <map>
#include <list>
#include <string>

#include "tinyxml\tinyxml.h"

class Logger;

#define INF INT_MAX

using namespace std;

class FileHandler
{
public:
	FileHandler();
	~FileHandler();

	bool createMatrixFromDoc(	const char* InputFileName,
								map<int, map<int, int>*>* pMatrix,
								int& NodeCount);
	bool writeMatrixToXML(		map<int, map<int, int>*>* pMatrix);

private:
	Logger* m_pLogger;

	string mInputFileName;
};

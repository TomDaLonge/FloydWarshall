#pragma once

#include <string>
#include <map>
#include <list>
#include <utility>

class Logger;
class FileHandler;

#define INF INT_MAX

using namespace std;

class ProcedureControl
{
public:
	ProcedureControl();
	~ProcedureControl();

	bool Start(const char* InputFileName);

private:
	FileHandler* m_pFileHandler;
	Logger* m_pLogger;

	map<int, map<int, int>*>* m_pMatrix;

	bool findNodeConnection(int source, int target, int& weight);
};


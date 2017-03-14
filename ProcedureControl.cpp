#include "ProcedureControl.h"

#include "Logger.h"
#include "FileHandler.h"

#include <limits.h>
#include <cstdlib>
#include <ctime>

ProcedureControl::ProcedureControl()
{
	m_pFileHandler = new FileHandler();
	m_pLogger = Logger::getLogger();
	m_pMatrix	= new map<int, map<int, int>*>;
}

ProcedureControl::~ProcedureControl()
{
	delete m_pFileHandler;

	size_t MatrixSize = m_pMatrix->size();
	for (int i = 0; i < MatrixSize; i++)
	{
		(*m_pMatrix)[i]->clear();
		delete (*m_pMatrix)[i];
	}
	m_pMatrix->clear();
	delete m_pMatrix;
}

bool ProcedureControl::Start(const char* InputFileName)
{
	bool Success = true;

	m_pLogger->LogTrace("=================================================================");
	m_pLogger->LogTrace("==                Preparing Floyd-Warshall..                   ==");
	m_pLogger->LogTrace("=================================================================");
	m_pLogger->LogTrace("");

	//Start timestamp for Read-in process
	time_t ReadInStart;
	time(&ReadInStart);

	int NodeCount = 0;
	Success &= m_pFileHandler->createMatrixFromDoc(InputFileName, m_pMatrix, NodeCount);

	//End timestamp for Read-in process
	time_t ReadInEnd;
	time(&ReadInEnd);

	//Calculate the time needed for Read-in process
	int ReadInSeconds = static_cast<int>(difftime(ReadInEnd, ReadInStart));
	int ReadInMinutes = ReadInSeconds / 60;

	m_pLogger->LogTrace("");
	m_pLogger->LogTrace("=================================================================");
	m_pLogger->LogTrace("==                 Starting Floyd-Warshall..                   ==");
	m_pLogger->LogTrace("=================================================================");
	m_pLogger->LogTrace("");

	//Start timestamp for algorithm
	time_t AlgorithmStart;
	time(&AlgorithmStart);

	if (Success && (0 != NodeCount))
	{
		for (int k = 0; k < NodeCount; k++)
		{
			//m_pLogger->LogTrace("K = " + to_string(k) + " / " + to_string(NodeCount));
			#pragma omp parallel for
			for (int SourceY = 0; SourceY < NodeCount; SourceY++)
			{
				int weightToK = INF;
				bool hasConnectionToK = findNodeConnection(SourceY, k, weightToK);
				if (hasConnectionToK)
				{
					map<int, int>* DistancesFromK = m_pMatrix->at(k);
					for (int TargetX = 0; TargetX < NodeCount; TargetX++)
					{
						int weightFromK = DistancesFromK->at(TargetX);
						if (INF != weightFromK)
						{
							int newWeight = weightToK + weightFromK;
							if (m_pMatrix->at(SourceY)->at(TargetX) > newWeight)
							{
								//m_pLogger->LogDebug("K" + to_string(k) + " | Changed [" + to_string(SourceY) + ", " +
								//	to_string(TargetX) + "] from " + to_string(m_pMatrix->at(SourceY)->at(TargetX)) +
								//	" to " + to_string(newWeight) + ".");
								m_pMatrix->at(SourceY)->at(TargetX) = newWeight;
							}
						}
					}
				}
			}
		}
	}

	//End timestamp for algorithm
	time_t AlgorithmEnd;
	time(&AlgorithmEnd);
	
	//Calculate the time needed for algortihm
	int AlgorithmSeconds = static_cast<int>(difftime(AlgorithmEnd, AlgorithmStart));
	int AlgorithmMinutes = AlgorithmSeconds / 60;

	//Start timestamp for write process
	time_t WriteResultStart;
	time(&WriteResultStart);

	if (Success)
	{
		Success &= m_pFileHandler->writeMatrixToXML(m_pMatrix);
	}

	//End timestamp for write process
	time_t WriteResultEnd;
	time(&WriteResultEnd);

	//Calculate the time needed for write process
	int WriteResultSeconds = static_cast<int>(difftime(WriteResultEnd, WriteResultStart));
	int WriteResultMinutes = WriteResultSeconds / 60;

	m_pLogger->LogTrace("");
	m_pLogger->LogTrace("=================================================================");
	m_pLogger->LogTrace("==                 Finished Floyd-Warshall..                   ==");
	m_pLogger->LogTrace("=================================================================");
	m_pLogger->LogTrace("");

	m_pLogger->LogTrace("Needed " + to_string(ReadInMinutes / 60) + " hours, " + 
		to_string(ReadInMinutes % 60) + " minutes and " + to_string(ReadInSeconds % 60) +
		" seconds for Read-In Process.");
	m_pLogger->LogTrace("Needed " + to_string(AlgorithmMinutes / 60) + " hours, " + 
		to_string(AlgorithmMinutes % 60) + " minutes and " + to_string(AlgorithmSeconds % 60) +
		" seconds for Floyd-Warshall-Algorithm.");
	m_pLogger->LogTrace("Needed " + to_string(WriteResultMinutes / 60) + " hours, " + 
		to_string(WriteResultMinutes % 60) + " minutes and " + to_string(WriteResultSeconds % 60) +
		" seconds to write results to XML.");

	return Success;
}

bool ProcedureControl::findNodeConnection(int source, int target, int& weight)
{
	bool hasConnection = (INF != m_pMatrix->at(source)->at(target));
	if (hasConnection)
	{
		weight = m_pMatrix->at(source)->at(target);
	}

	return hasConnection;
}

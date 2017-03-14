// FloydWarshallSingleCore.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include "Logger.h"
#include "ProcedureControl.h"

#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
	bool Success = true;

	//First set standard values, if input arguments exist, change to them
	//InputFileName is demanded, others are optional and have standard values in case they are not given by user
	const char* InputFileName = "";
	int LogLevel = 2;
	bool DumpToFile = false;
	string OutputFileName = "";

	//Check input parameters
	if (1 < argc && argv[1] != nullptr)
	{
		InputFileName = argv[1];

		if (2 < argc)
		{
			//Returns zero also if there is no convertible Integer,
			//so a wrong input leads to LogLevel Debug, which is not a beautiful implementation but it works
			LogLevel = atoi(argv[2]);

			if (3 < argc)
			{
				string BoolString = argv[3];
				if (BoolString == "true")
				{
					DumpToFile = true;
				}

				if (4 < argc)
				{
					OutputFileName = argv[4];
				}
			}
		}
	}
	else
	{
		//Do not know where to read from, InputFileName is demanded
		Success &= false;
	}

	//Get Logger instance
	Logger* iLogger = Logger::getLogger();
	iLogger->setLogLevel(LogLevel);
	iLogger->setDumpToFile(DumpToFile, OutputFileName);

	//Header
	iLogger->LogTrace("#################################################################");
	iLogger->LogTrace("##                Floyd-Warshall | Tom Daniek                  ##");
	iLogger->LogTrace("##   Parallel Programming | Intelligent Systems Design | HSHL  ##");
	iLogger->LogTrace("#################################################################");
	iLogger->LogTrace("");

	//Only do it if there is a name given for the InputFile
	if (Success)
	{
		//Start timestamp
		time_t StartingTime;
		time(&StartingTime);

		ProcedureControl* MainControl = new ProcedureControl();

		//Start ILPOptimizer
		if (MainControl->Start(InputFileName))
		{
			iLogger->LogTrace("");
			iLogger->LogTrace("Everything was successful.");
		}
		else
		{
			iLogger->LogTrace("");
			iLogger->LogTrace("Seems that something went wrong.");
		}

		//End timestamp
		time_t EndingTime;
		time(&EndingTime);

		//Calculate the time needed
		int totalSeconds = static_cast<int>(difftime(EndingTime, StartingTime));
		int totalMinutes = totalSeconds / 60;
		iLogger->LogTrace("");
		
		iLogger->LogTrace("Needed " + to_string(totalMinutes / 60) + " hours, " +
			to_string(totalMinutes % 60) + " minutes and " + to_string(totalSeconds % 60) +
			" seconds for whole computation.");

		//How many warnings/errors were found?
		iLogger->LogTrace("");
		iLogger->printAnalysis();
		iLogger->LogTrace("");

		delete MainControl;
	}
	else
	{
		iLogger->LogError("Floyd Warshall crashed at the beginning. Please give at least the name of the file you want to read from.");
	}

	//CleanUp
	iLogger->deleteLogger();

	//Wait for any key
	//system("pause");
    return 0;
}


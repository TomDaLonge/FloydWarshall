#include "FileHandler.h"

#include "Logger.h"
#include <ctime>


FileHandler::FileHandler()
{
	m_pLogger = Logger::getLogger();
}


FileHandler::~FileHandler()
{
}

bool FileHandler::createMatrixFromDoc(	const char * InputFileName, 
										map<int, map<int, int>*>* pMatrix, 
										int& NodeCount)
{
	bool Success = true;

	mInputFileName = InputFileName;

	if (!pMatrix->empty())
	{
		m_pLogger->LogWarning("FileHandler: Given Matrix was NOT empty. Had to clear it.");
		size_t sizeOfMatrix = pMatrix->size();
		for (int i = 0; i < sizeOfMatrix; i++)
		{
			(*pMatrix)[i]->clear();
			delete (*pMatrix)[i];
		}
		pMatrix->clear();
	}

	TiXmlDocument* doc = new TiXmlDocument(InputFileName);
	bool loadOkay = doc->LoadFile();
	TiXmlHandle docHandle(doc);

	//Check Graph mode
	TiXmlElement* GraphElement = docHandle.FirstChild("gexf").FirstChild("graph").ToElement();
	/*if (!(GraphElement->Attribute("mode") == "static") ||
		!(GraphElement->Attribute("defaultedgetype") == "directed"))
	{
		m_pLogger->LogError("Graph must be static and edges must be directed. I can't do anything else atm.");
		Success &= false;
	}*/

	//Count Children and check ID/Label
	TiXmlNode* NodeParentNode = docHandle.FirstChild("gexf").FirstChild("graph").FirstChild("nodes").ToNode();
	if (NodeParentNode && Success)
	{
		TiXmlNode* currentChildNode;
		for (currentChildNode = NodeParentNode->FirstChild("node"); currentChildNode;
			currentChildNode = currentChildNode->NextSibling("node"))
		{
			TiXmlElement* currentChildNodeElement = currentChildNode->ToElement();
			if (to_string(NodeCount) == currentChildNodeElement->Attribute("id") &&
				to_string(NodeCount) == currentChildNodeElement->Attribute("label"))
			{
				NodeCount++;
			}
			else
			{
				m_pLogger->LogWarning("FileHandler: Issue found: Id or Label of Node Nr." + to_string(NodeCount) +
					" did NOT fit the Id or Label. Normalized that. Node is now known under the Nr. " + to_string(NodeCount));
				NodeCount++;
			}
		}
		m_pLogger->LogTrace("Found " + to_string(NodeCount) + " Nodes.");
	}
	else
	{
		m_pLogger->LogError("FileHandler: Could not find any nodes.");
		Success &= false;
	}
	
	TiXmlNode* EdgeParentNode = docHandle.FirstChild("gexf").FirstChild("graph").FirstChild("edges").ToNode();
	if (EdgeParentNode && Success)
	{
		//Set the standard values 0 and INF
		for (int row = 0; row < NodeCount; row++)
		{
			map<int, int>* currentRow = new map<int, int>;
			
			for (int col = 0; col < NodeCount; col++)
			{
				if (row == col)
				{
					(*currentRow)[col] = 0;
				}
				else
				{
					(*currentRow)[col] = INF;
				}
			}
			(*pMatrix)[row] = currentRow;
		}

		//Set the values of the direct connections
		TiXmlNode* currentChildEdge;
		int EdgeCount = 0;
		for (currentChildEdge = EdgeParentNode->FirstChild("edge"); currentChildEdge;
			currentChildEdge = currentChildEdge->NextSibling("edge"))
		{
			TiXmlElement* currentChildEdgeElement = currentChildEdge->ToElement();
			int SourceId = atoi(currentChildEdgeElement->Attribute("source"));
			int TargetId = atoi(currentChildEdgeElement->Attribute("target"));
			int Weight   = atoi(currentChildEdgeElement->Attribute("weight"));

			pMatrix->at(SourceId)->at(TargetId) = Weight;
			
			EdgeCount++;
		}
		m_pLogger->LogTrace("Found " + to_string(EdgeCount) + " Edges.");
	}
	else
	{
		m_pLogger->LogError("Could not find any edges.");
		Success &= false;
	}

	delete doc;

	return Success;
}

bool FileHandler::writeMatrixToXML(map<int, map<int, int>*>* pMatrix)
{
	bool Success = true;

	//TODO: Check if doc is open

	//Basics
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "");
	TiXmlElement* rootElement = new TiXmlElement("gexf");

	//Meta data
	TiXmlElement* metaElement = new TiXmlElement("meta");
	metaElement->SetAttribute("lastmodifieddate", " ");
	rootElement->LinkEndChild(metaElement);

	TiXmlElement* creatorElement = new TiXmlElement("creator");
	metaElement->LinkEndChild(creatorElement);

	TiXmlElement* descrElement = new TiXmlElement("description");
	metaElement->LinkEndChild(descrElement);

	//Graph data
	TiXmlElement* graphElement = new TiXmlElement("graph");
	graphElement->SetAttribute("mode", "static");
	graphElement->SetAttribute("defaultedgetype", "directed");
	rootElement->LinkEndChild(graphElement);

	size_t matrixSize = pMatrix->size();

	for (int sourceId = 0; sourceId < matrixSize; sourceId++)
	{
		TiXmlElement* SourceElement = new TiXmlElement("SourceNode");
		SourceElement->SetAttribute("Id", sourceId);
		graphElement->LinkEndChild(SourceElement);
		for (int targetId = 0; targetId < matrixSize; targetId++)
		{
			TiXmlElement* TargetElement = new TiXmlElement("TargetNode");
			TargetElement->SetAttribute("Id", targetId);
			TargetElement->SetAttribute("Weight", pMatrix->at(sourceId)->at(targetId));
			SourceElement->LinkEndChild(TargetElement);
		}
	}

	doc.LinkEndChild(decl);
	doc.LinkEndChild(rootElement);

	//Datetime information for filename
	time_t currentTime;
	struct tm localTime;
	time(&currentTime);
	localtime_s(&localTime, &currentTime);

	string Year = to_string(localTime.tm_year + 1900);
	string Month = to_string(localTime.tm_mon + 1);
	string Day = to_string(localTime.tm_mday);

	string Hour = to_string(localTime.tm_hour);
	string Min = to_string(localTime.tm_min);
	string Sec = to_string(localTime.tm_sec);

	string InputName = mInputFileName.substr(0, mInputFileName.find(".", 0));

	string FileName = Year + "-" + Month + "-" + Day + "_" +
		Hour + "-" + Min + "-" + Sec + "-" + InputName +"-Output.xml";
	doc.SaveFile(FileName.c_str());

	return Success;
}

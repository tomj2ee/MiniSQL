#include "recordManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
using namespace std;

//************************************
// Method:    checkRecordCondition
// FullName:  checkRecordCondition
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: char * recordBegin
// Parameter: int recordSize
// Parameter: vector<Attribute> * attributeVector
// Parameter: vector<Condition> * conditionVector
// Function:  Check if the types fit in the right form
//************************************
bool RecordManager::checkRecordCondition(char* recordAddress, int recordSize, vector<Attribute>* vecAttribute, vector<Condition>* vecCondition)
{
	if (!vecCondition)
		return false;
	else
	{
		int i, j, type, typeSize;
		string attributeName;
		char *recordAddressTemp = recordAddress;
		char contentTemp[255];

		for (i = 0; i < vecAttribute->size(); i++)
		{
			type = (*vecAttribute)[i].type;
			typeSize = api->typeSizeGet(type);
			attributeName = (*vecAttribute)[i].name;

			memset(contentTemp, 0, 255);
			memcpy(contentTemp, recordAddressTemp, typeSize);
			for (j = 0; j < vecCondition->size(); j++)
				if ((*vecCondition)[j].attributeName == attributeName)
				{
					if (type == Attribute::TYPE_INT)
					{
						int temp;
						temp = *((int *)contentTemp);
						if (!(*vecCondition)[j].ifRight(temp))
							return false;
					}
					else if (type == Attribute::TYPE_FLOAT)
					{
						float temp;
						temp = *((float *)contentTemp);
						if (!(*vecCondition)[j].ifRight(temp))
							return false;
					}
					else
					{
						if (!(*vecCondition)[j].ifRight(contentTemp))
							return false;
					}
				}
			recordAddress += typeSize;
		}
		return true;
	}
}

//************************************
// Method:    printRecord
// FullName:  RecordManager::printRecord
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: char * recordAddress
// Parameter: int recordSize
// Parameter: vector<Attribute> * vecAttribute
// Parameter: vector<string> * vecAttributeName
// Function:  Print the record from the recordAddress
//************************************
void RecordManager::printRecord(char* recordAddress, int recordSize, vector<Attribute>* vecAttribute, vector<string> *vecAttributeName)
{
	int i, j, type, typeSize;
	string attributeName;
	char *recordAddressTemp = recordAddress;
	char contentTemp[255];
	bool flag;

	for (i = 0; i < vecAttribute->size(); i++)
	{
		type = (*vecAttribute)[i].type;
		typeSize = api->typeSizeGet(type);

		memset(contentTemp, 0, 255);
		memcpy(contentTemp, recordAddressTemp, typeSize);
		for (j = 0; j < vecAttributeName->size(); j++)
			if ((*vecAttributeName)[j] == (*vecAttribute)[i].name)
			{
				if (type == Attribute::TYPE_INT)
				{
					int temp;
					temp = *((int *)contentTemp);
					printf("%d ", temp);
				}
				else if (type == Attribute::TYPE_FLOAT)
				{
					float temp;
					temp = *((float *)contentTemp);
					printf("%f ", temp);
				}
				else
				{
					string temp = contentTemp;
					printf("%s ", temp.c_str());
				}
			}
		recordAddress += typeSize;
	}
}

//************************************
// Method:    tableCreate
// FullName:  RecordManager::tableCreate
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Function:  
//************************************
int RecordManager::tableCreate(string tableName)
{
	tableName = tableFileNameGet(tableName);
	FILE *fp;
	fp = fopen(tableName.c_str(), "w+");
	if (fp == NULL)
		return 0;
	fclose(fp);
	return 1;
}

//************************************
// Method:    tableDrop
// FullName:  RecordManager::tableDrop
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Function:  
//************************************
int RecordManager::tableDrop(string tableName)
{
	string tableFileName = tableFileNameGet(tableName);
	bm.delete_fileNode(tableFileName.c_str());
	if (remove(tableFileName.c_str()))
		return 0;
	else
		return 1;
}

//************************************
// Method:    indexCreate
// FullName:  RecordManager::indexCreate
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: string indexName
// Function:  
//************************************
int RecordManager::indexCreate(string indexName)
{
	string indexFileName = indexFileNameGet(indexName);
	FILE *fp;
	fp = fopen(indexFileName.c_str(), "w+");
	if (fp == NULL)
		return 0;
	fclose(fp);
	return 1;
}

//************************************
// Method:    indexDrop
// FullName:  RecordManager::indexDrop
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: string indexName
// Function:  
//************************************
int RecordManager::indexDrop(string indexName)
{
	string indexFileName = indexFileNameGet(indexName);
	bm.delete_fileNode(indexFileName.c_str());
	if (remove(indexFileName.c_str()))
		return 0;
	return 1;
}

//************************************
// Method:    recordInsert
// FullName:  RecordManager::recordInsert
// Access:    private 
// Returns:   int, return offset if success, -1 otherwise
// Qualifier:
// Parameter: string tableName, the table to be inserted
// Parameter: char * record, the record text
// Parameter: int recordSize, size of record
// Function:  Find the table and insert a record into it
//************************************
int RecordManager::recordInsert(string tableName, char* record, int recordSize)
{
	string strTemp = tableFileNameGet(tableName);
	fileNode *fileTemp = bm.getFile(strTemp.c_str());
	blockNode *blockTemp = bm.getBlockHead(fileTemp);

	if (recordSize < 0)
		return ERROR;
	while (blockTemp != NULL)
	{
		if (bm.get_usingSize(*blockTemp) + recordSize <= bm.getBlockSize())
		{
			char *startAddress = bm.get_content(*blockTemp) + bm.get_usingSize(*blockTemp);
			memcpy(startAddress, record, recordSize);
			bm.set_dirty(*blockTemp);
			bm.set_usingSize(*blockTemp, bm.get_usingSize(*blockTemp) + recordSize);
			bm.writtenBackToDiskAll();
			return blockTemp->offsetNum;
		} 
		else
			blockTemp = bm.getNextBlock(fileTemp, blockTemp);
	}
	return ERROR;
}

//************************************
// Method:    recordBlockShow
// FullName:  RecordManager::recordBlockShow
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Parameter: vector<string> * attributeNameVector
// Parameter: vector<Condition> * conditionVector
// Parameter: blockNode * block
// Function:  Show the block
//************************************
int RecordManager::recordBlockShow(string tableName, vector<string>* vecAttributeName, vector<Condition>* vecCondition, blockNode *block)
{
	if (block == NULL)
		return ERROR;
	else
	{
		int countNum = 0, recordSize;
		vector<Attribute> vecAttributes;
		char *recordAddress, *blockAddress;
		size_t usingSize = bm.get_usingSize(*block);
		recordSize = api->recordSizeGet(tableName);
		api->attributeGet(tableName, &vecAttributes);
		recordAddress = blockAddress = bm.get_content(*block);
		while (recordAddress - blockAddress < usingSize)
		{
			if (checkRecordCondition(recordAddress, recordSize, &vecAttributes, vecCondition))
			{
				countNum++;
				printRecord(recordAddress, recordSize, &vecAttributes, vecAttributeName);
				std::cout << endl;
			}
			recordAddress += recordSize;
		}
	}

}

//************************************
// Method:    recordBlockShow
// FullName:  RecordManager::recordBlockShow
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Parameter: vector<string> * attributeNameVector
// Parameter: vector<Condition> * conditionVector
// Parameter: int blockOffset
// Function:  Show the block by a offset: find the block, 
//			  then show it by the reload function
//************************************
int RecordManager::recordBlockShow(string tableName, vector<string>* vecAttributeName, vector<Condition>* vecCondition, int blockOffset)
{
	string temp = tableFileNameGet(tableName);
	fileNode *fileTemp = bm.getFile(temp.c_str());
	blockNode *blockTemp = bm.getBlockByOffset(fileTemp, blockOffset);
	if (!blockTemp)
		return ERROR;
	else
		recordBlockShow(tableName, vecAttributeName, vecCondition, blockTemp);
	return 1;
}

//************************************
// Method:    recordAllShow
// FullName:  RecordManager::recordAllShow
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: string tableName, the table to be printed
// Parameter: vector<string> * attributeNameVector, the attribute list
// Parameter: vector<Condition> * conditionVector, the condition list
// Function:  Print all the information of the table
//************************************
int RecordManager::recordAllShow(string tableName, vector<string>* vecAttributeName, vector<Condition>* vecCondition)
{
	string strTemp = tableFileNameGet(tableName);
	fileNode *fileTemp = bm.getFile(strTemp.c_str());
	blockNode *blockTemp = bm.getBlockHead(fileTemp);
	int countNum = 0, oneRecordNum;

	while (blockTemp)
	{
		oneRecordNum = recordBlockShow(tableName, vecAttributeName, vecCondition, blockTemp);
		countNum += oneRecordNum;
		if (blockTemp->ifbottom)
			return countNum;
		else
			blockTemp = bm.getNextBlock(fileTemp, blockTemp);
	}
	return ERROR;
}

//************************************
// Method:    recordBlockFind
// FullName:  RecordManager::recordBlockFind
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Parameter: vector<Condition> * vecCondition
// Parameter: blockNode * block
// Function:  
//************************************
int RecordManager::recordBlockFind(string tableName, vector<Condition>* vecCondition, blockNode* block)
{
	if (block == NULL)
		return ERROR;
	else
	{
		int countNum = 0, recordSize;
		vector<Attribute> vecAttributes;
		char *recordAddress, *blockAddress;
		size_t usingSize = bm.get_usingSize(*block);
		recordSize = api->recordSizeGet(tableName);
		api->attributeGet(tableName, &vecAttributes);
		recordAddress = blockAddress = bm.get_content(*block);
		while (recordAddress - blockAddress < usingSize)
		{
			if (checkRecordCondition(recordAddress, recordSize, &vecAttributes, vecCondition))
				countNum++;
			recordAddress += recordSize;
		}
		return countNum;
	}
}

//************************************
// Method:    recordAllFind
// FullName:  RecordManager::recordAllFind
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Parameter: vector<Condition> * conditionVector
// Function:  
//************************************
int RecordManager::recordAllFind(string tableName, vector<Condition>* vecCondition)
{
	string strTemp = tableFileNameGet(tableName);
	fileNode *fileTemp = bm.getFile(strTemp.c_str());
	blockNode *blockTemp = bm.getBlockHead(fileTemp);
	int countNum = 0, oneRecordNum;

	while (blockTemp)
	{
		oneRecordNum = recordBlockFind(tableName, vecCondition, blockTemp);
		countNum += oneRecordNum;
		if (blockTemp->ifbottom)
			return countNum;
		else
			blockTemp = bm.getNextBlock(fileTemp, blockTemp);
	}
	return ERROR;
}

//************************************
// Method:    recordBlockDelete
// FullName:  RecordManager::recordBlockDelete
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Parameter: vector<Condition> * vecCondition
// Parameter: blockNode * block
// Function:  
//************************************
int RecordManager::recordBlockDelete(string tableName, vector<Condition>* vecCondition, blockNode* block)
{
	if (block == NULL)
		return ERROR;
	else
	{
		int countNum = 0, recordSize;
		vector<Attribute> vecAttributes;
		char *recordAddress, *blockAddress;
		size_t usingSize = bm.get_usingSize(*block);

		recordSize = api->recordSizeGet(tableName);
		api->attributeGet(tableName, &vecAttributes);
		recordAddress = blockAddress = bm.get_content(*block);
		while (recordAddress - blockAddress < usingSize)
		{
			if (checkRecordCondition(recordAddress, recordSize, &vecAttributes, vecCondition))
			{
				countNum++;
				api->recordIndexDelete(recordAddress, recordSize, &vecAttributes, block->offsetNum);
				int i = 0;
				for (i = 0; i + recordSize + recordAddress - bm.get_content(*block) < bm.get_usingSize(*block); i++)
				{
					recordAddress[i] = recordAddress[i + recordSize];
				}
				memset(recordAddress + i, 0, recordSize);
				bm.set_usingSize(*block, bm.get_usingSize(*block) - recordSize);
				bm.set_dirty(*block);
				bm.writtenBackToDiskAll();
			}
			recordAddress += recordSize;
		}
		return countNum;
	}
}

//************************************
// Method:    recordBlockDelete
// FullName:  RecordManager::recordBlockDelete
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Parameter: vector<Condition> * conditionVector
// Parameter: int blockOffset
// Function:  
//************************************
int RecordManager::recordBlockDelete(string tableName, vector<Condition>* vecCondition, int blockOffset)
{
	string temp = tableFileNameGet(tableName);
	fileNode *fileTemp = bm.getFile(temp.c_str());
	blockNode *blockTemp = bm.getBlockByOffset(fileTemp, blockOffset);
	if (!blockTemp)
		return ERROR;
	else
		recordBlockDelete(tableName, vecCondition, blockTemp);
	return 1;
}

//************************************
// Method:    recordAllDelete
// FullName:  RecordManager::recordAllDelete
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Parameter: vector<Condition> * conditionVector
// Function:  
//************************************
int RecordManager::recordAllDelete(string tableName, vector<Condition>* vecCondition)
{
	string strTemp = tableFileNameGet(tableName);
	fileNode *fileTemp = bm.getFile(strTemp.c_str());
	blockNode *blockTemp = bm.getBlockHead(fileTemp);
	int countNum = 0, oneRecordNum;

	while (blockTemp)
	{
		oneRecordNum = recordBlockDelete(tableName, vecCondition, blockTemp);
		countNum += oneRecordNum;
		if (blockTemp->ifbottom)
			return countNum;
		else
			blockTemp = bm.getNextBlock(fileTemp, blockTemp);
	}
	return ERROR;
}

//************************************
// Method:    indexRecordBlockAlreadyInsert
// FullName:  RecordManager::indexRecordBlockAlreadyInsert
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Parameter: string indexName
// Parameter: blockNode * block
// Function:  
//************************************
int RecordManager::indexRecordBlockAlreadyInsert(string tableName, string indexName, blockNode* block)
{
	//if block is null, return -1
	if (block == NULL)
		return -1;

	int countNum = 0, type, typeSize;
	char* recordBegin = bm.get_content(*block);
	vector<Attribute> vecAttribute;
	int recordSize = api->recordSizeGet(tableName);
	char * contentBegin;

	api->attributeGet(tableName, &vecAttribute);
	while (recordBegin - bm.get_content(*block) < bm.get_usingSize(*block))
	{
		contentBegin = recordBegin;
		//if the recordBegin point to a record
		for (int i = 0; i < vecAttribute.size(); i++)
		{
			type = vecAttribute[i].type;
			typeSize = api->typeSizeGet(type);
			//find the index  of the record, and insert it to index tree
			if (vecAttribute[i].index == indexName)
			{
				api->indexInsert(indexName, contentBegin, type, block->offsetNum);
				countNum++;
			}
			contentBegin += typeSize;
		}
		recordBegin += recordSize;
	}
	return countNum;
}

//************************************
// Method:    indexRecordAllAlreadyInsert
// FullName:  RecordManager::indexRecordAllAlreadyInsert
// Access:    private 
// Returns:   int
// Qualifier:
// Parameter: string tableName
// Parameter: string indexName
// Function:  
//************************************
int RecordManager::indexRecordAllAlreadyInsert(string tableName, string indexName)
{
	fileNode *fileTemp = bm.getFile(tableFileNameGet(tableName).c_str());
	blockNode *blockTemp = bm.getBlockHead(fileTemp);
	int count = 0;
	while (blockTemp)
	{
		if (blockTemp->ifbottom)
		{
			int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, blockTemp);
			count += recordBlockNum;
			return count;
		}
		else
		{
			int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, blockTemp);
			count += recordBlockNum;
			blockTemp = bm.getNextBlock(fileTemp, blockTemp);
		}
	}

	return -1;
}

//************************************
// Method:    indexFileNameGet
// FullName:  RecordManager::indexFileNameGet
// Access:    public 
// Returns:   std::string
// Qualifier:
// Parameter: string indexName
// Function:  Reset the index file name
//************************************
string RecordManager::indexFileNameGet(string indexName)
{
	return "INDEX_" + indexName + ".index";
}

//************************************
// Method:    tableFileNameGet
// FullName:  RecordManager::tableFileNameGet
// Access:    public 
// Returns:   std::string
// Qualifier:
// Parameter: string tableName
// Function:  Reform the table name
//************************************
string RecordManager::tableFileNameGet(string tableName)
{
	return "TABLE_" + tableName + ".tb";
}

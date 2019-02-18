#ifndef _RECORDMANAGER_H
#define _RECORDMANAGER_H

#include <string>
#include <vector>
#include "API.h"
#include "Condition.h"
#include "Attribute.h"
#include "Minisql.h"
#include "BufferManager.h"
#define ERROR -1
using namespace std;

class API;
class BufferManager;
class RecordManager {
public:
	RecordManager() {}
	BufferManager bm;
	API *api;
	//operate table file
	int tableCreate(string tableName);
	int tableDrop(string tableName);
	//operate index file
	int indexDrop(string indexName);
	int indexCreate(string indexName);
	//insert record
	int recordInsert(string tableName, char* record, int recordSize);
	//show record
	int recordAllShow(string tableName, vector<string>* vecAttributeName, vector<Condition>* vecCondition);
	int recordBlockShow(string tableName, vector<string>* vecAttributeName, vector<Condition>* vecCondition, blockNode *block);
	int recordBlockShow(string tableName, vector<string>* vecAttributeName, vector<Condition>* vecCondition, int blockOffset);
	//find all record
	int recordAllFind(string tableName, vector<Condition>* vecCondition);
	//delete record
	int recordAllDelete(string tableName, vector<Condition>* vecCondition);
	int indexRecordBlockAlreadyInsert(string tableName, string indexName, blockNode* block);
	int recordBlockDelete(string tableName, vector<Condition>* vecCondition, blockNode* block);
	int recordBlockDelete(string tableName, vector<Condition>* vecCondition, int blockOffset);
	int indexRecordAllAlreadyInsert(string tableName, string indexName);

	string indexFileNameGet(string indexName);
	string tableFileNameGet(string tableName);

private:
	bool checkRecordCondition(char* recordAddress, int recordSize, vector<Attribute>* vecAttribute, vector<Condition>* vecCondition);
	//int recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, blockNode* block);
	int recordBlockFind(string tableName, vector<Condition>* vecCondition, blockNode* block);
	//int indexRecordBlockAlreadyInsert(string tableName, string indexName, blockNode* block);

	void printRecord(char* recordAddress, int recordSize, vector<Attribute>* vecAttribute, vector<string> *vecAttributeName);
	//bool contentConditionFit(char* content, int type, Condition* condition);
	//void contentPrint(char * content, int type);
};

#endif // _RECORDMANAGER_H
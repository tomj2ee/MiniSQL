#ifndef _INDEXINOF_H
#define _INDEXINOF_H

#include <string>
using namespace std;

class IndexInfo
{
public:
	IndexInfo(string i,string t,string a,int ty)
    {indexName = i;tableName = t;Attribute = a;type = ty;}
    string indexName;
    string tableName;
    string Attribute;
    int type;
};

#endif // _INDEXINOF_H
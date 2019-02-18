#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <string>
#include <vector>
#include "API.h"
using namespace std;
class Interpreter{
public:
    
    API* ap;
	string fileName ;
	int interpreter(string s);	

	string getWord(string s, int *st);
	
	Interpreter(){}
	~Interpreter(){}
};

#endif // _INTERPRETER_H
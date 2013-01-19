// DbHandler.h
#include <string>
#include <vector>
#include "Room.h"
#include "Game.h"

// Database Modification
int strToInt(std::string buffer);
int replaceAll(std::string& buffer, char sub, char rep);
int replaceAllStr(std::string& buffer, std::string sub, std::string rep);
int findFirstStr(std::string buffer, std::string sub);
int findMax(int a, int b, int c, int d);
std::string toUpperStr(std::string buffer);
std::string toLowerStr(std::string buffer);
void fillVectorStr(std::vector<std::string>& v, std::string s);

template<typename T>
int isInVector(std::vector<T> v, T buffer);

int removeFromVector(std::vector<int>& v, int buffer);
void pushBackInd(std::vector<std::string>& v, std::string buffer, int ind);
void pushBackInd(std::vector<std::vector<std::string>>& v, std::vector<std::string> buffer, int ind);
void convertToNumber(std::string& buffer);
bool isScript(std::string buffer);
// Game Control
int execute(std::string script, Room* room, Game* game);
void printText(std::string text, Game* game = NULL);
void clearScreen();

template<typename T>
int isInVector(vector<T> v, T buffer)
{
	for (int i = 0; (size_t)i < v.size(); i++)
		if (v[i] == buffer)
			return i;
	return -1;
}
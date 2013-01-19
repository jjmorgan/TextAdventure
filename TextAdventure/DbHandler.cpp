// DbHandler.cpp
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <queue>
#include "DbHandler.h"
using namespace std;

// Declarations
#define CIN_IGNORE 10000
#define CONSOLE_WIDTH 80

int strToInt(string buffer)
{
	if (buffer.size() < 1) return 0;
	char b = buffer[0]; int negative = 1, i = 0, t = 0;
	if (b == '-') {
		negative = -1;
		if (buffer.size() == 1) return 0;
		i = 1;
	}
	for (i; (size_t)i < buffer.size(); i++)
	{
		b = buffer[i];
		if (!isdigit(b)) break;
		t *= 10;
		t += b - '0';
	}
	return t * negative;
}

int replaceAll(string& buffer, char sub, char rep)
{
	int j = 0;
	for (int i = 0; (size_t)i < buffer.size(); i++)
	{
		if (buffer[i] == sub) buffer[i] = rep;
		j++;
	}
	return j;
}

int replaceAllStr(string& buffer, string sub, string rep)
{
	int j = 0;
	if (buffer.size() < sub.size()) return 0;
	for (int i = 0; (size_t)i < buffer.size() - sub.size() + 1; i++)
	{
		if (buffer.substr(i, sub.size()) == sub)
		{
			buffer = buffer.substr(0, i) + rep + buffer.substr(i + sub.size());
			i += rep.size() - 1;
			j++;
		}
	}
	return j;
}

int findFirstStr(string buffer, string sub)
{
	for (int i = 0; (size_t)i < buffer.size(); i++)
		if (buffer.at(i) == sub.at(0))
			if (buffer.substr(i, sub.size()) == sub)
				return i;
	return -1;
}

int findMax(int a, int b, int c, int d)
{
	if (a >= b && a >= c && a >= d) return a;
	if (b >= a && b >= c && b >= d) return b;
	if (c >= a && c >= b && c >= d) return c;
	if (d >= a && d >= b && d >= c) return d;
	return 0;
}

string toUpperStr(std::string buffer)
{
	for (int i = 0; (size_t)i < buffer.size(); i++)
		buffer.at(i) = toupper(buffer.at(i));
	return buffer;
}

string toLowerStr(std::string buffer)
{
	for (int i = 0; (size_t)i < buffer.size(); i++)
		buffer.at(i) = tolower(buffer.at(i));
	return buffer;
}

void fillVectorStr(vector<string>& v, string s)
{
	for (int i = 0; (size_t)i < v.size(); i++)
		v[i] = s;
}

int removeFromVector(vector<int>& v, int buffer)
{
	for (int i = 0; (size_t)i < v.size(); i++)
		if (v[i] == buffer)
		{
			for (int j = i; (size_t)j < v.size() - 1; j++)
				v[j] = v[j + 1];
			v.pop_back();
			return 1;
			break;
		}
	return 0;
}

void pushBackInd(vector<string>& v, string buffer, int ind)
{
	if (v.size() > (size_t)ind + 1)
	{
		v[ind] = buffer;
		return;
	}
	while (v.size() < (size_t)ind)
		v.push_back("");
	v.push_back(buffer);
}

void pushBackInd(vector<vector<string>>& v, vector<string> buffer, int ind)
{
	if (v.size() > (size_t)ind)
		return;
	while (v.size() < (size_t)ind)
		v.push_back(vector<string> (10, ""));
	v.push_back(buffer);
}

void convertToNumber(std::string& buffer)
{
	// Implement later
}

bool isScript(string buffer)
{
	int b = buffer.find('['), s = buffer.find(';');
	return (b > 0 && (size_t)b < buffer.size() && (size_t)s < buffer.size() && b < s);
}

int execute(string script, Room* room, Game* game)
{
	int x, n, c, extraC, extraD; string sub, cmd, data, extraA, extraB, extraE, extraF, cut; bool test;
	char y;
	while (1)
	{
		extraA = ""; extraB = "";
		x = script.find(';');
		if ((size_t)x >= script.size()) break;
		n = script.find_first_of('{');
		if (n > 0 && n < x) { // Isolate conditional
			c = 1;
			for (int i = n + 1; (size_t)i < script.size(); i++)
			{
				y = script.at(i);
				switch (y) {
				case '{': c++; break;
				case '}': c--; break;
				case ';': if (c == 0)
					{
						x = i;
						y = -1;
					}
				}
				if (y == -1) break;
			}
		}
		sub = script.substr(0, x);
		cmd = sub.substr(0, 4);
		script = script.substr(x + 1);
		int comma = 0, bracket = 0, z = 0;
		if (sub.size() > 4)
		{
			data = sub.substr(5);
			comma = data.find(',');
			bracket = data.find(']');
			z = data.find('}');
		}

		// Standard Functions
		
		if (cmd == "prnt") // Print: prnt[Text]
			if (data.at(0) == '{') // Standard Text
				printText(game->stnd(strToInt(data.substr(1, z - 1))), game);
			else printText(data.substr(0, data.size() - 1), game);
		else if (cmd == "addo") // Add Object: addo[00000];
			game->addObj(strToInt(data.substr(0, bracket)));
		else if (cmd == "addi") // Add to Inventory: addi[00000];
			game->addInv(strToInt(data.substr(0, bracket)));
		else if (cmd == "addf") // Add to Followers: addf[00000];
			game->addFollow(strToInt(data.substr(0, bracket)));
		else if (cmd == "adds") // Add Object: addo[00000];
			game->addObjS(strToInt(data.substr(0, bracket)));
		else if (cmd == "addd") { // Add to Droped Items: addd[00000,000];
			extraA = data.substr(0, comma);
			extraB = data.substr(comma+1, bracket-comma);
			if (extraB.at(0) == '+') extraD = game->room(extraC)->state() + strToInt(extraB.substr(1));
			else if (extraB.at(0) == '-') extraD = game->room(extraC)->state() - strToInt(extraB.substr(1));
			else extraD = strToInt(extraB);
			game->room(strToInt(extraA))->addItem(extraD); }
		else if (cmd == "remo") // Remove Object: remo[00000];
			game->remObj(strToInt(data.substr(0, bracket)));
		else if (cmd == "remi") // Remove from Inventory: remi[00000];
			game->remInv(strToInt(data.substr(0, bracket)));
		else if (cmd == "remf") // Remove from Followers: remf[00000];
			game->remFollow(strToInt(data.substr(0, bracket)));
		else if (cmd == "remd") {// Remove from Dropped Items: remd[00000,000];
			extraA = data.substr(0, comma);
			extraB = data.substr(comma+1, bracket-comma);
			if (extraB.at(0) == '+') extraD = game->room(extraC)->state() + strToInt(extraB.substr(1));
			else if (extraB.at(0) == '-') extraD = game->room(extraC)->state() - strToInt(extraB.substr(1));
			else extraD = strToInt(extraB);
			game->room(strToInt(extraA))->remItem(extraD); }
		else if (cmd == "stor") { // Store an object, remove from object list: stor[00000];
			game->remObj(strToInt(data.substr(0, comma)));
			room->remItem(strToInt(data.substr(0, comma)));
			room->addRemoved(strToInt(data.substr(0, comma)));
			game->addInv(strToInt(data.substr(comma+1, bracket-comma))); }
		else if (cmd == "drop") { // Drop an object, add to object list: drop[00000];
			game->remInv(strToInt(data.substr(0, comma)));
			game->addObj(strToInt(data.substr(comma+1, bracket-comma)));
			room->addItem(strToInt(data.substr(comma+1, bracket-comma))); }
		else if (cmd == "equi") { // Equip an object to a slot: equi[00000,0]
			game->setEquip(strToInt(data.substr(0, comma)), strToInt(data.substr(comma+1, bracket-comma))); }
		else if (cmd == "rmeq") { // Dequip a slot: rmeq[0]
			game->setEquip(0,strToInt(data.substr(0, bracket))); }
		else if (cmd == "rsto") { // Allow object in room to be replenished
			game->room(strToInt(data.substr(comma+1, bracket-comma)))->remRemoved(strToInt(data.substr(0, comma))); }
		else if (cmd == "stat") { // Set Room State: stat[000,01];
			extraA = data.substr(0, comma);
			extraB = data.substr(comma+1, bracket-comma);
			if (extraA.at(0) == '+') extraC = game->roomCurrent() + strToInt(extraA.substr(1));
			else if (extraA.at(0) == '-') extraC = game->roomCurrent() - strToInt(extraA.substr(1));
			else extraC = strToInt(extraA);
			if (extraB.at(0) == '+') extraD = game->room(extraC)->state() + strToInt(extraB.substr(1));
			else if (extraB.at(0) == '-') extraD = game->room(extraC)->state() - strToInt(extraB.substr(1));
			else extraD = strToInt(extraB);
			game->room(extraC)->setState(extraD);
			game->room(extraC)->setVisited(false);}
		else if (cmd == "move") { // Set Current Room: move[000];
			extraA = data.substr(0, bracket);
			if (extraA.at(0) == '+') extraC = game->roomCurrent() + strToInt(extraA.substr(1));
			else if (extraA.at(0) == '-') extraC = game->roomCurrent() - strToInt(extraA.substr(1));
			else extraC = strToInt(extraA);
			game->navigate(extraC); }
		else if (cmd == "visi") { // Set Room Visited: visi[000,1];
			extraA = data.substr(0, comma);
			extraB = data.substr(comma+1, bracket-comma);
			if (extraA.at(0) == '+') extraC = game->roomCurrent() + strToInt(extraA.substr(1));
			else if (extraA.at(0) == '-') extraC = game->roomCurrent() - strToInt(extraA.substr(1));
			else extraC = strToInt(extraA);
			game->room(extraC)->setVisited( strToInt(extraB)); }
		else if (cmd == "svar") { // Set Variable: svar[0,0];
			extraA = data.substr(0, comma);
			extraB = data.substr(comma+1, bracket-comma);
			game->setVar(strToInt(extraA), strToInt(extraB)); }
		else if (cmd == "ivar") { // Increment Variable: ivar[0];
			extraC = strToInt(data.substr(0, bracket));
			game->setVar(extraC, game->var(extraC) + 1); }
		else if (cmd == "dvar") { // Decrement Variable: dvar[0];
			extraC = strToInt(data.substr(0, bracket));
			game->setVar(extraC, game->var(extraC) - 1); }
		else if (cmd == "hcmd") { // Send command: hcmd[command];
			game->handleCommand(data.substr(0, bracket), false); }
		else if (cmd == "scen") { // Start a scenario: scen[id];
			extraC = strToInt(data.substr(0, bracket));
			game->setScenario(extraC); 
			return 1;}
		else if (cmd == "stop") // Stop Script: stop[];
			return 1;
		else if (cmd == "endg")
			game->endGame();
		// Scenario-Specific Functions
		else if (cmd == "quit")
			game->stopScenario();
		else if (cmd == "qcmd") {
			game->stopScenario();
			game->handleCommand(game->prevCmd(), false);
			return 1; }
		else if (cmd == "goto") {
			extraC = strToInt(data.substr(0, bracket));
			game->setScenarioStage(extraC); }

		// Conditionals (Recursive)
		else if (cmd.at(0) == '?') {
			x = data.find_first_of('{');
			c = 1;
			for (int i = x + 1; (size_t)i < data.size(); i++)
			{
				y = data.at(i);
				switch (y) {
				case '{': c++; break;
				case '}': c--; if (c == 0) { n = i; y = -1; }
				}
				if (y == -1) break;
			}
			extraA = data.substr(x + 1, n - x - 1);
			if ((size_t)data.find_first_of("else") < data.size()) {
				cut = data.substr(n);
				x = cut.find_first_of('{');
				c = 0;
				for (int j = x; (size_t)j < cut.size(); j++)
				{
					y = cut.at(j);
					switch (y) {
					case '{': c++; break;
					case '}': c--; if (c == 0) { n = j; y = -1; }
					}
					if (y == -1) break;
				}
				extraB = cut.substr(x + 1, n - x - 1);
			}

			if (cmd == "?exo") // If Object Exists: ?exo[00000]...
				test = (game->obj(strToInt(data.substr(0,bracket))) > -1);
			else if (cmd == "?exi") // If Object Is in Inventory: ?exi[00000]...
				test = (game->inv(strToInt(data.substr(0,bracket))) > -1);
			else if (cmd == "?exf") // If Object Is Following: ?exf[00000]...
				test = (game->follow(strToInt(data.substr(0,bracket))) > -1);
			else if (cmd == "?equ") // If Equipped Slot is Full: ?equ[0]...
				test = !!(game->equip(strToInt(data.substr(0, bracket))));
			else if (cmd == "?eqi") { // If Oject is Euipped: ?equ[00000,0]...
				extraE = data.substr(0, comma);
				extraF = data.substr(comma+1, bracket-comma);
				test = (game->equip(strToInt(extraF)) == strToInt(extraE)); }
			else if (cmd == "?roo") // If Room is Current: ?roo[000]...
				test = (strToInt(data.substr(0,bracket)) == game->roomCurrent());
			else if (cmd == "?vis") { // If Room is Visited: ?vis[000,1]
				extraE = data.substr(0, comma);
				extraF = data.substr(comma+1, bracket-comma);
				if (extraE.at(0) == '+') extraC = game->roomCurrent() + strToInt(extraE.substr(1));
				else if (extraE.at(0) == '-') extraC = game->roomCurrent() - strToInt(extraE.substr(1));
				else extraC = strToInt(extraE);
				test = (game->room(extraC)->state() == strToInt(extraF));
			}
			else if (cmd == "?sta") { // If Room Has State: ?sta[000,00]
				extraE = data.substr(0, comma);
				extraF = data.substr(comma+1, bracket-comma);
				if (extraE.at(0) == '+') extraC = game->roomCurrent() + strToInt(extraE.substr(1));
				else if (extraE.at(0) == '-') extraC = game->roomCurrent() - strToInt(extraE.substr(1));
				else extraC = strToInt(extraE);
				if (extraF.at(0) == '+') extraD = game->room(extraC)->state() + strToInt(extraF.substr(1));
				else if (extraF.at(0) == '-') extraD = game->room(extraC)->state() - strToInt(extraF.substr(1));
				else extraD = strToInt(extraF);
				test = (game->room(extraC)->state() == extraD);
			}
			else if (cmd == "?rem") { // If not already picked up
				extraC = strToInt(data.substr(0, comma));
				extraE = data.substr(comma+1, bracket-comma-1);
				if (extraE.at(0) == '+') extraD = game->roomCurrent() + strToInt(extraE.substr(1));
				else if (extraE.at(0) == '-') extraD = game->roomCurrent() - strToInt(extraE.substr(1));
				else extraD = strToInt(extraF);
				test = (!game->room(extraD)->isRemoved(extraC));
			}
			else if (cmd == "?var") { // If var has val
				extraC = strToInt(data.substr(0, comma));
				extraD = strToInt(data.substr(comma+1, bracket-comma));
				test = (game->var(extraC) == extraD);
			}
			else if (cmd == "?cmd") { // If previous command contained the phrase...: ?cmd[one two three];
				test = (game->prevCmd().find(data.substr(0, bracket)) != string::npos); }

			if (test) {
				if (execute(extraA, room, game) == 1) return 1; }
			else if (extraB.size() > 0 && execute(extraB, room, game) == 1) return 1;
		}
	}
	return 0;
}

void printText(string text, Game* game)
{
	size_t x, y;
	queue<string> line;
	string buffer;

	replaceAll(text, '\\', '\n');

	// Replace post-script identifiers
	while (1)
	{
		int varPos = findFirstStr(text, "&v");
		if (varPos == string::npos) break;

		int varId = text.at(varPos + 2) - '0';
		if (varId > 0 && varId < MAX_VARS)
		{
			stringstream newText;
			newText << text.substr(0, varPos) << game->var(varId) << text.substr(varPos + 3);
			text = newText.str();
		}
	}

	// If line is short enough, just print it
	if (text.size() < CONSOLE_WIDTH && findFirstStr(text, "{") == -1)
	{
		cout << text << endl;
		return;
	}

	// Parse for word-wrapping
	while (text.size() > 0)
	{
		buffer = text.substr(0, CONSOLE_WIDTH);
		if (text.size() > CONSOLE_WIDTH) text = text.substr(CONSOLE_WIDTH);
		else text = "";

		// Detect commands
		while (1)
		{
			x = findFirstStr(buffer, "{");
			if ((size_t)x != string::npos)
			{
				y = buffer.substr(0, x).size();
				line.push(buffer.substr(0, x));
				line.push(buffer.substr(x, 3));
				buffer = buffer.substr(x + 3);
				buffer += text.substr(0, 3 + y);
				if (text.size() >= 3 + y) text = text.substr(3 + y);
			}
			else break;
		}
			
		if (buffer.size() < CONSOLE_WIDTH)
		{
			if (buffer.size() > 0) line.push(buffer);
			break;
		}
		
		x = findFirstStr(buffer, "\n");
		if ((size_t)x != string::npos)
		{
			line.push(buffer.substr(0, x + 1));
			text = buffer.substr(x + 1) + text;
			continue;
		}

		// Word wrap
		x = CONSOLE_WIDTH - 1;
		while (buffer.at(x) != ' ' && x > 0)
			x--;
		if (x == 0) // No words
			line.push(buffer);
		else
		{
			line.push(buffer.substr(0, x) + '\n');
			text = buffer.substr(x + 1) + text;
		}
	}

	// Print
	buffer = "";
	while (!line.empty())
	{
		if (line.front().at(0) == '{')
		{
			if (line.front().at(1) == 'C') // Clear Screen
				clearScreen();
			else if (line.front().at(1) = 'P') // Pause
			{
				cout << buffer;
				buffer = "";
				cin.ignore(CIN_IGNORE, '\n');
			}
		}
		else
			buffer += line.front();
		line.pop();
	}
	cout << buffer << endl;
}

#ifdef _MSC_VER  //  Microsoft Visual C++

#include <windows.h>

void clearScreen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    COORD upperLeft = { 0, 0 };
    DWORD dwCharsWritten;
    FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
                                                        &dwCharsWritten);
    SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <cstring>

void clearScreen()
{
    static const char* term = getenv("TERM");
    static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
    if (term == NULL  ||  strcmp(term, "dumb") == 0)
        cout << endl;
     else
        cout << ESC_SEQ << "2J" << ESC_SEQ << "\x1B[H" << flush;
}

#endif
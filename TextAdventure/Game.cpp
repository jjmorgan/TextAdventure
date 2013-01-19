// Game.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include "DbHandler.h"
#include "Game.h"
using namespace std;

Game::Game(string sttgFile, string stndFile, string roomFile, string itemFile, string scenFile)
{
	// Init arrays
	for (int i = 0; i < MAX_EQUIP; i++) m_equip[i] = 0; // (**)
	for (int i = 0; i < MAX_VARS; i++)
	{ 
		m_var[i] = 0; 
	}
	for (int i = 0; i < CMD_HASH_SIZE; i++)
	{
		m_commandsHash[i] = NULL;
	}

	// Load texts
	loadStnd(&stndFile);
	loadRooms(&roomFile);
	loadItems(&itemFile);
	loadSettings(&sttgFile);
	loadScenarios(&scenFile);
}

Game::~Game()
{
	// Delete all dynamically allocated vars
	for (int i = 0; i < m_roomCount; i++)
		delete m_room[i];
	for (int i = 0; i < m_itemCount; i++)
		delete m_item[i];
	for (int i = 0; i < m_scenCount; i++)
		delete m_scenario[i];
}

void Game::launch()
{
	// Introduction
	printText(m_intro[0] + "\n"); // Title
	if (ask(m_intro[1])) // Instructions Prompt
	{
		printText("\n" + m_intro[2]); // Instructions
	}
	printText("\n" + m_intro[3] + "\n"); // Intro

	// Begin
	string input; int result;
	navigate(m_roomStart);
	while (1)
	{
		cout << "> ";
		getline(cin, input);
		if (m_scenCur != NULL) // A scenario is in progress
			result = handleScenarioCommand(input);
		else
			result = handleCommand(input);
		switch (result)
		{
		case 1: return; // Quit
		}
	}
}

bool Game::ask(string prompt)
{
	string response = "";
	while (response.size() < 1 || (tolower(response[0]) != 'y' && tolower(response[0]) != 'n'))
	{
		cout << prompt << " ";
		getline(cin, response);
	}
	return (tolower(response[0]) == 'y');
}

int Game::navigate(int roomid)
{
	m_roomCurrent = roomid;
	Room* r = room(roomid);
	string data;
	int st = r->state();
	// Clear all room objects
	m_obj.clear();
	// Print description
	if (!r->visited()) printText(r->desc(st));
	else printText(r->sdesc(st));
	r->setVisited(true);
	if (r->script(st).size() > 0) execute(r->script(st).substr(1), r, this); // Add objects, etc.
	for (int i = 0; (size_t)i < r->items().size(); i++) // Add items dropped by player
		addObjS(r->items().at(i));
	for (int i = 0; (size_t)i < m_obj.size(); i++) {
		data = item(m_obj[i])->find(); // Declare objects lying on ground
		if (data.size() > 0) {
			if (isScript(data)) execute(data.substr(1), r, this);
			else printText(data);
		}
	}
	for (int i = 0; (size_t)i < m_follow.size(); i++) {
		data = item(m_follow[i])->follow(); // Declare followers
		if (data.size() > 0) {
			if (isScript(data)) execute(data.substr(1), r, this);
			else printText(data);
		}
	}
	return 0;
}

void Game::showInventory()
{
	Item* it;
	if (m_inv.size() > 0) {
		printText(m_extra[0]);
		for (int i = 0; (size_t)i < m_inv.size(); i++)
		{
			it = item(m_inv[i]);
			cout << "- " << it->invdesc();
			if (it->equipped())
				cout << m_extra[2];
			cout << endl;
		}
	}
	else printText(m_extra[1]);
}

int Game::handleDir(string& dir, Room* rm)
{
	int b = dir.find('['), s = dir.find(';');
	if (b == 0) // Room
		navigate(strToInt(dir.substr(1, 3)));
	else if (dir.at(0) == '{') // Standard Text
		printText(stnd(strToInt(dir.substr(1, 3))));
	else if (isScript(dir)) // Script
		execute(dir, rm, this);
	else
		printText(dir);
	return 0;
}

int Game::handleCommand(string input, bool newline)
{
	if (input.size() == 0) return -1;

	vector<string> words;
	string buffer, tInput, object, response = "";
	Item* match = NULL;
	vector<Item*> matches; // Item match candidates
	vector<string> responses; // Item match candidates
	Command* command = NULL;
	int wordIndex = -1;

	filterCommand(input); // Set type case, remove particles
	if (input.size() == 0) return -1;
	tInput = input + ' ';

	if (newline) cout << endl; // Separation

	// Standard Commands (Navigation, Inventory, Settings, Quit)
	if (input == "n" || input == "north") { handleDir(room(m_roomCurrent)->dirB(room(m_roomCurrent)->state(), 0), room(m_roomCurrent)); return 0; }
	else if (input == "s" || input == "south") { handleDir(room(m_roomCurrent)->dirB(room(m_roomCurrent)->state(), 1), room(m_roomCurrent)); return 0; }
	else if (input == "e" || input == "east") { handleDir(room(m_roomCurrent)->dirB(room(m_roomCurrent)->state(), 2), room(m_roomCurrent)); return 0; }
	else if (input == "w" || input == "west") { handleDir(room(m_roomCurrent)->dirB(room(m_roomCurrent)->state(), 3), room(m_roomCurrent)); return 0; }
	else if (input == "ne" || input == "northeast") { handleDir(room(m_roomCurrent)->dirB(room(m_roomCurrent)->state(), 4), room(m_roomCurrent)); return 0; }
	else if (input == "nw" || input == "northwest") { handleDir(room(m_roomCurrent)->dirB(room(m_roomCurrent)->state(), 5), room(m_roomCurrent)); return 0; }
	else if (input == "se" || input == "southeast") { handleDir(room(m_roomCurrent)->dirB(room(m_roomCurrent)->state(), 6), room(m_roomCurrent)); return 0; }
	else if (input == "sw" || input == "southwest") { handleDir(room(m_roomCurrent)->dirB(room(m_roomCurrent)->state(), 7), room(m_roomCurrent)); return 0; }
	else if (input == "u" || input == "up") { handleDir(room(m_roomCurrent)->dirB(room(m_roomCurrent)->state(), 8), room(m_roomCurrent)); return 0; }
	else if (input == "d" || input == "down") { handleDir(room(m_roomCurrent)->dirB(room(m_roomCurrent)->state(), 9), room(m_roomCurrent)); return 0; }
	else if (input == "inventory" || input == "inv" || input == "i") { showInventory(); return 0;}
	else if (input == "help" || input == "commands") { printText(m_intro[2]); return 0;}
	else if (input == "quit" || input == "exit") { return 1; }

	// Splice keywords in input
	while (1)
	{
		int x = tInput.find(' ');
		if (x < 0 || (size_t)x > tInput.size()) break;
		buffer = tInput.substr(0, x);
		tInput = tInput.substr(x + 1);
		if (buffer.size() == 0) continue;
		words.push_back(buffer);
	}

	//////////////DEBUG!///////////////
	if (words[0] == "getstatus")
	{
		cout << "Room " << words[1] << " has status " << room(strToInt(words[1]))->state() << endl;
		return 0;
	}

	// Queued items
	if (!m_objQueue.empty())
	{
		int wSize = words.size();
		int qSize = m_objQueue.size();
		int *qMatch = new int[qSize];
		for (size_t i = 0; i < qSize; i++) qMatch[i] = 0;
		int total = 0;
		for (size_t i = 0; i < wSize; i++)
			for (size_t j = 0; j < qSize; j++)
				if (findFirstStr(m_objQueue[j], words[i]) > -1)
				{
					qMatch[j] += 1;
					total += 1;
					// Recall last command, but add the adjective
					/*m_objQueue.clear();
					handleCommand(m_prevCmd + " " + words[i], false);
					return 0;*/
				}
		if ((total % qSize) != 0 && total > 0)
		{
			// One queued item has more matches than the others
			int highest = 0;
			for (int i = 1; i < qSize; i++)
				if (qMatch[i] > qMatch[highest])
					highest = i;
			string returnCmd = m_prevCmd + " " + m_objQueue[highest];
			m_objQueue.clear();
			delete [] qMatch;
			handleCommand(returnCmd, false);
			return 0;
		}
		m_objQueue.clear(); // Not referenced
		delete [] qMatch;
	}

	m_prevCmd = input; // Save command

	// EXLK Correction
	if (words[0] == "examine" || words[0] == "look")
	{
		if (words[0] == "examine")
			input = "exlk" + input.substr(7);
		else
			input = "exlk" + input.substr(4);
		words[0] = "exlk";
	}

	// Grab command
	int index = cmdGetHashInd(words[0]);
	CommandHashAux* a = m_commandsHash[index];
	while (command == NULL)
	{
		if (a == NULL)
		{
			// Not found, use ????
			index = cmdGetHashInd("????");
			a = m_commandsHash[index];
			if (a == NULL)
			{
				// ???? must be listed! Print error
				printText("ERROR: Missing ???? in item list!");
				return -1;
			}
			command = a->m_cmd;
		}
		else
		{
			if (input.substr(0, a->m_name.size()) == toLowerStr(a->m_name))
			{
				command = a->m_cmd;
				tInput = command->command();
				object = input.substr(a->m_name.size());
				if (object.size() > 0 && object.at(0) == ' ') object = object.substr(1);
				while (1)
				{
					words.erase(words.begin());
					int x = tInput.find(' ');
					if (x < 0 || (size_t)x > tInput.size()) break;
					tInput = tInput.substr(x + 1);
				}
				break;
			}
			else
				a = a->m_next;
		}
	}

	// Grab command (Vector implementation
	/*for (int i = 0; (size_t)i < m_commands.size(); i++)
	{
		buffer = toLowerStr(m_commands[i].command());
		if (input.substr(0, buffer.size()) == buffer)
		{
			command = &m_commands[i];
			tInput = command->command();
			object = input.substr(buffer.size());
			if (object.size() > 0 && object.at(0) == ' ') object = object.substr(1);
			while (1)
			{
				words.erase(words.begin());
				int x = tInput.find(' ');
				if (x < 0 || (size_t)x > tInput.size()) break;
				tInput = tInput.substr(x + 1);
			}
			break;
		}
	}
	// Failure?
	if (command == NULL)
	{
		for (int i = m_commands.size() - 1; i >= 0; i--)
			if (m_commands[i].command() == "????")
			{
				command = &m_commands[i];
				words.clear();
			}
		if (command == NULL)
		{
			// This should not occur; ???? MUST be listed
			printText("ERROR: Missing ???? in item list!");
			return -1;
		}
	}*/

	// Match no object, if object is empty
	if (words.size() == 0)
	{
		match = command->findSpecial(response, NONE);
		if (match != NULL)
		{
			executeResponse(response, object);
			return 0;
		}
	}

	// Match default (No duplicates of these can exist)
	match = command->findDefault(words, wordIndex, response);
	if (match != NULL)
	{
		executeResponse(response, object);
		return 0;
	}

	// Number Correction
	if (words.size() > 1) convertToNumber(words[1]); // Number should always follow command

	// Multi-candidate time
	command->findItems(words, m_inv, matches, responses); // Match inventory
	command->findItems(words, m_follow, matches, responses); // Match follows
	command->findItems(words, m_obj, matches, responses); // Match room objects

	if (matches.size() == 0)
	{
		// Object not found
		match = command->findSpecial(response, NOTFOUND);
		if (match != NULL)
		{
			executeResponse(response, object);
			return 0;
		}
		else
		{
			// Command does not have a NOTFOUND entry. Grab unknown NONE
			for (int i = m_commands.size() - 1; i >= 0; i--)
				if (m_commands[i].command() == "????")
				{
					m_commands[i].findSpecial(response, NOTFOUND);
					executeResponse(response, object);
					return 0;
				}
			// This should never be reached; ???? MUST be listed
			printText("ERROR: Missing ???? in item list!");
			return -1;
		}
	}
	else if (matches.size() == 1)
	{
		// Object found, of course
		executeResponse(responses[0], object);
		return 0;
	}
	else
	{
		// Duplicate matches found. This is not allowed!
		buffer = "Which one: the";
		for (int x = 0; (size_t)x < matches.size(); x++)
		{
			if (x == matches.size() - 1) buffer += " or " + matches[x]->desc() + "?";
			else buffer += " " + matches[x]->desc();
			if ((size_t)x < matches.size() - 2) buffer += ",";
			m_objQueue.push_back(matches[x]->desc());
		}
		printText(buffer);
		replaceAllStr(m_prevCmd, object, "");
		return 0;

		// TODO: Add these items to a QUEUE so the
		//       user can simply respond with ONE
		//       to automatically perform its action
	}

	printText("(Fail) " + stnd(0));
	return -1;
}

int Game::handleScenarioCommand(string input)
{
	if (input.size() == 0) return -1;

	printf("\n");

	filterCommand(input); // Set type case, remove particles
	if (input.size() == 0) return -1;
	m_prevCmd = input;
	string response = m_scenCur->matchCommand(input);

	executeResponse(response, "");
	return 0;
}

void Game::filterCommand(string& cmd)
{
	char prev = 0;
	cmd = toLowerStr(cmd); // Treat in lowercase
	// Filter non-alphanumeric characters & extra spaces
	for (int i = 0; (size_t)i < cmd.size(); i++)
	{
		if ((!isalnum(cmd.at(i)) && cmd.at(i) != ' ')
			|| cmd.at(i) == ' ' && prev == ' ')
		{
			cmd = cmd.substr(0, i) + cmd.substr(i + 1);
			i--;
		}
		else 
			prev = cmd.at(i);
	}
	if (cmd.size() == 0) return;
	cmd = ' ' + cmd;
	// Remove efiltered words
	for (int i = 0; (size_t)i < m_filter.size(); i++)
		replaceAllStr(cmd, " " + m_filter[i] + " ", " ");
	// Exclude space at beginning and end
	while (cmd.at(0) == ' ') cmd = cmd.substr(1);
	while (cmd.at(cmd.size() - 1) == ' ') cmd = cmd.substr(0, cmd.size() - 1);
}

int Game::loadSettings(string* sttgFile)
{
	ifstream sttgData (*sttgFile);
	if (!sttgData.is_open()) return -1;
	string line; int c = 0, d;
	while (sttgData.good() && c <= 9)
	{
		getline (sttgData, line);
		if (line.size() < 1 || line[0] == '/') continue; // New line or comment
		switch (c)
		{
		case 0: case 1: case 2: case 3: // Grab intro
			//replaceAll(line, '\\', '\n');
			m_intro[c] = line;
			break;
		case 4: // Starting room
			d = strToInt(line);
			for (int i = 0; (size_t)i < m_roomId.size(); i++)
				if (m_roomId[i] == d)
				{
					m_roomStart = d;
					break;
				}
			break;
		case 5: // Keywords
			while (1)
			{
				d = line.find('/');
				if (d == -1 || (size_t)d >= line.size()) break;
				m_filter.push_back(line.substr(0, d));
				line = line.substr(d + 1);
			}
			break;
		case 6: // Prepositions
			while (1)
			{
				d = line.find('/');
				if (d == -1 || (size_t)d >= line.size()) break;
				m_preps.push_back(line.substr(0, d));
				line = line.substr(d + 1);
			}
			break;
		case 7: case 8: case 9: // Grab extras
			replaceAll(line, '\\', '\n');
			m_extra[c - 7] = line;
		}
		c++;
	}
	sttgData.close();
	return 0;
}

int Game::loadStnd(string* stndFile)
{
	// Load Standard Texts, used for miscellaneous descriptions
	ifstream stndData (*stndFile);
	if (!stndData.is_open()) return -1;
	string line;
	int count = 0;

	// Grab entries
	while (stndData.good())
	{
		getline (stndData, line);
		m_stnd.push_back(line.substr(3));
		m_stndId.push_back(strToInt(line.substr(0, 3)));
	}

	// Done
	stndData.close();
	m_stndCount = m_stnd.size();
	return m_stndCount;
}

int Game::loadRooms(string* roomFile)
{
	// Load rooms and their respective descriptions
	ifstream roomData (*roomFile);
	if (!roomData.is_open()) return -1;
	string line, text; int curId, curState, count = -1; vector<string> d (10, "");
	// Grab entries
	while (roomData.good())
	{
		getline(roomData, line);
		if (line.size() < 5 || line[0] == '\\') continue; // Comment
		curId = strToInt(line.substr(0,line.find('\\')));
		line = line.substr(line.find('\\') + 1);
		curState = strToInt(line.substr(0,line.find('\\')));
		if (isInVector(m_roomId, curId) == -1)
		{
			if (count > -1) m_room[count]->countStates();
			m_room.push_back(new Room(curId, this));
			m_roomId.push_back(curId);
			count++;
		}

		text = line.substr(line.find(':') + 1);
		replaceAll(text, '\\', '\n');
		switch (line.at(line.find('\\') + 1))
		{
		case '0': // Description
			if (text[0] == '[') text = m_room[curId]->desc(strToInt(text.substr(1,2)));
			m_room[count]->addDesc(text, curState);
			break;
		case '1': // Short Description
			if (text[0] == '[') text = m_room[curId]->sdesc(strToInt(text.substr(1,2)));
			m_room[count]->addSDesc(text, curState);
			break;
		case 'x': // Script
			m_room[count]->addScript(text, curState);
			break;
		// Directions
		case 'N': m_room[count]->addDir(text, curState, 0); break; case 'S': m_room[count]->addDir(text, curState, 1); break;
		  case 'E': m_room[count]->addDir(text, curState, 2); break; case 'W': m_room[count]->addDir(text, curState, 3); break;
		case 'n': m_room[count]->addDir(text, curState, 4); break; case 'w': m_room[count]->addDir(text, curState, 5); break;
		  case 'e': m_room[count]->addDir(text, curState, 6); break; case 's': m_room[count]->addDir(text, curState, 7); break;
		case 'U': m_room[count]->addDir(text, curState, 8); break; case 'D': m_room[count]->addDir(text, curState, 9); break;
		case '?': for (int i = 0; i < 10; i++) if (m_room[count]->dirB(curState, i) == "") m_room[count]->addDir(text, curState, i); break;
		case 'C': for (int i = 0; i < 4; i++) if (m_room[count]->dirB(curState, i) == "") m_room[count]->addDir(text, curState, i); break;
		case 'c': for (int i = 4; i < 8; i++) if (m_room[count]->dirB(curState, i) == "") m_room[count]->addDir(text, curState, i); break;
		case 'V': for (int i = 8; i < 10; i++) if (m_room[count]->dirB(curState, i) == "") m_room[count]->addDir(text, curState, i); break;
		}
	}
	m_roomCount = count + 1;
	return 0;
}

int Game::loadItems(string* itemFile)
{
	// Load items, as well as commands and the items to which each refer
	ifstream itemData (*itemFile);
	if (!itemData.is_open()) return -1;
	string line, text, sub, buffer; int curId, count = -1, x; vector<string> iKeyword; c_type itype;
	// Grab entries
	while (itemData.good())
	{
		//getline(itemData, line);
		line = "";
		while (1)
		{
			getline(itemData, buffer);
			line += buffer;
			if (line.size() > 0 && line.at(line.size() - 1) == '_')
				line = line.substr(0, line.size() - 1);
			else
				break;
		}

		if (line.size() < 3 || line[0] == '/') continue; // Comment
		replaceAllStr(line, "\t", ""); // Remove tabs

		if (line[0] == '[') // Item Declaration: [00000] keyword keyword
		{
			// Item ID
			if (line.substr(1,5) == "DFNUL") { curId = -2; itype = NONE; } // No Object Given
			else if (line.substr(1,5) == "DFOBJ") { curId = -1; itype = NOTFOUND; } // Object Not Found
			else if (line.substr(1,1) == "D") { curId = strToInt(line.substr(2, line.find(']' - 1))); itype = DEFAULT; curId = curId * -1 - 100; } // Pseudo-Obj Standard Response
			else { curId = strToInt(line.substr(1,line.find(']') - 1)); itype = OBJ; }

			//if (find(m_itemId.begin(), m_itemId.end(), curId) == m_itemId.end())
			//{
				m_item.push_back(new Item(curId, this));
				m_itemId.push_back(curId);
				iKeyword.clear();
			//}
			line = line.substr(line.find(']') + 2) + " "; // Keywords

			// Keywords
			while (1)
			{
				x = line.find(' ');
				if ((size_t)x >= line.size()) break;
				sub = line.substr(0, x);
				line = line.substr(x + 1);
				if (sub.size() == 0) continue;
				if (isInVector(iKeyword, sub) >= 0) continue;

				switch (sub.at(0))
				{
				case '&': // Adjective
					if (sub.at(1) == '|')
					{
						m_item.back()->addKeyword(sub.substr(2), NOUNADJ);
						iKeyword.push_back(sub.substr(2));
					}
					else
					{
						m_item.back()->addKeyword(sub.substr(1), ADJ);
						iKeyword.push_back(sub.substr(1));
					}
					break;
				case '@': // Preposition
					m_item.back()->addKeyword(sub.substr(1), PREP);
					iKeyword.push_back(sub.substr(1));
					break;
				default: // Noun/Other
					m_item.back()->addKeyword(sub, NOUN);
					iKeyword.push_back(sub);
				}
			}
		}
		else // Item Commands: COMMAND:Text
		{
			sub = line.substr(0, line.find_first_of(":"));
			if (sub == "#dsc") { m_item.back()->setDesc(line.substr(sub.size() + 1)); continue; }
			if (sub == "#inv") { m_item.back()->setInvDesc(line.substr(sub.size() + 1)); continue; }
			if (sub == "#fnd") { m_item.back()->setFind(line.substr(sub.size() + 1)); continue; }
			if (sub == "#fol") { m_item.back()->setFollow(line.substr(sub.size() + 1)); continue; }
			if (sub == "#mlp") { m_item.back()->toggleMultiple(); continue; }
			if (sub == "#prs") {
				m_item.back()->toggleSuperprep(); continue;
			}

			int index = cmdGetHashInd(sub);
			CommandHashAux* a = m_commandsHash[index];
			CommandHashAux* prev = NULL;
			Command* c = NULL;
			while (c == NULL)
			{
				if (a != NULL)
				{
					if (a->m_name == sub)
						c = a->m_cmd; // Existing command
					else
					{
						prev = a;
						a = a->m_next;
					}
				}
				else
				{
					// New command!
					a = new CommandHashAux(sub);
					c = new Command(sub);
					a->m_cmd = c;
					if (prev == NULL)
						m_commandsHash[index] = a;
					else
						prev->m_next = a;
					m_cmdCount++;
				}
			}

			if (itype != DEFAULT)
				c->addItem(m_item.back(), line.substr(sub.size() + 1), itype);
			else
				c->addDefault(m_item.back(), line.substr(sub.size() + 1));

			// Vector implementation
			/*x = isInVector(m_commands, Command(sub));
			if (x == -1) { // New Command
				m_commands.push_back(Command(sub));
				x = m_commands.size() - 1;
			}

			if (itype != DEFAULT)
				m_commands[x].addItem(m_item.back(), line.substr(sub.size() + 1), itype);
			else
				m_commands[x].addDefault(m_item.back(), line.substr(sub.size() + 1));
				*/
		}
	}
	m_itemCount = m_itemId.size();
	//m_cmdCount = m_commands.size();
	return 0;
}

int Game::loadScenarios(string* scenFile)
{
	ifstream itemData (*scenFile);
	if (!itemData.is_open()) return -1;
	string line, text, sub, buffer;
	int curId, curStage, x;
	Scenario* sCur;
	m_scenCount = 0;
	// Grab entries
	while (itemData.good())
	{
		line = "";
		while (1)
		{
			getline(itemData, buffer);
			line += buffer;
			if (line.size() > 0 && line.at(line.size() - 1) == '_')
				line = line.substr(0, line.size() - 1);
			else
				break;
		}

		if (line.size() < 3 || line[0] == '/') continue; // Comment
		replaceAllStr(line, "\t", ""); // Remove tabs

		if (line.at(0) == '[') // Scenario Declaration
		{
			curId = strToInt(line.substr(1, line.find(']')));
			sCur = new Scenario(curId, this);
			m_scenario.push_back(sCur);
			m_scenCount++;
			continue;
		}

		if (line.at(0) == '{') // Stage Declaration
		{
			buffer = line.substr(1, line.find('}') - 1);
			if (buffer == "G")
				curStage = -1; // Set aside -1 for global scope
			else
				curStage = strToInt(buffer);
			text = line.substr(line.find('}') + 1);
			sCur->addStage(curStage, text);
		}

		if (line.find(":") != string::npos)
		{
			// Command declaration
			x = line.find(":");
			sCur->addCommandToStage(line.substr(0, x), line.substr(x + 1));
		}
	}
	m_scenCur = NULL;
	return 0;
}

void Game::addObj(int ind, int count) { 
	if (room(m_roomCurrent)->isRemoved(ind)) return;
	for (int i = 0; (size_t)i < m_obj.size(); i++)
		if (ind == m_obj[i])
		{
			m_objCount[i]++;
			return;
		}
	m_obj.push_back(ind);
	m_objCount.push_back(count);
}

void Game::addObjS(int ind, int count) { 
	for (int i = 0; (size_t)i < m_obj.size(); i++)
		if (ind == m_obj[i])
		{
			m_objCount[i]++;
			return;
		}
	m_obj.push_back(ind);
	m_objCount.push_back(count);
}

void Game::addInv(int ind, int count) {
	for (int i = 0; (size_t)i < m_inv.size(); i++)
		if (ind == m_inv[i])
		{
			m_invCount[i]++;
			return;
		}
	m_inv.push_back(ind);
	m_invCount.push_back(count);
}

void Game::addFollow(int ind) { if (isInVector(m_follow, ind) == -1) m_follow.push_back(ind); }

void Game::remObj(int ind, int count) { 
	for (int i = 0; (size_t)i < m_obj.size(); i++)
		if (ind == m_obj[i])
		{
			if (m_objCount[i] > 1)
				m_objCount[i]--;
			else
			{
				m_obj.erase(m_obj.begin() + i);
				m_objCount.erase(m_objCount.begin() + i);
			}
		}
}

void Game::remInv(int ind, int count) {
	for (int i = 0; (size_t)i < m_inv.size(); i++)
		if (ind == m_inv[i])
		{
			if (m_invCount[i] > 1)
				m_invCount[i]--;
			else
			{
				m_inv.erase(m_inv.begin() + i);
				m_invCount.erase(m_invCount.begin() + i);
			}
		}
}

void Game::setEquip(int id, int slot)
{
	if (slot > 0 && slot < MAX_EQUIP)
	{
		if (m_equip[slot] != 0) item(m_equip[slot])->toggleEquip();
		m_equip[slot] = id;
		item(id)->toggleEquip();
	}
}

void Game::setVar(int id, int val) { m_var[id] = val; }

void Game::remFollow(int ind) { removeFromVector(m_follow, ind); }

string Game::intro(int ind) const { return m_intro[ind]; }

string Game::stnd(int ind) const
{
	for (int i = 0; i < m_stndCount; i++)
		if (m_stndId[i] == ind)
			return m_stnd[i];
	return "";
}

int Game::roomCurrent() const { return this->m_roomCurrent; }

Room* Game::room(int ind) const
{
	for (int i = 0; i < m_roomCount; i++)
		if (m_roomId[i] == ind)
			return m_room[i];
	return NULL;
}

Item* Game::item(int ind) const
{
	for (int i = 0; i < m_itemCount; i++)
		if (m_itemId[i] == ind)
			return m_item[i];
	return NULL;
}

Scenario* Game::scenario(int ind) const
{
	for (int i = 0; i < m_scenCount; i++)
		if (m_scenario[i]->id() == ind)
			return m_scenario[i];
	return NULL;
}

Scenario* Game::currentScenario() const { return m_scenCur; }

int Game::obj(int ind) const { return isInVector(m_obj, ind); }

int Game::inv(int ind) const { return isInVector(m_inv, ind); }

int Game::equip(int slot) const { if (slot > 0 && slot < MAX_EQUIP) return m_equip[slot]; return 0; }

int Game::follow(int ind) const { return isInVector(m_follow, ind); }

int Game::var(int ind) const { return m_var[ind]; }

string Game::prevCmd() const { return m_prevCmd; }

bool Game::matchKeywords(vector<string> first, vector<string> second)
{
	for (int i = 0; (size_t)i < first.size(); i++)
		for (int j = 0; (size_t)j < second.size(); j++)
			if (first[i] == second[j])
				return true;
	return false;
}

void Game::executeResponse(string response, string object)
{
	// Replace identifiers
	replaceAllStr(response, "&o", object);

	//if (isScript(response)) execute(response, room(m_roomCurrent), this); // Script
	if (response.at(0) == '@') execute(response.substr(1), room(m_roomCurrent), this);
	else if (response.size() > 0 && response.at(0) == '{') printText(stnd(strToInt(response.substr(1,3)))); // Standard Text
	/*else if (response.size() > 0 && response.substr(0, 2) == ">[")
	{
		// Allow other object, if it exists, to handle the command
		Item* grab = item(strToInt(response.substr(2, response.find(']'))));
		if (item != NULL)
			handleCommand(m_prevCmd.substr(0, m_prevCmd.find(object))
			+ grab->getAllKeywords() )
	}*/
	else printText(response);
}

int Game::cmdGetHashInd(std::string cmd)
{
	// Hash table is 26*26+1 entries for every first two-letter combination, and ???
	if (!isalpha(cmd.at(0)))
		return CMD_HASH_SIZE - 1;
	if (cmd.size() == 1) return (toupper(cmd.at(0)) - 'A');
	return (toupper(cmd.at(0)) - 'A') * 26 + (toupper(cmd.at(1)) - 'A');
}

void Game::setScenario(int id)
{
	m_scenCur = scenario(id);
	m_scenCur->setCurStage(0);
}

void Game::stopScenario() { m_scenCur = NULL; }

void Game::setScenarioStage(int id) { m_scenCur->setCurStage(id); }

void Game::endGame()
{
	// For now, quit the application.
	exit(1);
}
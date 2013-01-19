// Game.h

#ifndef GAME
#define GAME

#include <string>
#include <vector>
#include "Room.h"
#include "Item.h"
#include "Command.h"
#include "Scenario.h"
using namespace std;

#define CMD_HASH_SIZE 26*26 + 1

template<typename T>
struct ObjectAux
{
	T pointer;
	int id;
};

#define MAX_EQUIP 10 // Could not use this in certain places (**)
#define MAX_VARS 15

class Game
{
public:
	Game(string sttgFile, string stndFile, string roomFile, string itemFile, string scenFile); // Constructor
	~Game(); // Destructor
	// Functions
	int loadSettings(string* sttgFile);
	int loadStnd(string* stndFile);
	int loadRooms(string* roomFile);
	int loadItems(string* itemFile);
	int loadScenarios(string* scenFile);
	void launch();
	bool ask(string prompt);
	int navigate(int roomid);
	void showInventory();
	int handleDir(string& dir, Room* rm);
	int handleCommand(string input, bool newline = true);
	int handleScenarioCommand(string input);
	void filterCommand(string& cmd);
	void addObj(int id, int count = 1);
	void addObjS(int id, int count = 1);
	void addInv(int id, int count = 1);
	void addFollow(int id);
	void remObj(int id, int count = 1);
	void remInv(int id, int count = 1);
	void setEquip(int id, int slot);
	void setVar(int id, int val);
	void remFollow(int id);
	void setScenario(int id);
	void stopScenario();
	void setScenarioStage(int id);
	void endGame();
	// Access Members
	string intro(int ind) const;
	string stnd(int ind) const;
	Room* room(int ind) const;
	int roomCurrent() const;
	Item* item(int ind) const;
	int obj(int ind) const;
	int inv(int ind) const;
	int equip(int slot) const;
	int follow(int ind) const;
	int var(int ind) const;
	string prevCmd() const;
	Scenario* currentScenario() const;
private:
	// Private Structs
	struct CommandHashAux
	{
		CommandHashAux(std::string name) : m_name(name), m_cmd(NULL), m_next(NULL) {}
	    std::string m_name;
		Command* m_cmd;
	    CommandHashAux* m_next;
	};
	// Helpers
	bool matchKeywords(vector<string> first, vector<string> second);
	void executeResponse(string response, string object);
	int cmdGetHashInd(std::string cmd);
	Scenario* scenario(int ind) const;
	// Members (Poor vector design)
	string m_intro[4]; // Title, Instructions Prompt, Instructions, Prelude
	string m_extra[3]; // Inv, Inv (Blank), Equip?
	vector<string> m_filter; // Filtered Keywords
	vector<string> m_preps; // Prepositions
	vector<string> m_stnd; // Standard Text Db
	vector<int> m_stndId;
	vector<Room*> m_room; // All Rooms
	vector<int> m_roomId;
	vector<Item*> m_item; // All Items
	vector<int> m_itemId;
	vector<Scenario*> m_scenario; // All Scenarios
	CommandHashAux* m_commandsHash[CMD_HASH_SIZE]; // Hash Implementation
	vector<Command> m_commands; // All Commands
	vector<int> m_obj; // Room Objects
	vector<int> m_inv; // Inventory
	vector<int> m_follow; // Followers
	vector<int> m_objCount; // Item Count in Room
	vector<int> m_invCount; // Item Count in Inventory
	vector<string> m_objQueue; // Fast access for multiple matches
	int m_equip[MAX_EQUIP]; // Equipped Items
	int m_var[MAX_VARS];
	int m_stndCount;
	int m_roomCount;
	int m_roomStart;
	int m_itemCount;
	int m_scenCount;
	int m_cmdCount;
	int m_roomCurrent;
	string m_prevCmd;
	Scenario* m_scenCur;
};

#endif // GAME
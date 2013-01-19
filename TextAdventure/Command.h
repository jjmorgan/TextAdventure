// Command.h

#ifndef COMMAND
#define COMMAND

#include <string>
#include <vector>

enum c_type {
	OBJ, NOTFOUND, NONE, DEFAULT
};

class Item;

class Command
{
public:
	Command(std::string cmd) : m_cmd(cmd) {}
	~Command() {}
	// Functions
	bool operator==(const Command &rhs);
	Item* findDefault(std::vector<std::string> &keywords, int& index, std::string& response);
	Item* findSpecial(std::string& response, c_type type);
	int findItems(std::vector<std::string> &keywords, std::vector<int>& idVec,
				   std::vector<Item*>& resultItems, std::vector<std::string>& resultResponses);
	// Modifiers
	bool addItem(Item* item, std::string response, c_type type = OBJ);
	bool addDefault(Item* def, std::string response);
	// Access Members
	std::string command() const { return m_cmd; }
private:
	struct CommandAux
	{
		CommandAux(Item* i, std::string r, c_type t)
			: item(i), response(r), type(t) {}
		Item* item;
		std::string response;
		c_type type;
	};
	std::string m_cmd;
	std::vector<CommandAux> m_items;
	std::vector<CommandAux> m_defaults;
};

#endif // COMMAND
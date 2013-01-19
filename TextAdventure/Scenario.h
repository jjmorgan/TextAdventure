// Scenario.h

#ifndef SCENARIO
#define SCENARIO

#include <string>
#include <vector>

class Game;

class Scenario
{
public:
	Scenario(int id, Game* game) : m_game(game), m_id(id), m_stageCur(NULL), m_stageGlobal(-1) {}
	~Scenario() {}
	// Public Functions
	bool addStage(int id, std::string message);
	bool addCommandToStage(std::string cmd, std::string response);
	bool setCurStage(int id);
	void displayStageMessage();
	std::string matchCommand(std::string input);
	// Access Members
	int id() { return m_id; }
private:
	struct SCommand
	{
		std::vector<std::string> cWords;
		std::string response;
	};
	struct Stage
	{
		Stage(int tid, std::string msg) : id(tid), message(msg), cmdUnknown(-1) {}
		int id;
		std::string message;
		std::vector<SCommand> commands;
		int cmdUnknown;
	};
	// Helpers
	int matchKeywords(std::string input, Stage* stage);
	// Private Members
	std::vector<Stage> m_stage;
	Stage* m_stageCur;
	int m_stageGlobal;
	Game* m_game;
	int m_id;
};

#endif // SCENARIO
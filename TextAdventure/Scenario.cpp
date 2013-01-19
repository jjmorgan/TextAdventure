// Scenario.cpp

#include "Scenario.h"
#include "DbHandler.h"
#include <string>
using namespace std;

bool Scenario::addStage(int id, string message)
{
	m_stage.push_back(Stage(id, message));
	if (id == -1) m_stageGlobal = m_stage.size() - 1;
	return true;
}

bool Scenario::addCommandToStage(string cmd, string response)
{
	Stage* stage = &m_stage.back(); // Assume last stage to be added is
									//    receiving the command
	stage->commands.push_back(SCommand());
	stage->commands.back().response = response;
	cmd += ',';
	// Parse words in command
	while (1)
	{
		int x = cmd.find(',');
		if (x < 0 || (size_t)x > cmd.size()) break;
		string buffer = cmd.substr(0, x);
		cmd = cmd.substr(x + 1);
		if (buffer.size() == 0) continue;
		replaceAllStr(buffer, "+", " ");
		stage->commands.back().cWords.push_back(buffer);
		if (buffer == "????") stage->cmdUnknown = stage->commands.size() - 1;
	}
	return true;
}

bool Scenario::setCurStage(int id)
{
	for (size_t i = 0; i < m_stage.size(); i++)
	{
		if (m_stage[i].id == id)
		{
			m_stageCur = &m_stage[i];
			displayStageMessage();
			return true;
		}
	}
	return false;
}

string Scenario::matchCommand(string input)
{
	// Match keywords at current stage
	int match = matchKeywords(input, m_stageCur);
	if (match > -1)
		return m_stageCur->commands[match].response;
	// Unknown command, check global scope
	if (m_stageGlobal == -1) return ""; // Error!
	match = matchKeywords(input, &m_stage[m_stageGlobal]);
		if (match > -1) return m_stage[m_stageGlobal].commands[match].response;
	// No matches, check ???? at local scope
	int unkIndex = m_stageCur->cmdUnknown;
	if (unkIndex > -1)
		return m_stageCur->commands[unkIndex].response;
	// Still no matches, resort to ???? at global scope
	unkIndex = m_stage[m_stageGlobal].cmdUnknown;
	if (unkIndex == -1) return ""; // Error!
	return m_stage[m_stageGlobal].commands[unkIndex].response;
}

int Scenario::matchKeywords(string input, Stage* stage)
{
	SCommand* cmd = NULL;
	for (size_t i = 0; i < stage->commands.size(); i++)
	{
		cmd = &stage->commands[i];
		// As a rule, the command must appear at the beginning of input!
		for (size_t j = 0; j < cmd->cWords.size(); j++)
		{
			if (toLowerStr(cmd->cWords[j]) == input.substr(0, cmd->cWords[j].size()))
			{
				// Command matches
				return i;
			}
		}
	}
	return -1;
}

void Scenario::displayStageMessage()
{
	if (m_stageCur->message.at(0) == '@') execute(m_stageCur->message.substr(1), m_game->room(m_game->roomCurrent()), m_game);
	else printText(m_stageCur->message, m_game);
}
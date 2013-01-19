// Command.cpp

#include "Command.h"
#include "DbHandler.h"
#include <string>
#include <algorithm>
using namespace std;

bool Command::operator==(const Command& rhs)
{
	return m_cmd == rhs.command();
}

bool Command::addItem(Item* item, string response, c_type type)
{
	for (int i = 0; (size_t)i < m_items.size(); i++)
		if (m_items[i].item == item) return false;

	m_items.push_back(CommandAux(item, response, type));
	return true;
}

bool Command::addDefault(Item* def, string response)
{
	for (int i = 0; (size_t)i < m_defaults.size(); i++)
		if (m_defaults[i].item == def) return false;

	m_defaults.push_back(CommandAux(def, response, DEFAULT));
	return true;
}

Item* Command::findDefault(std::vector<std::string> &keywords, int& index, string& response)
{
	k_type type; // Temp
	for (int i = 0; (size_t)i < m_defaults.size(); i++)
		if (m_defaults[i].item->matchKeyword(keywords, index, type))
		{
			response = m_defaults[i].response;
			return m_defaults[i].item;
		}
	return NULL;
}

Item* Command::findSpecial(std::string& response, c_type type)
{
	for (int i = 0; (size_t)i < m_items.size(); i++)
		if (m_items[i].type == type)
		{
			response = m_items[i].response;
			return m_items[i].item;
		}
	return NULL;
}

int Command::findItems(vector<string> &keywords, vector<int>& idVec,
				   vector<Item*>& resultItems, vector<string>& resultResponses)
{
	// Goal: Analyze items stored by the command that concide with the given
	//       ID list, and identify matches by means of given keywords.
	//       Return this matching list of items, and all responses that
	//       should be executed with them.

	for (int i = 0; (size_t)i < idVec.size(); i++)
		for (int j = 0; (size_t)j < m_items.size(); j++)
			if (m_items[j].item->id() == idVec[i] && find(resultItems.begin(), resultItems.end(), m_items[j].item) == resultItems.end())
			{
				// Matching item!
				int nounCount = 0, adjCount = 0, prepCount = 0;
				vector<int> keywordMatchInd;
				int index;
				k_type type;

				// Find matching keywords
				for (int k = 0; (size_t)k < keywords.size(); k++)
					if (m_items[j].item->matchSingleKeyword(keywords[k], type))
						if (find(keywordMatchInd.begin(), keywordMatchInd.end(), index) == keywordMatchInd.end())
							// No duplicate matches
						{
							keywordMatchInd.push_back(k);
							if (type == NOUN)
								nounCount++;
							else if (type == ADJ)
								adjCount++;
							else
								prepCount++;
						}

				if (nounCount == 0) continue; // One noun must match
				if (nounCount + adjCount + prepCount != keywords.size())  // All words given by user must be accounted for
				    if (!m_items[j].item->superprep() || (m_items[j].item->superprep() && prepCount == 0))
						continue;
				if ((prepCount > 0 && adjCount == 0) // A preposition without a qualifier doesn't make sense
					&& !m_items[j].item->superprep()) // unless this is allowed and otherwise handled by a script
						continue;

				// Item fits; add to result
				resultItems.push_back(m_items[j].item);
				resultResponses.push_back(m_items[j].response);
			}
	return resultItems.size();
}
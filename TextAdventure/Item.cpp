// Item.cpp
#include <string>
#include <vector>
#include <algorithm>
#include "DbHandler.h"
#include "Item.h"
using namespace std;

Item::Item(int id, Game* game)
	: m_id(id), m_game(game), m_mult(false), m_prs(false), m_equipped(false)
{}

Item::~Item() {}

bool Item::addKeyword(string key, k_type type)
{
	for (vector<Keyword>::iterator p = m_keyword.begin(); p != m_keyword.end(); p++)
		if (p->name == key) return false;

	m_keyword.push_back(Keyword(key, type));
	return true;
}

bool Item::matchKeyword(vector<string> &keywords, int& index, k_type& resultType)
{
	for (index = 0; (size_t)index < keywords.size(); index++)
		for (int i = 0; (size_t)i < m_keyword.size(); i++)
			if (m_keyword[i].name == keywords[index])
			{
				resultType = m_keyword[i].type;
				return true;
			}
	return false;
}

bool Item::matchSingleKeyword(string& keyword, k_type& resultType)
{
	for (int i = 0; (size_t)i < m_keyword.size(); i++)
		if (m_keyword[i].name == keyword)
		{
			resultType = m_keyword[i].type;
			return true;
		}
	return false;
}

string Item::getAllKeywords() const
{
	string buffer = "";
	for (int i = 0; (size_t)i < m_keyword.size(); i++)
			buffer += m_keyword[i].name + " ";
	return buffer.substr(0, buffer.size() - 1); // Rmv last space
}
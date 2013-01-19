// Room.cpp
#include <iostream>
#include <algorithm>
#include "Room.h"
#include "DbHandler.h"
using namespace std;

Room::Room(int id, Game* gm)
{
	m_id = id;
	m_game = gm;
	m_state = 0;
	m_visited = false;
}

Room::~Room()
{

}

void Room::addDesc(string desc, int st) { pushBackInd(m_desc, desc, st); }

void Room::addSDesc(string desc, int st) { pushBackInd(m_sdesc, desc, st); }

void Room::addScript(string scr, int st) { pushBackInd(m_script, scr, st); }

void Room::addDir(string desc, int st, int dir) { pushBackInd(m_dir, vector<string> (10, ""), st); m_dir[st][dir] = desc; }

void Room::addItem(int id) { m_items.push_back(id); }

void Room::remItem(int id) { removeFromVector(m_items, id); }

void Room::addRemoved(int id) { if (find(m_itemsRemoved.begin(), m_itemsRemoved.end(), id) == m_itemsRemoved.end()) m_itemsRemoved.push_back(id); }

void Room::remRemoved(int id) { removeFromVector(m_itemsRemoved, id); }

void Room::countStates() { m_stateCount = m_desc.size(); }

int Room::stateCount() const { return m_stateCount; }

void Room::setState(int st) { m_state = st; }

void Room::setVisited(bool vis) { m_visited = vis; }

int Room::state() const { return m_state; }

bool Room::visited() const { return m_visited; }

string Room::desc(int ind) const { if ((size_t)ind >= m_desc.size()) return ""; return m_desc[ind]; }

string Room::sdesc(int ind) const { if ((size_t)ind >= m_sdesc.size()) return ""; return m_sdesc[ind]; }

string Room::script(int ind) const { if ((size_t)ind >= m_script.size()) return ""; return m_script[ind]; }

void Room::dir(vector<string>& v, int ind) const {
	for (int i = ind; i >= 0; i--)
	{
		if (m_dir.size() > (size_t)ind && m_dir[i][0] != "") { v = m_dir[i]; return; }
	}
	if (m_dir[0].size() > 0) v = m_dir[0];
}

string Room::dirB(int st, int dir) const {
	for (int i = st; i >= 0; i--)
		if (m_dir.size() > (size_t)st && m_dir[i][dir] != "") { return m_dir[i][dir]; }
	if ( m_dir[0].size() > 0 ) return m_dir[0][dir];
	return "";
}

bool Room::isRemoved(int id) const { return find(m_itemsRemoved.begin(), m_itemsRemoved.end(), id) != m_itemsRemoved.end(); }

vector<int> Room::items() const { return m_items; }
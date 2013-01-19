// Room.h
#ifndef ROOM
#define ROOM

#include <string>
#include <vector>
using namespace std;

class Game;

class Room
{
public:
	Room(int id, Game* gm); // Constructor
	~Room(); // Destructor
	// Functions
	void addDesc(string desc, int st);
	void addSDesc(string desc, int st);
	void addScript(string scr, int st);
	void addDir(string desc, int st, int dir);
	void addItem(int id);
	void remItem(int id);
	void addRemoved(int id);
	void remRemoved(int id);
	void countStates();
	void setState(int st);
	void setVisited(bool vis);
	// Access Members
	int stateCount() const;
	int state() const;
	bool visited() const;
	string desc(int ind) const;
	string sdesc(int ind) const;
	string script(int ind) const;
	void dir(vector<string>& v, int ind) const;
	string dirB(int st, int dir) const;
	bool isRemoved(int id) const;
	vector<int> items() const;
private:
	// Helpers
	// Members
	Game* m_game;
	int m_id;
	int m_state;
	int m_stateCount;
	bool m_visited;
	vector<string> m_desc;
	vector<string> m_sdesc;
	vector<string> m_script;
	vector< vector<string> > m_dir; // N, S, E, W, NE, SW, SE, NW, U, D; Per state
	vector<int> m_items; // Dropped items
	vector<int> m_itemsRemoved;
};

#endif
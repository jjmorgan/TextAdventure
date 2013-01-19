// Item.h
#ifndef ITEM
#define ITEM

#include <string>
#include <vector>
using namespace std;

enum k_type {
	NOUN, ADJ, PREP, NOUNADJ
};

class Game;

class Item
{
public:
	Item(int id, Game* game); // Constructor
	~Item(); // Destructor
	// Functions
	bool matchKeyword(vector<string> &keywords, int& index, k_type& resultType);
	bool matchSingleKeyword(string& keyword, k_type& resultType);
	string getAllKeywords() const;
	// Modifiers
	void setDesc(string desc) { m_desc = desc; }
	void setInvDesc(string inv) { m_inv = inv; }
	void setFind(string find) { m_find = find; }
	void setFollow(string follow) { m_follow = follow; }
	void toggleMultiple() { m_mult = !m_mult; }
	void toggleSuperprep() { m_prs = !m_prs; }
	void toggleEquip() { m_equipped = !m_equipped; }
	bool addKeyword(string key, k_type type);
	// Access Members
	int id() const { return m_id; }
	string desc() const { return m_desc; }
	string invdesc() const { return m_inv; }
	string find() const { return m_find; }
	string follow() const { return m_follow; }
	bool multiple() const { return m_mult; }
	bool superprep() const { return m_prs; }
	bool equipped() const { return m_equipped; }
	Game* game() const { return m_game; }
private:
	struct Keyword
	{
		Keyword(string n, k_type t) : name(n), type(t) {}
		string name;
		k_type type;
	};
	// Helpers
	// Members
	int m_id;
	vector<Keyword> m_keyword;
	string m_desc;
	string m_inv;
	string m_find;
	string m_follow;
	bool m_mult;
	bool m_prs;
	bool m_equipped;
	Game* m_game;
};

#endif
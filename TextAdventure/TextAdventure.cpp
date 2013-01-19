// Text Adventure Engine -- Made by Murugo
// Just over 1,000 lines of code. Not bad!
#include <iostream>
#include <string>
#include "DbHandler.h" // Debug, remove later
#include "Game.h"
using namespace std;

int main()
{
	Game game("settings.txt", "standard.txt", "rooms.ifr", "items.ifi", "scenarios.ifs");
	game.launch();
}
#include "Game.h"

int main()
{
	Game game;
	game.NewGame();
	while (true)
	{
		game.Dfs(0, true);
	}
}
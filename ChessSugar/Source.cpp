#include "Game.h"

int main()
{
	Game game;
	game.NewGame();
	int n = 0;
	while (true)
	{
		if (n % 2 == 0)
		{
			game.UserMove();
		}
		else
			game.Dfs(0, true);
		n++;
	}
}
#include "Game.h"

void printStructural(long long val)
{
	if (val == 0)
		return;
	printStructural(val / 1000);
	printf("%03d ", val % 1000);
}

int main()
{
	Game game;
	game.NewGame();
	int n = 0;
	int startDepth = 0;
	while (true) {
		if (n % 2 == 0) {
			game.UserMove();
		}
		else {
			game.callCnt = 0;
			printf("Depth = %d\n", MAX_DFS_DEPTH - startDepth);
			game.Dfs(startDepth, true);
			printStructural(game.callCnt);
			if (game.callCnt < 400000)
				startDepth--;
			if (game.callCnt > 7000000)
				startDepth++;

			putchar('\n');
		}
		n++;
	}
}
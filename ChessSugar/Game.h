#pragma once
#include "Desk.h"
#include "ConstantsAndEnums.h"
#include <stdio.h>


class Game
{
private:
	struct DfsState {
		int best, localRes;
		int fromX, fromY, toX, toY;
		int changeProbability;
		void Apply(int res, int fx, int fy, int tx, int ty);
		void Init();
	};

	Desk desk;

	int * was; //[HASH_MOD];
	int  * wasRes; //[HASH_MOD];

public:

	void NewGame();
	int Dfs(int depth, bool endWithMove = false);
	void PrintMove(int fx, int fy, int tx, int ty);
	Game();
	~Game();
};


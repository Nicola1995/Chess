#pragma once
#include "Desk.h"
#include "ConstantsAndEnums.h"
#include <stdio.h>
#include <string>

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

#ifdef HASH_ACTIVE
	int * was; //[HASH_MOD];
	int  * wasRes; //[HASH_MOD];
#endif // HASH_ACTIVE

public:
	long long callCnt;

	void NewGame();
	int Dfs(int depth, bool endWithMove = false);
	void PrintMove(int fx, int fy, int tx, int ty);
	void UserMove();
	Game();
	~Game();
};


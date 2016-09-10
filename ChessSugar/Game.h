#pragma once
#include "Desk.h"
#include "ConstantsAndEnums.h"
#include <stdio.h>
#include <map>


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

	std::map<long long, int> was;

public:

	int Dfs(int depth, bool endWithMove = false);
	Game();
	~Game();
};


#pragma once
#pragma once
#include "ConstantsAndEnums.h"
#include <stdio.h>
#include <iostream>

class Desk
{
private:
	struct Story {
		StoryType type;
		int x, y;
		Color oldColor;
		Figure oldFigure;
	};
	
	Color color[8][8];
	Figure figure[8][8];
	int turn, benefit;
	
	long long hMult[8][8];
	long long hTurn;
	long long hash;

	Story story[1000000];
	int storyTop;

	inline bool Valid(int x, int y);
	inline void NormolizeHash();
	inline long long GetFieldHash(int x, int y);
	inline int GetFieldBenefit(int x, int y);

public:
	int GetTurn();
	long long GetHash();
	void FixMove();
	void CancelMove();
	Color GetColor(int x, int y);
	Figure GetFigure(int x, int y);
	inline bool ValidEmpty(int x, int y);
	inline bool ValidBoth(int x, int y, Color myColor);
	inline bool ValidOpponent(int x, int y, Color myColor);
	void CloneField(int fromX, int fromY, int toX, int toY);
	void ClearField(int x, int y);
	int GetHeuristicBenefit();
	void NewGame();
	Desk();
	~Desk();
};

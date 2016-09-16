#pragma once
#include "ConstantsAndEnums.h"
#include <stdio.h>
#include <iostream>
#include <assert.h>

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
	int turn, benefit, focusX, focusY;
	bool castlingAble[2][2]; 

#ifdef HASH_ACTIVE
	long long hMult[8][8];
	long long hTurn;
	long long hash;
#endif

	Story story[1000];
	int storyTop;

	bool Valid(int x, int y);
#ifdef HASH_ACTIVE
	inline void NormolizeHash();
	inline long long GetFieldHash(int x, int y);
#endif // HASH_ACTIVE
	
	inline int GetFieldBenefit(int x, int y);

public:
	int GetTurn();
#ifdef HASH_ACTIVE
	long long GetHash();
#endif
	void FixMove();
	void CancelMove();
	Color GetColor(int x, int y);
	Figure GetFigure(int x, int y);
	bool ValidEmpty(int x, int y);
	bool ValidBoth(int x, int y, Color myColor);
	bool ValidOpponent(int x, int y, Color myColor);
	bool CastlingEnable(bool isRight);
	void CloneField(int fromX, int fromY, int toX, int toY);
	void ClearField(int x, int y);
	void SetField(int x, int y, Color color, Figure figure);
	int GetHeuristicBenefit();
	int GetRealBenefit();
	void NewGame();
	void PrintPosition();
	
	Desk();
	~Desk();
};


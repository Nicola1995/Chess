#include "Desk.h"

Desk::Desk()
{
	storyTop = 0;
	hMult[0][0] = 1;
	for (int i = 1; i < 64; i++)
	{
		hMult[i / 8][i % 8] = (hMult[(i - 1) / 8][(i - 1) % 8] * HASH_RDX) % HASH_MOD;
	}
	hTurn = (hMult[7][7] * HASH_RDX) % HASH_MOD;
}


Desk::~Desk()
{
}

#pragma region Public getters

long long Desk::GetHash()
{
	return hash;
}

int Desk::GetTurn()
{
	return turn;
}

Color Desk::GetColor(int x, int y)
{
	return color[x][y];
}

Figure Desk::GetFigure(int x, int y)
{
	return figure[x][y];
}

int Desk::GetHeuristicBenefit()
{
	return benefit;
}

#pragma endregion

#pragma region Validation functions

inline bool Desk::Valid(int x, int y)
{
	return x >= 0 && x < 8 && y >= 0 && y < 8;
}

inline bool Desk::ValidEmpty(int x, int y)
{
	return Valid(x, y) && color[x][y] == Color::EMPTY;
}

inline bool Desk::ValidOpponent(int x, int y, Color myColor)
{
	if (myColor == Color::WHITE)
		myColor = Color::BLACK;
	else
		myColor = Color::WHITE;
	return Valid(x, y) && color[x][y] == myColor;
}

inline bool Desk::ValidBoth(int x, int y, Color myColor)
{
	return ValidEmpty(x, y) || ValidOpponent(x, y, myColor);
}

#pragma endregion

#pragma region Position changing

inline void Desk::NormolizeHash()
{
	while (hash < 0) hash += HASH_MOD;
	while (hash >= HASH_MOD) hash -= HASH_MOD;
}

void Desk::FixMove()
{
	story[storyTop].type = StoryType::NXT_TURN;
	storyTop++;
	hash += hTurn;
	NormolizeHash();
	turn++;
	benefit *= -1;
}

void Desk::CancelMove()
{
	storyTop--;
	if (story[storyTop].type != StoryType::NXT_TURN)
		std::cerr << "Not NXT_TURN at the top of the story!\n";
	else {
		hash -= hTurn;
		turn--;
		storyTop--;
		benefit *= -1;
	}
	while (story[storyTop].type != StoryType::NXT_TURN) {
		if (story[storyTop].type == StoryType::FIELD_CHANGE) {
			int x = story[storyTop].x;
			int y = story[storyTop].y;
			hash -= GetFieldHash(x, y);
			benefit -= GetFieldBenefit(x, y);
			figure[x][y] = story[storyTop].oldFigure;
			color[x][y] = story[storyTop].oldColor;
			hash += GetFieldHash(x, y);
			benefit += GetFieldBenefit(x, y);
		}
		storyTop--;
	}
	NormolizeHash();
}

void Desk::ClearField(int x, int y)
{
	hash -= GetFieldHash(x, y);
	benefit -= GetFieldBenefit(x, y);
	story[storyTop].type = StoryType::FIELD_CHANGE;
	story[storyTop].x = x;
	story[storyTop].y = y;
	story[storyTop].oldColor = color[x][y];
	story[storyTop].oldFigure = figure[x][y];
	storyTop++;
	color[x][y] = Color::EMPTY;
	figure[x][y] = Figure::EMPTY;
	hash += GetFieldHash(x, y);
	benefit += GetFieldBenefit(x, y);
}



#pragma endregion



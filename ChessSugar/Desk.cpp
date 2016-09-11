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
	assert(hash >= 0 && hash < HASH_MOD);
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

int Desk::GetRealBenefit()
{
	Color curColor = (turn & 1) ? Color::WHITE : Color::BLACK;
	int ret = 0;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (color[i][j] == curColor)
				ret++;
			else if (color[i][j] != Color::EMPTY)
				ret--;
	return ret;
}

#pragma endregion

#pragma region Validation functions

bool Desk::Valid(int x, int y)
{
	return x >= 0 && x < 8 && y >= 0 && y < 8;
}

bool Desk::ValidEmpty(int x, int y)
{
	return Valid(x, y) && (color[x][y] == Color::EMPTY);
}

bool Desk::ValidOpponent(int x, int y, Color myColor)
{
	if (myColor == Color::WHITE)
		myColor = Color::BLACK;
	else
		myColor = Color::WHITE;
	return Valid(x, y) && (color[x][y] == myColor);
}

bool Desk::ValidBoth(int x, int y, Color myColor)
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
		assert(storyTop > 0);
		if (story[storyTop].type == StoryType::FIELD_CHANGE) {
			int x = story[storyTop].x;
			int y = story[storyTop].y;
			hash -= GetFieldHash(x, y);
			benefit -= GetFieldBenefit(x, y);
			figure[x][y] = story[storyTop].oldFigure;
		//	assert(figure[x][y] == Figure::NONE || figure[x][y] == Figure::PIECE);
			color[x][y] = story[storyTop].oldColor;
			hash += GetFieldHash(x, y);
			benefit += GetFieldBenefit(x, y);
		}
		storyTop--;
	}
	storyTop++;
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
	assert(figure[x][y] == Figure::NONE || 
		   figure[x][y] == Figure::PIECE ||
		   figure[x][y] == Figure::HORSE);
	storyTop++;
	color[x][y] = Color::EMPTY;
	figure[x][y] = Figure::NONE;
	hash += GetFieldHash(x, y);
	benefit += GetFieldBenefit(x, y);
	NormolizeHash();
}

void Desk::CloneField(int fromX, int fromY, int toX, int toY)
{
	hash -= GetFieldHash(toX, toY);
	benefit -= GetFieldBenefit(toX, toY);
	story[storyTop].type = StoryType::FIELD_CHANGE;
	story[storyTop].x = toX;
	story[storyTop].y = toY;
	story[storyTop].oldColor = color[toX][toY];
	story[storyTop].oldFigure = figure[toX][toY];
	storyTop++;
	color[toX][toY] = color[fromX][fromY];
	figure[toX][toY] = figure[fromX][fromY];
	//assert(figure[toX][toY] == Figure::NONE || figure[toX][toY] == Figure::PIECE);
	hash += GetFieldHash(toX, toY);
	benefit += GetFieldBenefit(toX, toY);
	//ClearField(fromX, fromY);
	NormolizeHash();
}

inline long long Desk::GetFieldHash(int x, int y)
{
	long long ret;
	switch (figure[x][y]) {
	case Figure::NONE:
		ret = 0;
		break;
	case Figure::PIECE:
		ret = 1;
		break;
	case Figure::HORSE:
		ret = 2;
		break;
	}
	if (color[x][y] == Color::BLACK)
		ret += FIG_NUM + 1;
	return (ret * hMult[x][y]) % HASH_MOD;
}

inline int Desk::GetFieldBenefit(int x, int y)
{
	if (figure[x][y] == Figure::NONE)
		return 0;
	int ret;
	switch (figure[x][y]) {
	case Figure::PIECE:
		ret = 1;
		break;
	case Figure::HORSE:
		ret = 3;
		break;
	}
	if (color[x][y] != ((turn & 1) ? Color::WHITE : Color::BLACK))
		ret *= -1;
	return ret;
}

void Desk::NewGame()
{
	benefit = 0;
	turn = 0;
	hash = 0;
	storyTop = 0;

	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++) {
			color[x][y] = Color::EMPTY;
			figure[x][y] = Figure::NONE;
		}
	for (int x = 0; x < 8; x++)
	{
		color[x][1] = Color::WHITE;
		figure[x][1] = Figure::PIECE;
		hash += GetFieldHash(x, 1);
		color[x][6] = Color::BLACK;
		figure[x][6] = Figure::PIECE;
		hash += GetFieldHash(x, 6);
	}
	color[1][0] = color[6][0] = Color::WHITE;
	color[1][7] = color[6][7] = Color::BLACK;
	figure[1][0] = figure[1][7] = figure[6][7] = figure[6][0] = Figure::HORSE;
}

#pragma endregion

void Desk::PrintPosition()
{
	printf("\n");
	for (int y = 7; y >= 0; y--){
		for (int x = 0; x < 8; x++) {
			switch (figure[x][y]) {
			case Figure::NONE:
				putchar('.');
				break;
			case Figure::PIECE:
				putchar('a' + (color[x][y] == Color::BLACK));
				break;
			case Figure::HORSE:
				putchar('A' + (color[x][y] == Color::BLACK));
				break;
			}
		}
		printf("\n");
	}
	//printf("\nType smth to continue\n");
	//std::cin.get();
}

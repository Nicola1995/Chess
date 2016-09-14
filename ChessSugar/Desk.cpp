#include "Desk.h"

Desk::Desk()
{
	storyTop = 0;
#ifdef HASH_ACTIVE
	hMult[0][0] = 1;
	for (int i = 1; i < 64; i++)
	{
		hMult[i / 8][i % 8] = (hMult[(i - 1) / 8][(i - 1) % 8] * HASH_RDX) % HASH_MOD;
	}
	hTurn = (hMult[7][7] * HASH_RDX) % HASH_MOD;
#endif 
}


Desk::~Desk()
{
}

#pragma region Public getters

#ifdef HASH_ACTIVE
long long Desk::GetHash()
{
	assert(hash >= 0 && hash < HASH_MOD);
	return hash;
}
#endif

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

#ifdef HASH_ACTIVE
inline void Desk::NormolizeHash()
{
	while (hash < 0) hash += HASH_MOD;
	while (hash >= HASH_MOD) hash -= HASH_MOD;
}
#endif

void Desk::FixMove()
{
	story[storyTop].type = StoryType::NXT_TURN;
	storyTop++;
#ifdef HASH_ACTIVE
	hash += hTurn;
	NormolizeHash();
#endif
	turn++;
	benefit *= -1;
}

void Desk::CancelMove()
{
	storyTop--;
	if (story[storyTop].type != StoryType::NXT_TURN)
		std::cerr << "Not NXT_TURN at the top of the story!\n";
	else {
#ifdef HASH_ACTIVE
		hash -= hTurn;
#endif
		turn--;
		storyTop--;
		benefit *= -1;
	}
	while (story[storyTop].type != StoryType::NXT_TURN) {
		assert(storyTop > 0);
		if (story[storyTop].type == StoryType::FIELD_CHANGE) {
			int x = story[storyTop].x;
			int y = story[storyTop].y;
#ifdef HASH_ACTIVE
			hash -= GetFieldHash(x, y);
#endif 
			benefit -= GetFieldBenefit(x, y);
			figure[x][y] = story[storyTop].oldFigure;
		//	assert(figure[x][y] == Figure::NONE || figure[x][y] == Figure::PIECE);
			color[x][y] = story[storyTop].oldColor;
#ifdef HASH_ACTIVE
			hash += GetFieldHash(x, y);
#endif
			benefit += GetFieldBenefit(x, y);
		}
		storyTop--;
	}
	storyTop++;
#ifdef HASH_ACTIVE
	NormolizeHash();
#endif
}

void Desk::ClearField(int x, int y)
{
#ifdef HASH_ACTIVE
	hash -= GetFieldHash(x, y);
#endif
	benefit -= GetFieldBenefit(x, y);
	story[storyTop].type = StoryType::FIELD_CHANGE;
	story[storyTop].x = x;
	story[storyTop].y = y;
	story[storyTop].oldColor = color[x][y];
	story[storyTop].oldFigure = figure[x][y];
	assert(figure[x][y] == Figure::NONE || 
		   figure[x][y] == Figure::PIECE ||
		   figure[x][y] == Figure::HORSE ||
		   figure[x][y] == Figure::BISHOP ||
		   figure[x][y] == Figure::ROOK ||
		   figure[x][y] == Figure::QUEEN);
	storyTop++;
	color[x][y] = Color::EMPTY;
	figure[x][y] = Figure::NONE;
#ifdef HASH_ACTIVE
	hash += GetFieldHash(x, y);
#endif
	benefit += GetFieldBenefit(x, y);
#ifdef HASH_ACTIVE
	NormolizeHash();
#endif // HASH_ACTIVE
}

void Desk::CloneField(int fromX, int fromY, int toX, int toY)
{
#ifdef HASH_ACTIVE
	hash -= GetFieldHash(toX, toY);
#endif
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
#ifdef HASH_ACTIVE
	hash += GetFieldHash(toX, toY);
#endif
	benefit += GetFieldBenefit(toX, toY);
	//ClearField(fromX, fromY);
#ifdef HASH_ACTIVE
	NormolizeHash();
#endif
}

#ifdef HASH_ACTIVE
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
	case Figure::BISHOP:
		ret = 3;
		break;
	case Figure::ROOK:
		ret = 5;
		break;
	}
	if (color[x][y] == Color::BLACK)
		ret += FIG_NUM + 1;
	return (ret * hMult[x][y]) % HASH_MOD;
}
#endif

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
	case Figure::BISHOP:
		ret = 3;
		break;
	case Figure::ROOK:
		ret = 4;
		break;
	case Figure::QUEEN:
		ret = 8;
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
#ifdef HASH_ACTIVE
	hash = 0;
#endif
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
#ifdef HASH_ACTIVE
		hash += GetFieldHash(x, 1);
#endif
		color[x][6] = Color::BLACK;
		figure[x][6] = Figure::PIECE;
#ifdef HASH_ACTIVE
		hash += GetFieldHash(x, 6);
#endif
	}

	color[1][0] = color[6][0] = Color::WHITE;
	color[1][7] = color[6][7] = Color::BLACK;
	figure[1][0] = figure[1][7] = figure[6][7] = figure[6][0] = Figure::HORSE;

	color[0][0] = color[7][0] = Color::WHITE;
	color[0][7] = color[7][7] = Color::BLACK;
	figure[0][0] = figure[0][7] = figure[7][7] = figure[7][0] = Figure::ROOK;

	color[2][0] = color[5][0] = Color::WHITE;
	color[2][7] = color[5][7] = Color::BLACK;
	figure[2][0] = figure[2][7] = figure[5][7] = figure[5][0] = Figure::BISHOP;

	color[3][0] = Color::WHITE;
	color[3][7] = Color::BLACK;
	figure[3][0] = figure[3][7] = Figure::QUEEN;
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

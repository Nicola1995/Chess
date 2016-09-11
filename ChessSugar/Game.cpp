#include "Game.h"



Game::Game()
{
	was = new int[HASH_MOD];
	wasRes = new int[HASH_MOD];
}


Game::~Game()
{
}

void Game::UserMove()
{
	printf("Your move:\n");
	int fy, ty, fx, tx;

	std::string umove;
	std::cin >> umove;
	fx = umove[0] - 'a';
	fy = umove[1] - '1';
	tx = umove[3] - 'a';
	ty = umove[4] - '1';

	desk.FixMove();
	desk.CloneField(fx, fy, tx, ty);
	desk.ClearField(fx, fy);
	desk.PrintPosition();
}

int Game::Dfs(int depth, bool endWithMove)
{
	static int unqNum = 0;
	if (endWithMove) unqNum++;
//	std::cerr << "dfs " << depth << " " << desk.GetTurn() << "\n";
	desk.FixMove();
	
	//very slow!!!
	//assert(desk.GetHeuristicBenefit() == desk.GetRealBenefit());

	//if (was[desk.GetHash()] == unqNum)
	//	return wasRes[desk.GetHash()];
	was[desk.GetHash()] = unqNum;
	if (depth >= MAX_DFS_DEPTH) {
		return wasRes[desk.GetHash()] = desk.GetHeuristicBenefit();
	}

	DfsState report;
	report.Init();
	Color curColor = desk.GetTurn() & 1 ? Color::WHITE : Color::BLACK;

	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			if (desk.GetColor(x, y) == curColor) {
				switch (desk.GetFigure(x, y)) {
					int dy;
#pragma region Moves of PIECE
				case Figure::PIECE:
					dy = curColor == Color::WHITE ? 1 : -1;
					if (desk.ValidEmpty(x, y + dy)) {
						desk.CloneField(x, y, x, y + dy);
						desk.ClearField(x, y);
						report.Apply(Dfs(depth + 1), x, y, x, y + dy);
						desk.CancelMove();
						if ((y == 1 || y == 6) && desk.ValidEmpty(x, y + 2 * dy)) {
							desk.CloneField(x, y, x, y + 2*dy);
							desk.ClearField(x, y);
							report.Apply(Dfs(depth + 1), x, y, x, y + 2*dy);
							desk.CancelMove();
						}
					}
					for (int dx = -1; dx <= 1; dx += 2) {
						if (desk.ValidOpponent(x + dx, y + dy, curColor)) {
							desk.CloneField(x, y, x + dx, y + dy);
							desk.ClearField(x, y);
							report.Apply(Dfs(depth + 1), x, y, x + dx, y + dy);
							desk.CancelMove();
						}
					}
					break;
#pragma endregion

#pragma region Moves of HORSE
				case Figure::HORSE:
					static int hdx[] = {2, 1, -1, -2, -2, -1, 1, 2};
					static int hdy[] = {1, 2, 2, 1, -1, -2, -2, -1};
					for (int ht = 0; ht < 8; ht++) {
						if (desk.ValidBoth(x + hdx[ht], y + hdy[ht], curColor)) {
							desk.CloneField(x, y, x + hdx[ht], y + hdy[ht]);
							desk.ClearField(x, y);
							report.Apply(Dfs(depth + 1), x, y, x + hdx[ht], y + hdy[ht]);
							desk.CancelMove();
						}
					}
					break;
#pragma endregion

				}
			}
	if (report.best > INF)
		return wasRes[desk.GetHash()] = desk.GetHeuristicBenefit();

	if (endWithMove) {
		desk.CloneField(report.fromX, report.fromY, report.toX, report.toY);
		desk.ClearField(report.fromX, report.fromY);
		PrintMove(report.fromX, report.fromY, report.toX, report.toY);
	}
	return wasRes[desk.GetHash()] = -report.best;
}

void Game::PrintMove(int fx, int fy, int tx, int ty)
{
	if (desk.GetTurn() & 1)
		printf("Whites:\n");
	else
		printf("Blacks:\n");
	printf("%c%d - %c%d\n", 'A' + fx, 1 + fy, 'A' + tx, 1 + ty);
	desk.PrintPosition();
}

void Game::NewGame()
{
	desk.NewGame();
}

#pragma region DfsState functions

void Game::DfsState::Init()
{
	best = INF + 10;
	changeProbability = 2;
}

void Game::DfsState::Apply(int res, int fx, int fy, int tx, int ty)
{
	if (res < best) {
		best = res;
		changeProbability = 2;
		fromX = fx, fromY = fy, toX = tx, toY = ty;
	}
	else if (res == best && (((changeProbability - 1) & rand()) == 0)) {
		changeProbability <<= 1;
		fromX = fx, fromY = fy, toX = tx, toY = ty;
	}
}

#pragma endregion

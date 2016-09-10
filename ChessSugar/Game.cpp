#include "Game.h"



Game::Game()
{
}


Game::~Game()
{
}

int Game::Dfs(int depth, bool endWithMove = false)
{
	desk.FixMove();
	
	if (was.find(desk.GetHash()) != was.end())
		return was[desk.GetHash()];
	if (depth >= MAX_DFS_DEPTH)
		return was[desk.GetHash()] = desk.GetHeuristicBenefit();

	DfsState report;
	report.Init();
	Color curColor = desk.GetTurn() & 1 ? Color::WHITE : Color::BLACK;

	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			if (desk.GetColor(x, y) == curColor) {
				switch (desk.GetFigure(x, y)) {
#pragma region Moves of PIECE
				case Figure::PIECE:
					int dy = curColor == Color::WHITE ? 1 : -1;
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
						if (desk.ValidOpponent(x + dx, y + dy)) {
							desk.CloneField(x, y, x + dx, y + dy);
							desk.ClearField(x, y);
							report.Apply(Dfs(depth + 1), x, y, x + dx, y + dy);
							desk.CancelMove();
						}
					}
					break;
#pragma endregion

				}
			}

	if (endWithMove) {
		desk.CloneField(report.fromX, report.fromY, report.toX, report.toY);
		desk.ClearField(report.fromX, report.fromY);
	}
	return was[desk.GetHash()] = -report.best;
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
	else if (res == best && ((changeProbability - 1) & rand()) == 0) {
		changeProbability <<= 1;
		fromX = fx, fromY = fy, toX = tx, toY = ty;
	}
}

#pragma endregion

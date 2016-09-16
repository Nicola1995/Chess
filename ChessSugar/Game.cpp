#include "Game.h"



Game::Game()
{
#ifdef HASH_ACTIVE
	was = new int[HASH_MOD];
	wasRes = new int[HASH_MOD];
#endif  
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
	//desk.PrintPosition();
}

int Game::Dfs(int depth, bool endWithMove)
{
	callCnt++;
	static int unqNum = 0;
	if (endWithMove) unqNum++;
	//std::cerr << "dfs " << depth << "\n";
	//	std::cerr << "dfs " << depth << " " << desk.GetTurn() << "\n";
	desk.FixMove();
	
	//very slow!!!
	//assert(desk.GetHeuristicBenefit() == desk.GetRealBenefit());

#ifdef HASH_ACTIVE
	if (was[desk.GetHash()] == unqNum)
		return wasRes[desk.GetHash()];
	was[desk.GetHash()] = unqNum;
#endif // DEBUG

	if (abs(desk.GetHeuristicBenefit()) > INF)
		return desk.GetHeuristicBenefit();
	 
	if (depth >= MAX_DFS_DEPTH + 2) {
#ifdef HASH_ACTIVE
		return wasRes[desk.GetHash()] = desk.GetHeuristicBenefit();
#else
		return desk.GetHeuristicBenefit();
#endif
	}
	

	DfsState report;
	report.Init();
	Color curColor = desk.GetTurn() & 1 ? Color::WHITE : Color::BLACK;

	static const int hdx[] = { 2, 1, -1, -2, -2, -1, 1, 2 };
	static const int hdy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };
	static const int ldx[] = { 1, 0, -1, 0, 1, -1, -1, 1 };
	static const int ldy[] = { 0, 1, 0, -1, 1, 1, -1, -1 };

	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			if (desk.GetColor(x, y) == curColor) {
				switch (desk.GetFigure(x, y)) {
					int dy;
					
#pragma region Moves of PIECE
				case Figure::PIECE:
					dy = curColor == Color::WHITE ? 1 : -1;
					if (depth < MAX_DFS_DEPTH && desk.ValidEmpty(x, y + dy)) {
						desk.CloneField(x, y, x, y + dy);
						desk.ClearField(x, y);
						if (y + dy == 0 || y + dy == 7)
							desk.SetField(x, y + dy, curColor, Figure::QUEEN);
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
					
					for (int ht = 0; ht < 8; ht++) {
						if (desk.ValidBoth(x + hdx[ht], y + hdy[ht], curColor) &&
							(depth < MAX_DFS_DEPTH ||
								desk.ValidOpponent(x + hdx[ht], y + hdy[ht], curColor)
								)) {
							desk.CloneField(x, y, x + hdx[ht], y + hdy[ht]);
							desk.ClearField(x, y);
							report.Apply(Dfs(depth + 1), x, y, x + hdx[ht], y + hdy[ht]);
							desk.CancelMove();
						}
					}
					break;
#pragma endregion

#pragma region Moves of QUEEN, ROOK and BISHOP
				case Figure::BISHOP:
					for (int dir = 4; dir < 8; dir++) {
						int k = 1;
						while (true) {
							if (desk.ValidBoth(x + ldx[dir] * k, y + ldy[dir] * k, curColor)) {
								if (!(depth < MAX_DFS_DEPTH ||
									desk.ValidOpponent(x + ldx[dir] * k, y + ldy[dir] * k, curColor))) {
									k++;
									continue;
								}
								desk.CloneField(x, y, x + ldx[dir] * k, y + ldy[dir] * k);
								desk.ClearField(x, y);
								report.Apply(Dfs(depth + 1), x, y, x + ldx[dir] * k, y + ldy[dir] * k);
								desk.CancelMove();
								if (!desk.ValidEmpty(x + ldx[dir] * k, y + ldy[dir] * k))
									break;
							}
							else
								break;
							k++;
						}
					}
					break;
				case Figure::ROOK:
					for (int dir = 0; dir < 4; dir++) {
						int k = 1;
						while (true) {
							if (desk.ValidBoth(x + ldx[dir] * k, y + ldy[dir] * k, curColor)) {
								if (!(depth < MAX_DFS_DEPTH ||
									desk.ValidOpponent(x + ldx[dir] * k, y + ldy[dir] * k, curColor))) {
									k++;
									continue;
								}
								desk.CloneField(x, y, x + ldx[dir] * k, y + ldy[dir] * k);
								desk.ClearField(x, y);
								report.Apply(Dfs(depth + 1), x, y, x + ldx[dir] * k, y + ldy[dir] * k);
								desk.CancelMove();
								if (!desk.ValidEmpty(x + ldx[dir] * k, y + ldy[dir] * k))
									break;
							}
							else
								break;
							k++;
						}
					}
					break;
				case Figure::QUEEN:
					for (int dir = 0; dir < 8; dir++) {
						int k = 1;
						while (true) {
							if (desk.ValidBoth(x + ldx[dir] * k, y + ldy[dir] * k, curColor)) {
								if (!(depth < MAX_DFS_DEPTH ||
									desk.ValidOpponent(x + ldx[dir] * k, y + ldy[dir] * k, curColor))) {
									k++;
									continue;
								}
								desk.CloneField(x, y, x + ldx[dir] * k, y + ldy[dir] * k);
								desk.ClearField(x, y);
								report.Apply(Dfs(depth + 1), x, y, x + ldx[dir] * k, y + ldy[dir] * k);
								desk.CancelMove();
								if (!desk.ValidEmpty(x + ldx[dir] * k, y + ldy[dir] * k))
									break;
							}
							else
								break;
							k++;
						}
					}
					break;
#pragma endregion

#pragma region Moves of KING
				
				case Figure::KING:
					bool freeLeft = false, freeRight = false;
					
					for (int dir = 0; dir < 8; dir++) {
						if (desk.ValidBoth(x + ldx[dir], y + ldy[dir], curColor)) {
							if (!(depth < MAX_DFS_DEPTH ||
								desk.ValidOpponent(x + ldx[dir], y + ldy[dir], curColor)))
								continue;
							desk.CloneField(x, y, x + ldx[dir], y + ldy[dir]);
							desk.ClearField(x, y);
							int localRes = report.Apply(Dfs(depth + 1), x, y, x + ldx[dir], y + ldy[dir]);
							if (dir == 2 && localRes < INF)
								freeLeft = true;
							if (dir == 0 && localRes < INF)
								freeRight = true;
							desk.CancelMove();
						}
					}
					
					if (freeRight && desk.CastlingEnable(true)) {
						//здесь мы должны сделать рокировку
					}

					break;
#pragma endregion

				}
			}
			if (report.best > INF)
#ifdef HASH_ACTIVE
				return wasRes[desk.GetHash()] = desk.GetHeuristicBenefit();
#else
				return desk.GetHeuristicBenefit();
#endif 


		

	if (endWithMove) {
		desk.CloneField(report.fromX, report.fromY, report.toX, report.toY);
		desk.ClearField(report.fromX, report.fromY);
		PrintMove(report.fromX, report.fromY, report.toX, report.toY);
	}
#ifdef HASH_ACTIVE
	return wasRes[desk.GetHash()] = -report.best;
#else
	return -report.best;
#endif
}

void Game::PrintMove(int fx, int fy, int tx, int ty)
{
	if (desk.GetTurn() & 1)
		printf("Whites:\n");
	else
		printf("Blacks:\n");
	printf("%c%d - %c%d\n", 'A' + fx, 1 + fy, 'A' + tx, 1 + ty);
	//desk.PrintPosition();
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

int Game::DfsState::Apply(int res, int fx, int fy, int tx, int ty)
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
	return res;
}

#pragma endregion

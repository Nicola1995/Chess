#pragma once

const int INF = 1000000000;
const int MAX_DFS_DEPTH = 4;
const long long HASH_MOD = 1000003;
const long long HASH_RDX = 13;
const int FIG_NUM = 6;

enum Color {
	BLACK = 0, WHITE = 1, EMPTY = 2
};

enum Figure {
	PIECE, KING, HORSE, BISHOP, ROOK, QUEEN, NONE
};

enum StoryType {
	NXT_TURN, FIELD_CHANGE
};



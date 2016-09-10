#pragma once

const int INF = 1000000000;
const int MAX_DFS_DEPTH = 8;
const long long HASH_MOD = 1000003;
const long long HASH_RDX = 3;
const int FIG_NUM = 1;

enum Color {
	BLACK = 0, WHITE = 1, EMPTY = 2
};

enum Figure {
	PIECE, KING, NONE = 2
};

enum StoryType {
	NXT_TURN, FIELD_CHANGE
};



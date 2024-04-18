#ifndef BFS_H
#define BFS_H

#include <queue>
#include "linked_list.h"

const int BRD_SIZE = 10;
char findOptimalDirection(char board[][BRD_SIZE], int robotRow, int robotCol, int playerRow, int playerCol);

#endif

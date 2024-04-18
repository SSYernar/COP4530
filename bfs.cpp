#include "bfs.h"

char findOptimalDirection(char board[][BRD_SIZE], int robotRow, int robotCol, int playerRow, int playerCol)
{
    const int dx[] = {0, 0, 1, -1};
    const int dy[] = {1, -1, 0, 0};
    bool visited[BRD_SIZE][BRD_SIZE] = {false};
    std::queue<std::pair<int, int>> q;
    q.push({robotRow, robotCol});
    visited[robotRow][robotCol] = true;

    char optimalDirection = ' ';

    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        if (abs(x - playerRow) + abs(y - playerCol) == 1)
        {
            if (x < playerRow)
                optimalDirection = 'S';
            else if (x > playerRow)
                optimalDirection = 'W';
            else if (y < playerCol)
                optimalDirection = 'D';
            else if (y > playerCol)
                optimalDirection = 'A';
            break;
        }

        for (int k = 0; k < 4; ++k)
        {
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (nx >= 0 && nx < BRD_SIZE && ny >= 0 && ny < BRD_SIZE && !visited[nx][ny])
            {
                q.push({nx, ny});
                visited[nx][ny] = true;
            }
        }
    }

    return optimalDirection;
}

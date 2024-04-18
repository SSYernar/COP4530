#include <iostream>
#include <queue>
#include <stack>
#include <cstdlib> // For system("cls") on Windows or system("clear") on Unix-like systems
#include <random>  // For random number generation

#ifdef _WIN32
#include <conio.h> // For _getch() on Windows
#else
#include <termios.h> // For termios, tcgetattr(), tcsetattr() on Unix-like systems
#include <unistd.h>  // For STDIN_FILENO on Unix-like systems
#endif

using namespace std;

const int BOARD_SIZE = 10;

// Linked list node to store item locations
struct ItemNode
{
    int row;
    int col;
    ItemNode *next;

    ItemNode(int r, int c) : row(r), col(c), next(nullptr) {}
};

// Function to read a single character from the console
char getch()
{
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

// Function to check if an item exists at a given location in the linked list
bool itemExists(ItemNode *items, int row, int col)
{
    ItemNode *current = items;
    while (current != nullptr)
    {
        if (current->row == row && current->col == col)
            return true;
        current = current->next;
    }
    return false;
}

// Function to add an item to the linked list
void addItem(ItemNode *&items, int row, int col)
{
    if (items == nullptr)
        items = new ItemNode(row, col);
    else
    {
        ItemNode *newNode = new ItemNode(row, col);
        newNode->next = items;
        items = newNode;
    }
}

// Function to remove an item from the linked list
void removeItem(ItemNode *&items, int row, int col)
{
    if (items == nullptr)
        return;
    if (items->row == row && items->col == col)
    {
        ItemNode *temp = items;
        items = items->next;
        delete temp;
        return;
    }
    ItemNode *prev = items;
    ItemNode *current = items->next;
    while (current != nullptr)
    {
        if (current->row == row && current->col == col)
        {
            prev->next = current->next;
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Function to display the board
void displayBoard(char board[][BOARD_SIZE], ItemNode *items)
{
    // Clear the screen
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    // Display the board
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            char symbol = board[i][j];
            if (symbol == 'P')
                cout << "\033[1;32m" << symbol << "\033[0m "; // Green color for player, reset color after printing
            else if (symbol == 'R')
                cout << "\033[1;31m" << symbol << "\033[0m "; // Red color for robot, reset color after printing
            else if (items != nullptr && itemExists(items, i, j))
                cout << "\033[1;33mI\033[0m "; // Yellow color for item, reset color after printing
            else
                cout << symbol << " ";
        }
        cout << endl;
    }
}

// Function to find the optimal direction for the robot to move towards the player
char findOptimalDirection(char board[][BOARD_SIZE], int robotRow, int robotCol, int playerRow, int playerCol)
{
    const int dx[] = {0, 0, 1, -1};
    const int dy[] = {1, -1, 0, 0};
    bool visited[BOARD_SIZE][BOARD_SIZE] = {false};
    queue<pair<int, int>> q;
    q.push({robotRow, robotCol});
    visited[robotRow][robotCol] = true;

    // Initialize optimal direction as no movement
    char optimalDirection = ' ';

    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        // Check if the robot is adjacent to the player
        if (abs(x - playerRow) + abs(y - playerCol) == 1)
        {
            // Determine the direction towards the player
            if (x < playerRow)
                optimalDirection = 'S'; // Move down
            else if (x > playerRow)
                optimalDirection = 'W'; // Move up
            else if (y < playerCol)
                optimalDirection = 'D'; // Move right
            else if (y > playerCol)
                optimalDirection = 'A'; // Move left
            break;
        }

        // Explore neighboring cells
        for (int k = 0; k < 4; ++k)
        {
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && !visited[nx][ny])
            {
                q.push({nx, ny});
                visited[nx][ny] = true;
            }
        }
    }

    return optimalDirection;
}

int main()
{
    // Seed the random number generator
    random_device rd;
    mt19937 gen(rd());

    char board[BOARD_SIZE][BOARD_SIZE];

    // Initialize the board with '.' indicating empty spaces
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            board[i][j] = '.';
        }
    }

    // Starting position of the player and robot
    int playerRow = 0;
    int playerCol = 0;
    int robotRow = 5;
    int robotCol = 5;
    board[playerRow][playerCol] = 'P'; // 'P' represents the player
    board[robotRow][robotCol] = 'R';   // 'R' represents the robot

    // Linked list to store item locations
    ItemNode *items = nullptr;

    // Add items to collect on the board
    uniform_int_distribution<int> dist(0, BOARD_SIZE - 1);
    for (int i = 0; i < 5; ++i)
    {
        int itemRow = dist(gen);
        int itemCol = dist(gen);
        while (board[itemRow][itemCol] == 'P')
        {
            itemRow = dist(gen);
            itemCol = dist(gen);
        }
        addItem(items, itemRow, itemCol); // Add item to the linked list
    }

    // Display the initial board
    displayBoard(board, items);

    // Stack to store previous positions for undo operation
    stack<pair<int, int>> playerHistory;
    stack<pair<int, int>> robotHistory;

    // Game loop
    char move;
    bool gameOver = false;
    int itemsRemaining = 5; // Number of items remaining to collect
    while (!gameOver)
    {
        // Ask the player for input
        cout << "Enter move (W - up, S - down, A - left, D - right, U - undo): ";
        move = getch();
        bool undoOperation = (move == 'U' || move == 'u');
        // Update player position based on input
        switch (move)
        {
        case 'W':
        case 'w':
            if (playerRow > 0)
            {
                playerHistory.push({playerRow, playerCol});
                board[playerRow][playerCol] = '.';
                playerRow--;
                board[playerRow][playerCol] = 'P';
            }
            break;
        case 'S':
        case 's':
            if (playerRow < BOARD_SIZE - 1)
            {
                playerHistory.push({playerRow, playerCol});
                board[playerRow][playerCol] = '.';
                playerRow++;
                board[playerRow][playerCol] = 'P';
            }
            break;
        case 'A':
        case 'a':
            if (playerCol > 0)
            {
                playerHistory.push({playerRow, playerCol});
                board[playerRow][playerCol] = '.';
                playerCol--;
                board[playerRow][playerCol] = 'P';
            }
            break;
        case 'D':
        case 'd':
            if (playerCol < BOARD_SIZE - 1)
            {
                playerHistory.push({playerRow, playerCol});
                board[playerRow][playerCol] = '.';
                playerCol++;
                board[playerRow][playerCol] = 'P';
            }
            break;
        case 'U':
        case 'u':
            if (!playerHistory.empty())
            {
                // Undo player move
                pair<int, int> prevPlayerPos = playerHistory.top();
                playerHistory.pop();
                board[playerRow][playerCol] = '.';
                playerRow = prevPlayerPos.first;
                playerCol = prevPlayerPos.second;
                board[playerRow][playerCol] = 'P';

                // Undo robot move
                if (!robotHistory.empty())
                {
                    pair<int, int> prevRobotPos = robotHistory.top();
                    robotHistory.pop();
                    board[robotRow][robotCol] = '.';
                    robotRow = prevRobotPos.first;
                    robotCol = prevRobotPos.second;
                    board[robotRow][robotCol] = 'R';
                }
            }
            break;
        default:
            cout << "Invalid move! Please enter W, S, A, D, or U." << endl;
            break;
        }

        // Update robot position to chase the player optimally
        char optimalDirection = findOptimalDirection(board, robotRow, robotCol, playerRow, playerCol);
        if (optimalDirection != ' ' && !undoOperation && dist(gen) & 1)
        {
            robotHistory.push({robotRow, robotCol});
            board[robotRow][robotCol] = '.'; // Clear the current position of the robot
            // Move the robot in the optimal direction
            if (optimalDirection == 'W' && robotRow > 0)
                robotRow--; // Move up
            else if (optimalDirection == 'S' && robotRow < BOARD_SIZE - 1)
                robotRow++; // Move down
            else if (optimalDirection == 'A' && robotCol > 0)
                robotCol--; // Move left
            else if (optimalDirection == 'D' && robotCol < BOARD_SIZE - 1)
                robotCol++;                  // Move right
            board[robotRow][robotCol] = 'R'; // 'R' represents the robot
        }

        // Display the updated board
        displayBoard(board, items);

        // Check if the player and robot are in the same cell
        if (playerRow == robotRow && playerCol == robotCol)
        {
            gameOver = true;
            break;
        }

        // Check if the player moved to a cell containing an item
        if (itemExists(items, playerRow, playerCol))
        {
            removeItem(items, playerRow, playerCol); // Remove the item from the linked list
            itemsRemaining--;                        // Decrement the number of items remaining
            if (itemsRemaining == 0)
            {
                gameOver = true;
                break;
            }
        }
    }

    // Print "Game Over" with appropriate color
    if (itemsRemaining == 0)
        cout << "\n\033[1;32mYou won!\033[0m" << endl; // Green color for winning message
    else
        cout << "\n\033[1;31mGame Over\033[0m" << endl; // Red color for losing message

    return 0;
}

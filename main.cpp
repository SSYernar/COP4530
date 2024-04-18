#include <iostream>
#include <cstdlib>
#include <random>
#include <stack>
#include "linked_list.h"
#include "bfs.h"

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

const int BOARD_SIZE = 10;

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

bool gameOver = false;

// Function to display the board
void displayBoard(char board[][BOARD_SIZE], ItemNode *items)
{
    // Clear the screen
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    bool playerExists = false;
    // Display the board
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            char symbol = board[i][j];
            if (symbol == 'P')
            {
                cout << "\033[1;32m" << symbol << "\033[0m "; // Green color for player, reset color after printing
                playerExists = true;
            }
            else if (symbol == 'R')
                cout << "\033[1;31m" << symbol << "\033[0m "; // Red color for robot, reset color after printing
            else if (items != nullptr && itemExists(items, i, j))
                cout << "\033[1;33mI\033[0m "; // Yellow color for item, reset color after printing
            else
                cout << symbol << " ";
        }
        cout << endl;
    }
    if (!playerExists)
    {
        gameOver = true;
    }
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

        // Check if the player and robot are in the same cell
        if (playerRow == robotRow && playerCol == robotCol)
        {
            if (board[playerRow][playerCol] == 'P')
            {
                board[playerRow][playerCol] = 'R';
            }

            gameOver = true;
        }

        // Check if the player moved to a cell containing an item
        if (itemExists(items, playerRow, playerCol))
        {
            removeItem(items, playerRow, playerCol); // Remove the item from the linked list
            itemsRemaining--;                        // Decrement the number of items remaining
            if (itemsRemaining == 0)
            {
                gameOver = true;
            }
        }

        // Display the updated board
        displayBoard(board, items);

        if (gameOver)
        {
            break;
        }
    }

    // Print "Game Over" with appropriate color
    if (itemsRemaining == 0)
        cout << "\n\033[1;32mYou won!\033[0m" << endl; // Green color for winning message
    else
        cout << "\n\033[1;31mGame Over\033[0m" << endl; // Red color for losing message

    return 0;
}

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <thread>
#include <chrono>
using namespace std;

char board[3][3];
char player = 'X';
char ai = 'O';
int playerWins = 0, aiWins = 0, draws = 0;
string difficulty = "hard";

// ------------ Utility Functions ------------
void resetBoard()
{
    char c = '1';
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = c++;
}

void printBoard()
{
    system("cls"); // use "clear" on Linux
    cout << "\n     TIC TAC TOE\n";
    cout << "  Player: " << player << "  |  AI: " << ai << "\n\n";
    for (int i = 0; i < 3; i++)
    {
        cout << "     ";
        for (int j = 0; j < 3; j++)
        {
            cout << board[i][j];
            if (j < 2)
                cout << " | ";
        }
        if (i < 2)
            cout << "\n    -----------\n";
    }
    cout << "\n\n";
}

bool isMovesLeft()
{
    for (auto &row : board)
        for (char cell : row)
            if (cell != 'X' && cell != 'O')
                return true;
    return false;
}

int evaluate()
{
    for (int i = 0; i < 3; i++)
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return (board[i][0] == ai) ? 10 : (board[i][0] == player ? -10 : 0);

    for (int i = 0; i < 3; i++)
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return (board[0][i] == ai) ? 10 : (board[0][i] == player ? -10 : 0);

    if (board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return (board[0][0] == ai) ? 10 : (board[0][0] == player ? -10 : 0);

    if (board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return (board[0][2] == ai) ? 10 : (board[0][2] == player ? -10 : 0);

    return 0;
}

int minimax(bool isMax)
{
    int score = evaluate();
    if (score == 10 || score == -10)
        return score;
    if (!isMovesLeft())
        return 0;

    int best = isMax ? -1000 : 1000;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] != 'X' && board[i][j] != 'O')
            {
                char backup = board[i][j];
                board[i][j] = isMax ? ai : player;
                int val = minimax(!isMax);
                board[i][j] = backup;
                best = isMax ? max(best, val) : min(best, val);
            }
    return best;
}

pair<int, int> bestAIMove()
{
    int bestVal = -1000;
    pair<int, int> bestMove = {-1, -1};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] != 'X' && board[i][j] != 'O')
            {
                char backup = board[i][j];
                board[i][j] = ai;
                int moveVal = minimax(false);
                board[i][j] = backup;
                if (moveVal > bestVal)
                {
                    bestMove = {i, j};
                    bestVal = moveVal;
                }
            }
    return bestMove;
}

pair<int, int> randomAIMove()
{
    int empty[9][2], count = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] != 'X' && board[i][j] != 'O')
            {
                empty[count][0] = i;
                empty[count][1] = j;
                count++;
            }
    if (count == 0)
        return {-1, -1};
    int r = rand() % count;
    return {empty[r][0], empty[r][1]};
}

void aiMove()
{
    cout << "🤖 AI is thinking...\n";
    this_thread::sleep_for(chrono::milliseconds(700));

    pair<int, int> move;
    if (difficulty == "easy")
    {
        move = randomAIMove();
    }
    else if (difficulty == "medium")
    {
        move = (rand() % 2 == 0) ? bestAIMove() : randomAIMove();
    }
    else
    {
        move = bestAIMove();
    }
    board[move.first][move.second] = ai;
}

bool checkWin(char sym)
{
    return evaluate() == (sym == ai ? 10 : -10);
}

void playerMove()
{
    int move;
    while (true)
    {
        cout << "Enter move (1-9): ";
        cin >> move;
        if (cin.fail() || move < 1 || move > 9)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "❌ Invalid input. Try again.\n";
            continue;
        }
        int r = (move - 1) / 3, c = (move - 1) % 3;
        if (board[r][c] != 'X' && board[r][c] != 'O')
        {
            board[r][c] = player;
            break;
        }
        else
        {
            cout << "⚠️ Cell already taken. Try again.\n";
        }
    }
}

void chooseDifficulty()
{
    string input;
    while (true)
    {
        cout << "Select difficulty (easy / medium / hard): ";
        cin >> input;
        if (input == "easy" || input == "medium" || input == "hard")
        {
            difficulty = input;
            break;
        }
        cout << "❌ Invalid choice. Try again.\n";
    }
}

void printScore()
{
    cout << "\n🏆 SCOREBOARD:\n";
    cout << " Player Wins : " << playerWins << "\n";
    cout << " AI Wins     : " << aiWins << "\n";
    cout << " Draws       : " << draws << "\n\n";
}

// ------------ Main Game ------------
int main()
{
    srand(time(0));
    cout << "🎮 Welcome to Tic Tac Toe!\n";
    chooseDifficulty();

    char again;
    do
    {
        resetBoard();
        bool gameOver = false;

        while (!gameOver)
        {
            printBoard();
            playerMove();
            if (checkWin(player))
            {
                printBoard();
                cout << "🎉 You WIN!\n";
                playerWins++;
                gameOver = true;
                break;
            }
            if (!isMovesLeft())
                break;

            aiMove();
            if (checkWin(ai))
            {
                printBoard();
                cout << "💀 AI WINS!\n";
                aiWins++;
                gameOver = true;
                break;
            }
            if (!isMovesLeft())
            {
                printBoard();
                cout << "🤝 It's a DRAW!\n";
                draws++;
                gameOver = true;
                break;
            }
        }

        printScore();
        cout << "Play again? (y/n): ";
        cin >> again;
        while (again != 'y' && again != 'Y' && again != 'n' && again != 'N')
        {
            cout << "❌ Invalid input. Enter y/n: ";
            cin >> again;
        }
    } while (again == 'y' || again == 'Y');

    cout << "\n👋 Thanks for playing!\n";
    return 0;
}

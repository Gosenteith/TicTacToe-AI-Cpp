#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <iomanip>
#include <string>
#include <map>
#include <thread>
#include <chrono>
using namespace std;

char board[3][3];
char player = 'X';
char ai = 'O';
string difficulty = "hard";

struct DifficultyStats
{
    int games = 0;
    int wins = 0;
    int losses = 0;
    int draws = 0;

    double ratio() const
    {
        return losses == 0 ? (wins > 0 ? wins : 0.0) : (double)wins / losses;
    }
};

map<string, DifficultyStats> stats;

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
    cout << "  Player: " << player << "  |  AI: " << ai << "\n";
    cout << "  Difficulty: " << difficulty << "\n\n";
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
    cout << " AI is thinking...\n";
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
            cout << " Invalid input. Try again.\n";
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
            cout << " Cell already taken. Try again.\n";
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
            return;
        }
        cout << " Invalid choice. Try again.\n";
    }
}

void printScoreboard()
{
    cout << "\n🏆 SCOREBOARD\n";
    cout << left << setw(12) << "Difficulty"
         << right << setw(8) << "Games"
         << setw(8) << "Wins"
         << setw(10) << "Losses"
         << setw(8) << "Draws"
         << setw(10) << "W/L Ratio" << "\n";

    cout << string(56, '-') << "\n";

    for (const auto &pair : stats)
    {
        const auto &name = pair.first;
        const auto &s = pair.second;
        cout << left << setw(12) << name
             << right << setw(8) << s.games
             << setw(8) << s.wins
             << setw(10) << s.losses
             << setw(8) << s.draws
             << fixed << setprecision(2)
             << setw(10) << s.ratio() << "\n";
    }

    cout << "\n";
}

// ------------ Main Game ------------
int main()
{
    srand(time(0));
    cout << " Welcome to Tic Tac Toe!\n";
    chooseDifficulty();

    char again;
    do
    {
        resetBoard();
        bool gameOver = false;
        stats[difficulty].games++;

        while (!gameOver)
        {
            printBoard();
            playerMove();
            if (checkWin(player))
            {
                printBoard();
                cout << " You WIN!\n";
                stats[difficulty].wins++;
                gameOver = true;
                break;
            }
            if (!isMovesLeft())
                break;

            aiMove();
            if (checkWin(ai))
            {
                printBoard();
                cout << " AI WINS!\n";
                stats[difficulty].losses++;
                gameOver = true;
                break;
            }
            if (!isMovesLeft())
            {
                printBoard();
                cout << " It's a DRAW!\n";
                stats[difficulty].draws++;
                gameOver = true;
                break;
            }
        }

        printScoreboard();

        cout << "Do you want to change difficulty? (y/n): ";
        char changeDiff;
        cin >> changeDiff;
        if (changeDiff == 'y' || changeDiff == 'Y')
            chooseDifficulty();

        cout << "Play again? (y/n): ";
        cin >> again;
        while (again != 'y' && again != 'Y' && again != 'n' && again != 'N')
        {
            cout << " Invalid input. Enter y/n: ";
            cin >> again;
        }

    } while (again == 'y' || again == 'Y');

    cout << "\n Thanks for playing!\n";
    return 0;
}

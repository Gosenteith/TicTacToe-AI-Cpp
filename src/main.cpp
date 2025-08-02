#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <iomanip>
#include <string>
#include <map>
#include <thread>
#include <chrono>
#include <vector>
using namespace std;

char board[3][3];
string mode = "single"; // or "multi"
string difficulty = "hard";

// --- Player Setup ---
struct Player
{
    string name;
    char symbol;
    string color;
};

Player player1, player2;
map<string, string> colorCodes = {
    {"red", "\033[31m"},
    {"green", "\033[32m"},
    {"yellow", "\033[33m"},
    {"blue", "\033[34m"},
    {"magenta", "\033[35m"},
    {"cyan", "\033[36m"},
    {"reset", "\033[0m"}};

// --- Score Tracking for AI Mode ---
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

// ------------ Utility ------------
void resetBoard()
{
    char c = '1';
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = c++;
}

string colorize(char symbol)
{
    if (symbol == player1.symbol)
        return colorCodes[player1.color] + symbol + colorCodes["reset"];
    if (symbol == player2.symbol)
        return colorCodes[player2.color] + symbol + colorCodes["reset"];
    return string(1, symbol);
}

void printBoard()
{
    system("cls"); // use "clear" on Mac/Linux
    cout << "\n     TIC TAC TOE\n";
    if (mode == "single")
    {
        cout << "  Player: " << player1.name << "  |  AI\n";
        cout << "  Difficulty: " << difficulty << "\n\n";
    }
    else
    {
        cout << "  " << player1.name << " (" << player1.symbol << ") vs "
             << player2.name << " (" << player2.symbol << ")\n\n";
    }

    for (int i = 0; i < 3; i++)
    {
        cout << "     ";
        for (int j = 0; j < 3; j++)
        {
            cout << colorize(board[i][j]);
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
            return (board[i][0] == player2.symbol) ? 10 : (board[i][0] == player1.symbol ? -10 : 0);

    for (int i = 0; i < 3; i++)
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return (board[0][i] == player2.symbol) ? 10 : (board[0][i] == player1.symbol ? -10 : 0);

    if (board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return (board[0][0] == player2.symbol) ? 10 : (board[0][0] == player1.symbol ? -10 : 0);

    if (board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return (board[0][2] == player2.symbol) ? 10 : (board[0][2] == player1.symbol ? -10 : 0);

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
                board[i][j] = isMax ? player2.symbol : player1.symbol;
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
                board[i][j] = player2.symbol;
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
    int r = rand() % count;
    return {empty[r][0], empty[r][1]};
}

void playerMove(Player &p)
{
    int move;
    while (true)
    {
        cout << p.name << " (" << colorCodes[p.color] << p.symbol << colorCodes["reset"] << ") enter move (1-9): ";
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
            board[r][c] = p.symbol;
            break;
        }
        else
        {
            cout << " Cell already taken. Try again.\n";
        }
    }
}

void aiMove()
{
    cout << " AI is thinking...\n";
    this_thread::sleep_for(chrono::milliseconds(700));
    pair<int, int> move =
        (difficulty == "easy") ? randomAIMove() : (difficulty == "medium" && rand() % 2 == 0) ? randomAIMove()
                                                                                              : bestAIMove();
    board[move.first][move.second] = player2.symbol;
}

bool checkWin(char sym)
{
    return evaluate() == (sym == player2.symbol ? 10 : -10);
}

void chooseColor(Player &p, string taken = "")
{
    while (true)
    {
        cout << p.name << ", choose a color (red, green, yellow, blue, magenta, cyan): ";
        cin >> p.color;
        if (colorCodes.count(p.color) && p.color != taken)
            break;
        cout << " Invalid or already taken. Try again.\n";
    }
}

void setupMode()
{
    cout << "Select mode: [1] Single Player  [2] Multiplayer\n> ";
    int input;
    cin >> input;
    mode = (input == 2) ? "multi" : "single";

    if (mode == "multi")
    {
        player1.symbol = 'X';
        player2.symbol = 'O';

        cout << "Enter Player 1 name: ";
        cin >> player1.name;
        chooseColor(player1);

        cout << "Enter Player 2 name: ";
        cin >> player2.name;
        chooseColor(player2, player1.color);
    }
    else
    {
        player1.name = "You";
        player2.name = "AI";
        player1.symbol = 'X';
        player2.symbol = 'O';
        chooseColor(player1);
        vector<string> pool = {"red", "green", "yellow", "blue", "magenta", "cyan"};
        pool.erase(remove(pool.begin(), pool.end(), player1.color), pool.end());
        player2.color = pool[rand() % pool.size()];

        cout << "Select difficulty (easy / medium / hard): ";
        cin >> difficulty;
    }
}

// ------------ Main Game ------------
int main()
{
    srand(time(0));
    cout << " Welcome to Tic Tac Toe!\n";
    setupMode();

    char again;
    do
    {
        resetBoard();
        bool gameOver = false;

        if (mode == "single")
            stats[difficulty].games++;

        while (!gameOver)
        {
            printBoard();
            playerMove(player1);
            printBoard();

            if (checkWin(player1.symbol))
            {
                printBoard();
                cout << " " << player1.name << " WINS!\n";
                if (mode == "single")
                    stats[difficulty].wins++;
                gameOver = true;
                break;
            }

            if (!isMovesLeft())
                break;

            if (mode == "multi")
            {
                playerMove(player2);
                printBoard();
            }
            else
            {
                aiMove();
                printBoard();
            }

            if (checkWin(player2.symbol))
            {
                printBoard();
                cout << " " << player2.name << " WINS!\n";
                if (mode == "single")
                    stats[difficulty].losses++;
                gameOver = true;
                break;
            }

            if (!isMovesLeft())
            {
                printBoard();
                cout << " It's a DRAW!\n";
                if (mode == "single")
                    stats[difficulty].draws++;
                break;
            }
        }

        if (mode == "single")
        {
            cout << "\n SCOREBOARD for " << difficulty << " mode:\n";
            const auto &s = stats[difficulty];
            cout << " Games:  " << s.games << "\n";
            cout << " Wins:   " << s.wins << "\n";
            cout << " Losses: " << s.losses << "\n";
            cout << " Draws:  " << s.draws << "\n";
            cout << " W/L:    " << fixed << setprecision(2) << s.ratio() << "\n\n";
        }

        cout << "Play again? (y/n): ";
        cin >> again;
        while (again != 'y' && again != 'Y' && again != 'n' && again != 'N')
        {
            cout << " Invalid input. Enter y/n: ";
            cin >> again;
        }

        if (again == 'y' || again == 'Y')
        {
            cout << "\nDo you want to change mode or difficulty? (y/n): ";
            char change;
            cin >> change;
            if (change == 'y' || change == 'Y')
            {
                setupMode();
            }
        }

    } while (again == 'y' || again == 'Y');

    cout << "\n Thanks for playing!\n";
    return 0;
}

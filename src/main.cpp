#include <iostream>
#include <limits>
using namespace std;

char board[3][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'}};

char player = 'X';
char ai = 'O';

void printBoard()
{
    system("clear"); // use "cls" on Windows
    cout << "\n";
    for (int i = 0; i < 3; i++)
    {
        cout << " ";
        for (int j = 0; j < 3; j++)
        {
            cout << board[i][j];
            if (j < 2)
                cout << " | ";
        }
        if (i < 2)
            cout << "\n-----------\n";
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
    for (int row = 0; row < 3; row++)
        if (board[row][0] == board[row][1] &&
            board[row][1] == board[row][2])
        {
            return (board[row][0] == ai) ? 10 : -10;
        }

    for (int col = 0; col < 3; col++)
        if (board[0][col] == board[1][col] &&
            board[1][col] == board[2][col])
        {
            return (board[0][col] == ai) ? 10 : -10;
        }

    if (board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return (board[0][0] == ai) ? 10 : -10;

    if (board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return (board[0][2] == ai) ? 10 : -10;

    return 0;
}

int minimax(bool isMax)
{
    int score = evaluate();

    if (score == 10 || score == -10)
        return score;

    if (!isMovesLeft())
        return 0;

    if (isMax)
    {
        int best = -1000;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (board[i][j] != 'X' && board[i][j] != 'O')
                {
                    char backup = board[i][j];
                    board[i][j] = ai;
                    best = max(best, minimax(false));
                    board[i][j] = backup;
                }
        return best;
    }
    else
    {
        int best = 1000;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (board[i][j] != 'X' && board[i][j] != 'O')
                {
                    char backup = board[i][j];
                    board[i][j] = player;
                    best = min(best, minimax(true));
                    board[i][j] = backup;
                }
        return best;
    }
}

pair<int, int> findBestMove()
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

bool checkWin(char symbol)
{
    return evaluate() == (symbol == ai ? 10 : -10);
}

void playerMove()
{
    int move;
    while (true)
    {
        cout << "Enter your move (1-9): ";
        cin >> move;
        if (cin.fail() || move < 1 || move > 9)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        int r = (move - 1) / 3;
        int c = (move - 1) % 3;
        if (board[r][c] != 'X' && board[r][c] != 'O')
        {
            board[r][c] = player;
            break;
        }
    }
}

int main()
{
    while (true)
    {
        printBoard();
        if (!isMovesLeft() || checkWin(ai) || checkWin(player))
            break;

        playerMove();
        printBoard();
        if (checkWin(player))
            break;

        if (!isMovesLeft())
            break;

        cout << "AI is thinking...\n";
        pair<int, int> aiMove = findBestMove();
        board[aiMove.first][aiMove.second] = ai;

        if (checkWin(ai))
            break;
    }

    printBoard();

    if (checkWin(player))
        cout << "You win!\n";
    else if (checkWin(ai))
        cout << "AI wins!\n";
    else
        cout << "Draw!\n";

    return 0;
}

#include <iostream>
#include <vector>
#include <limits>
#include <unordered_map>
#include <bitset>
#include <string>

using namespace std;

/*
UserId=Moinul
Password=Moinul909
*/

const string USER_ID = "Moinul";
const string PASSWORD = "Moinul909";

bool login() {
    string userId, password;
    cout << "Enter User ID: ";
    cin >> userId;
    cout << "Enter Password: ";
    cin >> password;

    if (userId == USER_ID && password == PASSWORD) {
        cout << "Login successful!\n";
        return true;
    } else {
        cout << "Login failed. Incorrect User ID or Password.\n";
        return false;
    }
}

const int BOARD_SIZE = 64;
const int MAX_DEPTH = 5;
typedef uint64_t Bitboard;

enum Player { WHITE, BLACK };

// Structure to represent a chess move
struct Move {
    int from;
    int to;
    int value;
};

// Chessboard class using bitboards
class Chessboard {
public:
    Bitboard whitePieces, blackPieces;
    Bitboard pawns, knights, bishops, rooks, queens, kings;

    Chessboard() {
        // Initialize starting bitboards for pieces
        pawns = 0x00FF00000000FF00ULL;
        knights = 0x4200000000000042ULL;
        bishops = 0x2400000000000024ULL;
        rooks = 0x8100000000000081ULL;
        queens = 0x0800000000000008ULL;
        kings = 0x1000000000000010ULL;

        whitePieces = 0xFFFF000000000000ULL;
        blackPieces = 0x000000000000FFFFULL;
    }

    // Get the piece at a given square
    char getPieceAtSquare(int square) {
        if (pawns & (1ULL << square)) return 'P';
        if (knights & (1ULL << square)) return 'N';
        if (bishops & (1ULL << square)) return 'B';
        if (rooks & (1ULL << square)) return 'R';
        if (queens & (1ULL << square)) return 'Q';
        if (kings & (1ULL << square)) return 'K';
        return '.';
    }

    // Function to visualize the chessboard in the terminal
    void visualizeBoard() {
        cout << "  a b c d e f g h\n";
        cout << " +----------------+\n";
        for (int rank = 7; rank >= 0; --rank) {
            cout << rank + 1 << "| ";
            for (int file = 0; file < 8; ++file) {
                int square = rank * 8 + file;
                char piece = getPieceAtSquare(square);

                if (whitePieces & (1ULL << square)) {
                    cout << piece << " ";  // White piece
                } else if (blackPieces & (1ULL << square)) {
                    cout << (char)(tolower(piece)) << " ";  // Black piece
                } else {
                    cout << ". ";  // Empty square
                }
            }
            cout << "| " << rank + 1 << "\n";
        }
        cout << " +----------------+\n";
        cout << "  a b c d e f g h\n";
    }

    // Evaluate the board state
    int evaluateBoard(Player player) {
        int materialScore = 0;
        materialScore += __builtin_popcountll(pawns & whitePieces) * 100;
        materialScore += __builtin_popcountll(knights & whitePieces) * 320;
        materialScore += __builtin_popcountll(bishops & whitePieces) * 330;
        materialScore += __builtin_popcountll(rooks & whitePieces) * 500;
        materialScore += __builtin_popcountll(queens & whitePieces) * 900;
        materialScore += __builtin_popcountll(kings & whitePieces) * 20000;

        materialScore -= __builtin_popcountll(pawns & blackPieces) * 100;
        materialScore -= __builtin_popcountll(knights & blackPieces) * 320;
        materialScore -= __builtin_popcountll(bishops & blackPieces) * 330;
        materialScore -= __builtin_popcountll(rooks & blackPieces) * 500;
        materialScore -= __builtin_popcountll(queens & blackPieces) * 900;
        materialScore -= __builtin_popcountll(kings & blackPieces) * 20000;

        return (player == WHITE) ? materialScore : -materialScore;
    }

    // Get all legal moves for the current player (simplified)
    vector<Move> getLegalMoves(Player player) {
        vector<Move> moves;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            if ((whitePieces & (1ULL << i)) && player == WHITE) {
                moves.push_back({i, i + 8, 0});
            }
            if ((blackPieces & (1ULL << i)) && player == BLACK) {
                moves.push_back({i, i - 8, 0});
            }
        }
        return moves;
    }

    // Make a move on the board (simplified)
    void makeMove(Move move, Player player) {
        Bitboard fromBit = 1ULL << move.from;
        Bitboard toBit = 1ULL << move.to;

        if (player == WHITE) {
            whitePieces = (whitePieces & ~fromBit) | toBit;
        } else {
            blackPieces = (blackPieces & ~fromBit) | toBit;
        }
    }

    // Undo a move on the board (simplified)
    void undoMove(Move move, Player player) {
        Bitboard fromBit = 1ULL << move.from;
        Bitboard toBit = 1ULL << move.to;

        if (player == WHITE) {
            whitePieces = (whitePieces & ~toBit) | fromBit;
        } else {
            blackPieces = (blackPieces & ~toBit) | fromBit;
        }
    }
};

// Minimax with Alpha-Beta Pruning
int minimax(Chessboard& board, int depth, int alpha, int beta, bool isMaximizingPlayer, Player player) {
    if (depth == 0) {
        return board.evaluateBoard(player);
    }

    vector<Move> legalMoves = board.getLegalMoves(player);

    if (isMaximizingPlayer) {
        int maxEval = numeric_limits<int>::min();
        for (Move move : legalMoves) {
            board.makeMove(move, player);
            int eval = minimax(board, depth - 1, alpha, beta, false, player);
            board.undoMove(move, player);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        return maxEval;
    } else {
        int minEval = numeric_limits<int>::max();
        Player opponent = (player == WHITE) ? BLACK : WHITE;
        for (Move move : legalMoves) {
            board.makeMove(move, opponent);
            int eval = minimax(board, depth - 1, alpha, beta, true, player);
            board.undoMove(move, opponent);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}

// Find the best move for the AI
Move findBestMove(Chessboard& board, Player player) {
    vector<Move> legalMoves = board.getLegalMoves(player);
    Move bestMove;
    int bestValue = numeric_limits<int>::min();

    for (Move move : legalMoves) {
        board.makeMove(move, player);
        int boardValue = minimax(board, MAX_DEPTH, numeric_limits<int>::min(), numeric_limits<int>::max(), false, player);
        board.undoMove(move, player);

        if (boardValue > bestValue) {
            bestMove = move;
            bestValue = boardValue;
        }
    }

    return bestMove;
}

int main() {
    if (!login()) {
        cout << "Access denied!\n";
        return 0;
    }

    Chessboard board;
    Player currentPlayer = WHITE;

    while (true) {
        board.visualizeBoard();  // Visualize the board state before each move

        if (currentPlayer == WHITE) {
            cout << "AI (White) is thinking...\n";
            Move bestMove = findBestMove(board, WHITE);
            board.makeMove(bestMove, WHITE);
            cout << "AI moved from " << bestMove.from << " to " << bestMove.to << "\n";
            currentPlayer = BLACK;
        } else {
            // Placeholder for user input
            cout << "User (Black), enter your move (from to): ";
            int from, to;
            cin >> from >> to;
            board.makeMove({from, to, 0}, BLACK);
            currentPlayer = WHITE;
        }
    }

    return 0;
}


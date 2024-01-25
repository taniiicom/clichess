/* Piece */

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

class Piece {
protected:
    int x, y; // 駒の位置
    bool isWhite; // 駒の色

public:
    explicit Piece(int x, int y, bool isWhite) : x(x), y(y), isWhite(isWhite) {}

    virtual ~Piece() {}

    virtual bool isMoveValid(int destX, int destY) const = 0;
    virtual std::string getType() const = 0;

    int getX() const { return x; }
    int getY() const { return y; }
    bool getIsWhite() const { return isWhite; }

    void setPosition(int newX, int newY) { x = newX; y = newY; }
};

class King final: public Piece {
public:
    explicit King(int x, int y, bool isWhite) : Piece(x, y, isWhite) {}

    bool isMoveValid(int destX, int destY) const override {
        // キングは1マスだけどの方向にも動ける
        return std::max(abs(x - destX), abs(y - destY)) == 1;
    }

    std::string getType() const override { return "King"; }
};

class Queen : public Piece {
public:
    explicit Queen(int x, int y, bool isWhite) : Piece(x, y, isWhite) {}

    bool isMoveValid(int destX, int destY) const override {
        // クイーンはどの方向にも任意のマス動ける
        return x == destX || y == destY || abs(x - destX) == abs(y - destY);
    }

    std::string getType() const override { return "Queen"; }
};

class Bishop : public Piece {
public:
    explicit Bishop(int x, int y, bool isWhite) : Piece(x, y, isWhite) {}

    bool isMoveValid(int destX, int destY) const override {
        // ビショップは斜めに任意のマス動ける
        return abs(x - destX) == abs(y - destY);
    }

    std::string getType() const override { return "Bishop"; }
};

class Knight : public Piece {
public:
    explicit Knight(int x, int y, bool isWhite) : Piece(x, y, isWhite) {}

    bool isMoveValid(int destX, int destY) const override {
        // ナイトはL字型に動ける
        return (abs(x - destX) == 1 && abs(y - destY) == 2) || 
               (abs(x - destX) == 2 && abs(y - destY) == 1);
    }

    std::string getType() const override { return "Knight"; }
};

class Rook : public Piece {
public:
    explicit Rook(int x, int y, bool isWhite) : Piece(x, y, isWhite) {}

    bool isMoveValid(int destX, int destY) const override {
        // ルークは縦横に任意のマス動ける
        return x == destX || y == destY;
    }

    std::string getType() const override { return "Rook"; }
};

class Pawn : public Piece {
public:
    explicit Pawn(int x, int y, bool isWhite) : Piece(x, y, isWhite) {}

    bool isMoveValid(int destX, int destY) const override {
        // ポーンは前に1マス、または初めての動きで2マス動ける
        int direction = isWhite ? 1 : -1;
        return (x == destX && y + direction == destY) || 
               (isFirstMove() && x == destX && y + 2 * direction == destY);
    }

    bool isFirstMove() const {
        return (isWhite && y == 1) || (!isWhite && y == 6);
    }

    std::string getType() const override { return "Pawn"; }
};


/* Board */

#include <memory>
#include <vector>
#include <iostream>

class Board {
    std::unique_ptr<Piece> squares[8][8];

public:
    Board() {
        // ボードの初期化、駒の配置など
        setupPieces();
    }

    // ここで各駒を初期位置に配置します
    void setupPieces() {
        // 白の駒の配置
        for (int i = 0; i < 8; i++) {
            squares[i][1] = std::make_unique<Pawn>(i, 1, true); // 白のポーン
        }
        squares[0][0] = std::make_unique<Rook>(0, 0, true); // 白のルーク
        squares[7][0] = std::make_unique<Rook>(7, 0, true);
        squares[1][0] = std::make_unique<Knight>(1, 0, true); // 白のナイト
        squares[6][0] = std::make_unique<Knight>(6, 0, true);
        squares[2][0] = std::make_unique<Bishop>(2, 0, true); // 白のビショップ
        squares[5][0] = std::make_unique<Bishop>(5, 0, true);
        squares[3][0] = std::make_unique<Queen>(3, 0, true); // 白のクイーン
        squares[4][0] = std::make_unique<King>(4, 0, true); // 白のキング

        // 黒の駒の配置
        for (int i = 0; i < 8; i++) {
            squares[i][6] = std::make_unique<Pawn>(i, 6, false); // 黒のポーン
        }
        squares[0][7] = std::make_unique<Rook>(0, 7, false); // 黒のルーク
        squares[7][7] = std::make_unique<Rook>(7, 7, false);
        squares[1][7] = std::make_unique<Knight>(1, 7, false); // 黒のナイト
        squares[6][7] = std::make_unique<Knight>(6, 7, false);
        squares[2][7] = std::make_unique<Bishop>(2, 7, false); // 黒のビショップ
        squares[5][7] = std::make_unique<Bishop>(5, 7, false);
        squares[3][7] = std::make_unique<Queen>(3, 7, false); // 黒のクイーン
        squares[4][7] = std::make_unique<King>(4, 7, false); // 黒のキング
    }

    bool movePiece(int srcX, int srcY, int destX, int destY) {
        if (!isInBounds(srcX, srcY) || !isInBounds(destX, destY))
            return false;

        auto &piece = squares[srcX][srcY];
        if (!piece || !piece->isMoveValid(destX, destY))
            return false;

        // 他の駒を取る場合の処理（必要に応じて）
        if (squares[destX][destY]) {
            // 取る駒の処理
        }

        // 駒を移動
        squares[destX][destY] = std::move(piece);
        piece = nullptr;
        return true;
    }

    Piece* getPiece(int x, int y) const {
        if (!isInBounds(x, y))
            return nullptr;
        return squares[x][y].get();
    }

    void printBoard() const {
        for (int y = 7; y >= 0; y--) {
            std::cout << (y + 1) << " "; // ボードの縦軸ラベル
            for (int x = 0; x < 8; x++) {
                if (squares[x][y]) {
                    std::string pieceType = squares[x][y]->getType();
                    // std::cout << pieceType[0]; // 駒の最初の文字を使用

                    char pieceTypeInitial = pieceType[0];

                    // 色をつける
                    if (squares[x][y]->getIsWhite()) {
                        std::cout << "\033[35m\033[47m" << pieceTypeInitial << "\033[0m";
                    } else {
                        std::cout << "\033[35m\033[40m" << pieceTypeInitial << "\033[0m";
                    }
                } else {
                    std::cout << "."; // 空のセル
                }
                std::cout << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "  a b c d e f g h" << std::endl; // ボードの横軸ラベル
        std::cout << std::endl;
    }

private:
    bool isInBounds(int x, int y) const {
        return x >= 0 && x < 8 && y >= 0 && y < 8;
    }
};


/* Game */

#include <iostream>
#include <string>

class Game {
    Board board;
    bool isWhiteTurn;
    bool isGameOver;

public:
    Game() : isWhiteTurn(true), isGameOver(false) {
        board.setupPieces();
    }

    void start() {
        while (!isGameOver) {
            board.printBoard();
            processTurn();
            isGameOver = checkGameOver();
            isWhiteTurn = !isWhiteTurn;
        }

        std::cout << "Game over! " << (isWhiteTurn ? "Black" : "White") << " wins!" << std::endl;
    }

private:
    void processTurn() {
        std::string playerColor = isWhiteTurn ? "White" : "Black";
        std::cout << playerColor << "'s turn." << std::endl;

        int srcX, srcY, destX, destY;
        bool validInput;
        do {
            std::cout << "Enter move (e.g., e2 e4): ";
            validInput = getInput(srcX, srcY, destX, destY);

            if (!validInput) {
                std::cout << "Invalid input format. Please use the format: a1 a2" << std::endl;
                continue;
            }

            if (!board.movePiece(srcX, srcY, destX, destY)) {
                std::cout << "Invalid move. Try again." << std::endl;
                validInput = false;
            }
        } while (!validInput);
    }

    bool getInput(int &srcX, int &srcY, int &destX, int &destY) {
        std::string src, dest;
        std::cin >> src >> dest;

        if (src.length() != 2 || dest.length() != 2)
            return false;

        srcX = src[0] - 'a';
        srcY = src[1] - '1';
        destX = dest[0] - 'a';
        destY = dest[1] - '1';

        return srcX >= 0 && srcX < 8 && srcY >= 0 && srcY < 8 &&
            destX >= 0 && destX < 8 && destY >= 0 && destY < 8;
    }

    bool checkGameOver() {
        // [TODO] キングのチェックメイトやステイルメイトを確認する
        return false;
    }
};


/* main */

int main() {
    Game chessGame;
    chessGame.start();
    return 0;
}

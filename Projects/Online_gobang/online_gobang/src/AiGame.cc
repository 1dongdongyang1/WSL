#include "AiGame.h"

#include <chrono>
#include <thread>

AiGame::AiGame(int userId)
    : board_(BOARD_SIZE, std::vector<std::string>(BOARD_SIZE, EMPTY))
    , lastMove_({-1, -1})
    , userId_(userId)
    , movesCount_(0)
    , gameOver_(false)
    , winner_(EMPTY) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

bool AiGame::playerMove(int x, int y) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (gameOver_ || !isValidMove(x, y)) {
        return false;
    }
    board_[x][y] = BLACK;
    lastMove_ = {x, y};
    movesCount_++;
    if (checkWin(x, y, BLACK)) {
        gameOver_ = true;
        winner_ = BLACK;
    }
    return true;
}

void AiGame::aiMove() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (gameOver_) {
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 模拟AI思考时间

    auto [x, y] = findBestMove();
    board_[x][y] = WHITE;
    lastMove_ = {x, y};
    movesCount_++;
    if (checkWin(x, y, WHITE)) {
        gameOver_ = true;
        winner_ = WHITE;
    }
}

bool AiGame::checkWin(int x, int y, const std::string& player) const {
    const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    for (const auto& dir : directions) {
        int count = 1;
        for (int step = 1; step < 5; ++step) {
            int nx = x + step * dir[0];
            int ny = y + step * dir[1];
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board_[nx][ny] == player) {
                count++;
            }
            else {
                break;
            }
        }
        for (int step = 1; step < 5; ++step) {
            int nx = x - step * dir[0];
            int ny = y - step * dir[1];
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board_[nx][ny] == player) {
                count++;
            }
            else {
                break;
            }
        }
        if (count >= 5) {
            return true;
        }
    }
    return false;
}

bool AiGame::isDraw() const {
    return movesCount_ >= BOARD_SIZE * BOARD_SIZE && !gameOver_;
}

bool AiGame::isGameOver() const {
    return gameOver_ || isDraw();
}

std::pair<int, int> AiGame::getLastMove() const {   
    std::lock_guard<std::mutex> lock(mutex_);
    return lastMove_;
}

const std::vector<std::vector<std::string>>& AiGame::getBoard() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return board_;
}

std::string AiGame::getWinner() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return winner_;
}

bool AiGame::isValidMove(int x, int y) const {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board_[x][y] == EMPTY;
}

int AiGame::evaluateThreat(int x, int y) const {
    // 简单的威胁评估函数，可以根据实际需求进行改进
    int score = 0;
    const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    for (const auto& dir : directions) {
        int count = 0;
        for (int step = 1; step < 5; ++step) {
            int nx = x + step * dir[0];
            int ny = y + step * dir[1];
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                if (board_[nx][ny] == WHITE) {
                    count++;
                }
                else if (board_[nx][ny] == BLACK) {
                    count--;
                    break;
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        }
        for (int step = 1; step < 5; ++step) {
            int nx = x - step * dir[0];
            int ny = y - step * dir[1];
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                if (board_[nx][ny] == WHITE) {
                    count++;
                }
                else if (board_[nx][ny] == BLACK) {
                    count--;
                    break;
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        }
        score += count * count; // 威胁值平方，增加连续威胁的权重
    }
    return score;
}

std::pair<int, int> AiGame::findBestMove() const {
    int bestScore = -1;
    std::pair<int, int> bestMove = {-1, -1};
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board_[i][j] == EMPTY && isNearOccupied(i, j)) {
                int score = evaluateThreat(i, j);
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = {i, j};
                }
            }
        }
    }
    // 如果没有找到合适的威胁点，随机选择一个空位
    if (bestMove.first == -1) {
        do {
            bestMove = {std::rand() % BOARD_SIZE, std::rand() % BOARD_SIZE};
        } while (board_[bestMove.first][bestMove.second] != EMPTY);
    }
    return bestMove;
}

bool AiGame::isNearOccupied(int x, int y) const {
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                if (board_[nx][ny] != EMPTY) {
                    return true;
                }
            }
        }
    }
    return false;
}


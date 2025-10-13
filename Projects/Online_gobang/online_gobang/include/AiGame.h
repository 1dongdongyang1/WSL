#pragma once

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <mutex>

const int BOARD_SIZE = 15;

const std::string EMPTY = "empty";
const std::string BLACK = "black";  // player
const std::string WHITE = "white";  // ai

class AiGame {
public:
    AiGame(int userId);

    bool playerMove(int x, int y);
    void aiMove();
    bool checkWin(int x, int y, const std::string& player) const;

    bool isDraw() const;
    bool isGameOver() const;

    std::pair<int, int> getLastMove() const;
    const std::vector<std::vector<std::string>>& getBoard() const;
    std::string getWinner() const;

private:
    bool isValidMove(int x, int y) const;
    int evaluateThreat(int x, int y) const;
    std::pair<int, int> findBestMove() const;
    bool isNearOccupied(int x, int y) const;

private:
    std::vector<std::vector<std::string>>   board_;
    std::pair<int, int>                     lastMove_;
    mutable std::mutex                      mutex_;
    int                                     userId_;
    int                                     movesCount_;
    bool                                    gameOver_;
    std::string                             winner_;
};
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <math.h>
#include <ctime>
#include <sys/time.h>
#include <chrono>
#include <limits.h>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
using namespace std;

class Setup {

    public:
        string player;
        float remainingTime; 
        vector<vector<char> > board;
        int totalWhiteCaptures; 
        int totalBlackCaptures;
        Setup () {}
        Setup (string player, float remainingTime, vector<vector<char> > board, int totalWhiteCaptures, int totalBlackCaptures) {
            this->player = player;
            this->remainingTime = remainingTime;
            this->board = board;
            this->totalBlackCaptures = totalBlackCaptures;
            this->totalWhiteCaptures = totalWhiteCaptures;
        }
};

class Game {
    
    public:

        // Ref: https://www.geeksforgeeks.org/how-to-create-an-unordered_map-of-pairs-in-c/
        struct hash_pair {
            template <class T1, class T2>
            size_t operator()(const pair<T1, T2>& p) const {
                auto hashmap1 = hash<T1>{}(p.first);
                auto hashmap2 = hash<T2>{}(p.second);
                if (hashmap1 != hashmap2) {
                    return hashmap1 ^ hashmap2;             
                }
                return hashmap1;
            }
        };

        Setup setup;
        vector<int> selectedMove;

        // check for the valid move with respect to the board boundary
        bool isValidMove(int x, int y) {
            if (x < 0 || x >= setup.board.size() || y < 0 || y >= setup.board[0].size()) {
                return false;
            }
            return true;
        }

        // double and single open four for any player 
        vector<int> doubleSingleOpenFour(unordered_map<pair<int, int>, int, hash_pair>& whiteMoves, unordered_map<pair<int, int>, int, hash_pair>& blackMoves, string player) {
            int total = 0;
            unordered_map<pair<int, int>, int, hash_pair> moves, player2Moves;
            if (player == "WHITE") {
                moves = whiteMoves;
                player2Moves = blackMoves;
            } else {
                moves = blackMoves;
                player2Moves = whiteMoves;
            }
            int doubleOpenTotal = 0, openTotal = 0, doubleOpenThree = 0, stretchThree = 0;
            vector<vector<int>> dir{{1, 0}, {0, 1}, {1, 1}, {1, -1}};
            for (auto it:moves) {
                for (int i = 0; i < 4; i++) {
                    int x = it.first.first;
                    int y = it.first.second;
                    if (isValidMove(x + dir[i][0], y + dir[i][1]) && moves.find({x + dir[i][0], y + dir[i][1]}) != moves.end()) {

                        if (isValidMove(x + 2*dir[i][0], y + 2*dir[i][1]) && moves.find({x + 2*dir[i][0], y + 2*dir[i][1]}) != moves.end()) {

                            if (isValidMove(x + 3*dir[i][0], y + 3*dir[i][1]) && moves.find({x + 3*dir[i][0], y + 3*dir[i][1]}) != moves.end()) {

                                if (isValidMove(x + 4*dir[i][0], y + 4*dir[i][1]) && isValidMove(x - dir[i][0], y - dir[i][1])
                                    && player2Moves.find({x + 4*dir[i][0], y + 4*dir[i][1]}) == player2Moves.end() && player2Moves.find({x - dir[i][0], y - dir[i][1]}) == player2Moves.end()
                                    && moves.find({x - dir[i][0], y - dir[i][1]}) == moves.end() && moves.find({x + 4*dir[i][0], y + 4*dir[i][1]}) == moves.end())
                                    doubleOpenTotal++;


                                else if ((isValidMove(x + 4*dir[i][0], y + 4*dir[i][1]) && player2Moves.find({x + 4*dir[i][0], y + 4*dir[i][1]}) == player2Moves.end() && moves.find({x + 4*dir[i][0], y + 4*dir[i][1]}) == moves.end())
                                        || (isValidMove(x - dir[i][0], y - dir[i][1]) && player2Moves.find({x - dir[i][0], y - dir[i][1]}) == player2Moves.end() && moves.find({x - dir[i][0], y - dir[i][1]}) == moves.end()))
                                    openTotal++;

                            } else {
                                int cnt = 0; 
                                if (isValidMove(x + 3*dir[i][0], y + 3*dir[i][1]) && moves.find({x + 3*dir[i][0], y + 3*dir[i][1]}) == moves.end() && player2Moves.find({x + 3*dir[i][0], y + 3*dir[i][1]}) == player2Moves.end()
                                    && isValidMove(x - dir[i][0], y - dir[i][1]) && moves.find({x - dir[i][0], y - dir[i][1]}) == moves.end() && player2Moves.find({x - dir[i][0], y - dir[i][1]}) == player2Moves.end()) {
                                    if (player2Moves.find({x + 4*dir[i][0], y + 4*dir[i][1]}) == player2Moves.end() && moves.find({x - dir[i][0], y - dir[i][1]}) == moves.end() || 
                                        player2Moves.find({x - dir[i][0], y - dir[i][1]}) == player2Moves.end() && moves.find({x + 4*dir[i][0], y + 4*dir[i][1]}) == moves.end())
                                        doubleOpenThree++;
                                }
                            }
                        } else {
                            if (isValidMove(x + 2*dir[i][0], y + 2*dir[i][1]) && moves.find({x + 2*dir[i][0], y + 2*dir[i][1]}) == moves.end() && player2Moves.find({x + 2*dir[i][0], y + 2*dir[i][1]}) == player2Moves.end()
                                && isValidMove(x - dir[i][0], y - dir[i][1]) && moves.find({x - dir[i][0], y - dir[i][1]}) == moves.end() && player2Moves.find({x - dir[i][0], y - dir[i][1]}) == player2Moves.end()) {
                                
                                if ((isValidMove(x + 3*dir[i][0], y + 3*dir[i][1]) && moves.find({x + 3*dir[i][0], y + 3*dir[i][1]}) != moves.end() 
                                    && isValidMove(x + 4*dir[i][0], y + 4*dir[i][1]) && moves.find({x + 4*dir[i][0], y + 4*dir[i][1]}) == moves.end() 
                                    && player2Moves.find({x + 4*dir[i][0], y + 4*dir[i][1]}) == player2Moves.end()) ||
                                    (isValidMove(x - 2*dir[i][0], y - 2*dir[i][1]) && moves.find({x - 2*dir[i][0], y - 2*dir[i][1]}) != moves.end()
                                     && isValidMove(x - 3*dir[i][0], y - 3*dir[i][1]) && moves.find({x - 3*dir[i][0], y - 3*dir[i][1]}) == moves.end()
                                     && player2Moves.find({x - 3*dir[i][0], y - 3*dir[i][1]}) == player2Moves.end()))
                                        stretchThree++;
                            }
                        }
                    }
                }
            }
            return {doubleOpenTotal, openTotal, stretchThree + doubleOpenThree};
        }

        // this cost rewards all those moves that blocks opponent's possible patterns like open four, open three etc
        int blockingCost(unordered_map<pair<int, int>, int, hash_pair >& whiteMoves, unordered_map<pair<int, int>, int, hash_pair >& blackMoves, string player, vector<int>& captures, string callFrom) {
            
            // check whose move is it and assign player moves and opponent moves
            unordered_map<pair<int, int>, int, hash_pair > moves, opponentMoves;
            if (player == "BLACK") {
                moves = blackMoves;
                opponentMoves = whiteMoves;
            } else {
                moves = whiteMoves;
                opponentMoves = blackMoves;
            }
            
            vector<vector<int>> dir{{1, 0}, {0, 1}, {1, 1}, {1, -1}};
            int sum = 0, x, y;
            
            // Capturing cost to promote our captures and demotes opponent's captures
            sum = 350*captures[0] - 600*captures[1];
            sum = 250*pow(captures[0], 1.5) - 350*pow(captures[1], 1.5);
            
            // main blocking/hedging logic for heuristic functions
            for (auto it:moves) {
                
                for (int k=0; k<4; ++k) {
                    bool left = false, right = false;
                    int goingLeft = 0, goingRight = 0;
                    x = it.first.first + dir[k][0];
                    y = it.first.second + dir[k][1];
                    while (isValidMove(x, y) && opponentMoves.find({x, y}) != opponentMoves.end()) {
                        goingLeft++;
                        x += dir[k][0];
                        y += dir[k][1];
                    }
                    if (goingLeft > 0 && isValidMove(x, y) && moves.find({x, y}) == moves.end()) {
                        left = true;
                    }
                    x = it.first.first - dir[k][0];
                    y = it.first.second - dir[k][1];
                    while (isValidMove(x, y) && opponentMoves.find({x, y}) != opponentMoves.end()) {
                        goingRight++;
                        x -= dir[k][0];
                        y -= dir[k][1];
                    }
                    if (goingRight > 0 && isValidMove(x, y) && moves.find({x, y}) == moves.end()) {
                        right = true;
                    }
                    if (left || right) {
                        if (goingLeft >= 4 || goingRight >= 4) {
                        sum += 800;
                        } else if (goingLeft + goingRight >= 4) {
                            sum += 600;
                        } else if (goingLeft + goingRight == 3) {
                            sum += 400;
                        } else if (goingLeft + goingRight == 2) {
                            sum += 80;
                        } else if (goingLeft + goingRight == 1) {
                            sum += 0;
                        }
                    }
                }
            }
            return sum;
        }   

        // stretch three and open two
        int stretchThreeAndOpenTwo(unordered_map<pair<int, int>, int, hash_pair >& moves, unordered_map<pair<int, int>, int, hash_pair >& opponentMoves, int goingLeft, int goingRight, int x, int y, int dirX, int dirY, int sumSides) {
            
            int sum = 0;
            // stretch three right side
            if (isValidMove(x + (1 + goingRight)*dirX, y + (1 + goingRight)*dirY) 
                && opponentMoves.find({x + (1 + goingRight)*dirX, y + (1 + goingRight)*dirY}) == opponentMoves.end()
                && moves.find({x + (2 + goingRight)*dirX, y + (2 + goingRight)*dirY}) != moves.end()) {
                
                // left and right should open for stretch three
                if ((isValidMove(x + (3 + goingRight)*dirX, y + (3 + goingRight)*dirY) && opponentMoves.find({x + (3 + goingRight)*dirX, y + (3 + goingRight)*dirY}) == opponentMoves.end()) &&  
                    (isValidMove(x - (1 + goingLeft)*dirX, y - (1 + goingLeft)*dirY) && opponentMoves.find({x - (goingLeft + 1)*dirX, y - (goingLeft + 1)*dirY}) == opponentMoves.end()))
                    sum += 50;
                else if ((isValidMove(x + (3 + goingRight)*dirX, y + (3 + goingRight)*dirY) && opponentMoves.find({x + (3 + goingRight)*dirX, y + (3 + goingRight)*dirY}) == opponentMoves.end()) ||  
                    (isValidMove(x - (1 + goingLeft)*dirX, y - (1 + goingLeft)*dirY) && opponentMoves.find({x - (goingLeft + 1)*dirX, y - (goingLeft + 1)*dirY}) == opponentMoves.end()))
                    sum += 0;
                else {
                    sum += 0;
                }
            } 

            // stretch three left side
            else if (isValidMove(x - (goingLeft + 1)*dirX, y - (1 + goingLeft)*dirY) 
                && opponentMoves.find({x - (1 + goingLeft)*dirX, y - (1 + goingLeft)*dirY}) == opponentMoves.end()
                && moves.find({x - (2 + goingLeft)*dirX, y - (2 + goingLeft)*dirY}) != moves.end()) {
                
                // left and right should open for stretch three
                if ((isValidMove(x - (3 + goingLeft)*dirX, y - (3 + goingLeft)*dirY) && opponentMoves.find({x - (3 + goingLeft)*dirX, y - (3 + goingLeft)*dirY}) == opponentMoves.end()) && 
                    (isValidMove(x + (1 + goingRight)*dirX, y + (1 + goingRight)*dirY) && opponentMoves.find({x + (goingRight + 1)*dirX, y + (goingRight + 1)*dirY}) == opponentMoves.end()))
                    sum += 50;
                else if ((isValidMove(x - (3 + goingLeft)*dirX, y - (3 + goingLeft)*dirY) && opponentMoves.find({x - (3 + goingLeft)*dirX, y - (3 + goingLeft)*dirY}) == opponentMoves.end()) || 
                    (isValidMove(x + (1 + goingRight)*dirX, y + (1 + goingRight)*dirY) && opponentMoves.find({x + (goingRight + 1)*dirX, y + (goingRight + 1)*dirY}) == opponentMoves.end()))
                    sum += 0;
                else {
                    sum += 0;
                }
            }
            // open two 
            else {
                sum += 2*(sumSides*sumSides);
            }
            return sum;
        }

        // stretch four and open three
        int stretchFourAndOpenThree(unordered_map<pair<int, int>, int, hash_pair >& moves, unordered_map<pair<int, int>, int, hash_pair >& opponentMoves, int goingLeft, int goingRight, int x, int y, int dirX, int dirY, int sumSides) {

            int sum = 0;
            // stretch four right side
            if (isValidMove(x + (1 + goingRight)*dirX, y + (1 + goingRight)*dirY) 
                && opponentMoves.find({x + (1 + goingRight)*dirX, y + (1 + goingRight)*dirY}) == opponentMoves.end()
                && moves.find({x + (2 + goingRight)*dirX, y + (2 + goingRight)*dirY}) != moves.end()) {

                // left and right should open for stretch four
                if ((isValidMove(x + (3 + goingRight)*dirX, y + (3 + goingRight)*dirY) && opponentMoves.find({x + (3 + goingRight)*dirX, y + (3 + goingRight)*dirY}) == opponentMoves.end()) &&  
                    (isValidMove(x - (1 + goingLeft)*dirX, y - (1 + goingLeft)*dirY) && opponentMoves.find({x - (goingLeft + 1)*dirX, y - (goingLeft + 1)*dirY}) == opponentMoves.end()))
                    sum += 80;
                else if ((isValidMove(x + (3 + goingRight)*dirX, y + (3 + goingRight)*dirY) && opponentMoves.find({x + (3 + goingRight)*dirX, y + (3 + goingRight)*dirY}) == opponentMoves.end()) ||  
                    (isValidMove(x - (1 + goingLeft)*dirX, y - (1 + goingLeft)*dirY) && opponentMoves.find({x - (goingLeft + 1)*dirX, y - (goingLeft + 1)*dirY}) == opponentMoves.end()))
                    sum += 50;
                else 
                    sum += 20;
            }
            // stretch four left side 
            else if (isValidMove(x - (goingLeft + 1)*dirX, y - (1 + goingLeft)*dirY) 
                && opponentMoves.find({x - (1 + goingLeft)*dirX, y - (1 + goingLeft)*dirY}) == opponentMoves.end()
                && moves.find({x - (2 + goingLeft)*dirX, y - (2 + goingLeft)*dirY}) != moves.end()) {
                
                // left and right should open for stretch four
                if ((isValidMove(x - (3 + goingLeft)*dirX, y - (3 + goingLeft)*dirY) && opponentMoves.find({x - (3 + goingLeft)*dirX, y - (3 + goingLeft)*dirY}) == opponentMoves.end()) &&
                    (isValidMove(x + (1 + goingRight)*dirX, y + (1 + goingRight)*dirY) && opponentMoves.find({x + (goingRight + 1)*dirX, y + (goingRight + 1)*dirY}) == opponentMoves.end()))
                    sum += 80;
                if ((isValidMove(x - (3 + goingLeft)*dirX, y - (3 + goingLeft)*dirY) && opponentMoves.find({x - (3 + goingLeft)*dirX, y - (3 + goingLeft)*dirY}) == opponentMoves.end()) ||  
                    (isValidMove(x + (1 + goingRight)*dirX, y + (1 + goingRight)*dirY) && opponentMoves.find({x + (goingRight + 1)*dirX, y + (goingRight + 1)*dirY}) == opponentMoves.end()))
                    sum += 50;
                else {
                    sum += 20;
                }
            } 
            // open three
            else {
                sum += 10 + 10*sumSides*sumSides;
            }
            return sum;
        }

        // check for four out of five in any 8 direction with one empty space
        bool fourInFive(unordered_map<pair<int, int>, int, hash_pair >& whiteMoves, unordered_map<pair<int, int>, int, hash_pair >& blackMoves, string player) {
            int total = 0;
            unordered_map<pair<int, int>, int, hash_pair > moves, opponentMoves;
            if (player == "WHITE") {
                moves = whiteMoves;
                opponentMoves = blackMoves;
            } else {
                moves = blackMoves;
                opponentMoves = whiteMoves;
            }

            vector<vector<int>> dir{{1, 0}, {0, 1}, {1, 1}, {-1, 1}};        
            for (auto it:moves) {
                for (int i = 0; i < 4; i++) {
                    int left = 0, right = 0, remainingPos = 0, iterator = 0;                    
                    int x = it.first.first + dir[i][0];
                    int y = it.first.second + dir[i][1];
                    while (isValidMove(x, y) && moves.find({x, y}) != moves.end()) {
                        right++;
                        x += dir[i][0];
                        y += dir[i][1];
                    }
                    x = it.first.first - dir[i][0];
                    y = it.first.second- dir[i][1];
                    while (isValidMove(x, y) && moves.find({x, y}) != moves.end()) {
                        left++;
                        x -= dir[i][0];
                        y -= dir[i][1];
                    }
                    int remaining_moves = 5 - (left+right+1);
                    if (remaining_moves < 1) {
                        return true;
                    }
                    x = it.first.first;
                    y = it.first.second;
                    remainingPos = 0;
                    iterator = 0;
                    while(iterator < remaining_moves && isValidMove(x - (left+1+iterator)* dir[i][0], y- (left+1+iterator)*dir[i][1]) 
                        && opponentMoves.find({x - (left+iterator+1)* dir[i][0], y- (left+1+iterator)*dir[i][1] }) == opponentMoves.end()) {
                        if (moves.find({x - (left+1+iterator)* dir[i][0], y- (left+1+iterator)*dir[i][1] }) == moves.end()) {
                            remainingPos++;
                            if (remainingPos == 2) {
                                break;
                            }
                        }
                        iterator++;
                    }
                    if (remainingPos <2 && iterator == remaining_moves) {
                        return true;
                    }
                    remainingPos = 0;
                    iterator = 0;
                    while(iterator < remaining_moves && isValidMove(x + (right+1+iterator)* dir[i][0], y+ (right+1+iterator)*dir[i][1]) 
                        && opponentMoves.find({x + (right+iterator+1)* dir[i][0], y+ (right+1+iterator)*dir[i][1] }) == opponentMoves.end()) {
                        if (moves.find({x + (right+1+iterator)* dir[i][0], y + (right+1+iterator)*dir[i][1] }) == moves.end()) {
                            remainingPos++;
                            if (remainingPos == 2) {
                                break;
                            }
                        }
                        iterator++;
                    }

                    if (remainingPos <2 && iterator == remaining_moves) {
                        return true;
                    }
                }
            }
            return false;
        }
        
        // this cost rewards all those moves that helps us win. open three, open four etc
        int constructionCost(unordered_map<pair<int, int>, int, hash_pair >& whiteMoves, unordered_map<pair<int, int>, int, hash_pair >& blackMoves, string player, vector<int>& captures, string callFrom) {
            
            unordered_map<pair<int, int>, int, hash_pair > moves, opponentMoves;
            if (player == "BLACK") {
                moves = blackMoves;
                opponentMoves = whiteMoves;
            } else {
                moves = whiteMoves;
                opponentMoves = blackMoves;
            }
            
            vector<vector<int>> dir{{1, 0}, {0, 1}, {1, 1}, {1, -1}};
            int x, y, sum = 0;
            for (auto it:moves) {
                
                for (int k=0; k<4; ++k) {
                    bool right = false, left = false;
                    int goingLeft = 0, goingRight = 0;
                    x = it.first.first + dir[k][0];
                    y = it.first.second + dir[k][1];
                    while (isValidMove(x, y) && moves.find({x, y}) != moves.end()) {
                        goingRight++;
                        x += dir[k][0];
                        y += dir[k][1];
                    }
                    if (isValidMove(x, y) && opponentMoves.find({x, y}) == opponentMoves.end()) {
                        right = true;
                    }

                    x = it.first.first - dir[k][0];
                    y = it.first.second - dir[k][1];
                    while (isValidMove(x, y) && moves.find({x, y}) != moves.end()) {
                        goingLeft++;
                        x -= dir[k][0];
                        y -= dir[k][1];
                    }
                    if (isValidMove(x, y) && opponentMoves.find({x, y}) == opponentMoves.end()) {
                        left = true;
                    }

                    if (left || right) {

                        int sumSides = (left && right) ? 2 : 1;
                        // single open four
                        if (goingLeft + goingRight == 3) {
                            sum += 50;
                        } else if (goingLeft + goingRight == 2) {
                            sum += stretchFourAndOpenThree(moves, opponentMoves, goingLeft, goingRight, it.first.first, it.first.second, dir[k][0], dir[k][1], sumSides);
                        } else if (goingLeft + goingRight == 1) {
                            sum += stretchThreeAndOpenTwo(moves, opponentMoves, goingLeft, goingRight, it.first.first, it.first.second, dir[k][0], dir[k][1], sumSides);
                        }
                    } 
                }
            }
            return sum;
        } 

        // Avoid going through all 19*19 locations by finding all nearby location from the existing pieces. 
        vector<vector<int>> calculateRange(vector<int>& inference, unordered_map<pair<int, int>, int, hash_pair >& whiteMoves, unordered_map<pair<int, int>, int, hash_pair >& blackMoves) {
            vector<vector<int>> rangeMoves; 
            for (int i=inference[0]; i<=inference[1]; ++i) {
                for (int j=inference[2]; j<=inference[3]; ++j) {
                    int distance = 0;
                    
                    // if (whiteMoves.find({i, j}) == whiteMoves.end() && blackMoves.find({i, j}) == blackMoves.end()) {
                    for (auto it:whiteMoves) { 
                        distance += abs(it.first.first - i) +  abs(it.first.second - j);
                    }

                    for (auto it2:blackMoves) { 
                        distance += abs(it2.first.first - i) + abs(it2.first.second - j);                        
                    }
                    rangeMoves.push_back({distance, i, j});
                    
                }
            }
            sort(rangeMoves.begin(), rangeMoves.end());
            return rangeMoves;
        }

        // This is the main evaluation function for the game. It will internally calculate hedging and opportunity cost
        int getHeuristicCost(string player, unordered_map<pair<int, int>, int, hash_pair >& whiteMoves, unordered_map<pair<int, int>, int, hash_pair >& blackMoves, vector<int>& captures, string callFrom) {
            
            string opponent;
            if (player == "WHITE") {
                opponent = "BLACK";
            } else {
                opponent = "WHITE";
                swap(captures[0], captures[1]);
            }

            // cost functions for absolute winning moves
            vector<int> maxPlayer  = doubleSingleOpenFour(whiteMoves, blackMoves, player);
            vector<int> minPlayer = doubleSingleOpenFour(whiteMoves, blackMoves, opponent);
            bool check_max = fourInFive(whiteMoves, blackMoves, player);
            bool check_min = fourInFive(whiteMoves, blackMoves, opponent);
            
            // if it is opponent's move and we have double open four
            if (callFrom == "min" && maxPlayer[0] > 0) {
                // We win if opponent has neither open four nor double open four in next move
                if (minPlayer[0] == 0 && minPlayer[1] == 0) {
                    return INT_MAX - 1;
                }
                else {
                    return INT_MIN;
                }
            }

            // if it is our move and opponent has double open four
            if (callFrom == "max" && minPlayer[0] > 0) {
                // We win if we have either open four or double open four in this move
                if (maxPlayer[0] == 0 && maxPlayer[1] == 0) 
                    return INT_MIN;
                else {
                    return INT_MAX - 1;
                }
            }

            // if it is our turn, and we have either open four, double open four or four in a row, we win
            if (callFrom == "max" && (maxPlayer[1] > 0 || maxPlayer[0] > 0 || check_max)) {
                return INT_MAX - 1;
            }

            // if it is opponent's turn, and have either open four, double open four or four in a row, opponent wins
            if (callFrom == "min" && (minPlayer[1] > 0 || minPlayer[0] > 0 || check_min)) {
                return INT_MIN;
            }

            if (callFrom == "max" && maxPlayer[2] > 0) {
                return INT_MAX - 2;
            }

            if (callFrom == "min" && minPlayer[2] > 0) {
                return INT_MIN;
            }

            // cost calculations
            int cost = 0;
            if (player == "BLACK") {
                cost += 5*blockingCost(whiteMoves, blackMoves, player, captures, callFrom);
                cost += 5*constructionCost(whiteMoves, blackMoves, player, captures, callFrom);
                // cost -= 5*constructionCost(whiteMoves, blackMoves, opponent, captures, callFrom);
            } else {
                cost += 2*blockingCost(whiteMoves, blackMoves, player, captures, callFrom);
                cost += 5*constructionCost(whiteMoves, blackMoves, player, captures, callFrom);
                // cost -= 5*constructionCost(whiteMoves, blackMoves, opponent, captures, callFrom);
            }
            return cost;
        }

        // checks for the terminal state at each location i, j
        bool isTerminalState(unordered_map<pair<int, int>, int, hash_pair >& whiteMoves, unordered_map<pair<int, int>, int, hash_pair >& blackMoves, int i, int j, vector<int>& captures, string callFrom) {
            if (captures[0] == 5 || captures[1] == 5) {
                return true;
            }

            unordered_map<pair<int, int>, int, hash_pair > moves;
            if (whiteMoves.find({i, j}) != whiteMoves.end()) {
                moves = whiteMoves;
            } else {
                moves = blackMoves;
            }
            vector<vector< int> > directions = {{-1, 0}, {0, 1}, {1, 1}, {-1, 1}};
            for (int k = 0; k < directions.size(); k++) {
                int x = i + directions[k][0];
                int y = j + directions[k][1];
                int count = 1;
                while (isValidMove(x, y) && moves.find({x, y}) != moves.end()) {
                    count++;
                    x += directions[k][0];
                    y += directions[k][1];
                }
                x = i - directions[k][0];
                y = j - directions[k][1];
                while (isValidMove(x, y) && moves.find({x, y}) != moves.end()) {
                    count++;
                    x -= directions[k][0];
                    y -= directions[k][1];
                }
                
                if (count >= 5) {
                    return true;
                }
            }
            return false;
        }

        // this function checks if move i, j captures any opponent pieces and updates the moves accordingly
        vector<int> buildNextState(unordered_map<pair<int, int>, int, hash_pair >& whiteMoves, unordered_map<pair<int, int>, int, hash_pair >& blackMoves, int i, int j, vector<int> captures) {
            
            string player = whiteMoves.find({i, j}) != whiteMoves.end() ? "WHITE" : "BLACK";
            vector<vector< int> > directions = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
            int totalWhiteCaptures = captures[0], totalBlackCaptures = captures[1];
            unordered_map<pair<int, int>, int, hash_pair > moves, opponentMoves;
            if (player == "WHITE") {
                moves = whiteMoves;
                opponentMoves = blackMoves;
            } else {
                moves = blackMoves;
                opponentMoves = whiteMoves;
            }

            for (int k=0; k<8; ++k) {
                int x = i + directions[k][0];
                int y = j + directions[k][1];
                if (opponentMoves.find({x, y}) != opponentMoves.end()
                    && opponentMoves.find({x + directions[k][0], y + directions[k][1]}) != opponentMoves.end()
                    && moves.find({x + 2*directions[k][0], y + 2*directions[k][1]}) != moves.end()) {

                    opponentMoves.erase({x + directions[k][0], y + directions[k][1]});
                    opponentMoves.erase({x, y});
                    if (player == "WHITE") {
                        totalWhiteCaptures++;
                    } else {
                        totalBlackCaptures++;
                    }
                }
            }

            if (player == "WHITE") {
                whiteMoves = moves;
                blackMoves = opponentMoves;
            } else {
                blackMoves = moves;
                whiteMoves = opponentMoves;
            }
            return vector<int>{totalWhiteCaptures, totalBlackCaptures};
        }

        // max function for the alpha beta pruning algorithm
        int alphaBetaMax(int depth, int currentDepth, int alpha, int beta, vector<vector<int>>& range, string player, unordered_map<pair<int, int>, int, hash_pair > whiteMoves, unordered_map<pair<int, int>, int, hash_pair > blackMoves, vector<int> captures, bool alphaBetaPruning) {

            if (depth == currentDepth) {
                return getHeuristicCost(player, whiteMoves, blackMoves, captures, "max");
            }
            int v = INT_MIN, maxAns = INT_MIN;
            bool isNotAssigned = true;
            for (int x=0; x<(int)range.size(); ++x) {
                int i = range[x][1];
                int j = range[x][2];
                if (isValidMove(i, j) && whiteMoves.find({i, j}) == whiteMoves.end() && blackMoves.find({i, j}) == blackMoves.end()) {
                    unordered_map<pair<int, int>, int, hash_pair > whiteMovesLocal = whiteMoves;
                    unordered_map<pair<int, int>, int, hash_pair > blackMovesLocal = blackMoves;

                    if (player == "WHITE") {
                        whiteMovesLocal[{i, j}] = 1;
                    } else {
                        blackMovesLocal[{i, j}] = 1;
                    }
                        
                    vector<int> captures_2 = buildNextState(whiteMovesLocal, blackMovesLocal, i, j, captures);
                    if (isTerminalState(whiteMovesLocal, blackMovesLocal, i, j, captures_2, "max")) {
                        if (currentDepth == 0) {
                            selectedMove = {i, j};
                        }
                        return INT_MAX;
                    }
                    v = max(v, alphaBetaMin(depth, currentDepth + 1, alpha, beta, range, player, 
                                            whiteMovesLocal, blackMovesLocal, captures_2, alphaBetaPruning));

                    if (currentDepth == 0) {
                        if (v > maxAns || isNotAssigned) {
                            isNotAssigned = false;
                            maxAns = v;
                            selectedMove = {i, j};
                        }
                    }
                    if (player == "WHITE") {
                        whiteMovesLocal.erase({i, j});
                    } else {
                        blackMovesLocal.erase({i, j});
                    }
                    alpha = max(alpha, v);
                    if (beta <= alpha) {
                        return v;
                    }
                }
            }
            return v;
        }

        // min function for the alpha beta pruning algorithm
        int alphaBetaMin(int depth, int currentDepth, int alpha, int beta, vector<vector<int>>& range, string player, unordered_map<pair<int, int>, int, hash_pair > whiteMoves, unordered_map<pair<int, int>, int, hash_pair > blackMoves, vector<int> captures, bool alphaBetaPruning) {
            
            if (depth == currentDepth) {
                return getHeuristicCost(player, whiteMoves, blackMoves, captures, "min");
            }

            int v = INT_MAX;
            for (int x = 0; x<(int)range.size(); ++x) {
                int i = range[x][1];
                int j = range[x][2];
                if (isValidMove(i, j) && whiteMoves.find({i, j}) == whiteMoves.end() && blackMoves.find({i, j}) == blackMoves.end()) {
                    
                    unordered_map<pair<int, int>, int, hash_pair > whiteMovesLocal = whiteMoves;
                    unordered_map<pair<int, int>, int, hash_pair > blackMovesLocal = blackMoves;
                    if (player == "WHITE") {
                        blackMovesLocal[{i, j}] = 1;
                    } else {
                        whiteMovesLocal[{i, j}] = 1;
                    }
                    
                    vector<int> captures_2 = buildNextState(whiteMovesLocal, blackMovesLocal, i, j, captures);
                    if (isTerminalState(whiteMovesLocal, blackMovesLocal, i, j, captures_2, "min")) {
                        return INT_MIN;
                    }
                    v = min(v, alphaBetaMax(depth, currentDepth + 1, alpha, beta, range, player, 
                                            whiteMovesLocal, blackMovesLocal, captures_2, alphaBetaPruning));

                    if (player == "WHITE") {
                        blackMovesLocal.erase({i, j});
                    } else {
                        whiteMovesLocal.erase({i, j});
                    }
                    beta = min(beta, v);
                    if (beta <= alpha) {
                        return v;
                    }
                }
            }
            return v;
        }

        int alphaBetaPruning(string player, int depth, bool alphaBetaPruning) {
            
            vector<vector< char>> board = {
                {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', 'b', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', 'b', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', 'w', '.', '.', 'w', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', 'w', '.', '.', 'w', '.', '.', '.', 'b', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', 'w', '.', 'b', 'w', '.', 'w', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', 'b', 'w', 'b', '.', 'b', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', '.', 'b', 'b', 'b', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', 'w', '.', 'b', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', '.', 'b', 'w', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', 'w', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
                {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'}
            };
            setup = Setup("WHITE", 300.00, board, 0, 0);
            unordered_map<pair<int, int>, int, hash_pair > whiteMoves;
            unordered_map<pair<int, int>, int, hash_pair > blackMoves;
            for (int i = 0; i < 19; i++) {
                for (int j = 0; j < 19; j++) {
                    if (setup.board[i][j] == 'w') {
                        whiteMoves[{i, j}] = 1;
                    } else if (setup.board[i][j] == 'b') {
                        blackMoves[{i, j}] = 1;
                    }
                }
            }
            vector<int> inference = {0, 18, 0, 18};
            vector<vector<int>> range = calculateRange(inference, whiteMoves, blackMoves);
            vector<int> captures{setup.totalWhiteCaptures, setup.totalBlackCaptures};
            int ans = alphaBetaMax(depth, 0, INT_MIN, INT_MAX, range, player, whiteMoves, blackMoves, captures, alphaBetaPruning);
            return ans;
        }
};

int main() {

    Game game;
    auto timeStart = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    game.alphaBetaPruning("WHITE", 2, true);
    float totalTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - timeStart)/1000.00;
    
    // writing in file
    ofstream outputFile;
    outputFile.open("calibrate.txt");
    outputFile << totalTime << endl;
    outputFile.close();
    return 0;
}
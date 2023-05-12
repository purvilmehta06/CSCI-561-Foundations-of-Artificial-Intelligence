#ifndef HOMEWORK_HPP 
#define HOMEWORK_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
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
        Game () {}
        Setup setup;
        vector<int> selectedMove;

        vector<string> splitString(string str, char delimiter);
        void printSetup();
        void readSetupFromFile(string filename);
        vector<int> getInference(map<pair<int, int>, int> &whiteMoves, map<pair<int, int>, int> &blackMoves, int depth);
        bool isValidMove(int x, int y);

        vector<int> doubleSingleOpenFour(map<pair<int, int>, int>& whiteMoves, map<pair<int, int>, int>& blackMoves, string player);
        int hedgingCost(map<pair<int, int>, int>& whiteMoves, map<pair<int, int>, int>& blackMoves, string player, vector<int>& captures, string callFrom);
        int opportunityCost(map<pair<int, int>, int>& whiteMoves, map<pair<int, int>, int>& blackMoves, string player, vector<int>& captures, string callFrom);
        vector<vector<int>> calculateRange(vector<int>& inference, map<pair<int, int>, int>& whiteMoves, map<pair<int, int>, int>& blackMoves);
        int getHeuristicCost(string player, map<pair<int, int>, int>& whiteMoves, map<pair<int, int>, int>& blackMoves, vector<int>& captures, string callFrom);
        int stretchThreeAndOpenTwo(map<pair<int, int>, int>& moves, map<pair<int, int>, int>& opponentMoves, int count1, int count2, int x, int y, int dirX, int dirY, int sumSides);
        int stretchFourAndOpenThree(map<pair<int, int>, int>& moves, map<pair<int, int>, int>& opponentMoves, int count1, int count2, int x, int y, int dirX, int dirY, int sumSides);
        bool fourInFive(map<pair<int, int>, int>& whiteMoves, map<pair<int, int>, int>& blackMoves, string player);

        int getDepth(map<pair<int, int>, int>& whiteMoves, map<pair<int, int>, int>& blackMoves, string player);
        bool isTerminalState(map<pair<int, int>, int>& whiteMoves, map<pair<int, int>, int>& blackMoves, int i, int j, vector<int>& captures, string callFrom);
        vector<int> buildNextState(map<pair<int, int>, int>& whiteMoves, map<pair<int, int>, int>& blackMoves, int i, int j, vector<int> captures);
        int alphaBetaMax(int depth, int currentDepth, int alpha, int beta, vector<vector<int>>& range, string player, map<pair<int, int>, int> whiteMoves, map<pair<int, int>, int> blackMoves, vector<int> captures, string path);
        int alphaBetaMin(int depth, int currentDepth, int alpha, int beta, vector<vector<int>>& range, string player, map<pair<int, int>, int> whiteMoves, map<pair<int, int>, int> blackMoves, vector<int> captures, string path);
        int alphaBetaPruning(string player);
        void selectSecondMove(map<pair<int, int>, int> whiteMoves, map<pair<int, int>, int> blackMoves);

        string mapping(int i, int j);
        vector<int> reverseMapping(string s);
        void writeOutputToFile(string outputFileName);
};

#endif
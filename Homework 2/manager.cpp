#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <ctime>
#include <sys/time.h>
#include <chrono>
#include <limits.h>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
using namespace std;

int blackCaptures = 0, whiteCaptures = 0;

bool isValid(int x, int y) {
    if (x < 0 || x >= 19 || y < 0 || y >= 19) {
        return false;
    }
    return true;
}

string mappingToBoard(int i, int j) {
    if (j > 7) ++j;
    return to_string(19-i) + (char)(j + 'A');
}

vector<int> reverseMappingToBoard(string s) {
    vector<int> ans = {};
    ans.push_back(19 - stoi(s.substr(0, s.size() - 1)));
    ans.push_back(s[s.size() - 1] - 'A');
    if (ans.back() > 8) --ans.back();
    return ans;
}

void checkForCapture(map<pair<int, int>, int>& whiteMoves, map<pair<int, int>, int>& blackMoves, int i, int j, vector<vector<char> > &board) {

    string player = whiteMoves.find({i, j}) != whiteMoves.end() ? "WHITE" : "BLACK";
    vector<vector< int> > directions = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
    map<pair<int, int>, int> moves, opponentMoves;
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
        if (isValid(x, y) && opponentMoves.find({x, y}) != opponentMoves.end()
            && isValid(x + directions[k][0], y + directions[k][1]) 
            && opponentMoves.find({x + directions[k][0], y + directions[k][1]}) != opponentMoves.end()
            && isValid(x + 2*directions[k][0], y + 2*directions[k][1])
            && moves.find({x + 2*directions[k][0], y + 2*directions[k][1]}) != moves.end()) {
            opponentMoves.erase({x + directions[k][0], y + directions[k][1]});
            opponentMoves.erase({x, y});
            board[x][y] = '.';
            board[x + directions[k][0]][y + directions[k][1]] = '.';
            if (player == "WHITE") {
                ++whiteCaptures;
            } else {
                ++blackCaptures;
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
}

bool isGameOver(map<pair<int, int>, int> whiteMoves, map<pair<int, int>, int> blackMoves, int i, int j, vector<int> captures, string callFrom) {
    if (captures[0] == 5 || captures[1] == 5) {
        return true;
    }

    map<pair<int, int>, int> moves;
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
        while (isValid(x, y) && moves.find({x, y}) != moves.end()) {
            count++;
            x += directions[k][0];
            y += directions[k][1];
        }
        x = i - directions[k][0];
        y = j - directions[k][1];
        while (isValid(x, y) && moves.find({x, y}) != moves.end()) {
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

vector<int> nextRandomMove(map<pair<int, int>, int> availableMoves) {
    
    int x = rand() % availableMoves.size();
    auto it = availableMoves.begin();
    for (int i=0; i<x; ++i) {
        ++it;
    }
    vector<int> ans = {it->first.first, it->first.second};
    return ans;
}

void printGameDetails(string player, float remainingTime, int totalWhiteCaptures, int totalBlackCaptures, vector<vector<char> > board) {
    cout << "Player's turn: " << player << endl;
    cout << "Remaining Time: " << remainingTime << endl;
    cout << "Total White Captures: " << totalWhiteCaptures << endl;
    cout << "Total Black Captures: " << totalBlackCaptures << endl;
    // cout << "Board: " << endl;
    // for (int i = 0; i < board.size(); i++) {
    //     for (int j = 0; j < board[i].size(); j++) {
    //         cout << board[i][j] << " ";
    //     }
    //     cout << endl;
    // }
}

int main() {

    srand (time(NULL));
    // player = BLACK if agent is BLACK else WHITE 
    string player = "WHITE";
    bool isRandom = false;
    float remainingTime = 300.00;
    string opponent = "WHITE";

    // basic game setup
    map<pair<int, int>, int> blackMoves, whiteMoves;
    vector<vector<char> > board(19, vector<char>(19, '.'));
    map<pair<int, int>, int> availableMoves; 
    for (int i=0; i<19; ++i) {
        for (int j=0; j<19; ++j) {
            availableMoves[{i, j}] = 1;
        }
    }
    if (player == "WHITE") opponent = "BLACK";

    // game loop
    while(true) {
        
        // ====================== BUILD INPUT ======================
        ofstream buildInputFile;
        buildInputFile.open("input.txt");
        buildInputFile << player + "\n" + to_string(remainingTime) << endl;
        buildInputFile << to_string(whiteCaptures*2) + ',' +  to_string(blackCaptures*2) << endl;

        // if agent is playing black then first move is fixed from manager's end. 
        if (player == "BLACK") {
            board[9][9] = 'w';
            whiteMoves[{9, 9}] = 1;
            availableMoves.erase({9, 9});
            printGameDetails(player, remainingTime, whiteCaptures*2, blackCaptures*2, board);
        } 

        for (int i=0; i<19; ++i) {
            for (int j=0; j<19; ++j) {
                buildInputFile << (char)board[i][j];
            }
            buildInputFile << endl;
        }
        buildInputFile.close();
        // ====================== BUILD INPUT ======================

        // ====================== RUN AGENT ======================
        cout << "-------------------------" << endl;
        auto millisec_since_epoch_1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        system("./run.sh");
        auto millisec_since_epoch_2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        remainingTime -= (millisec_since_epoch_2 - millisec_since_epoch_1) / 1000.0;
        ifstream getOutputFromFile;
        getOutputFromFile.open("output.txt");
        string line;
        getline(getOutputFromFile, line);
        vector<int> move = reverseMappingToBoard(line);
        
        availableMoves.erase({move[0], move[1]});
        if (player == "WHITE") {
            board[move[0]][move[1]] = 'w';
            whiteMoves[{move[0], move[1]}] = 1;
        } else {
            board[move[0]][move[1]] = 'b';
            blackMoves[{move[0], move[1]}] = 1;
        }
        checkForCapture(whiteMoves, blackMoves, move[0], move[1], board);
        printGameDetails(player, remainingTime, whiteCaptures*2, blackCaptures*2, board);
        cout<<player<<" Move: "<<line<<" ("<<move[0]<<" "<<move[1]<<")"<<endl;
        getOutputFromFile.close();
        // ====================== RUN AGENT ======================

        // check for winning state
        if (isGameOver(whiteMoves, blackMoves, move[0], move[1], {whiteCaptures, blackCaptures}, "max")) {
            cout<<"You win!"<<endl;
            return 0;
        }

        // ====================== RUN MANAGER ======================
        if (isRandom) {
            move = nextRandomMove(availableMoves);
            line = mappingToBoard(move[0], move[1]);
        } else {
            cout<<"\nEnter your move: ";
            cin>>line;
            move = reverseMappingToBoard(line);
        }
        availableMoves.erase({move[0], move[1]});
        if (player == "WHITE") {
            board[move[0]][move[1]] = 'b';
            blackMoves[{move[0], move[1]}] = 1;
        } else {
            board[move[0]][move[1]] = 'w';
            whiteMoves[{move[0], move[1]}] = 1;
        }
        checkForCapture(whiteMoves, blackMoves, move[0], move[1], board);
        cout << "-------------------------" << endl;
        printGameDetails(opponent, remainingTime, whiteCaptures*2, blackCaptures*2, board);
        cout<<opponent<<" Move: "<<line<<" ("<<move[0]<<" "<<move[1]<<")"<<endl;
        // ====================== RUN MANAGER ======================

        // check for winning state
        if (isGameOver(whiteMoves, blackMoves, move[0], move[1], {whiteCaptures, blackCaptures}, "max")) {
            cout<<"Game Over"<<endl;
            return 0;
        }
    }
    return 0;
}
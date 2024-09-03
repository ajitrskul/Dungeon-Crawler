#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream myFile(fileName);
    string currLine;
    bool isDoorOrExit = false;
    int count = 0;
    maxRow = 0;
    maxCol = 0;

    if (!myFile.is_open()) {
        return nullptr;
    }


    for (int i = 0; i < 4; i++) {
        myFile >> currLine;
        if (myFile.bad()) {
            return nullptr;
        }
        switch (i) {
            case 0:
                try {
                    maxRow = stoi(currLine);
                }
                catch(...) {
                    return nullptr;
                }
                break;
            case 1:
                try {
                    maxCol = stoi(currLine);
                }
                catch(...) {
                    return nullptr;
                }
                break;
            case 2:
                try {
                    player.row = stoi(currLine);
                }
                catch(...) {
                    return nullptr;
                }
                break;
            case 3:
                try {
                    player.col = stoi(currLine);
                }
                catch(...) {
                    return nullptr;
                }
                break;
        }
    }

    if(maxRow <= 0 || maxCol <= 0) {
        return nullptr;
    }
    if (maxRow > INT32_MAX/maxCol) {
        return nullptr;
    }
    else if ((maxRow > 999999) || (maxCol > 999999)) {
        return nullptr;
    }
    else if (player.row < 0 || player.col < 0 || player.row > maxRow-1 || player.col > maxCol-1) {
        return nullptr;
    }

    char** outputArr = createMap(maxRow, maxCol);

    int col = 0;
    int row = 0;
    while (myFile >> currLine) {
        if (myFile.bad() || myFile.fail()) {
            return nullptr;
        }
        if (currLine.size() > 1) {
            for (unsigned int i = 0; i < currLine.size(); i++) {
                if (currLine.at(i) == TILE_DOOR || currLine.at(i) == TILE_EXIT) {
                    isDoorOrExit = true;
                }
                else if (currLine.at(i) != TILE_AMULET && currLine.at(i) != TILE_MONSTER && currLine.at(i) != TILE_OPEN && currLine.at(i) != TILE_PILLAR && currLine.at(i) != TILE_TREASURE) {
                    return nullptr;
                }
                outputArr[row][col] = currLine.at(i);
                count++;
                if (col < maxCol-1){
                    col++;
                }
                else {
                    col = 0;
                    row++;
                }
            }
        }
        else {
            if (currLine.at(0) == TILE_DOOR || currLine.at(0) == TILE_EXIT) {
                isDoorOrExit = true;
            }
            else if (currLine.at(0) != TILE_AMULET && currLine.at(0) != TILE_MONSTER && currLine.at(0) != TILE_OPEN && currLine.at(0) != TILE_PILLAR && currLine.at(0) != TILE_TREASURE) {
                return nullptr;
            }
            outputArr[row][col] = currLine.at(0);
            count++;
            if (col < maxCol-1){
                col++;
            }
            else {
                col = 0;
                row++;
            }
        }
    }

    if (row != maxRow && col != 0) {
        return nullptr;
    }
    else if (!isDoorOrExit) {
        return nullptr;
    }
    else if (count != maxRow*maxCol) {
        return nullptr;
    }
    else if (!myFile.eof()) {
        return nullptr;
    }
    else {
        outputArr[player.row][player.col] = TILE_PLAYER;
        return outputArr;
    }
    
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    if (input == 'w') {
        nextRow--;
    }
    else if (input == 'a') {
        nextCol--;
    }
    else if (input == 's') {
        nextRow++;
    }
    else if (input == 'd') {
        nextCol++;
    }
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    char** map = new char*[maxRow];
    for (int i = 0; i < maxRow; i++) {
        map[i] = new char[maxCol];
    }

    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            map[i][j] = TILE_OPEN;
        }
    }
    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if (map == nullptr) {
        maxRow = 0;
        return;
    }
    for (int i = 0; i < maxRow; i++) {
        delete[] map[i];
    }
    delete[] map;
    maxRow = 0;
    map = nullptr;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    if (maxRow <= 0 || maxCol <= 0) {
        return nullptr;
    }
    else if (map == nullptr) {
        return nullptr;
    }
    else if (maxRow/2 > INT32_MAX/2 || maxCol/2 > INT32_MAX/2) {
        return nullptr;
    }

    char** newMap = createMap(maxRow*2, maxCol*2);

    if (newMap == nullptr) {
        return nullptr;
    }
    //subArray A
    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            newMap[i][j] = map[i][j];
        }
    }
    //subArray B
    for (int i = 0; i < maxRow; i++) {
        for (int j = maxCol; j < 2*maxCol; j++) {
            newMap[i][j] = map[i][j-maxCol];
            if (newMap[i][j] == TILE_PLAYER) {
                newMap[i][j] = TILE_OPEN;
            }

        }
    }
    //subArray C
    for (int i = maxRow; i < 2*maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            newMap[i][j] = map[i-maxRow][j];
            if (newMap[i][j] == TILE_PLAYER) {
                newMap[i][j] = TILE_OPEN;
            }

        }
    }
    //subArray D
    for (int i = maxRow; i < 2*maxRow; i++) {
        for (int j = maxCol; j < 2*maxCol; j++) {
            newMap[i][j] = map[i-maxRow][j-maxCol];
            if (newMap[i][j] == TILE_PLAYER) {
                newMap[i][j] = TILE_OPEN;
            }
        }
    }

    maxCol = 2*maxCol;
    int temp = 2*maxRow;
    deleteMap(map, maxRow);
    maxRow = temp;
    return newMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    int currRow = player.row;
    int currCol = player.col;
    if (nextRow > (maxRow-1) || nextCol > (maxCol-1) || nextRow < 0 || nextCol < 0) {
        nextRow = currRow;
        nextCol = currCol;
        return STATUS_STAY;
    }
    
    if (map[nextRow][nextCol] == TILE_PILLAR || map[nextRow][nextCol] == TILE_MONSTER) {
        nextRow = currRow;
        nextCol = currCol;
        return STATUS_STAY;
    }
    else if (map[nextRow][nextCol] == TILE_EXIT && player.treasure < 1) {
        nextRow = currRow;
        nextCol = currCol;
        return STATUS_STAY;
    }
    else if (map[nextRow][nextCol] == TILE_EXIT && player.treasure >= 1) {
        map[currRow][currCol] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_ESCAPE;
    }
    else if (map[nextRow][nextCol] == TILE_TREASURE) {
        map[currRow][currCol] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.treasure++;
        return STATUS_TREASURE;
    }
    else if (map[nextRow][nextCol] == TILE_DOOR){
        map[currRow][currCol] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_LEAVE;
    }
    else if (map[nextRow][nextCol] == TILE_AMULET){
        map[currRow][currCol] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_AMULET;
    }
    else {
        map[currRow][currCol] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_MOVE;
    }
    

}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    int currRow = player.row;
    int currCol = player.col;
    bool atkMonster = false;

    //below
    for(int i = currRow+1; i < maxRow; i++) {
        if (map[i][currCol] == TILE_PILLAR) {
            break;
        }
        else if (map[i][currCol] == TILE_MONSTER){
            map[i-1][currCol] = TILE_MONSTER;
            map[i][currCol] = TILE_OPEN;
            if (i-1 == currRow) {
                atkMonster = true;
            }
        }
    }
    //above
    for(int i = currRow-1; i >= 0; i--) {
        if (map[i][currCol] == TILE_PILLAR) {
            break;
        }
        else if (map[i][currCol] == TILE_MONSTER){
            map[i+1][currCol] = TILE_MONSTER;
            map[i][currCol] = TILE_OPEN;
            if (i+1 == currRow) {
                atkMonster = true;
            }
        }
    }
    //left
    for(int i = currCol-1; i >= 0; i--) {
        if (map[currRow][i] == TILE_PILLAR) {
            break;
        }
        else if (map[currRow][i] == TILE_MONSTER){
            map[currRow][i+1] = TILE_MONSTER;
            map[currRow][i] = TILE_OPEN;
            if (i+1 == currCol) {
                atkMonster = true;
            }
        }
    }
    //right
    for(int i = currCol+1; i < maxCol; i++) {
        if (map[currRow][i] == TILE_PILLAR) {
            break;
        }
        else if (map[currRow][i] == TILE_MONSTER){
            map[currRow][i-1] = TILE_MONSTER;
            map[currRow][i] = TILE_OPEN;
            if (i-1 == currCol) {
                atkMonster = true;
            }
        }
    }
    return atkMonster;
}

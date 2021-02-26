#pragma once
#include <vector>
#include <windows.h>

using namespace std;

class Player{
public:
    vector<int> head_position;
    vector<int> segment_positions;
    CHAR_INFO character;
    int head_direction, snake_length;

    Player(int width, int height, int inputCharacter){
        head_position.push_back(width/2);
        head_position.push_back(height/2);
        character.Char.AsciiChar = inputCharacter;
        character.Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        head_direction = rand() % 4;
        snake_length = 2;
    }

    void reset(int width, int height){
        head_position[0] = width/2;
        head_position[1] = height/2;
        head_direction = rand() % 4;
        snake_length = 2;
        segment_positions.clear();
    }
};
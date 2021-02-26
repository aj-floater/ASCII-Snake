#pragma once
#include <vector>
#include <windows.h>



using namespace std;

class Apple{
int randomEven(int range){
    while (true){
        int temp = rand() % range;
        if (temp % 2 == 0){
            return temp;
        }
    }
}
public:
    vector<int> position;
    CHAR_INFO character;

    Apple (int width, int height, int inputCharacter){
        position.push_back(randomEven(width));
        position.push_back(randomEven(height));
        character.Char.AsciiChar = inputCharacter;
        character.Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
    }

    void reset(int width, int height){
        position[0] = randomEven(width);
        position[1] = randomEven(height);
    }
};
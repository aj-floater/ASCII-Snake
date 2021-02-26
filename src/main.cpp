#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <Tchar.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>
#include <conio.h>

#include "asciiMenu.h"
#include "coord.h"
#include "player.h"
#include "apple.h"

#define CURRENT_BRANCH branches[branchPos]
#define MAIN 0
#define OPTIONS 1

using namespace std;

//---------------------------------------------------- Console Variable Declerations ----------------------------------------------------

HANDLE wHnd;    // Handle to write to the console.
HANDLE rHnd;    // Handle to read from the console.

const int u_width = 60;
const int u_height = 30;

bool appIsRunning = true;
DWORD numEvents = 0;
DWORD numEventsRead = 0;
CHAR_INFO consoleBuffer[u_width * u_height];

//---------------------------------------------------- Game Variable Declarations ----------------------------------------------------

std::chrono::steady_clock::time_point fpsStart;
float fpsInterval = 100;
float fastestSpeed = 20;
float slowestSpeed = 150;
bool wrapAround = false;
bool mainMenu = true;
int branchPos = MAIN;
int snakeExtension = 0;

// initialize player:
Player player_1(u_width, u_height, 0xDB);

// initialize apple:
Apple apple(u_width, u_height, 0xDB);

//---------------------------------------------------- Functions ----------------------------------------------------

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

int randomEven(int range){
    while (true){
        int temp = rand() % range;
        if (temp % 2 == 0){
            return temp;
        }
    }
}

void gameOver(){
    mainMenu = true;
    branchPos = MAIN;
    player_1.reset(u_width, u_height);
    apple.reset(u_width, u_height);
    snakeExtension = 0;
    fpsStart = chrono::steady_clock::now();
}

void clearConsoleBuffer(){
    for (int i=0; i < u_width * u_height; ++i) {
        CHAR_INFO character;
        character.Attributes = BACKGROUND_RED | BACKGROUND_INTENSITY - 256;
        character.Char.AsciiChar = ' ';
        consoleBuffer[i] = character;
    }
}

void clearConsoleBuffer(int colour){ // clears the console buffer
    for (int i=0; i < u_width * u_height; ++i) {
        CHAR_INFO character;
        character.Attributes = colour;
        character.Char.AsciiChar = ' ';
        consoleBuffer[i] = character;
    }
}

bool fps(){
    auto fpsEnd = chrono::steady_clock::now();
    if (chrono::duration_cast<chrono::milliseconds>(fpsEnd - fpsStart).count() >= fpsInterval){
        fpsStart = chrono::steady_clock::now();
        return true;
    } else return false;
}

int main() {

    ShowConsoleCursor(false);

    // initialize random seed:
    srand (time(NULL));

    // Set up the handles for reading/writing:
    wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
    rHnd = GetStdHandle(STD_INPUT_HANDLE);

    // name window
    SetConsoleTitle(TEXT("Snake"));

    // Set up the required window size:
    SMALL_RECT windowSize = {0, 0, (u_width - 1), (u_height - 1)};
    // Change the console window size:
    SetConsoleWindowInfo(wHnd, TRUE, &windowSize);

    // Create a COORD to hold the buffer size:
    COORD bufferSize = {u_width, u_height};

    // Change the internal buffer size:
    SetConsoleScreenBufferSize(wHnd, bufferSize);

    clearConsoleBuffer();

    // Set up the positions:
    COORD charBufSize = {u_width, u_height};
    COORD characterPos = {0,0};
    SMALL_RECT writeArea = {0,0,u_width - 1, u_height - 1}; 

    // Write the consoleBuffer to the console:
    WriteConsoleOutputA(wHnd, consoleBuffer, charBufSize, characterPos, &writeArea);

    fpsStart = chrono::steady_clock::now();

    //---------------------------------------------------- Menu Setup ----------------------------------------------------

    Branch main;
    main.newButton("Play Snake");
    main.newButton("Options");
    main.newButton("Quit");
    Branch options;
    options.newButton("Speed", 10 - ((fpsInterval - fastestSpeed) / (slowestSpeed - fastestSpeed) * 10), 10);
    options.newButton("Wrap-around", wrapAround);
    options.newButton("< Back");

    vector<Branch> branches = {main, options};

    //---------------------------------------------------- Render Loop ----------------------------------------------------

    while (appIsRunning) {

        if (_kbhit()){
            char key;
            if (!mainMenu){
                key = _getch();
                switch(key){
                    case 'w' | 'W':{
                        if (player_1.head_direction != 2){
                            player_1.head_direction = 0;
                        } break; 
                    }
                    case 'd':{
                        if (player_1.head_direction != 3){
                            player_1.head_direction = 1;
                        } break; 
                    }
                    case 's':{
                        if (player_1.head_direction != 0){
                            player_1.head_direction = 2;
                        } break; 
                    }
                    case 'a':{
                        if (player_1.head_direction != 1){
                            player_1.head_direction = 3;
                        } break; 
                    }
                    case VK_ESCAPE:{
                        mainMenu = true;
                        branchPos = MAIN;
                        break; 
                    }
                    
                }
            }
            else if (mainMenu){
                key = _getch();
                switch(key){
                    case 'w':{
                        CURRENT_BRANCH.decreaseButtonPos(1); 
                        break; 
                    }
                    case 'd':{
                        if (CURRENT_BRANCH.buttons.at(CURRENT_BRANCH.buttonPosition).title == "Speed"){
                            fpsInterval -= 10;
                            if (fpsInterval < fastestSpeed)
                                fpsInterval = fastestSpeed;
                        }
                        break; 
                    }
                    case 's':{
                        CURRENT_BRANCH.increaseButtonPos(1);
                        break; 
                    }
                    case 'a':{
                        if (CURRENT_BRANCH.buttons.at(CURRENT_BRANCH.buttonPosition).title == "Speed"){
                            fpsInterval += 10;
                            if (fpsInterval > slowestSpeed)
                                fpsInterval = slowestSpeed;
                        }
                        break; 
                    }
                    case VK_SPACE:{
                        if (branchPos == MAIN){
                            if (CURRENT_BRANCH.buttons.at(CURRENT_BRANCH.buttonPosition).title == "Play Snake")
                                mainMenu = false;
                            else if (CURRENT_BRANCH.buttons.at(CURRENT_BRANCH.buttonPosition).title == "Options")
                                branchPos = OPTIONS;
                            else if (CURRENT_BRANCH.buttons.at(CURRENT_BRANCH.buttonPosition).title == "Quit")
                                appIsRunning = false;
                        }
                        else if (branchPos == OPTIONS){
                            if (CURRENT_BRANCH.buttons.at(CURRENT_BRANCH.buttonPosition).title == "Wrap-around")
                                wrapAround = (wrapAround) ? false : true;
                            else if (CURRENT_BRANCH.buttons.at(CURRENT_BRANCH.buttonPosition).title == "< Back")
                                branchPos = MAIN;
                        }
                        break;
                    }
                }
            }
        }

        if (mainMenu){ // if the program is in the main menus, output the menu graphics
            if (fps()){ // ensures the consoleBuffer is updated however many times per second
                coordinate center(u_width/2, u_height/2);
                clearConsoleBuffer();

                //cout << input.records[0].key << endl;

                int y_offset = -CURRENT_BRANCH.buttons.size()/2;
                for (int i = 0; i < CURRENT_BRANCH.buttons.size(); i++){

                    CURRENT_BRANCH.updateButton("Speed", 10 - ((fpsInterval - fastestSpeed) / (slowestSpeed - fastestSpeed) * 10), 10);
                    CURRENT_BRANCH.updateButton("Wrap-around", wrapAround);

                    vector<CHAR_INFO> characters = CURRENT_BRANCH.buttons[i].contents();
                    
                    for (int j = 0; j < characters.size(); j++){
                        int x_offset = -(characters.size())/2;
                        CHAR_INFO character = characters.at(j);
                        consoleBuffer[(center.x+x_offset+j) + u_width * (center.y + y_offset)] = character;
                    }

                    if (i == CURRENT_BRANCH.buttonPosition){
                        CHAR_INFO left_arrow, right_arrow;
                        right_arrow.Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                        left_arrow.Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                        if (CURRENT_BRANCH.buttons[i].buttonKind == SLIDER){
                            left_arrow.Char.AsciiChar = '<';
                            right_arrow.Char.AsciiChar = '>';
                        } else {
                            left_arrow.Char.AsciiChar = '>';
                            right_arrow.Char.AsciiChar = '<';
                        }
                        consoleBuffer[((center.x - (characters.size() - 1)/2) - 4) + u_width * (center.y + y_offset)] = left_arrow;
                        consoleBuffer[((center.x + (characters.size())/2) + 2) + u_width * (center.y + y_offset)] = right_arrow;
                    }

                    y_offset += 2;
                }

                WriteConsoleOutputA(wHnd, consoleBuffer, charBufSize, characterPos, &writeArea);
            }
        }
        
        if (!mainMenu){ // if the program is not in the main menus, play the game
            if (fps()){ // ensures the consoleBuffer is updated however many times per second
                if (player_1.head_direction == 0) { // up
                    player_1.head_position[1] -= 1;
                    if (player_1.head_position[1] < 0){
                        if (wrapAround)
                            player_1.head_position[1] = u_height - 1;
                        else gameOver();
                    }
                }
                if (player_1.head_direction == 1) { // right
                    player_1.head_position[0] += 2;
                    if (player_1.head_position[0] >= u_width){
                        if (wrapAround)
                            player_1.head_position[0] = 2;
                        else gameOver();
                    }
                }
                if (player_1.head_direction == 2) { // down
                    player_1.head_position[1] += 1;
                    if (player_1.head_position[1] >= u_height){
                        if (wrapAround)
                            player_1.head_position[1] = 0;
                        else gameOver();
                    }
                }
                if (player_1.head_direction == 3) { // left
                    player_1.head_position[0] -= 2;
                    if (player_1.head_position[0] < 0){
                        if (wrapAround)
                            player_1.head_position[0] = u_width - 2;
                        else gameOver();
                    }
                }
                if (apple.position[0] == player_1.head_position[0] && apple.position[1] == player_1.head_position[1]){
                    apple.position[0] = randomEven(u_width);
                    apple.position[1] = randomEven(u_height);
                    snakeExtension = player_1.snake_length - 1;
                }
                for (int x : player_1.segment_positions){
                    if (player_1.head_position[0] + u_width * player_1.head_position[1] == x){
                        gameOver();
                    }
                }

                clearConsoleBuffer();
                
                // draw apple

                int offsetposition = apple.position[0] + u_width * apple.position[1];
                consoleBuffer[offsetposition] = apple.character;
                consoleBuffer[offsetposition + 1] = apple.character;

                // draw snake
                
                player_1.segment_positions.push_back(player_1.head_position[0] + u_width * player_1.head_position[1]);
                if (snakeExtension >= player_1.snake_length)
                    player_1.segment_positions.erase(player_1.segment_positions.begin());
                snakeExtension++;

                for (int offsetPosition : player_1.segment_positions){
                    consoleBuffer[offsetPosition] = player_1.character;
                    consoleBuffer[offsetPosition + 1] = player_1.character;
                }

                WriteConsoleOutputA(wHnd, consoleBuffer, charBufSize, characterPos, &writeArea);
            }
        }
    }
}
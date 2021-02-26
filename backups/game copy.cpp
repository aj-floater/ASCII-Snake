#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <Tchar.h>
#include <vector>
#include <iostream>
#include <chrono>

using namespace std;

HANDLE wHnd;    // Handle to write to the console.
HANDLE rHnd;    // Handle to read from the console.

const int u_width = 60;
const int u_height = u_width / 2;

bool appIsRunning = true;
DWORD numEvents = 0;
DWORD numEventsRead = 0;
CHAR_INFO consoleBuffer[u_width * u_height];

auto menuKeyStart = chrono::steady_clock::now();
auto keyStart = chrono::steady_clock::now();
auto fpsStart = chrono::steady_clock::now();
const long menuKeyPressInterval = 150;
const long keyPressInterval = 20;
float fpsInterval = 100;
float fastestSpeed = 20;
float slowestSpeed = 150;
vector<vector<string>> menu = {
    {"Play Snake!", "Options", "Quit"},
    {"Speed", "Wrap-around", "< Back"}
};
bool wrapAround = false;
bool mainMenu = true;
int menuPos = 0;
int menuBranch = 0;

bool keyPress(char key);
void clearConsoleBuffer();
bool menuKeyPressValid();
bool timeKeyPressValid();
bool fps();
int randomEven(int range);
void displayBar(string text, int barLength, int progress, int y_offset, int branch, char colour);
void displayMenu(string text, int y_offset, int branch, char colour);
void displayChoice(string text, int y_offset, int branch);
void gameOver();
void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

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

class Apple{
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

// initialize player:
Player player_1(u_width, u_height, 0xDB);

// initialize apple:
Apple apple(u_width, u_height, 0xDB);

int tick = 0;

int _tmain(int argc, _TCHAR* argv[]) {

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

    keyStart = chrono::steady_clock::now();
    menuKeyStart = chrono::steady_clock::now();
    fpsStart = chrono::steady_clock::now();

    while (appIsRunning) {
        // Find out how many console events have happened:

        GetNumberOfConsoleInputEvents(rHnd, &numEvents);
        if (numEvents != 0) {
            // Create a buffer of that size to store the events
            INPUT_RECORD *eventBuffer = new INPUT_RECORD[numEvents];
            // Read the console events into that buffer, and save how
            // many events have been read into numEventsRead.
            ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
            // Now, cycle through all the events that have happened:
            for (DWORD i = 0; i < numEventsRead; ++i) {
                // Check the event type: was it a key?
                if (eventBuffer[i].EventType == KEY_EVENT) {
                    // Yes! Was the key code the desired key?
                    if (!mainMenu){
                        if ((eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'w' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'W' || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_UP) && player_1.head_direction != 2 && timeKeyPressValid()) {
                            player_1.head_direction = 0;                            
                        }
                        else if ((eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 's' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'S' || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN) && player_1.head_direction != 0 && timeKeyPressValid()) {
                            player_1.head_direction = 2;
                        }
                        else if ((eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'a' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'A' || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT) && player_1.head_direction != 1 && timeKeyPressValid()) {
                            player_1.head_direction = 3;
                        }
                        else if ((eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'd' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'D' || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT) && player_1.head_direction != 3 && timeKeyPressValid()) {
                            player_1.head_direction = 1;
                        }
                        else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
                            mainMenu = true;
                        }
                    } 
                    else{
                        if (menuBranch == 1){
                            if ( (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'w' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'W' || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_UP) && menuKeyPressValid()){
                                menuPos--;
                                if (menuPos < 0){
                                    menuPos = menu[0].size() - 1;
                                }
                            }
                            else if ( (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 's' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'S' || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN) && menuKeyPressValid()){
                                menuPos++;
                                if (menuPos > menu[0].size() - 1){
                                    menuPos = 0;
                                }
                            }
                            else if (menu[menuBranch].at(menuPos) == "Speed"){
                                if ((eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'a' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'A' || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT) && menuKeyPressValid()){
                                    fpsInterval += 10;
                                    if (fpsInterval > slowestSpeed)
                                        fpsInterval = slowestSpeed;
                                }
                                else if ((eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'd' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'D' || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT) && menuKeyPressValid()) {
                                    fpsInterval -= 10;
                                    if (fpsInterval < fastestSpeed)
                                        fpsInterval = fastestSpeed;
                                }
                            }
                        }
                        if (menuBranch == 0){
                            if ( (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'w' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'W' || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_UP) && menuKeyPressValid()){
                                menuPos--;
                                if (menuPos < 0){
                                    menuPos = menu[0].size() - 1;
                                }
                            }
                            else if ( (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 's' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'S' || eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN) && menuKeyPressValid()){
                                menuPos++;
                                if (menuPos > menu[0].size() - 1){
                                    menuPos = 0;
                                }
                            }
                        }
                        if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_SPACE && menuKeyPressValid()){
                            if (menu[menuBranch].at(menuPos) == "Play Snake!"){
                                mainMenu = false;
                            }
                            else if (menu[menuBranch].at(menuPos) == "Quit"){
                                appIsRunning = false;
                            }
                            else if (menu[menuBranch].at(menuPos) == "Options"){
                                menuBranch = 1;
                                menuPos = 0;
                            }
                            else if (menu[menuBranch].at(menuPos) == "< Back"){
                                menuBranch = 0;
                                menuPos = 1;
                            }
                            else if (menu[menuBranch].at(menuPos) == "Wrap-around"){
                                wrapAround = (wrapAround) ? false : true;
                            }
                            else{
                                mainMenu = true;
                            }
                        }
                    }
                }
            }

            // Clean up our event buffer:
            delete[] eventBuffer;
        }

        if (mainMenu){
            if (fps()){

                clearConsoleBuffer();
                
                if (menuBranch == 0){
                    int y_offset = -menu[0].size()/2;
                    for (int i = 0; i < menu[0].size(); i++){
                        displayMenu(menu[0][i], y_offset, 0, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                        y_offset += 2;
                    }

                    y_offset = -menu[0].size()/2 + (menuPos * 2);
                    displayChoice(menu[0][menuPos], y_offset, 0);
                }
                if (menuBranch == 1){
                    int y_offset = -menu[1].size()/2 - 1;

                    float progress = 10 - ((fpsInterval - fastestSpeed) / (slowestSpeed - fastestSpeed) * 10);
                    displayBar(menu[1][0], 10, progress, y_offset, 1, FOREGROUND_GREEN | FOREGROUND_INTENSITY); // outputs the speed of the snake
                    
                    y_offset += 2;
                    
                    progress = (wrapAround) ? 2 : 0;
                    displayBar(menu[1][1], 2, progress, y_offset, 1, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

                    y_offset += 2;
                    displayMenu(menu[1][2], y_offset, menuBranch, FOREGROUND_BLUE | FOREGROUND_INTENSITY);

                    y_offset = -menu[1].size()/2 + ((menuPos * 2) - 1);
                    if (menu[menuBranch].at(menuPos) == "Speed"){
                        displayChoice(menu[1][menuPos] + " |          |", y_offset, 1);
                    }
                    else if (menu[menuBranch].at(menuPos) == "Wrap-around"){
                        displayChoice(menu[1][menuPos] + " | |", y_offset, 1);
                    }
                    else
                        displayChoice(menu[1][menuPos], y_offset, 1);
                }
                
                WriteConsoleOutputA(wHnd, consoleBuffer, charBufSize, characterPos, &writeArea);
            }
        }

        if (!mainMenu){
            if (fps()){

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
                    tick = player_1.snake_length - 1;
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
                if (tick >= player_1.snake_length)
                    player_1.segment_positions.erase(player_1.segment_positions.begin());
                tick++;

                for (int offsetPosition : player_1.segment_positions){
                    consoleBuffer[offsetPosition] = player_1.character;
                    consoleBuffer[offsetPosition + 1] = player_1.character;
                }

                WriteConsoleOutputA(wHnd, consoleBuffer, charBufSize, characterPos, &writeArea);
            }

        }
    }
}

void gameOver(){
    mainMenu = true;
    player_1.reset(u_width, u_height);
    apple.reset(u_width, u_height);
    tick = 0;
    keyStart = chrono::steady_clock::now();
    menuKeyStart = chrono::steady_clock::now();
    fpsStart = chrono::steady_clock::now();
}

void displayBar(string text, int barLength, int progress, int y_offset, int branch, char colour){
    vector<int> center = {u_width/2, u_height/2};
    int lineSize = text.size() + barLength + 2;
    CHAR_INFO character;
    character.Attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

    for (int i = 0; i < lineSize; i++){
        if (i < text.size()){
            character.Char.AsciiChar = text.at(i);
            consoleBuffer[((center.at(0) - lineSize/2) + i) + u_width * (center.at(1) + y_offset)] = character;
        }
        else {
            if (i == text.size() + 1 || i == lineSize - 1){
                character.Char.AsciiChar = '|';
                character.Attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
            else if (i == text.size())
                character.Char.AsciiChar = ' ';
            else if (i > text.size() + 1){
                if (progress > 0){
                    character.Char.AsciiChar = 0xDB;
                    character.Attributes = colour;
                } else {
                    character.Char.AsciiChar = ' ';
                }
                progress--;
            }
            consoleBuffer[((center.at(0) - lineSize/2) + i) + u_width * (center.at(1) + y_offset)] = character;
        }
    }
}

void displayMenu(string text, int y_offset, int branch, char colour){
    vector<int> center = {u_width/2, u_height/2};
    for (int i = 0; i < text.size(); i++){
        int offsetPosition = ((center.at(0) - text.size()/2) + i) + u_width * (center.at(1) + y_offset);
        CHAR_INFO character;
        character.Char.AsciiChar = text.at(i);
        character.Attributes = colour;
        consoleBuffer[offsetPosition] = character;
    }
}

void displayChoice(string text, int y_offset, int branch){
    vector<int> center = {u_width/2, u_height/2};
    int leftArrowOffsetPos = ( ((center.at(0) - text.size()/2) - 2) + u_width * (center.at(1) + y_offset) );
    int rightArrowOffsetPos = ( ((center.at(0) + text.size()/2) + 2) + u_width * (center.at(1) + y_offset) );

    CHAR_INFO character;
    character.Char.AsciiChar = '>';
    character.Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    consoleBuffer[leftArrowOffsetPos] = character;
    character.Char.AsciiChar = '<';
    consoleBuffer[rightArrowOffsetPos] = character;
}

void clearConsoleBuffer(){ // clears the console buffer with blue colour
    for (int i=0; i < u_width * u_height; ++i) {
        // Fill it with blue-backgrounded spaces
        consoleBuffer[i].Char.AsciiChar = ' ';
    }
}

bool keyPress(char key){
    // Find out how many console events have happened:
    GetNumberOfConsoleInputEvents(rHnd, &numEvents);
    bool isKeyPress = false;
    if (numEvents != 0) {
        // Create a buffer of that size to store the events
        INPUT_RECORD *eventBuffer = new INPUT_RECORD[numEvents];
        // Read the console events into that buffer, and save how
        // many events have been read into numEventsRead.
        ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
        // Now, cycle through all the events that have happened:
        for (DWORD i = 0; i < numEventsRead; ++i) {
            // Check the event type: was it a key?
            if (eventBuffer[i].EventType == KEY_EVENT) {
                // Yes! Was the key code the desired key?
                if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == (int)key) {
                    isKeyPress = true;
                }
                else if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == key) {
                    isKeyPress = true;
                }
            }
        }

        // Clean up our event buffer:
        delete[] eventBuffer;
    }
}

int randomEven(int range){
    while (true){
        int temp = rand() % range;
        if (temp % 2 == 0){
            return temp;
        }
    }
}

bool menuKeyPressValid(){
    auto menuKeyEnd = chrono::steady_clock::now();
    if (chrono::duration_cast<chrono::milliseconds>(menuKeyEnd - menuKeyStart).count() >= menuKeyPressInterval){
        menuKeyStart = chrono::steady_clock::now();
        return true;
    } else return false;
}

bool timeKeyPressValid(){
    auto keyEnd = chrono::steady_clock::now();
    if (chrono::duration_cast<chrono::milliseconds>(keyEnd - keyStart).count() >= keyPressInterval){
        keyStart = chrono::steady_clock::now();
        return true;
    } else return false;
}

bool fps(){
    auto fpsEnd = chrono::steady_clock::now();
    if (chrono::duration_cast<chrono::milliseconds>(fpsEnd - fpsStart).count() >= fpsInterval){
        fpsStart = chrono::steady_clock::now();
        return true;
    } else return false;
}
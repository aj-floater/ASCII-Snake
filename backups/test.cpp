#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <Tchar.h>
#include <vector>
#include <iostream>
#include <chrono>
#include "F:\ASCII Game\headers\asciiMenu.h"

using namespace std;

HANDLE wHnd;    // Handle to write to the console.
HANDLE rHnd;    // Handle to read from the console.

const int u_width = 60;
const int u_height = u_width / 2;

bool appIsRunning = true;
DWORD numEvents = 0;
DWORD numEventsRead = 0;
CHAR_INFO consoleBuffer[u_width * u_height];

void clearConsoleBuffer(){ // clears the console buffer with blue colour
    for (int i=0; i < u_width * u_height; ++i) {
        // Fill it with blue-backgrounded spaces
        CHAR_INFO character;
        character.Attributes = BACKGROUND_INTENSITY - 256 | FOREGROUND_INTENSITY - 256;
        character.Char.AsciiChar = ' ';
        consoleBuffer[i] = character;
    }
}

int _tmain(int argc, _TCHAR* argv[]) {
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

    // Set up the positions:
    COORD charBufSize = {u_width, u_height};
    COORD characterPos = {0,0};
    SMALL_RECT writeArea = {0,0,u_width - 1, u_height - 1};

    WriteConsoleOutputA(wHnd, consoleBuffer, charBufSize, characterPos, &writeArea);

    vector<int> center = {u_width/2, u_height/2};

    bool on = true;

    Menu menu;
    menu.newBranch("Main");
    menu.branch("Main").newButton("Test");
    
    int offsetPosition = (30) + u_width * (15);
    consoleBuffer[offsetPosition].Char.AsciiChar = 'A';

    WriteConsoleOutputA(wHnd, consoleBuffer, charBufSize, characterPos, &writeArea);
    system("PAUSE");
}
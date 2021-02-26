#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <Tchar.h>
#include <vector>
#include <iostream>
#include <chrono>

#define NORMAL 0xA
#define BOOL 0xB
#define SLIDER 0xC

using namespace std;

class Button{
    int buttonColour = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    int sliderColour = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    vector<CHAR_INFO> charTitle;
    vector<CHAR_INFO> slider;
public:
    int buttonKind;
    string title;

    vector<CHAR_INFO> contents(){
        vector<CHAR_INFO> tempContents;
        
        for (auto &c : charTitle){
            tempContents.push_back(c);
        }
        for (auto &c : slider){
            tempContents.push_back(c);
        }
        return tempContents;
    }

    void setButtonTitle(string buttonTitle){
        CHAR_INFO character;
        character.Attributes = buttonColour;
        for (char c : buttonTitle){
            character.Char.AsciiChar = c;
            charTitle.push_back(character);
        }
        title = buttonTitle;
    }

    void initializeSlider(int variable, int maximum){
        slider.clear();
        CHAR_INFO character;
        character.Attributes = buttonColour;
        // adds a space between the slider and the title when displayed
        character.Char.AsciiChar = ' ';
        slider.push_back(character);
        // border line
        character.Char.AsciiChar = '|';
        slider.push_back(character);
        // sets character colour to the sliderColour
        character.Attributes = sliderColour;
        for (int i = 0; i < maximum; i++){
            if (variable > 0){
                character.Char.AsciiChar = 0xDB;
                variable--;
            }
            else {
                character.Char.AsciiChar = ' ';
            }
            slider.push_back(character);
        }
        // border line
        character.Attributes = buttonColour;
        character.Char.AsciiChar = '|';
        slider.push_back(character);
    }
};

class Branch{
public:
    vector<Button> buttons;
    vector<Branch> branches;
    int buttonPosition = 0;

    void newButton(string buttonName){ // when you want a normal button
        Button tempButton;
        tempButton.setButtonTitle(buttonName);
        tempButton.buttonKind = NORMAL;

        buttons.push_back(tempButton);
    }
    void newButton(string buttonName, bool buttonVariable){ // when you want to display the value of a variable
        Button tempButton;
        tempButton.setButtonTitle(buttonName);
        tempButton.initializeSlider((buttonVariable) ? 1 : 0, 1);
        tempButton.buttonKind = BOOL;

        buttons.push_back(tempButton);
    }
    void newButton(string buttonName, int buttonVariable, int buttonMaxValue){ // when you want to display the value of a variable in the form of a slider (assumes minimum value is zero)
        Button tempButton;
        tempButton.setButtonTitle(buttonName);
        tempButton.initializeSlider(buttonVariable, buttonMaxValue);
        tempButton.buttonKind = SLIDER;

        buttons.push_back(tempButton);
    }
    void updateButton(string buttonName, bool buttonVariable){
        for (int i = 0; i < buttons.size(); i++){
            if (buttons[i].title == buttonName){
                buttons[i].initializeSlider((buttonVariable) ? 1 : 0, 1);
            }
        }
    }
    void updateButton(string buttonName, int buttonVariable, int buttonMaxValue){
        for (int i = 0; i < buttons.size(); i++){
            if (buttons[i].title == buttonName){
                buttons[i].initializeSlider(buttonVariable, buttonMaxValue);
            }
        }
    }
    int longestButton(){
        int recordHigh = 0; 
        for (Button b : buttons){
            recordHigh = (b.contents().size() > recordHigh) ? b.contents().size() : recordHigh;
        }
        return recordHigh;
    }
    void increaseButtonPos(int change){
        if (buttonPosition + change > buttons.size() - 1)
            buttonPosition = 0;
        else buttonPosition += change;
    }
    void decreaseButtonPos(int change){
        if (buttonPosition - change < 0)
            buttonPosition = buttons.size() - 1;
        else buttonPosition -= change;
    }
};
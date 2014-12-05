//
//  DebugGame.cpp
//  ServerXcode
//
//  Created by James Lennon on 10/31/14.
//  Copyright (c) 2014 James Lennon. All rights reserved.
//

#include "DebugGame.h"

using namespace openpad;

DebugGame::DebugGame(int maxPlayers){
    numPlayers = 0;
    this->maxPlayers = maxPlayers;
}

void DebugGame::onStart(){
    printf("Game started\n");
}

bool DebugGame::canJoin(Client *cli){
    if(cli->clientID->firstname == "Christian"){
        return false;
    }else{
        return true;
    }
}

string DebugGame::whyIsBanned(openpad::Client *cli){
    if(cli->getID().firstname == "Christian"){
        return "Get outta here Christian";
    }
    return "";
}

string DebugGame::getName(){
    return "Debug Game";
}

string DebugGame::getDesc(){
    return "Game program to debug openpad server";
}

string DebugGame::getIconFilePath(){
    return "DebugSmall.png";
}

int DebugGame::getFilledSlots(){
    return numPlayers;
}

int DebugGame::getOpenSlots(){
    return maxPlayers-numPlayers;
}

void DebugGame::onJoin(Client *cli){
    printf("Client joined: %s\n", cli->clientID->username.c_str());
}

PadConfig DebugGame::getDefaultControls(){
    PadConfig conf;
    ButtonControl *btn1 = new ButtonControl(.2, .2, .17, 0, BUTTON_A);
    ButtonControl *btn2 = new ButtonControl(.4, .2, .17, 1, BUTTON_B);
    ButtonControl *btn3 = new ButtonControl(.6, .2, .17, 2, BUTTON_X);
    ButtonControl *btn4 = new ButtonControl(.8, .2, .17, 3, BUTTON_Y);
    DPadControl *dpad = new DPadControl(.5, .5, .2, 1);
    JoystickControl *stick = new JoystickControl(.5, .8, .2, 2);
    
    conf.addControl(btn1);
    conf.addControl(btn2);
    conf.addControl(btn3);
    conf.addControl(btn4);
    conf.addControl(dpad);
    conf.addControl(stick);
    
    return PadConfig::joystickWithButtonsLayout();
}

void DebugGame::onDisconnect(Client *cli){
    printf("%s disconnected\n", cli->clientID->username.c_str());
}

void DebugGame::onPadUpdate(Client *cli, PadUpdateObject update){
    //Player controller action
    printf("Player did something with control %d\n", update.controlid);
    if (update.controlid==0) {
        if (update.action == ACTION_DOWN) {
            printf("user pressed button\n");
        }else{
            printf("user released button\n");
        }
    }else if(update.controlid==1){
        printf("dpad action: %d at coordinates: (%f,%f)\n", update.action, update.x, update.y);
    }
    
}
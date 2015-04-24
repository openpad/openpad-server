//
//  main.cpp
//  HWConsoleServer
//
//  Created by James Lennon on 10/9/14.
//  Copyright (c) 2014 James Lennon. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "openpad.h"
#include <cstdio>
#include <sstream>
#include <thread>
#include <unistd.h>

#include "DebugGame.h"

openpad::Server *serv;

int main(){
    DebugGame game(4);
    serv = new openpad::Server(game);
    
    serv->start();
    sleep(60);
    serv->stop();
    return 0;
}

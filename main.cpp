//
//  main.cpp
//  HWConsoleServer
//
//  Created by James Lennon on 10/9/14.
//  Copyright (c) 2014 James Lennon. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "openpad/openpad.h"
#include <cstdio>
#include <sstream>
#include <thread>
#include <unistd.h>

#include "Simulator.h"
#include "DebugGame.h"

openpad::Server *serv;

void doServer(){
    DebugGame game(4);
    serv = new openpad::Server(game);
    
    serv->start();
}

int main(){
//    thread t(doServer);
//    t.detach();
//    doServer();
    
    DebugGame game(4);
    serv = new openpad::Server(game);
    
    serv->start();
    sleep(5);
    serv->stop();
    sleep(2);
    serv->start();
    sleep(15);
    serv->stop();
//    t.join();
//    simulate();
//    doServer();
    return 0;
}

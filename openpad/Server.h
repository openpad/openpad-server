//
//  HWPPServer.h
//  HWConsoleServer
//
//  Created by James Lennon on 10/9/14.
//  Copyright (c) 2014 James Lennon. All rights reserved.
//

#ifndef Server_H
#define Server_H

#include "PracticalSocket.h"
#include "openpad.h"
#include <mutex>
#include <map>
#include <thread>

namespace openpad {
    
    class Client;
    class Server;
    class Serializable;
    class Request;
    class Response;
    class ServerHandler;
    class IDObject;
    class PadConfig;
    class GameObject;
    class ControlObject;
    
    void sendMsg(TCPSocket* sock, Serializable& r);
    
    class Client {
        
    public:
        int socketID;
        TCPSocket* sock;
        Server* serv;
        void* userData;
        bool shouldRun, hasJoined;
        shared_ptr<IDObject> clientID;
        
        Client(TCPSocket* sock, int sockID, Server* serv);
        void send(Serializable &s);
        void run();
        IDObject& getID();
        void setControls(PadConfig& ctrls);
        void disconnect(string msg);
    private:
        void handleMsg(string msg);
        thread listenThread;
    };
    
    class Server {
        
    public:
        int currentClientID;
        map<int, Client&> clients;
        bool shouldRun;
        ServerHandler& handler;
        
        Server(ServerHandler& handler);
        void start();
        void stop();
        void refreshClients();
        void setControls(PadConfig& ctrls);
        
        Response handleRequest(Request& r, Client* cli);
        void handleResponse(Response& r, Client* cli);
        void removeClient(Client* cli);
    private:
        mutex mut;
        unsigned short port;
        thread advertiseThread, listenThread;
        TCPServerSocket* serverSock;
        UDPSocket* uSock;
        PadConfig* defaultConfig;
        
        void run();
        void advertiseLocation(unsigned short port);
        bool send(Serializable& s, int clientID);
        void broadcast(Serializable &s, int except=-1);
        int getClientID();
        void handleClient(TCPSocket* sock);
        void listenForSockets(TCPServerSocket *serverSock);
        GameObject getGameObject();
    };
    
}
#endif /* defined(__HWConsoleServer__HWPPServer__) */

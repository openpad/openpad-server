//
//  HWServer.c
//  HWConsoleServer
//
//  Created by James Lennon on 10/9/14.
//  Copyright (c) 2014 James Lennon. All rights reserved.
//

#include "Server.h"
#include <thread>
#include <sstream>
#include <stdio.h>
#include <cstdio>
#include <unistd.h>

using namespace openpad;
using namespace Json;

void openpad::sendMsg(TCPSocket* sock, Serializable& r){
    try{
        if(!r.hasSerialized)r.serializeJSON();
        string s = r.getJSONString();
        try {
            sock->send(s.c_str(), s.length()+1);
        } catch (exception ex) {
            printf("%s\n", ex.what());
        }
        if(OP_DEBUG)printf("sent: %s\n", s.c_str());
    }catch(exception ex){
        printf("Error sending message: %s\n", ex.what());
    }
}

Server::Server(ServerHandler& h): handler(h){
    this->handler = h;
    handler.serv = this;
}

void Server::advertiseLocation(unsigned short port){
    uSock = new UDPSocket(9999);
    char msg[2], buf[10];
    msg[0] = port / 256;
    msg[1] = port % 256;
    string from;
    unsigned short fromPort;
    while (shouldRun) {
        try {
            uSock->recvFrom(buf, 10, from, fromPort);
            if (OP_DEBUG)printf("packet from %s\n", from.c_str());
            uSock->sendTo(msg, 2, from, fromPort);
        } catch (exception ex) {
            if (OP_DEBUG) {
                printf("TCP exception: %s\n", ex.what());
            }
            break;
        }
    }
}

void Server::start(){
    currentClientID = 0;
    this->shouldRun = true;
    
    serverSock = nullptr;
    unsigned short currentPort = START_PORT;
    while(serverSock==nullptr){
        try {
            //See if currentPort is in use
            serverSock = new TCPServerSocket("0.0.0.0", currentPort);
            break;
        } catch (SocketException ex) {
            if(OP_DEBUG)printf("Port %d in use\n", currentPort);
            currentPort++;
        }
    }
    if(OP_DEBUG)printf("Successfully connected to port %d\n", currentPort);
    
    //Advertise IP Address
    advertiseThread = thread(&Server::advertiseLocation, this, currentPort);
    handler.onStart();
    
    defaultConfig = new PadConfig(handler.getDefaultControls());
    
    listenThread = thread(&Server::listenForSockets, this, serverSock);
}

void Server::stop(){
    Request disconnectReq(3);
    Value& obj = disconnectReq.serializeJSON();
    obj["msg"] = "Server disconnected";
    broadcast(disconnectReq);
    for (map<int, Client&>::iterator it = clients.begin(); it!=clients.end(); ++it) {
        it->second.shouldRun = false;
    }
    shouldRun = false;
    delete serverSock;
    serverSock = nullptr;
    uSock->disconnect();
    delete uSock;
    uSock = nullptr;
    
    if(listenThread.joinable()){
        if(OP_DEBUG)printf("joining Listen\n");
        listenThread.join();
    }
    if(advertiseThread.joinable()){
        if(OP_DEBUG)printf("joining Advertise\n");
        advertiseThread.join();
    }
}

void Server::listenForSockets(TCPServerSocket *serverSock){
    while (shouldRun) {
        //Accept new client, spawn thread to handle it
        try {
            TCPSocket *sock = serverSock->accept();
            thread t(&Server::handleClient, this, sock);
            t.detach();
        } catch (exception ex) {
            if (OP_DEBUG) {
                printf("TCP exception: %s\n", ex.what());
            }
            break;
        }
    }
}

int Server::getClientID(){
    mut.lock();
    int val = currentClientID++;
    mut.unlock();
    return val;
}

void Server::handleClient(TCPSocket* sock){
    if(OP_DEBUG)printf("New client connected\n");
    Client cli(sock, getClientID(), this);
    clients.insert(std::pair<int,Client&>(cli.socketID, cli));
    cli.run();
}

bool Server::send(Serializable &s, int clientID){
    mut.lock();
    map<int, Client&>::iterator it = clients.find(clientID);
    if(it!=clients.end()){
        it->second.send(s);
        mut.unlock();
        return true;
    }
    mut.unlock();
    return false;
}

void Server::broadcast(Serializable &s, int except){
    mut.lock();
    for(map<int, Client&>::iterator it = clients.begin(); it != clients.end(); ++it){
        if(it->first != except){
            it->second.send(s);
        }
    }
    mut.unlock();
}

void Server::refreshClients(){
    Request req(1);
    Value& obj = req.serializeJSON();
    GameObject g = getGameObject();
    obj["game"] = g.serializeJSON();
    for(map<int, Client&>::iterator it = clients.begin(); it != clients.end(); ++it){
        sendMsg(it->second.sock, req);
    }
}

Response error(const char* text){return Response(404, text);}

GameObject Server::getGameObject(){
    GameObject gobj;
    gobj.name = handler.getName();
    gobj.desc = handler.getDesc();
    gobj.filledslots = handler.getFilledSlots();
    gobj.openslots = handler.getOpenSlots();
    
    return gobj;
}

Response Server::handleRequest(Request &r, Client* cli){
    //handle requests here
    try {
        if(r.operation==0){
            //Discovery Request
            Value& cid = r.JSONvalue["id"];
            cli->clientID = shared_ptr<IDObject>(new IDObject);
            if(!(*cli->clientID).parseJSON(cid))return error("bad id json");
            
            Response resp(200, "OK");
            GameObject gobj;
            gobj.name = handler.getName();
            gobj.desc = handler.getDesc();
            gobj.filledslots = handler.getFilledSlots();
            gobj.openslots = handler.getOpenSlots();
            gobj.icon = fileToBase64(handler.getIconFilePath());
            
            Value& obj = resp.serializeJSON();
            Value& game = gobj.serializeJSON();
            obj["game"] = game;
            
            Value banned;
            string why;
            bool is = !handler.canJoin(cli);
            if(is){
                why = handler.whyIsBanned(cli);
            }
            banned["is"] = is;
            banned["why"] = why;
            obj["banned"] = banned;
            
            return resp;
        }else if(r.operation==2){
            //Join Request
            bool canJoin = cli->clientID!=nullptr && !cli->hasJoined && handler.getOpenSlots()>0 && handler.canJoin(cli);
            
            Response resp(200,"OK");
            Value& obj = resp.serializeJSON();
            obj["accepted"] = canJoin;
            if(canJoin){
                handler.onJoin(cli);
                cli->hasJoined = true;
                obj["padconfig"] = defaultConfig->serializeJSON();
                refreshClients();
            }
            
            return resp;
        }else if(r.operation == 3){
            //Disconnect Request
            Response resp(200,"OK");
            if(cli->hasJoined){
                handler.onDisconnect(cli);
                cli->hasJoined = false;
            }else{
                Value& obj = resp.serializeJSON();
                obj["msg"] = "not in game";
            }
            
            return resp;
        }else if(r.operation == 5){
            //Pad State Change Request
            Response resp(200,"OK");
            if(cli->hasJoined){
                PadUpdateObject update;
                if(update.parseJSON(r.JSONvalue)){
                    handler.onPadUpdate(cli, update);
                }else{
                    resp.statusMsg = "Bad padupdate JSON";
                }
            }else{
                Value& obj = resp.serializeJSON();
                obj["msg"] = "not in game";
            }
            return resp;
        }
        
        return error("unimplemented op");
    }catch(exception ex){
        return error("json missing required field");
    }
}

void Server::handleResponse(Response &r, Client* cli){
    //handle responses here
    
}

void Server::setControls(PadConfig &ctrls){
    for(map<int, Client&>::iterator it = clients.begin(); it != clients.end(); ++it){
        it->second.setControls(ctrls);
    }
    defaultConfig = new PadConfig(ctrls);
}

void Server::removeClient(Client* cli){
    mut.lock();
    if(cli->hasJoined){
        handler.onDisconnect(cli);
        cli->hasJoined = false;
    }
    clients.erase(clients.find(cli->socketID));
    mut.unlock();
}

Client::Client(TCPSocket* sock, int id, Server* serv){
    this->socketID = id;
    this->sock = sock;
    this->serv = serv;
    shouldRun = true;
    hasJoined = false;
    this->clientID = nullptr;
    this->userData = nullptr;
}

void Client::send(Serializable &s){
    sendMsg(sock, s);
}

void Client::handleMsg(string in){
    if(in==""){
        //client disconnected
        return;
    }
    if(OP_DEBUG)printf("received %s\n", in.c_str());
    Request req;
    Response resp;
    if(req.parseJSON(in.c_str())){
        Response resp = serv->handleRequest(req, this);
        sendMsg(sock, resp);
    }else if(resp.parseJSON(in.c_str())){
        serv->handleResponse(resp, this);
    }else{
        Response resp(404,"bad json");
        sendMsg(sock, resp);
    }
}

void Client::run(){
    char buf[BUFFER_LENGTH];
    string msg;
    
    while (shouldRun) {
        int amt = sock->recv(buf, BUFFER_LENGTH-1);
        if(amt<=0)break;
        buf[amt] = 0;
        
        int bytesProcessed = 0;
        do {
            int len = (int)strlen(buf+bytesProcessed);
            msg.append(buf+bytesProcessed, len);
            if(bytesProcessed+len<amt){
                handleMsg(msg);
                msg = "";
            }
            bytesProcessed+=len+1;
        } while (bytesProcessed<amt);
    }
    serv->removeClient(this);
    delete sock;
    sock=NULL;
}

IDObject& Client::getID(){
    return *clientID;
}

void Client::setControls(PadConfig& ctrls){
    Request r(4);
    Value& obj = r.serializeJSON();
    obj["padconfig"] = ctrls.serializeJSON();
    send(r);
}

void Client::disconnect(string msg){
    Request r(3);
    Value& obj = r.serializeJSON();
    obj["msg"] = msg;
    send(r);
    hasJoined = false;
    shouldRun = false;
}

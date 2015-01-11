//
//  Request.cpp
//  OpenPad
//
//  Created by James Lennon on 10/10/14.
//  Copyright (c) 2014 James Lennon. All rights reserved.
//

#include "Request.h"

using namespace Json;
using namespace std;
using namespace openpad;

const char* openpad::getStringFromJSON(Value& v){
    Json::FastWriter w;
    string out = w.write(v);
    
    return out.c_str();
}

string Serializable::getJSONString(){
    Json::FastWriter w;
    string out = w.write(JSONvalue);
    
    return out;
}

Request::Request(){
    operation = -1;
}

Request::Request(int op){
    Request();
    this->operation = op;
}

Value& Request::serializeJSON(){
    hasSerialized = true;
    JSONvalue["op"] = operation;
    JSONvalue["ts"] = (int)time(0);
    
    return JSONvalue;
}

bool Request::parseJSON(const char *json){
    Reader r;
    bool success = r.parse(json, JSONvalue);
    if(!success)return false;
    if(JSONvalue.get("op", "")=="")return false;
    if (JSONvalue.get("ts", "")=="")return false;
    
    operation = JSONvalue["op"].asInt();
    timestamp = JSONvalue["ts"].asInt();
    return true;
}

Response::Response(){
    statusCode = -1;
}

Response::Response(int code, const char* text): statusMsg(text), statusCode(code){
    Response();
}

bool Response::parseJSON(const char *json){
    Reader r;
    bool success = r.parse(json, JSONvalue);
    if(!success)return false;
    
    if(JSONvalue.get("sts", "")=="")return false;
    Value& sts = JSONvalue["sts"];
    
    if(!sts.isMember("code"))return false;
    if(!sts.isMember("msg"))return false;
    statusCode = sts["code"].asInt();
    statusMsg = sts["msg"].asString();
    return true;
}

Value& Response::serializeJSON(){
    hasSerialized = true;
    Value sts;
    
    sts["code"] = statusCode;
    sts["msg"] = statusMsg;
    
    JSONvalue["sts"] = sts;
    return JSONvalue;
}

IDObject::IDObject(): phoneid(""), firstname(""), lastname(""), username(""),fbuid("") {}

Value& IDObject::serializeJSON(){
    hasSerialized = true;
    
    JSONvalue["phoneID"] = phoneid;
    JSONvalue["firstname"] = firstname;
    JSONvalue["lastname"] = lastname;
    JSONvalue["username"] = username;
    JSONvalue["fbuid"] = fbuid;
    
    return JSONvalue;
}

bool IDObject::parseJSON(Value& v){
    if(!v.isMember("phoneID"))return false;
    if(!v.isMember("firstname"))return false;
    if(!v.isMember("lastname"))return false;
    if(!v.isMember("username"))return false;
    if(!v.isMember("fbuid"))return false;
    
    phoneid = v["phoneID"].asString();
    firstname = v["firstname"].asString();
    lastname = v["lastname"].asString();
    fbuid = v["fbuid"].asString();
    username = v["username"].asString();
    return true;
}

GameObject::GameObject():openslots(0),filledslots(0){}

Value& GameObject::serializeJSON(){
    hasSerialized = true;
    
    JSONvalue["name"] = name;
    JSONvalue["openslots"] = openslots;
    JSONvalue["filledslots"] = filledslots;
    JSONvalue["icon"] = icon;
    JSONvalue["desc"] = desc;
    
    return JSONvalue;
}

bool GameObject::parseJSON(Value &v){
    try {
        name = v["name"].asString();
        openslots = v["openslots"].asInt();
        filledslots = v["filledslots"].asInt();
        icon = v["icon"].asString();
        desc = v["desc"].asString();
        
        return true;
    } catch (exception e) {
        return false;
    }
}

bool FrameObject::parseJSON(Value &v){
    try {
        x = v["x"].asDouble();
        y = v["y"].asDouble();
        w = v["w"].asDouble();
        h = v["h"].asDouble();
        return true;
    } catch (exception ex) {
        return false;
    }
}

Value& FrameObject::serializeJSON(){
    
    JSONvalue["x"] = x;
    JSONvalue["y"] = y;
    JSONvalue["w"] = w;
    JSONvalue["h"] = h;
    
    return JSONvalue;
}

void FrameObject::set(float x, float y, float w, float h){
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

bool ControlObject::parseJSON(Value &v){
    try {
        type = v["type"].asInt();
        controlID = v["type"].asInt();
        img = v["img"].asString();
        if(!frame.parseJSON(v["frame"]))return false;
        return true;
    } catch (exception ex) {
        return false;
    }
}

Value& ControlObject::serializeJSON(){

    JSONvalue["type"] = type;
    
    Value& fr = frame.serializeJSON();
    JSONvalue["frame"] = fr;
    JSONvalue["id"] = controlID;
    if(img.length()>0){
        JSONvalue["img"] = img;
    }
    
    return JSONvalue;
}

PadConfig::PadConfig(){
}

PadConfig::PadConfig(const PadConfig& other){
    bgimg = other.bgimg;
    controls = other.controls;
}

PadConfig& PadConfig::operator=(const openpad::PadConfig &other){
    bgimg = other.bgimg;
    controls = other.controls;
    return *this;
}

Value& PadConfig::serializeJSON(){
    
    JSONvalue["bgimg"] = bgimg;
    
    Value ctrls;
    for(int i=0; i<controls.size(); i++){
        ctrls.append(controls[i]->serializeJSON());
    }
    JSONvalue["controls"] = ctrls;
    
    return JSONvalue;
}

bool PadConfig::parseJSON(Value &v){
    return true;
}

void PadConfig::addControl(openpad::ControlObject *c){
    controls.push_back(c);
}

PadConfig PadConfig::joystickWithButtonsLayout(){
    PadConfig conf;
    
    JoystickControl *stick = new JoystickControl(.22,.5,.6,0);
    conf.addControl(stick);
    ButtonControl *abtn = new ButtonControl(.65,.6,.26,1,BUTTON_A), *bbtn = new ButtonControl(.8,0.4,.26,2,BUTTON_B);
    conf.addControl(abtn);
    conf.addControl(bbtn);
    
    return conf;
}

bool PadUpdateObject::parseJSON(Value &v){
    try {
        action = v["action"].asInt();
        controlid = v["controlid"].asInt();
        Value& pos = v["position"];
        x = pos["x"].asDouble();
        y = pos["y"].asDouble();
        
        return true;
    } catch (exception ex) {
        return false;
    }
}

ButtonControl::ButtonControl(float x, float y, float w, int _controlid, int _btntype){
    this->type = BUTTON;
    frame.set(x, y, w, w);
    controlID = _controlid;
    btntype = _btntype;
}

Value& ButtonControl::serializeJSON(){
    ControlObject::serializeJSON();
    JSONvalue["btntype"] = btntype;
    
    return JSONvalue;
}

DPadControl::DPadControl(float x, float y, float w, int _controlid){
    this->type = DPAD;
    frame.set(x, y, w, w);
    controlID = _controlid;
}

JoystickControl::JoystickControl(float x, float y, float w, int _controlid){
    this->type = JOYSTICK;
    frame.set(x, y, w, w);
    controlID = _controlid;
}

//
//  networkingModule.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 12/01/2021.
//

#include <iostream>
#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <thread>
#include "networkingModule.hpp"
#include "blockEngine.hpp"
#include "gameLoop.hpp"
#include "otherPlayers.hpp"
#include "itemEngine.hpp"
#include "singleWindowLibrary.hpp"
#include "playerHandler.hpp"
#include "inventory.hpp"

#define BUFFER_SIZE 1024
#define PORT 33770
#define PORT_STR "33770"

int sock;
ogl::texture connecting_text;

packets::packet networking::getPacket() {
    return packets::getPacket(sock);
}

struct listener {
    networking::listenerFunction function;
    packets::packetType type;
};

std::vector<listener>& getListeners() {
    static std::vector<listener> listeners;
    return listeners;
}

// packet listener is a function that gets executed every time a certain type of packet is received

networking::registerListener::registerListener(listenerFunction function, packets::packetType type) {
    listener new_listener{};
    new_listener.function = function;
    new_listener.type = type;
    getListeners().push_back(new_listener);
}


void networking::sendPacket(packets::packet packet_) {
    packets::sendPacket(sock, std::move(packet_));
}

void networking::init() {
    connecting_text.loadFromText("Connecting to server", {255, 255, 255});
    connecting_text.scale = 3;
}

bool networking::establishConnection(const std::string &ip) {
    // establish connection to server, made so it works on windows and macos
    swl::setDrawColor(0, 0, 0);
        swl::clear();
        connecting_text.render();
        swl::update();

    #ifdef WIN32
        WSADATA wsaData;
        if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
            return false;
    #endif

        sockaddr_in serv_addr{};
        if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            return false;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

    #ifdef WIN32
        struct addrinfo *result = nullptr, hints{};
        if(getaddrinfo(ip.c_str(), (const char *)PORT_STR, &hints, &result) != 0)
            return false;

        if(connect(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
            return false;
    #else
        if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
            return false;

        if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            return false;
    #endif
    
    packets::packet join_packet(packets::PLAYER_JOIN);
    join_packet << playerHandler::position_x << playerHandler::position_y;
    sendPacket(join_packet);
    
    return true;
}

void listenerLoop() {
    while(gameLoop::running) {
        packets::packet packet = networking::getPacket();
        for(listener& i : getListeners())
            if(i.type == packet.type)
                i.function(packet);
    }
}

void networking::spawnListener() {
    std::thread listener(listenerLoop);
    listener.detach();
}

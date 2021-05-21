//
//  packets.cpp
//  Terralistic-server
//
//  Created by Jakob Zorz on 04/05/2021.
//

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <chrono>
#include "print.hpp"
#include "clickEvents.hpp"
#include "packets.hpp"
#include "serverNetworking.hpp"
#include "serverMap.hpp"

void serverMap::onPacket(packets::packet& packet, connection& conn) {
    player* curr_player = getPlayerByConnection(&conn);
    switch (packet.type) {
        case packets::STARTED_BREAKING: {
            unsigned short y = packet.getUShort(), x = packet.getUShort();
            curr_player->breaking_x = x;
            curr_player->breaking_y = y;
            curr_player->breaking = true;
            break;
        }
            
        case packets::STOPPED_BREAKING: {
            curr_player->breaking = false;
            break;
        }
            
        case packets::RIGHT_CLICK: {
            unsigned short y = packet.getUShort(), x = packet.getUShort();
            serverMap::block block = getBlock(x, y);
            if(clickEvents::click_events[(int)block.getType()].rightClickEvent)
                clickEvents::click_events[(int)block.getType()].rightClickEvent(&block, curr_player);
            break;
        }
            
        case packets::CHUNK: {
            packets::packet chunk_packet(packets::CHUNK);
            unsigned short x = packet.getUShort(), y = packet.getUShort();
            for(int i = 0; i < 16 * 16; i++) {
                serverMap::block block = getBlock((x << 4) + 15 - i % 16, (y << 4) + 15 - i / 16);
                chunk_packet << (char)block.getType() << (unsigned char)block.getLightLevel();
            }
            chunk_packet << y << x;
            conn.sendPacket(chunk_packet);
            break;
        }
            
        case packets::VIEW_SIZE_CHANGE: {
            unsigned short width = packet.getUShort(), height = packet.getUShort();
            curr_player->sight_width = width;
            curr_player->sight_height = height;
            break;
        }
            
        case packets::PLAYER_MOVEMENT: {
            curr_player->flipped = packet.getChar();
            curr_player->y = packet.getInt();
            curr_player->x = packet.getInt();

            packets::packet movement_packet(packets::PLAYER_MOVEMENT);
            movement_packet << curr_player->x << curr_player->y << (char)curr_player->flipped << curr_player->id;
            manager->sendToEveryone(movement_packet, curr_player->conn);
            break;
        }
            
        case packets::PLAYER_JOIN: {
            char raw_name[16];
            for(int i = 0; i < 16; i++)
                raw_name[15 - i] = packet.getChar();
            
            player* curr_player = getPlayerByName(raw_name);
            curr_player->conn = &conn;

            packets::packet spawn_packet(packets::SPAWN_POS);
            spawn_packet << curr_player->y << curr_player->x;
            conn.sendPacket(spawn_packet);
            
            for(player* player : online_players) {
                packets::packet join_packet(packets::PLAYER_JOIN);
                join_packet << player->x << player->y << player->id;
                curr_player->conn->sendPacket(join_packet);
            }
            
            for(item& i : items) {
                packets::packet item_packet(packets::ITEM_CREATION);
                item_packet << i.x << i.y << i.getId() << (char)i.getItemId();
                curr_player->conn->sendPacket(item_packet);
            }
            
            for(int i = 0; i < INVENTORY_SIZE; i++)
            if(curr_player->inventory.inventory_arr[i].getId() != itemType::NOTHING) {
                packets::packet packet(packets::INVENTORY_CHANGE);
                packet << (unsigned short)curr_player->inventory.inventory_arr[i].getStack() << (unsigned char)curr_player->inventory.inventory_arr[i].getId() << char(i);
                curr_player->conn->sendPacket(packet);
            }
            
            for(inventoryItem& i : curr_player->inventory.inventory_arr)
                if(i.getId() != itemType::NOTHING) {
                    packets::packet packet(packets::INVENTORY_CHANGE);
                    packet << (unsigned short)i.getStack() << (unsigned char)i.getId() << char((long)&i - (long)&curr_player->inventory.inventory_arr[0]);
                    curr_player->conn->sendPacket(packet);
                }
            
            packets::packet join_packet_out(packets::PLAYER_JOIN);
            join_packet_out << curr_player->x << curr_player->y << curr_player->id;
            manager->sendToEveryone(join_packet_out, curr_player->conn);
            
            curr_player->conn->registered = true;
            
            online_players.push_back(curr_player);
            
            print::info(curr_player->name + " (" + curr_player->conn->ip + ") connected (" + std::to_string(online_players.size()) + " players online)");
            break;
        }
            
        case packets::DISCONNECT: {
            print::info(curr_player->name + " (" + curr_player->conn->ip + ") disconnected (" + std::to_string(online_players.size() - 1) + " players online)");
            player* player = getPlayerByConnection(&conn);
            close(conn.socket);
            for(connection& i : manager->connections)
                if(i.socket == conn.socket) {
                    i.socket = -1;
                    i.ip.clear();
                    break;
                }
            
            packets::packet quit_packet(packets::PLAYER_QUIT);
            quit_packet << player->id;
            
            for(auto i = online_players.begin(); i != online_players.end(); i++)
                if((*i)->id == player->id) {
                    online_players.erase(i);
                    break;
                }
            manager->sendToEveryone(quit_packet);
            break;
        }
            
        case packets::INVENTORY_SWAP: {
            unsigned char pos = packet.getUChar();
            player* player = getPlayerByConnection(&conn);
            player->inventory.swapWithMouseItem(&player->inventory.inventory_arr[pos]);
            break;
        }
            
        case packets::HOTBAR_SELECTION: {
            curr_player->inventory.selected_slot = packet.getChar();
            break;
        }
            
        default:;
    }
}

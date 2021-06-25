//
//  items.cpp
//  Server
//
//  Created by Jakob Zorz on 22/06/2021.
//

#include "items.hpp"
#include <random>
#include "assert.hpp"

void items::initItems() {
    // all currently unique items
    unique_items = {
        {"nothing",     0,  blockType::AIR        },
        {"stone",       99, blockType::STONE      },
        {"dirt",        99, blockType::DIRT       },
        {"stone_block", 99, blockType::STONE_BLOCK},
        {"wood_planks", 99, blockType::AIR        },
    };
}

void items::updateItems(float frame_length) {
    for(auto & item : item_arr)
        item.update(frame_length);
}

void items::spawnItem(itemType item_id, int x, int y, short id) {
    static short curr_id = 0;
    if(id == -1)
        id = curr_id++;
    item_arr.emplace_back();
    item_arr.back().create(item_id, x, y, id, parent_blocks);
}

void item::create(itemType item_id_, int x_, int y_, unsigned short id_, blocks* parent_blocks_) {
    static std::random_device device;
    static std::mt19937 engine(device());
    velocity_x = (int)engine() % 100;
    velocity_y = -int(engine() % 100) - 50;
    
    x = x_ * 100;
    y = y_ * 100;
    id = id_;
    item_id = item_id_;
    parent_blocks = parent_blocks_;
    
    packets::packet packet(packets::ITEM_CREATION, sizeof(x) + sizeof(y) + sizeof(getId()) + sizeof(char));
    packet << x << y << getId() << (char)getItemId();
    parent_blocks->manager->sendToEveryone(packet);
}

void item::destroy() {
    packets::packet packet(packets::ITEM_DELETION, sizeof(getId()));
    packet << getId();
    parent_blocks->manager->sendToEveryone(packet);
}

uniqueItem::uniqueItem(std::string  name, unsigned short stack_size, blockType places) : name(std::move(name)), stack_size(stack_size), places(places) {}

[[maybe_unused]] uniqueItem& item::getUniqueItem() const {
    ASSERT((int)item_id >= 0 && (int)item_id < unique_items.size(), "item_id is not valid")
    return unique_items[(int)item_id];
}

void item::update(float frame_length) {
    int prev_x = x, prev_y = y;
    
    // move and go back if colliding
    velocity_y += (int)frame_length / 16 * 5;
    for(int i = 0; i < frame_length / 16 * velocity_x; i++) {
        x++;
        if(colliding()) {
            x--;
            break;
        }
    }
    for(int i = 0; i > frame_length / 16 * velocity_x; i--) {
        x--;
        if(colliding()) {
            x++;
            break;
        }
    }
    for(int i = 0; i < frame_length / 16 * velocity_y; i++) {
        y++;
        if(colliding()) {
            y--;
            break;
        }
    }
    for(int i = 0; i > frame_length / 16 * velocity_y; i--) {
        y--;
        if(colliding()) {
            y++;
            break;
        }
    }
    
    y++;
    if(colliding())
        velocity_y = 0;
    y--;
    
    if(velocity_x > 0) {
        velocity_x -= frame_length / 8;
        if(velocity_x < 0)
            velocity_x = 0;
    }
    else if(velocity_x < 0) {
        velocity_x += frame_length / 8;
        if(velocity_x > 0)
            velocity_x = 0;
    }
    
    if(prev_x != x || prev_y != y) {
        packets::packet packet(packets::ITEM_MOVEMENT, sizeof(x) + sizeof(y) + sizeof(getId()));
        packet << x << y << getId();
        parent_blocks->manager->sendToEveryone(packet);
    }
}

bool item::colliding() const {
    int height_x = 1, height_y = 1;
    if(x / 100 % BLOCK_WIDTH)
        height_x++;
    if(y / 100 % BLOCK_WIDTH)
        height_y++;
    int block_x = x / 100 / BLOCK_WIDTH, block_y = y / 100 / BLOCK_WIDTH;
    for(int x_ = 0; x_ < height_x; x_++)
        for(int y_ = 0; y_ < height_y; y_++)
            if(!parent_blocks->getBlock((unsigned short)(block_x + x_), (unsigned short)(block_y + y_)).isTransparent())
                return true;
    return false;
}
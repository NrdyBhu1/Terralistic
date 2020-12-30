//
//  block.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 09/12/2020.
//

#include <algorithm>
#include "singleWindowLibrary.hpp"
#include "blockEngine.hpp"
#include "lightingEngine.hpp"

void blockEngine::block::draw() {
    SDL_Rect rect = getRect();
    if(getUniqueBlock().texture && lightingEngine::light_map[this - world].level)
        swl::render(getUniqueBlock().texture, rect, {0, 8 * block_orientation, 8, 8});
    lightingEngine::light_map[this - world].render((short)rect.x, (short)rect.y);
}

SDL_Rect blockEngine::block::getRect() {
    return {(getX() * BLOCK_WIDTH - view_x + swl::window_width / 2),
            (getY() * BLOCK_WIDTH - view_y + swl::window_height / 2), BLOCK_WIDTH, BLOCK_WIDTH};
}

unsigned short blockEngine::block::getX() {
    return (unsigned short)((this - world) % world_width);
}

unsigned short blockEngine::block::getY() {
    return (unsigned short)((this - world) / world_width);
}

void blockEngine::block::update() {
    block_orientation = 0;
    
    if(getUniqueBlock().only_on_floor) {
        if(getBlock(getX(), (unsigned short)(getY() + 1)).getUniqueBlock().transparent)
            getBlock(getX(), getY()).block_id = AIR;
    }
    
    if(!getUniqueBlock().single_texture) {
        char x[] = {0, 1, 0, -1};
        char y[] = {-1, 0, 1, 0};
        Uint8 c = 1;
        for(int i = 0; i < 4; i++) {
            if(getX() + x[i] >= world_width || getX() + x[i] < 0) {
                block_orientation += c;
                continue;
            }
            if(getY() + y[i] >= world_height || getY() + y[i] < 0) {
                block_orientation += c;
                continue;
            }
            if(getBlock(getX() + x[i], getY() + y[i]).block_id == block_id || std::count(getUniqueBlock().connects_to.begin(), getUniqueBlock().connects_to.end(), getBlock(getX() + x[i], getY() + y[i]).block_id))
                block_orientation += c;
            c += c;
        }
    }
}

blockEngine::uniqueBlock& blockEngine::block::getUniqueBlock() const {
    return unique_blocks.at(block_id);
}

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

void blockEngine::block::draw(unsigned short x, unsigned short y) {
    SDL_Rect rect = {x * BLOCK_WIDTH, y * BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH};
    if(getUniqueBlock().texture && lightingEngine::getLightBlock(x, y).level)
        swl::render(getUniqueBlock().texture, rect, {0, 8 * block_orientation, 8, 8});
}

SDL_Rect blockEngine::block::getRect(unsigned short x, unsigned short y) {
    return {(x * BLOCK_WIDTH - view_x + swl::window_width / 2), (y * BLOCK_WIDTH - view_y + swl::window_height / 2), BLOCK_WIDTH, BLOCK_WIDTH};
}

void blockEngine::block::update(unsigned short x, unsigned short y) {
    block_orientation = 0;
    
    if(getUniqueBlock().only_on_floor) {
        if(getBlock(x, (unsigned short)(y + 1)).getUniqueBlock().transparent)
            getBlock(x, y).block_id = AIR;
    }
    
    if(!getUniqueBlock().single_texture) {
        char x_[] = {0, 1, 0, -1};
        char y_[] = {-1, 0, 1, 0};
        Uint8 c = 1;
        for(int i = 0; i < 4; i++) {
            if(x + x_[i] >= world_width || x + x_[i] < 0) {
                block_orientation += c;
                continue;
            }
            if(y + y_[i] >= world_height || y + y_[i] < 0) {
                block_orientation += c;
                continue;
            }
            if(getBlock(x + x_[i], y + y_[i]).block_id == block_id || std::count(getUniqueBlock().connects_to.begin(), getUniqueBlock().connects_to.end(), getBlock(x + x_[i], y + y_[i]).block_id))
                block_orientation += c;
            c += c;
        }
    }
    
    setUpdateBlock(x, y, true);
    getChunk(x >> 4, y >> 4).update = true;
}

blockEngine::uniqueBlock& blockEngine::block::getUniqueBlock() const {
    return unique_blocks.at(block_id);
}

void blockEngine::block::setBlockType(blockType id, unsigned short x, unsigned short y) {
    if(id != block_id) {
        block_id = id;
        setUpdateBlock(x, y, true);
        getChunk(x >> 4, y >> 4).update = true;
    }
}

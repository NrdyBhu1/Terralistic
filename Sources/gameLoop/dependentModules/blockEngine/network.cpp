//
//  network.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 13/02/2021.
//

#include "blockEngine.hpp"
#include "networkingModule.hpp"

void blockChangeListener(packets::packet& packet) {
    auto type = (blockEngine::blockType)packet.getUChar();
    unsigned short y = packet.getUShort(), x = packet.getUShort();
    blockEngine::removeNaturalLight(x);
    blockEngine::getBlock(x, y).setBlockType(type, x, y);
    blockEngine::setNaturalLight(x);
    blockEngine::getBlock(x, y).update(x, y);
    blockEngine::getBlock(x, y).light_update(x, y);
    blockEngine::updateNearestBlocks(x, y);
}

void chunkListener(packets::packet& packet) {
    unsigned short x = packet.getUShort(), y = packet.getUShort();
    blockEngine::chunk& chunk = blockEngine::getChunk(x, y);
    for(unsigned short x_ = x << 4; x_ < (x << 4) + 16; x_++)
        blockEngine::removeNaturalLight(x_);
    for(int i = 0; i < 16 * 16; i++)
        chunk.blocks[i % 16][i / 16].block_id = (blockEngine::blockType)packet.getChar();
    for(unsigned short x_ = (x << 4) - 1; x_ < (x << 4) + 17; x_++)
        for(unsigned short y_ = (y << 4) - 1; y_ < (y << 4) + 17; y_++)
            if((x_ && y_ && x_ != (x << 4) + 16 && y_ != (y << 4) + 16) || blockEngine::getChunk(x_ >> 4, y_ >> 4).loaded)
                blockEngine::getBlock(x_, y_).update(x_, y_);
    for(unsigned short x_ = x << 4; x_ < (x << 4) + 16; x_++)
        blockEngine::setNaturalLight(x_);
    chunk.loaded = true;
}

void blockBreakProgressChangeListener(packets::packet& packet) {
    unsigned short progress = packet.getUShort(), x = packet.getUShort(), y = packet.getUShort();
    blockEngine::getBlock(x, y).break_progress = progress;
    blockEngine::getBlock(x, y).to_update = true;
    blockEngine::getChunk(x >> 4, y >> 4).update = true;
}

networking::registerListener block_change_listener(blockChangeListener, packets::BLOCK_CHANGE), chunk_listener(chunkListener, packets::CHUNK), block_break_progress_change_listener(blockBreakProgressChangeListener, packets::BLOCK_BREAK_PROGRESS_CHANGE);

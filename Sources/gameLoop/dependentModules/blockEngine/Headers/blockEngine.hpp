//
//  blockEngine.h
//  Terralistic
//
//  Created by Jakob Zorz on 25/06/2020.
//

#ifndef blockEngine_h
#define blockEngine_h

#include <SDL2/SDL.h>

#include <iostream>
#include <vector>

#define BLOCK_WIDTH 16

namespace blockEngine {

enum blockType {BLOCK_AIR, BLOCK_DIRT};

struct block {
    block() : block_id(BLOCK_AIR) {}
    block(unsigned short block_id) : block_id(block_id) {}
    
    void draw();
    SDL_Rect getRect();
    unsigned int getX();
    unsigned int getY();
    unsigned short block_id;
};

struct unique_block {
    std::string name;
    SDL_Texture* texture;
    
    unique_block(std::string name) : name(name) {}
};

void init();

void render_blocks();

inline block *world;
inline unsigned int world_width;
inline unsigned int world_height;

block& getBlock(unsigned int x, unsigned int y);

inline long position_x, position_y;

inline std::vector<unique_block> block_types;
}

#endif /* blockEngine_h */

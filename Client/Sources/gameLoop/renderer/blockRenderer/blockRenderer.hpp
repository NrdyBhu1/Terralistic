//
//  blockRenderer.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 27/02/2021.
//

#ifndef blockRenderer_hpp
#define blockRenderer_hpp

#include <SDL2/SDL.h>

namespace blockRenderer {

struct uniqueRenderBlock {
    uniqueRenderBlock(blockEngine::uniqueBlock* unique_block);
    SDL_Texture* texture;
    std::vector<blockEngine::blockType> connects_to;
    bool single_texture;
};

struct renderBlock {
    void updateOrientation();
    void draw();
    Uint8 block_orientation{0};
    bool to_update = true;
    void scheduleTextureUpdate();
    unsigned short getX() const;
    unsigned short getY() const;
    blockEngine::block& getRelatedBlock();
    uniqueRenderBlock& getUniqueRenderBlock();
    blockEngine::uniqueBlock& getUniqueBlock();
};

struct renderChunk {
    void render() const;
    bool update = true;
    SDL_Texture* texture = nullptr;
    void createTexture();
    void updateTexture();
    unsigned short getX() const;
    unsigned short getY() const;
};

inline renderChunk* chunks;
inline renderBlock* blocks;
inline std::vector<uniqueRenderBlock> unique_render_blocks;

renderBlock& getBlock(unsigned short x, unsigned short y);
renderChunk& getChunk(unsigned short x, unsigned short y);

void prepare();
void render();
void close();

}

#endif /* blockRenderer_hpp */
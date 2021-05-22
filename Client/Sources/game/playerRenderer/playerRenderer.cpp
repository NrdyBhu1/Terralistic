//
//  playerRenderer.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 25/04/2021.
//

#include "playerRenderer.hpp"

#ifdef _WIN32
#include "graphics.hpp"
#else

#ifdef DEVELOPER_MODE
#include <Graphics_Debug/graphics.hpp>
#else
#include <Graphics/graphics.hpp>
#endif


#endif

static gfx::image player;

void playerRenderer::init() {
    player.setTexture(gfx::loadImageFile("texturePack/misc/player.png"));
    player.scale = 2;
}

void playerRenderer::render(int x, int y, int view_x, int view_y, bool flipped) {
    player.flipped = flipped;
    gfx::render(player, gfx::getWindowWidth() / 2 - playerRenderer::getPlayerWidth() / 2 + x - view_x, gfx::getWindowHeight() / 2 - playerRenderer::getPlayerHeight() / 2 + y - view_y);
}

unsigned short playerRenderer::getPlayerWidth() {
    return player.getTextureWidth() * player.scale;
}

unsigned short playerRenderer::getPlayerHeight() {
    return player.getTextureHeight() * player.scale;
}
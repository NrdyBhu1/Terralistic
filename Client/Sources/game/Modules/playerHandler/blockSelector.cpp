//
//  blockSelector.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 13/07/2020.
//

#include "playerHandler.hpp"
#include "playerRenderer.hpp"

// this is a rectangle with which you select which block to break or where to place selected block

struct clickEvents {
    void (*rightClickEvent)(map::block*, inventory*) = nullptr;
    void (*leftClickEvent)(map::block*, inventory*) = nullptr;
};

static std::vector<clickEvents> click_events;

// you can register special click events to blocks for custom behaviour
static void grass_block_leftClickEvent(map::block* block, inventory* player_inventory) {
    block->setType(map::blockType::DIRT);
}

static void air_rightClickEvent(map::block* block, inventory* player_inventory) {
    map::blockType type = player_inventory->getSelectedSlot()->getUniqueItem().places;
    if(type != map::blockType::AIR && player_inventory->getSelectedSlot()->decreaseStack(1)) {
        block->setType(type);
        block->lightUpdate();
    }
}

static void air_leftClickEvent(map::block* block, inventory* player_inventory) {}

void playerHandler::initEvents() {
    click_events = std::vector<clickEvents>(map::unique_blocks.size());

    click_events[(int)map::blockType::GRASS_BLOCK].leftClickEvent = &grass_block_leftClickEvent;
    click_events[(int)map::blockType::AIR].rightClickEvent = &air_rightClickEvent;
    click_events[(int)map::blockType::AIR].leftClickEvent = &air_leftClickEvent;
}

void playerHandler::renderBlockSelector() {
    if((prev_selected_y != selected_block_y || prev_selected_x != selected_block_x) && is_left_button_pressed) {
        if(multiplayer) {
            packets::packet packet(packets::STARTED_BREAKING);
            packet << selected_block_x << selected_block_y;
            manager->sendPacket(packet);
        }
        prev_selected_x = selected_block_x;
        prev_selected_y = selected_block_y;
    }
    
    if(is_left_button_pressed && !multiplayer) {
        map::block block = map->getBlock(selected_block_x, selected_block_y);
        if(click_events[(int)block.getType()].leftClickEvent)
            click_events[(int)block.getType()].leftClickEvent(&block, &player->player_inventory);
        else {
            block.setBreakProgress(block.getBreakProgress() + gfx::getDeltaTime());
            if(!multiplayer && block.getBreakProgress() >= block.getBreakTime())
                block.breakBlock();
        }
    }
    
    if(!playerHandler::hovered) {
        selected_block_x = (unsigned short)(gfx::getMouseX() + map->view_x - gfx::getWindowWidth() / 2) / BLOCK_WIDTH;
        selected_block_y = (unsigned short)(gfx::getMouseY() + map->view_y - gfx::getWindowHeight() / 2) / BLOCK_WIDTH;
        select_rect.x = -map->view_x + gfx::getWindowWidth() / 2 + selected_block_x * BLOCK_WIDTH;
        select_rect.y = -map->view_y + gfx::getWindowHeight() / 2 + selected_block_y * BLOCK_WIDTH;
        gfx::render(select_rect, false);
    }
}

void playerHandler::onKeyDownSelector(gfx::key key) {
    if(key == gfx::KEY_MOUSE_LEFT && !playerHandler::hovered) {
        is_left_button_pressed = true;
        prev_selected_x = map->getWorldWidth();
        prev_selected_y = map->getWorldHeight();
    } else if(key == gfx::KEY_MOUSE_RIGHT && !playerHandler::hovered) {
        gfx::rectShape rect = gfx::rectShape(gfx::getWindowWidth() / 2 - playerRenderer::getPlayerWidth() / 2, gfx::getWindowHeight() / 2 - playerRenderer::getPlayerHeight() / 2, playerRenderer::getPlayerWidth(), playerRenderer::getPlayerHeight());
        if(!gfx::colliding(rect, select_rect.getTranslatedRect())) {
            if(multiplayer) {
                packets::packet packet(packets::RIGHT_CLICK);
                packet << selected_block_x << selected_block_y;
                manager->sendPacket(packet);
            } else {
                map::block block = map->getBlock(selected_block_x, selected_block_y);
                if(click_events[(int)block.getType()].rightClickEvent)
                    click_events[(int)block.getType()].rightClickEvent(&block, &player->player_inventory);
            }
        }
    }
}

void playerHandler::onKeyUpSelector(gfx::key key) {
    if(key == gfx::KEY_MOUSE_LEFT && !playerHandler::hovered) {
        is_left_button_pressed = false;
        if(multiplayer) {
            packets::packet packet(packets::STOPPED_BREAKING);
            manager->sendPacket(packet);
        }
    }
}

//
//  block.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 24/06/2021.
//

#include "blocks.hpp"

#include "serverMap.hpp"
#include "assert.hpp"
#include "serverNetworking.hpp"

void blocks::initBlocks() {
    unique_blocks = {
        uniqueBlock("air",               /*ghost*/true,  /*only_on_floorfalse,*/  /*transparent*/true,  /*dropitemType::NOTHING,    */ /*break_time*/UNBREAKABLE),
        uniqueBlock("dirt",              /*ghost*/false, /*only_on_floorfalse,*/  /*transparent*/false, /*dropitemType::DIRT,       */ /*break_time*/1000       ),
        uniqueBlock("stone_block",       /*ghost*/false, /*only_on_floorfalse,*/  /*transparent*/false, /*dropitemType::STONE_BLOCK,*/ /*break_time*/1000       ),
        uniqueBlock("grass_block",       /*ghost*/false, /*only_on_floorfalse,*/  /*transparent*/false, /*dropitemType::NOTHING,    */ /*break_time*/1000       ),
        uniqueBlock("stone",             /*ghost*/true,  /*only_on_floortrue ,*/   /*transparent*/true,  /*dropitemType::STONE,      */ /*break_time*/1500       ),
        uniqueBlock("wood",              /*ghost*/true,  /*only_on_floorfalse,*/  /*transparent*/false, /*dropitemType::WOOD_PLANKS,*/ /*break_time*/1000       ),
        uniqueBlock("leaves",            /*ghost*/true,  /*only_on_floorfalse,*/  /*transparent*/false, /*dropitemType::NOTHING,    */ /*break_time*/UNBREAKABLE),
        uniqueBlock("sand",              /*ghost*/false, /*only_on_floorfalse,*/  /*transparent*/false, /*dropitemType::NOTHING,    */ /*break_time*/500        ),
        uniqueBlock("snowy_grass_block", /*ghost*/false, /*only_on_floorfalse,*/  /*transparent*/false, /*dropitemType::NOTHING,    */ /*break_time*/1000       ),
        uniqueBlock("snow_block",        /*ghost*/false, /*only_on_floorfalse,*/  /*transparent*/false, /*dropitemType::NOTHING,    */ /*break_time*/500        ),
        uniqueBlock("ice_block",         /*ghost*/false, /*only_on_floorfalse,*/  /*transparent*/false, /*dropitemType::NOTHING,    */ /*break_time*/500        ),

    };

    /*unique_blocks[(int)blockType::WOOD].onBreak = [](blocks* world_map, block* this_block) {
        block blocks[] = {world_map->getBlock(this_block->getX(), this_block->getY() - 1), world_map->getBlock(this_block->getX() + 1, this_block->getY()), world_map->getBlock(this_block->getX() - 1, this_block->getY())};
        for(block& i : blocks)
            if(i.getType() == blockType::WOOD || i.getType() == blockType::LEAVES)
                i.breakBlock();
    };

    unique_blocks[(int)blockType::LEAVES].onBreak = unique_blocks[(int)blockType::WOOD].onBreak;

    unique_blocks[(int)blockType::GRASS_BLOCK].onLeftClick = [](block* this_block, player* peer) {
        this_block->setType(blockType::DIRT);
    };

    unique_blocks[(int)blockType::AIR].onRightClick = [](block* this_block, player* peer) {
        blockType type = peer->player_inventory.getSelectedSlot()->getUniqueItem().places;
        if(type != blockType::AIR && peer->player_inventory.inventory_arr[peer->player_inventory.selected_slot].decreaseStack(1)) {
            this_block->setType(type);
            this_block->update();
        }
    };

    unique_blocks[(int)blockType::SNOWY_GRASS_BLOCK].onLeftClick = unique_blocks[(int)blockType::GRASS_BLOCK].onLeftClick;*/
}

block blocks::getBlock(unsigned short x, unsigned short y) {
    ASSERT(y >= 0 && y < height && x >= 0 && x < width, "requested block is out of bounds")
    return block(x, y, &block_arr[y * width + x], this);
}

void block::setType(blockType block_id, bool process) {
    setType(block_id, block_data->liquid_id, process);
}

void block::setType(liquidType liquid_id, bool process) {
    setType(block_data->block_id, liquid_id, process);
}

void block::setType(blockType block_id, liquidType liquid_id, bool process) {
    if(!process) {
        block_data->block_id = block_id;
        block_data->liquid_id = liquid_id;
    }
    else if(block_id != block_data->block_id || liquid_id != block_data->liquid_id) {
        bool was_transparent = isTransparent();
        block_data->block_id = block_id;
        block_data->liquid_id = liquid_id;
        if(liquid_id == liquidType::EMPTY)
            setLiquidLevel(0);

        if(isTransparent() != was_transparent) {
            if(isTransparent())
                for(int curr_y = y; parent_map->getBlock(x, curr_y).isTransparent(); curr_y++)
                    parent_map->getBlock(x, curr_y).setLightSource(MAX_LIGHT);
            else
                for(int curr_y = y; parent_map->getBlock(x, curr_y).isLightSource(); curr_y++)
                    parent_map->getBlock(x, curr_y).removeLightSource();
        }

        update();
        updateNeighbors();
        syncWithClient();
    }
}

void block::updateNeighbors() {
    // update upper, lower, right and left block (neighbours)
    if(x != 0)
        parent_map->getBlock(x - 1, y).update();
    if(x != parent_map->width - 1)
        parent_map->getBlock(x + 1, y).update();
    if(y != 0)
        parent_map->getBlock(x, y - 1).update();
    if(y != parent_map->height - 1)
        parent_map->getBlock(x, y + 1).update();
}

void block::syncWithClient() {
    packets::packet packet(packets::BLOCK_CHANGE, sizeof(getX()) + sizeof(getY()) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned char));
    packet << getX() << getY() << (unsigned char)getLiquidType() << (unsigned char)getLiquidLevel() << (unsigned char)getLightLevel() << (unsigned char)getType();
    parent_map->manager->sendToEveryone(packet);
}

void block::setBreakProgress(unsigned short ms) {
    block_data->break_progress = ms;
    auto stage = (unsigned char)((float)getBreakProgress() / (float)getBreakTime() * 9.0f);
    if(stage != getBreakStage()) {
        block_data->break_stage = stage;
        packets::packet packet(packets::BLOCK_PROGRESS_CHANGE, sizeof(getY()) + sizeof(getX()) + sizeof(getBreakStage()));
        packet << getY() << getX() << getBreakStage();
        parent_map->manager->sendToEveryone(packet);
    }
}

void block::update() {
    //if(isOnlyOnFloor() && parent_map->getBlock(x, (unsigned short)(y + 1)).isTransparent())
        //breakBlock();
    scheduleLightUpdate();
}

/*void block::breakBlock() {
    if(getDrop() != itemType::NOTHING)
        parent_map->spawnItem(getDrop(), x * BLOCK_WIDTH, y * BLOCK_WIDTH);
    uniqueBlock *unique_block = &block_data->getUniqueBlock();
    setType(blockType::AIR);
    setBreakProgress(0);
    if(unique_block->onBreak)
        unique_block->onBreak(parent_map, this);
}*/

uniqueBlock& blockData::getUniqueBlock() const {
    return unique_blocks[(int)block_id];
}

/*void block::leftClickEvent(connection& connection, unsigned short tick_length) {
    if(block_data->getUniqueBlock().onLeftClick)
        block_data->getUniqueBlock().onLeftClick(this, parent_map->getPlayerByConnection(&connection));
    else {
        setBreakProgress(getBreakProgress() + tick_length);
        if(getBreakProgress() >= getBreakTime())
            breakBlock();
    }
}

void block::rightClickEvent(player* peer) {
    if(block_data->getUniqueBlock().onRightClick)
        block_data->getUniqueBlock().onRightClick(this, peer);
}*/
#include "serverBlocks.hpp"

void ServerBlock::lightUpdate() {
    block_data->update_light = false;
    
    ServerBlock neighbors[4];
    if(x != 0)
        neighbors[0] = parent_map->getBlock(x - 1, y);
    if(x != parent_map->getWidth() - 1)
        neighbors[1] = parent_map->getBlock(x + 1, y);
    if(y != 0)
        neighbors[2] = parent_map->getBlock(x, y - 1);
    if(y != parent_map->getHeight() - 1)
        neighbors[3] = parent_map->getBlock(x, y + 1);
    
    if(!block_data->light_source) {
        unsigned char level_to_be = 0;
        for(auto & neighbor : neighbors)
            if(neighbor.refersToABlock()) {
                unsigned char light_step = neighbor.getUniqueBlock().transparent ? 3 : 15;
                unsigned char light = light_step > neighbor.getLightLevel() ? 0 : neighbor.getLightLevel() - light_step;
                if(light > level_to_be)
                    level_to_be = light;
            }
        setLightLevel(level_to_be);
    }
}

void ServerBlock::setLightSource(unsigned char power) {
    block_data->light_source = true;
    setLightLevel(power);
}

void ServerBlock::removeLightSource() {
    block_data->light_source = false;
    setLightLevel(0);
}

void ServerBlocks::setNaturalLight(unsigned short x) {
    is_light_set[x] = true;
    for(unsigned short y = 0; y < height && getBlock(x, y).getUniqueBlock().transparent; y++)
        getBlock(x, y).setLightSource(MAX_LIGHT);
}

void ServerBlocks::removeNaturalLight(unsigned short x) {
    is_light_set[x] = false;
    for(unsigned short y = 0; y < height && getBlock(x, y).getUniqueBlock().transparent; y++)
        getBlock(x, y).removeLightSource();
}

void ServerBlock::setLightLevel(unsigned char light_level) {
    if(block_data->light_level != light_level) {
        block_data->has_changed_light = true;
        block_data->light_level = light_level;
        updateNeighbors();
    }
}

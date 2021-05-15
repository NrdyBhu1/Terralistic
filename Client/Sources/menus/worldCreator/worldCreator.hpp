//
//  worldCreator.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 05/12/2020.
//

#ifndef worldCreator_hpp
#define worldCreator_hpp

#ifdef _WIN32
#include "graphics.hpp"
#else

#ifdef DEVELOPER_MODE
#include <Graphics_Debug/graphics.hpp>
#else
#include <Graphics/graphics.hpp>
#endif


#endif

#include <string>
#include <vector>

struct worldCreator : gfx::scene {
    worldCreator(const std::vector<std::string>& worlds) : worlds(worlds) {}
    bool running = true, can_create = true;
    void init() override;
    void onKeyDown(gfx::key key) override;
    void render() override;
    
private:
    std::vector<std::string> worlds;
    gfx::button back_button, create_button;
    gfx::sprite new_world_title, faded_create;
    gfx::textInput world_name;
};

#endif /* worldCreator_hpp */

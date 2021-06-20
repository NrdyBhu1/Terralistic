//
//  worldSelector.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 05/12/2020.
//

#ifndef worldSelector_hpp
#define worldSelector_hpp

#ifdef __APPLE__

#ifdef DEVELOPER_MODE
#include <Graphics_Debug/graphics.hpp>
#else
#include <Graphics/graphics.hpp>
#endif

#else
#include "graphics.hpp"
#endif

#include <iostream>

class worldSelector : public gfx::scene {
    struct world_to_select {
        std::string name;
        explicit world_to_select(std::string name) : name(std::move(name)) {}
        gfx::button button, delete_button;
        void render(int position);
        int button_y{};
    };

    gfx::sprite title;
    gfx::image x_image;
    gfx::button back_button, new_button;
    std::vector<std::string> worlds_names;
    std::vector<world_to_select> worlds;
    int scroll_limit, position;
    bool shift_pressed = false;
public:
    void init() override;
    void refresh();
    void onKeyDown(gfx::key key) override;
    void onKeyUp(gfx::key key) override;
    void render() override;
    void onMouseScroll(int distance) override;
};

#endif /* worldSelector_hpp */

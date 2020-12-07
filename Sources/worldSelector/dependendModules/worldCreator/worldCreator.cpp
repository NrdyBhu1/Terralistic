//
//  worldCreator.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 05/12/2020.
//

#include <vector>
#include "worldCreator.hpp"
#include "UIKit.hpp"
#include "framerateRegulator.hpp"
#include "singleWindowLibrary.hpp"
#include "worldSelector.hpp"
#include "gameLoop.hpp"

ui::button back_button_creator(ogl::objectType::bottom), create_button(ogl::objectType::bottom);
ogl::texture new_world_title(ogl::objectType::top), world_name, faded_create(ogl::objectType::bottom);
std::string name;

#define PADDING 20

void worldCreator::init() {
    back_button_creator.setColor(0, 0, 0);
    back_button_creator.setHoverColor(100, 100, 100);
    back_button_creator.setTextColor(255, 255, 255);
    back_button_creator.setScale(3);
    back_button_creator.setText("Back");
    back_button_creator.setY(-PADDING);
    
    new_world_title.loadFromText("New world name:", {255, 255, 255});
    new_world_title.scale = 3;
    new_world_title.setY(PADDING);
    
    create_button.setColor(0, 0, 0);
    create_button.setHoverColor(100, 100, 100);
    create_button.setTextColor(255, 255, 255);
    create_button.setScale(3);
    create_button.setText("Create world");
    create_button.setY(-PADDING);
    
    back_button_creator.setX((-create_button.getWidth() - back_button_creator.getWidth() + back_button_creator.getWidth() - PADDING) / 2);
    create_button.setX((create_button.getWidth() + back_button_creator.getWidth() - create_button.getWidth() + PADDING) / 2);
    
    faded_create.loadFromText("Create world", {100, 100, 100});
    faded_create.scale = 3;
    faded_create.setX((create_button.getWidth() + back_button_creator.getWidth() - create_button.getWidth() + PADDING) / 2);
    faded_create.setY(-PADDING - 10 * faded_create.scale);
    
    world_name.scale = 3;
}

void renderText() {
    if(!name.empty()) {
        world_name.loadFromText(name, {255, 255, 255});
        world_name.setY(-world_name.getHeight() / 7);
    }
}

void worldCreator::loop(std::vector<std::string> worlds) {
    bool running = true, can_create = false;
    SDL_Event event;
    name = "";
    
    renderText();
    
    while(running && !gameLoop::quit) {
        can_create = !name.empty() && !std::count(worlds.begin(), worlds.end(), name);
        
        framerateRegulator::regulateFramerate();
        while(SDL_PollEvent(&event)) {
            SDL_StartTextInput();
            if(swl::handleBasicEvents(event, &running) && !running)
                gameLoop::quit = true;
            else if(can_create && create_button.isPressed(event)) {
                gameLoop::main(name);
                running = false;
            }
            else if(back_button_creator.isPressed(event))
                running = false;
            else if(event.type == SDL_TEXTINPUT) {
                char c = event.text.text[0];
                if(c == ' ')
                    c = '-';
                if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-') {
                    name.push_back(c);
                    renderText();
                }
            }
            else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && !name.empty()) {
                name.pop_back();
                renderText();
            }
        }
        
        swl::setDrawColor(0, 0, 0);
        swl::clear();
        
        if(can_create)
            create_button.render();
        else
            faded_create.render();
        back_button_creator.render();
        new_world_title.render();
        if(!name.empty())
            world_name.render();
        
        swl::update();
    }
}
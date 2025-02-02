#ifndef debugMenu_hpp
#define debugMenu_hpp

#include "graphics.hpp"
#include "clientPlayers.hpp"
#include "clientBlocks.hpp"

class DebugMenu : public gfx::GraphicalModule {
    bool debug_menu_open = false, m_down = false;
    gfx::Sprite fps_text, coords_text;
    ClientPlayers* player_handler;
    unsigned int fps_count = 0;
    ClientBlocks* blocks;
    
    void updateFpsText();
    void updateCoordsText();
    
    void init() override;
    void update() override;
    void render() override;
    void onKeyDown(gfx::Key key) override;
    void onKeyUp(gfx::Key key) override;
public:
    DebugMenu(ClientPlayers* player_handler, ClientBlocks* world_map) : player_handler(player_handler), blocks(world_map) {}
};

#endif

//
//  playerRenderer.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 25/04/2021.
//

#ifndef playerRenderer_hpp
#define playerRenderer_hpp

namespace playerRenderer {

void init();
void render(int x, int y, int view_x, int view_y, bool flipped);
unsigned short getPlayerWidth();
unsigned short getPlayerHeight();

}
#endif /* playerRenderer_hpp */
//
//  lightingEngine.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 28/12/2020.
//

#ifndef lightingEngine_hpp
#define lightingEngine_hpp

#define MAX_LIGHT 10

namespace lightingEngine {

struct lightBlock {
    unsigned char level = 0;
    bool source = false;
    void render(short x, short y);
    void update();
    unsigned short getX();
    unsigned short getY();
};

void init();
void prepare();
void close();

inline lightBlock* light_map;

void removeNaturalLight(unsigned short x);
void setNaturalLight(unsigned short x);

lightBlock& getLightBlock(unsigned short x, unsigned short y);

}

#endif /* lightingEngine_hpp */
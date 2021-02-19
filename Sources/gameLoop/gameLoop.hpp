//
//  gameLoop.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 06/07/2020.
//

#ifndef gameLoop_h
#define gameLoop_h

#include <string>

#undef main

namespace gameLoop {

int main(const std::string& world_name, bool multiplayer);
inline bool running = true, online;
inline float frame_length;

}

#endif /* gameLoop_h */
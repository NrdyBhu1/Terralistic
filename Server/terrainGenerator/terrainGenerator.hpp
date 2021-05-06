//
//  terrainGenerator.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 25/06/2020.
//

#ifndef terrainGenerator_hpp
#define terrainGenerator_hpp

namespace terrainGenerator {

int generateTerrainDaemon(unsigned int seed, serverMap* world_serverMap);
inline long loading_current = 0, loading_total = -1;

}

#endif /* terrainGenerator_hpp */

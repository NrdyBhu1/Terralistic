//
//  worldGenerator.hpp
//  Terralistic
//
//  Created by Jakob Zorz on 22/06/2021.
//

#ifndef worldGenerator_hpp
#define worldGenerator_hpp

#include <string>
#include "blocks.hpp"

class worldGenerator {
    struct structure {
        std::string name;
        int x_size, y_size;
        blockType* blocks;
        structure(std::string cname, int x, int y, blockType* cBlocks) {
            name = std::move(cname);
            x_size = x;
            y_size = y;
            blocks = cBlocks;
        }
    };
    struct structurePosition
    {
        std::string name;
        int x, y;
        structurePosition(std::string cname, int cx, int cy) {
            name = std::move(cname);
            x = cx;
            y = cy;
        }
    };

    std::vector<structure> structures;
    std::vector<structurePosition> structurePositions;
    
    void biomeGeneratorSwitch(unsigned int x, SimplexNoise& noise);
    int calculateHeight(int x, SimplexNoise& noise);
    //static int heightGeneratorInt(unsigned int x, SimplexNoise& noise);
    void generatePlains(int x, SimplexNoise& noise);
    void generateDesert(int x, SimplexNoise& noise);
    void generateSnowyTundra(int x, SimplexNoise& noise);
    void terrainGenerator(int x, SimplexNoise& noise);
    void generateSea(int x, SimplexNoise& noise);
    void generateIcySea(int x, SimplexNoise& noise);
    void generateWarmOcean(int x, SimplexNoise& noise);
    void generateForest(int x, SimplexNoise& noise);
    void generateColdHills(int x, SimplexNoise& noise);
    void generateSavana(int x, SimplexNoise& noise);

    //void generateOakTree(int x, int y);
    void generateAccaciaTree(int x, int y);
    void generateStructure(const std::string& name, int x, int y);
    void loadAssets();

    [[nodiscard]] int getSpawnX() const;
    int getSpawnY();

    std::string resource_path;
    
    unsigned int generating_current = 0, generating_total = 6;
    int generateTerrain(unsigned int seed);
};

#endif /* worldGenerator_hpp */
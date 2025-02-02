#ifndef properties_hpp
#define properties_hpp

#define UNBREAKABLE -1

#include <string>
#include <vector>

enum class BlockType {NOTHING = -1, AIR, DIRT, STONE_BLOCK, GRASS_BLOCK, STONE, WOOD, LEAVES, SAND, SNOWY_GRASS_BLOCK, SNOW_BLOCK, ICE, NUM_BLOCKS};
enum class ItemType {NOTHING, STONE, DIRT, STONE_BLOCK, WOOD_PLANKS, NUM_ITEMS};
enum class LiquidType {EMPTY, WATER, NUM_LIQUIDS};
enum class Biome {NO_BIOME = -1, ICY_SEAS, SNOWY_TUNDRA, COLD_HILLS, SNOWY_MOUNTAINS, SEA, PLAINS, FOREST, MOUNTAINS, WARM_OCEAN, DESERT, SAVANA, SAVANA_MOUNTAINS, NUM_BIOMES};

struct BlockInfo {
    BlockInfo() = default;
    BlockInfo(std::string name, bool ghost, bool transparent, bool only_on_floor, short break_time, ItemType drop, std::vector<BlockType> connects_to);
    
    bool ghost, transparent, only_on_floor;
    std::string name;
    std::vector<BlockType> connects_to;
    short break_time;
    ItemType drop;
};

struct ItemInfo {
    ItemInfo() = default;
    ItemInfo(std::string name, unsigned short stack_size, BlockType places);
    
    std::string name;
    unsigned short stack_size;
    BlockType places;
};

struct LiquidInfo {
    LiquidInfo() = default;
    LiquidInfo(std::string name, unsigned short flow_time, float speed_multiplier);
    
    std::string name;
    unsigned short flow_time;
    float speed_multiplier;
};

struct ItemStack {
    ItemStack(ItemType type, unsigned short stack) : type(type), stack(stack) {}
    ItemStack() = default;
    ItemType type = ItemType::NOTHING;
    unsigned short stack = 0;
};

struct Recipe {
    Recipe(std::vector<ItemStack> ingredients, ItemStack result) : ingredients(ingredients), result(result) {}
    std::vector<ItemStack> ingredients;
    ItemStack result;
};

void initProperties();
const BlockInfo& getBlockInfo(BlockType type);
const ItemInfo& getItemInfo(ItemType type);
const LiquidInfo& getLiquidInfo(LiquidType type);
const std::vector<Recipe>& getRecipes();
unsigned short getRecipeIndex(const Recipe* recipe);

#endif

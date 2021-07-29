#ifndef serverPlayers_hpp
#define serverPlayers_hpp

#define INVENTORY_SIZE 20

#include "serverItems.hpp"

class ServerInventory;

class InventoryItem {
    unsigned short stack;
    ServerInventory* inventory;
    ItemType type;
public:
    InventoryItem() : inventory(nullptr), type(ItemType::NOTHING), stack(0) {}
    explicit InventoryItem(ServerInventory* holder) : inventory(holder), type(ItemType::NOTHING), stack(0) {}

    inline ItemType getType() const { return type; }
    void setType(ItemType type_);
    void setTypeWithoutProcessing(ItemType type_);
    
    const ItemInfo& getUniqueItem() const;
    unsigned short getStack() const;
    void setStack(unsigned short stack_);
    void setStackWithoutProcessing(unsigned short stack_);
    
    unsigned short increaseStack(unsigned short stack_);
    bool decreaseStack(unsigned short stack_);
    
    unsigned char getPosInInventory();
    inline ServerInventory* getInventory() { return inventory; } 
};

class ServerInventory {
    InventoryItem mouse_item;
public:
    ServerInventory();
    InventoryItem inventory_arr[INVENTORY_SIZE];
    char addItem(ItemType id, int quantity);
    unsigned char selected_slot = 0;
    InventoryItem* getSelectedSlot();
    void swapWithMouseItem(InventoryItem* item);
};

class ServerPlayer {
    static inline unsigned int curr_id = 0;
public:
    ServerPlayer(const std::string& name) : id(curr_id++), name(name) {}
    ServerPlayer(const std::string& path, const std::string& name);
    const std::string name;
    const unsigned short id;
    
    bool flipped = false;
    int x = 0, y = 0;
    unsigned short sight_width = 0, sight_height = 0;
    int sight_x, sight_y;
    unsigned short getSightBeginX();
    unsigned short getSightEndX();
    unsigned short getSightBeginY();
    unsigned short getSightEndY();
    
    ServerInventory inventory;
    
    bool breaking = false;
    unsigned short breaking_x = 0, breaking_y = 0;
    
    void saveTo(std::string path) const;
};

struct blockEvents {
    void (*onUpdate)(ServerBlocks*, ServerBlock*) = nullptr;
    void (*onRightClick)(ServerBlock*, ServerPlayer*) = nullptr;
    void (*onLeftClick)(ServerBlock*, ServerPlayer*) = nullptr;
};

class Players : EventListener<ServerBlockUpdateEvent> {
    ServerItems* items;
    ServerBlocks* blocks;
    
    std::vector<ServerPlayer*> all_players;
    std::vector<ServerPlayer*> online_players;
    
    void onEvent(ServerBlockUpdateEvent& event) override;

    blockEvents custom_block_events[(int)BlockType::NUM_BLOCKS];
    
    void leftClickEvent(ServerBlock this_block, ServerPlayer* peer, unsigned short tick_length);
public:
    Players(ServerBlocks* parent_blocks, ServerItems* parent_items);
    
    void rightClickEvent(ServerBlock this_block, ServerPlayer* peer);
    
    inline const std::vector<ServerPlayer*>& getAllPlayers() { return all_players; }
    inline const std::vector<ServerPlayer*>& getOnlinePlayers() { return online_players; }
    
    ServerPlayer* getPlayerByName(const std::string& name);
    ServerPlayer* addPlayer(const std::string& name);
    ServerPlayer* addPlayerFromFile(const std::string& path);
    void removePlayer(ServerPlayer* player);
    
    void updatePlayersBreaking(unsigned short tick_length);
    void updateBlocksInVisibleAreas();
    void lookForItemsThatCanBePickedUp();
    
    ~Players();
};



class ServerInventoryItemTypeChangeEvent : public Event<ServerInventoryItemTypeChangeEvent> {
public:
    ServerInventoryItemTypeChangeEvent(InventoryItem& item, ItemType type) : item(item), type(type) {}
    InventoryItem& item;
    ItemType type;
};

class ServerInventoryItemStackChangeEvent : public Event<ServerInventoryItemStackChangeEvent> {
public:
    ServerInventoryItemStackChangeEvent(InventoryItem& item, unsigned short stack) : item(item), stack(stack) {}
    InventoryItem& item;
    unsigned short stack;
};

#endif
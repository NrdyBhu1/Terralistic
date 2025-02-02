#include "serverPlayers.hpp"
#include "properties.hpp"

void InventoryItem::setTypeWithoutProcessing(ItemType type_) {
    inventory->item_counts[(int)type] -= stack;
    inventory->item_counts[(int)type_] += stack;
    type = type_;
}

void InventoryItem::setType(ItemType type_) {
    if(type != type_) {
        ServerInventoryItemTypeChangeEvent event(*this, type_);
        event.call();
        
        if(event.cancelled)
            return;
        
        setTypeWithoutProcessing(type_);
        inventory->updateAvailableRecipes();
    }
}

const ItemInfo& InventoryItem::getUniqueItem() const {
    return ::getItemInfo(type);
}

void InventoryItem::setStackWithoutProcessing(unsigned short stack_) {
    inventory->item_counts[(int)type] += (int)stack_ - stack;
    stack = stack_;
}

void InventoryItem::setStack(unsigned short stack_) {
    if(stack != stack_) {
        ServerInventoryItemStackChangeEvent event(*this, stack_);
        event.call();
        
        if(event.cancelled)
            return;
        
        setStackWithoutProcessing(stack_);
        inventory->updateAvailableRecipes();
        if(!stack)
            setType(ItemType::NOTHING);
    }
}

unsigned short InventoryItem::getStack() const {
    return stack;
}

unsigned short InventoryItem::increaseStack(unsigned short stack_) {
    int stack_to_be = stack + stack_, result;
    if(stack_to_be > getUniqueItem().stack_size)
        stack_to_be = getUniqueItem().stack_size;
    result = stack_to_be - stack;
    setStack((unsigned short)stack_to_be);
    return (unsigned short)result;
}

bool InventoryItem::decreaseStack(unsigned short stack_) {
    if(stack_ > stack)
        return false;
    else {
        setStack(stack - stack_);
        return true;
    }
}

unsigned char InventoryItem::getPosInInventory() {
    return this - &inventory->inventory_arr[0];
}

ServerInventory::ServerInventory() {
    for(InventoryItem& i : inventory_arr)
        i = InventoryItem(this);
    
    for(unsigned int& i : item_counts)
        i = 0;
}

char ServerInventory::addItem(ItemType id, int quantity) {
    for(int i = 0; i < INVENTORY_SIZE; i++)
        if(inventory_arr[i].getType() == id) {
            quantity -= inventory_arr[i].increaseStack((unsigned short)quantity);
            if(!quantity)
                return (char)i;
        }
    for(int i = 0; i < INVENTORY_SIZE; i++)
        if(inventory_arr[i].getType() == ItemType::NOTHING) {
            inventory_arr[i].setType(id);
            quantity -= inventory_arr[i].increaseStack((unsigned short)quantity);
            if(!quantity)
                return (char)i;
        }
    return -1;
}

InventoryItem* ServerInventory::getSelectedSlot() {
    return &inventory_arr[(int)(unsigned char)selected_slot];
}

void ServerInventory::swapWithMouseItem(InventoryItem* item) {
    InventoryItem temp = mouse_item;
    mouse_item = *item;
    *item = temp;
}

char* InventoryItem::loadFromSerial(char* iter) {
    setTypeWithoutProcessing((ItemType)*iter++);
    setStackWithoutProcessing(*(short*)iter);
    iter += 2;
    return iter;
}

void InventoryItem::serialize(std::vector<char>& serial) const {
    serial.push_back((char)getType());
    serial.insert(serial.end(), {0, 0});
    *(short*)&serial[serial.size() - 2] = getStack();
}

bool ServerInventory::hasIngredientsForRecipe(const Recipe& recipe) {
    for(const ItemStack& ingredient : recipe.ingredients)
       if(item_counts[(int)ingredient.type] < ingredient.stack)
           return false;
    return true;
}

const std::vector<const Recipe*>& ServerInventory::getAvailableRecipes() {
    return available_recipes;
}

void ServerInventory::updateAvailableRecipes() {
    std::vector<const Recipe*> new_available_recipes;
    for(const Recipe& recipe : getRecipes())
        if(hasIngredientsForRecipe(recipe))
            new_available_recipes.emplace_back(&recipe);
    
    if(available_recipes != new_available_recipes) {
        available_recipes = new_available_recipes;
        RecipeAvailabilityChangeEvent(this).call();
    }
}


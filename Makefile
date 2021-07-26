MODE 				?=		DEBUG
SFML_DIR		?=		Deps/SFML
CC					?=		g++
CFLAGS 			?=		-Wall -std=c++17
CXXFLAGS		?= 		-ILibraries -ILibraries/properties -ILibraries/simplexNoise -ILibraries/events -ILibraries/graphics -ILibraries/graphics/scene -ILibraries/graphics/rect -ILibraries/graphics/ui -ILibraries/packetType -ILibraries/resourcePath -ILibraries/sago -ILibraries/configManager -I$(SFML_DIR)/include
LDFLAGS			?=		-lm -lpthread -lstdc++ -L$(SFML_DIR)/lib -lsfml-system -lsfml-graphics -lsfml-audio -lsfml-network -lsfml-window

ifeq ($(MODE),DEBUG)
	CFLAGS 	+= -Og -g3
else
	ifeq ($(MODE),RELEASE)
		CFLAGS += -march=native -mtune=native -Ofast -pipe
		LDFLAGS += -fno-pie -fdata-sections -ffunction-sections -static-libgcc -static-libstdc++ -s -Wl,--as-needed -Wl,--gc-sections
	else
		@echo "Mode not supported"
	endif
endif

all: client server

server: Server/main.cpp $(wildcard Server/src/*.cpp)
	@echo "Compiling Server"
	@$(CC) $(CFLAGS) $(CXXFLAGS) -IServer/include $(wildcard Server/src/*.cpp) Server/main.cpp Libraries/properties/properties.cpp Libraries/simplexNoise/SimplexNoise.cpp Libraries/graphics/scene/scene.cpp Libraries/graphics/graphics.cpp Libraries/graphics/rect/rect.cpp Libraries/graphics/ui/ui.cpp Libraries/packetType/packetType.cpp Libraries/resourcePath/resourcePath.cpp Libraries/sago/sago.cpp Libraries/configManager/configManager.cpp $(LDFLAGS) -o Terralistic-Server
	@echo "Compiled Server"

client: Client/Sources/main.cpp $(wildcard Client/Sources/**/*.cpp) $(wildcard Client/Sources/**/**/*.cpp)
	@echo "Compiling Client"
	$(CC) $(CFLAGS) $(CXXFLAGS) -IClient/Sources/game -IClient/Sources/game/inventoryHandler -IClient/Sources/game/debugMenu -IClient/Sources/game/clientMap -IClient/Sources/game/playerHandler -IClient/Sources/game/pauseScreen -IClient/Sources/game/clientNetworking -IClient/Sources/game/chat -IClient/Sources/game/textures -IClient/Sources/fileManager -IClient/Sources/menus -IClient/Sources/menus/worldCreator -IClient/Sources/menus/multiplayerSelector -IClient/Sources/menus/worldSelector -IClient/Sources/menus/choiceScreen -IClient/Sources/menus/startMenu -IClient/Sources/menus/textScreen -IServer/serverNetworking -IServer/items -IServer/print -IServer/biomes -IServer/server -IServer/worldGenerator -IServer/blocks -IServer/players Client/Sources/main.cpp $(wildcard Client/Sources/**/*.cpp) $(wildcard Client/Sources/**/**/*.cpp) Libraries/properties/properties.cpp Libraries/simplexNoise/SimplexNoise.cpp Libraries/graphics/scene/scene.cpp Libraries/graphics/graphics.cpp Libraries/graphics/rect/rect.cpp Libraries/graphics/ui/ui.cpp Libraries/packetType/packetType.cpp Libraries/resourcePath/resourcePath.cpp Libraries/sago/sago.cpp Libraries/configManager/configManager.cpp $(wildcard Server/**/*.cpp) $(LDFLAGS) -o Terralistic
	@echo "Compiled Client"


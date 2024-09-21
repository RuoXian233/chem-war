#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>
#include "utils.hpp"


namespace engine {
    // Temprorarily useless
    struct Tile {
        int x;
        int y;
        int w;
        int h;
        int type;
        std::string source;
    };

    struct TileConfiguration {
        std::string tileset;
        int tileTypesCount;
        std::unordered_map<int, Vec2> textureSize;
        int colHeight;
        int placeHolderWidth;
        Vec2 offset = Vec2();
    };

    class TileManager final {
    public:
        using Map = std::vector<std::vector<int>>;

        static void Initalize();
        static TileConfiguration LoadFromFile(const std::string &src);
        static void UnloadCurrentTileSet();
        static const TileConfiguration &GetCurrentTileSet();
        static void AddTile(const std::string &tile, int id);
        static void RemoveTile(int id);
        static void SetMapOffset(const Vec2 &off);
        static void Render();
        static void AddLayer(const std::string &map, const std::string &tileSet = "");
        static void Update(float dt);
        static void SetTile(int x, int y, int type);
        static void SetMap(const std::string &map);
        static void RenderLayer(const Map &map, TileConfiguration &tile);
        // static Tile *GetTile(int x, int y);
        static void ConfigTiles(const TileConfiguration &cfg);
        static int GetTile(int x, int y);
        static void ResetLevel();
        static void Finalize();

        static Map LoadMap(const std::string &file);

    private:
        static Map tileMap;
        static std::vector<std::pair<TileConfiguration, Map>> layers;
        static TileConfiguration currentTileset;
    };
}

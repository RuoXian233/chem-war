#include "tile.h"
#include "resource.h"
#include "log.h"
#include "render.h"
#include <fstream>

using namespace engine;

static Logger logger("TileManager");
std::vector<std::vector<int>> TileManager::tileMap;
TileConfiguration TileManager::currentTileset;
std::vector<std::pair<TileConfiguration, std::vector<std::vector<int>>>> TileManager::layers;


void TileManager::Initalize() {
    INFO("Initalized");
    logger.SetDisplayLevel(GLOBAL_LOG_LEVEL);
}
void TileManager::Finalize() {
    INFO("Finalized");
}

TileConfiguration TileManager::LoadFromFile(const std::string &src) {
    std::fstream fs;
    fs.open(src, std::ios::in);
    if (fs.fail()) {
        ERROR("Tileset configuration load failed");
        ERROR_F("Cause: {}", strerror(errno));
        return {};
    }

    int colHeight;
    int placeHolderWidth;
    std::string line;
    std::string tileSetPath, tileSetName;
    TileConfiguration cfg { "", 0 };
    while (std::getline(fs, line)) {
        if (line.starts_with('[') && line.ends_with(']')) {
            auto tileSetInfo = StringSplit(line.substr(1, line.length() - 2), ",");
            if (tileSetInfo.size() != 4) {
                ERROR_F("Load tile set failed due to syntax error in `{}`", src);
                return {}; 
            }
        
            tileSetName = tileSetInfo[0];
            tileSetPath = tileSetInfo[1];
            std::stringstream ss;
            ss << tileSetInfo[2];
            ss >> colHeight;
            ss.clear();
            ss << tileSetInfo[3];
            ss >> placeHolderWidth;
            ResourceManager::Load(tileSetName + ".tilemap", ResourceType::Texture, tileSetPath);
            cfg.tileset = tileSetName;
        } else {
            int tileId;
            Vec2 tilePos, tileSize;

            auto tileInfo = StringSplit(line, "=");
            if (tileInfo.size() != 2) {
                ERROR_F("Load tile set failed due to syntax error in `{}`", src);
                return {}; 
            } 

            std::stringstream ss;
            ss << tileInfo[0];
            ss >> tileId;

            auto tileRectInfo = StringSplit(tileInfo[1], ",");
            if (tileRectInfo.size() != 4 && tileRectInfo.size() != 1) {
                ERROR_F("Load tile set failed due to syntax error in `{}`", src);
                return {}; 
            }

            if (tileRectInfo.size() == 4) {
                ss.clear();
                ss << tileRectInfo[0];
                ss >> tilePos.x;
                ss.clear();
                ss << tileRectInfo[1];
                ss >> tilePos.y;
                ss.clear();
                ss << tileRectInfo[2];
                ss >> tileSize.x;
                ss.clear();
                ss << tileRectInfo[3];
                ss >> tileSize.y;

                // std::cout << "Tile id=" << tileId << ": " << "rel=" << tilePos.ToString() << ", " << "size=" << tileSize.ToString() << std::endl;
                auto t = Renderer::Clip(
                    ResourceManager::Get(tileSetName + ".tilemap")->GetAs<SDL_Surface>(),
                    tilePos, tileSize
                );
                ResourceManager::RegisterResource(
                    std::format("{}.tilemap.{}", tileSetName, tileId),
                    ResourceType::RenderData,
                    t.textureData
                );
                
            } else if (tileRectInfo.size() == 1) {
                auto img = ResourceManager::OpenRawImage(tileRectInfo[0]);
                tileSize.x = img->w;
                tileSize.y = img->h;
                auto t = Renderer::CreateTexture(img);
                ResourceManager::RegisterResource(
                    std::format("{}.tilemap.{}", tileSetName, tileId),
                    ResourceType::RenderData,
                    t.textureData
                );
            }

            cfg.tileTypesCount++;
            cfg.textureSize[tileId] = tileSize;
        }
    }

    cfg.colHeight = colHeight;
    cfg.placeHolderWidth = placeHolderWidth;
    fs.close();
    return cfg;
}

void TileManager::ConfigTiles(const TileConfiguration &cfg) {
    TileManager::currentTileset = cfg;
    DEBUG_F("Map Height: {}", cfg.colHeight);
}

const TileConfiguration &TileManager::GetCurrentTileSet() {
    return TileManager::currentTileset;
}

TileManager::Map TileManager::LoadMap(const std::string &file) {
    std::fstream fs;
    fs.open(file, std::ios::in);
    if (fs.fail()) {
        ERROR("Tile map load failed");
        ERROR_F("Cause: {}", strerror(errno));
        return {};
    }

    std::string colDef;
    std::vector<int> mapCol;
    std::stringstream ss;
    Map map;
    while (std::getline(fs, colDef)) {
        mapCol.clear();
        for (const auto &s : StringSplit(colDef, ",")) {
            int type;
            ss.clear();
            ss << s;
            ss >> type;
            if (type > TileManager::currentTileset.tileTypesCount || type < -1) {
                ERROR_F("Invaild tile : {}", type);
                return {};
            }
            mapCol.push_back(type);
        }
        map.push_back(mapCol);
    }

    fs.close();
    return map;
}

void TileManager::SetMap(const std::string &map) {
    TileManager::tileMap = TileManager::LoadMap(map);
}

void TileManager::SetMapOffset(const Vec2 &off) {
    TileManager::currentTileset.offset = off;
}

void TileManager::Update(float dt) {
    // Parse tile collisons
}

void TileManager::RenderLayer(const Map &map, TileConfiguration &tile) {
    Vec2 currentPos = tile.offset;
    for (const auto &col : map) {
        for (int type : col) {
            if (type == -1) {
                continue;
            } else if (type == 0) {
                currentPos.x += tile.placeHolderWidth;
                continue;
            }
            Renderer::Texture tileTexture;
            tileTexture.textureData = 
                ResourceManager::Get(
                    std::format("{}.tilemap.{}", tile.tileset, type))->GetAs<SDL_Texture>();
            tileTexture.size = tile.textureSize[type];
            Renderer::RenderTexture(tileTexture, currentPos);
            currentPos.x += tileTexture.size.x;
        }
        currentPos.y += tile.colHeight;
        currentPos.x = tile.offset.x;
    }
}

void TileManager::Render() {
    TileManager::RenderLayer(TileManager::tileMap, TileManager::currentTileset);

    if (!layers.empty()) {
        for (auto &[cfg, layer] : layers) {
            TileManager::RenderLayer(layer, cfg);
        }
    }
}

void TileManager::UnloadCurrentTileSet() {
    TileManager::currentTileset = TileConfiguration {};
}

void TileManager::SetTile(int x, int y, int type) {
    if (y < 0 || y > TileManager::tileMap.size()) {
        ERROR_F("Tile index out of bounds: {}, {}", x, y);
        return;
    }

    if (x < 0 || x > TileManager::tileMap[y].size()) {
        ERROR_F("Tile index out of bounds: {}, {}", x, y);
        return;
    }

    TileManager::tileMap[y][x] = type;
}

int TileManager::GetTile(int x, int y) {
    if (y < 0 || y > TileManager::tileMap.size()) {
        ERROR_F("Tile index out of bounds: {}, {}", x, y);
        return 0;
    }

    if (x < 0 || x > TileManager::tileMap[y].size()) {
        ERROR_F("Tile index out of bounds: {}, {}", x, y);
        return 0;
    }

    return TileManager::tileMap[y][x];
}

void TileManager::AddLayer(const std::string &map, const std::string &tileSet) {
    TileConfiguration c;
    if (!tileSet.empty()) {
        c = TileManager::LoadFromFile(tileSet);
    } else {
        c = TileManager::currentTileset;
    }
    TileManager::layers.push_back(
        std::make_pair(c, TileManager::LoadMap(map))
    );
}

namespace chem_war {
    struct Tile {
        int x;
        int y;
        int type;
    };

    class TileManager final {
    public:
        static void Initalize(const Vec2 &tileSize);
        static void ConfigureTileType(int type, SDL_Texture *t);
        static void Render();
        static void Update();
        static void SetTile(int x, int y, int type);
        static Tile *GetTile(int x, int y);
        static void Finalize();
    private:
        std::vector<std::vector<Tile *>> tileMap;
    };
}
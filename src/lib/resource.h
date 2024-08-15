#pragma once

#include <vector>
#include <memory>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "utils.hpp"

namespace chem_war {
    enum class ResourceType {
        Texture,
        RenderData,
        Font,
        Music,
        Sound,
        PlainText
    };

    enum class ResourceState {
        Good,
        Released
    };

    struct Resource final {
        std::string id;
        ResourceType type;
        ResourceState state;
        void *data;

        template<typename T>
        T *GetAs() {
            return reinterpret_cast<T *>(this->data);
        }
    };

    using ResourcePack = std::vector<std::shared_ptr<Resource>>;

    class ResourceManager final {
    public:
        static void Initialize(int argc, char **argv);
        static std::shared_ptr<Resource> Load(const std::string &id, ResourceType type, const std::string &path);
        
        template<typename T>
        static void RegisterResource(const std::string &id, ResourceType type, T *data)  {
            if (utils_MapHasKey(ResourceManager::resourceDatabase, id)) {
                assert(false && "Resource already exists");
            }
            auto resource = std::make_shared<Resource>();
            resource->type = type;
            resource->id = id;
            resource->state = ResourceState::Good;
            resource->data = reinterpret_cast<void *>(data);
            
            ResourceManager::resourceDatabase.insert(std::make_pair(id, resource));
        }

        static std::shared_ptr<Resource> Get(const std::string &id);
        static void Unload(const std::string &id);
        static void Finalize();
        static void Remove(const std::string &id);

    private:
        static std::map<std::string, std::shared_ptr<Resource>> resourceDatabase;
        static int argc;
        static char **argv;
    };
}

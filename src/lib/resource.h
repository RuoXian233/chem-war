#pragma once

#include <vector>
#include <memory>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "utils.hpp"
#include "log.h"

namespace engine {
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

    enum class LifeCycleSpec {
        Constant,
        Temporary
    };

    struct Resource final {
        std::string id;
        ResourceType type;
        ResourceState state;
        void *data;
        LifeCycleSpec spec = LifeCycleSpec::Constant;

        template<typename T>
        T *GetAs() {
            return reinterpret_cast<T *>(this->data);
        }
    };

    using ResourcePack = std::vector<Resource *>;

    class ResourceManager final {
    public:
        static void Initialize(int argc, char **argv);
        static Resource *Load(const std::string &id, ResourceType type, const std::string &path, LifeCycleSpec spec = LifeCycleSpec::Constant);
        
        template<typename T>
        static void RegisterResource(const std::string &id, ResourceType type, T *data, LifeCycleSpec spec = LifeCycleSpec::Constant)  {
            if (utils_MapHasKey(ResourceManager::resourceDatabase, id)) {
                assert(false && "Resource already exists");
            }
            auto resource = new Resource;
            resource->type = type;
            resource->id = id;
            resource->state = ResourceState::Good;
            resource->data = reinterpret_cast<void *>(data);
            resource->spec = spec;
            
            ResourceManager::resourceDatabase.insert(std::make_pair(id, resource));
        }

        static void Check();

        static Resource *Get(const std::string &id);
        static void Unload(const std::string &id);
        static void Finalize();
        static void Remove(const std::string &id);
        static bool Has(const std::string &id);

        static std::vector<std::string> QueryUnqualified(const std::string &pref);

        static size_t Size();

    private:
        static std::map<std::string, Resource *> resourceDatabase;
        // static std::map<std::string, std::shared_ptr<Resource>> resourceDatabase;
        static int argc;
        static char **argv;
        static bool releasing;
    };
}

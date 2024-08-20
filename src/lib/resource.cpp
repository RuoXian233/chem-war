#include "resource.h"

using namespace chem_war;
std::map<std::string, Resource *> ResourceManager::resourceDatabase;
int ResourceManager::argc;
char **ResourceManager::argv;
bool ResourceManager::releasing;


void ResourceManager::Initialize(int argc, char **argv) {
    ResourceManager::argc = argc;
    ResourceManager::argv = argv;
    ResourceManager::resourceDatabase = decltype(ResourceManager::resourceDatabase)();
    ResourceManager::releasing = false;
}

Resource *ResourceManager::Load(const std::string &id, ResourceType type, const std::string &path) {
    if (utils_MapHasKey(ResourceManager::resourceDatabase, id)) {
        assert(false && "Resource already exists");
    }
    auto resource = new Resource;

    switch (type) {
    case ResourceType::Texture: {
        SDL_Surface *surf = IMG_Load(path.c_str());
        if (!surf) {
            Fatal("Unable to load texture");
        }
        resource->id = id;
        resource->type = type;
        resource->data = reinterpret_cast<void *>(surf);
        resource->state = ResourceState::Good;
        break;
    }
    case ResourceType::Music: {
        Mix_Music *music = Mix_LoadMUS(path.c_str());
        if (!music) {
            Fatal("Unable to load music");
        }
        resource->id = id;
        resource->type = type;
        resource->data = reinterpret_cast<void *>(music);
        resource->state = ResourceState::Good;
        break;
    }
    case ResourceType::Sound: {
        Mix_Chunk *sound = Mix_LoadWAV(path.c_str());
        if (!sound) {
            Fatal("Unable to load sound");
        }
        resource->id = id;
        resource->type = type;
        resource->data = reinterpret_cast<void *>(sound);
        resource->state = ResourceState::Good;
        break;
    }
    default:
        assert(false && "Not implemented");
    }
    
    ResourceManager::resourceDatabase.insert(std::make_pair(id, resource));
    return resource;
}

Resource *ResourceManager::Get(const std::string &id) {
    if (!utils_MapHasKey(ResourceManager::resourceDatabase, id)) {
        assert(false && "Cannot find requested resource");
    }

    auto res = ResourceManager::resourceDatabase.at(id);
    if (res->state != ResourceState::Good) {
        assert(false && "Requested resource is invalid");
    }
    return res;
}

void ResourceManager::Unload(const std::string &id) {
    if (!utils_MapHasKey(ResourceManager::resourceDatabase, id)) {
        assert(false && "Cannot find requested resource");
    }

    auto res = ResourceManager::Get(id);
    switch (res->type) {
    case ResourceType::Texture:
        SDL_FreeSurface(reinterpret_cast<SDL_Surface *>(res->data));
        break;
    case ResourceType::RenderData:
        SDL_DestroyTexture(reinterpret_cast<SDL_Texture *>(res->data));
        break;
    case ResourceType::Music:
        Mix_FreeMusic(reinterpret_cast<Mix_Music *>(res->data));
        break;
    case ResourceType::Sound:
        Mix_FreeChunk(reinterpret_cast<Mix_Chunk *>(res->data));
        break;
    default:
        assert(false && "Not impemented");
    }
    res->state = ResourceState::Released;
}

void ResourceManager::Remove(const std::string &id) {
    if (!utils_MapHasKey(ResourceManager::resourceDatabase, id)) {
        assert(false && "Cannot find requested resource");
    }
    auto res = ResourceManager::resourceDatabase.at(id);
    if (res->state != ResourceState::Released) {
        assert(false && "Resource must be released before removed");
    }

    auto it = ResourceManager::resourceDatabase.find(id);
    ResourceManager::resourceDatabase.erase(it);
}


void ResourceManager::Finalize() {
    for (auto &&[id, res] : ResourceManager::resourceDatabase) {
        if (res->state == ResourceState::Good) {
            ResourceManager::Unload(id);
        }
    }
    ResourceManager::resourceDatabase.clear();
}

size_t ResourceManager::Size() {
    return ResourceManager::resourceDatabase.size();
}

void ResourceManager::Check() {
    if (ResourceManager::Size() > MAX_RESOURCE_TEXTURE_CACHE_SIZE) {
        ResourceManager::releasing = true;
    } 

    if (releasing) {
        int index = 1;
        // for (auto [resId, res] : ResourceManager::resourceDatabase) {
        for (auto it = ResourceManager::resourceDatabase.begin(); it != ResourceManager::resourceDatabase.end(); it++) {
            if (it->second->type == ResourceType::Texture) {
                if (it->second->state == ResourceState::Good) {
                    ResourceManager::Unload(it->first);
                }
            }
        }  
    }
    
    std::vector<std::string> toRemove;
    for (auto it = ResourceManager::resourceDatabase.begin(); it != ResourceManager::resourceDatabase.end(); it++) {
        if (it->second->state == ResourceState::Released) {
            toRemove.push_back(it->first);
        }
    }

    for (auto id : toRemove) {
        ResourceManager::Remove(id);
    }

    if (ResourceManager::Size() <= RESOURCE_ACCUMULATING_MINIMUM + 1) {
        ResourceManager::releasing = false;
    }
}

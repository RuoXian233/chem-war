#include "resource.h"

using namespace engine;
std::map<std::string, Resource *> ResourceManager::resourceDatabase;
int ResourceManager::argc;
char **ResourceManager::argv;
bool ResourceManager::releasing;

static Logger logger("ResourceManager");

void ResourceManager::Initialize(int argc, char **argv) {
    ResourceManager::argc = argc;
    ResourceManager::argv = argv;
    logger.SetDisplayLevel(GLOBAL_LOG_LEVEL);
    INFO("Resource manager initialized");
    DEBUG_F("argc: {}, argv: {}", argc, (void *) argv);
    ResourceManager::resourceDatabase = decltype(ResourceManager::resourceDatabase)();
    ResourceManager::releasing = false;
}

Resource *ResourceManager::Load(const std::string &id, ResourceType type, const std::string &path, LifeCycleSpec spec) {
    if (utils_MapHasKey(ResourceManager::resourceDatabase, id)) {
        FATAL_F("ResourceExistException: id={}->{} Resource already exists", id, path);
        assert(false && "Resource already exists");
    }
    auto resource = new Resource;
    INFO_F("Try to load resource: {}", id);
    logger.StartParagraph(Logger::Level::Debug);
    DEBUG_F("id: {}, type: {}, path: {}", id, (int) type, path);

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
        resource->spec = spec;
        DEBUG_F("Texture loaded: {}", (void *) resource->data);
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
        DEBUG_F("Music loaded: {}", (void *) resource->data);
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
        DEBUG_F("Sound loaded: {}", (void *) resource->data);
        break;
    }
    default:
        assert(false && "Not implemented");
    }
    
    ResourceManager::resourceDatabase.insert(std::make_pair(id, resource));
    logger.EndParagraph();
    DEBUG_F("Current resources count: {}", ResourceManager::Size());
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
    // INFO_F("Try to unload resource: {}", id);

    auto res = ResourceManager::Get(id);
    switch (res->type) {
    case ResourceType::Texture:
        SDL_FreeSurface(reinterpret_cast<SDL_Surface *>(res->data));
        break;
    case ResourceType::RenderData:
        SDL_DestroyTexture(reinterpret_cast<SDL_Texture *>(res->data));
        break;
    case ResourceType::Music:
        DEBUG_F("Regular resource found: {}", (void *) res->data);
        Mix_FreeMusic(reinterpret_cast<Mix_Music *>(res->data));
        break;
    case ResourceType::Sound:
        DEBUG_F("Regular resource found: {}", (void *) res->data);
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
    INFO("Resource manager finalizing");
    INFO("Clearing resource ...");
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
        // logger.StartParagraph(Logger::Level::Debug);

        // DEBUG("Begin querying ...");
        // DEBUG_F("Queried for `character*`: {}", ToString(ResourceManager::QueryUnqualified("character")));

        // logger.EndParagraph();

        logger.StartParagraph(Logger::Level::Debug);
        DEBUG("Clearing texture cache ... [Prepared]");
        int index = 1;
        // for (auto [resId, res] : ResourceManager::resourceDatabase) {
        for (auto it = ResourceManager::resourceDatabase.begin(); it != ResourceManager::resourceDatabase.end(); it++) {
            if (it->second->type == ResourceType::Texture && it->second->spec == LifeCycleSpec::Temporary) {
                if (it->second->state == ResourceState::Good) {
                    ResourceManager::Unload(it->first);
                }
            }
        }
        DEBUG("Clearing texture cache ... [Finished]");

        // logger.StartParagraph(Logger::Level::Debug);

        //DEBUG("Begin querying ...");
        //DEBUG_F("Queried for `character*`: {}", ToString(ResourceManager::QueryUnqualified("character")));

        logger.EndParagraph();
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

    if (!toRemove.empty()) {
        DEBUG_F("Successfully removed {} resources", toRemove.size());
    }

    if (ResourceManager::Size() <= RESOURCE_ACCUMULATING_MINIMUM / 2) {
        logger.EndParagraph();
        ResourceManager::releasing = false;
    }
}


std::vector<std::string> ResourceManager::QueryUnqualified(const std::string &s) {
    std::vector<std::string> candidate;
    for (const auto &[resId, _] : ResourceManager::resourceDatabase) {
        if (resId.starts_with(s)) {
            candidate.emplace_back(resId); 
        }
    }
    return candidate;
}

SDL_Surface *ResourceManager::OpenRawImage(const std::string &img) {
    auto surf = IMG_Load(img.c_str());
    if (surf) {
        return surf;
    } else {
        ERROR_F("Raw image: '{}' load failed", img);
        ERROR_F("Cause: {}", SDL_GetError());
        return nullptr;
    }
}

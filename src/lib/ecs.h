#pragma once
#include "utils.hpp"
#include <vector>
#include <cassert>
#include <memory>
#include <array>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <cstdint>

#define assertm(cond, msgf) assert((cond) && msgf)

#ifndef __linux__
#endif

namespace engine {
    namespace ecs {
        using ComponentID = uint32_t;
        using Entity = uint32_t;

        struct Component {};
        struct Resource {};

        template<typename T>
        class EventStaging final {
        public:
            static bool Has() {
                return data.has_value();
            }

            static void Set(const T &t) {
                data = t;
            }

            static T &Get() {
                return data.value();
            }

            static void Clear() {
                data = std::nullopt;
            }
        private:
            inline static std::optional<T> data = std::nullopt;
        };

        template<typename T>
        class EventReader final {
        public:
            bool Has() {
                return EventStaging<T>::Has();
            }

            T Read() {
                return EventStaging<T>::Get();
            }
        };

        class Events;

        template<typename T>
        class EventWriter final {
        public:
            EventWriter(Events &e) : e(e) {}
            void Write(const T &t);

        private:
            Events &e;
        };


        class World;
        class Events final {
        public:
            friend class World;
            template<typename T>
            friend class EventWriter;

            template<typename T>
            auto Reader();

            template<typename T>
            auto Writer();

        private:
            std::vector<void (*)()> removeEventFuncs;
            std::vector<void (*)()> removeOldEventFuncs;
            std::vector<std::function<void()>> addEventFuncs;

            void RemoveOldEvents() {
                for (auto func : removeOldEventFuncs) {
                    func();
                }
                removeOldEventFuncs = removeEventFuncs;
                removeEventFuncs.clear();
            }

            void AddAllEvents() {
                for (auto func : addEventFuncs) {
                    func();
                }
                addEventFuncs.clear();
            }
        };

        template<typename Category>
        class IndexGetter final {
        public:
            template<typename T>
            static uint32_t Get() {
                static uint32_t id = currentIndex++;
                return id;
            }
        private:
            inline static uint32_t currentIndex = 0;
        };

        template<typename T, typename = std::enable_if<std::is_integral_v<T>>>
        struct IDGenerator final {
            static T Generate() {
                return currentId++;
            }
        private:
            inline static T currentId = {};
        };

        template<typename T, size_t PageSize, typename = std::enable_if<std::is_integral_v<T>>>
        class SparseSet final {
        public:
            void Add(T t) {
                this->density.push_back(t);
                this->Assure(t);
                this->Index(t) = this->density.size() - 1;
            }

            void Remove(T t) {
                if (!Contains(t)) {
                    return;
                }
                auto &idx = this->Index(t);
                if (idx == this->density.size() - 1) {
                    idx = this->null;
                    this->density.pop_back();
                } else {
                    auto last = this->density.back();
                    this->Index(last) = idx;
                    std::swap(this->density[idx], this->density.back());
                    idx = null;
                    this->density.pop_back();
                }
            }

            bool Contains(T t) const {
                assert(t != this->null);

                auto p = this->Page(t);
                auto o = this->Offset(t);
                return p < this->sparse.size() && this->sparse[p]->at(o) != this->null;
            }

            void Clear() {
                this->density.clear();
                this->sparse.clear();
            }

            auto begin() { return this->density.begin(); }
            auto end() { return this->density.end(); }
            static constexpr T null = std::numeric_limits<T>::max();

        private:
            std::vector<T> density;
            std::vector<std::unique_ptr<std::array<T, PageSize>>> sparse;
        
            size_t Page(T t) const {
                return t / PageSize;
            }

            T Index(T t) const {
                return this->sparse[this->Page(t)]->at(this->Offset(t));
            }

            T &Index(T t) {
                return this->sparse[this->Page(t)]->at(this->Offset(t));
            }

            size_t Offset(T t) const {
                return t % PageSize;
            }

            void Assure(T t) {
                auto p = this->Page(t);
                if (p >= this->sparse.size()) {
                    for (size_t i = this->sparse.size(); i <= p; i++) {
                        this->sparse.emplace_back(std::make_unique<std::array<T, PageSize>>());
                        this->sparse[i]->fill(this->null);
                    }
                }
            }
        };

        class Commands;
        class Resources;
        class Querier;
        using UpdateSystem = void (*)(Commands &, Querier, Resources, Events &);
        using StartupSystem = void (*)(Commands &);

        class World final {
        public:
            friend class Commands;
            friend class Resources;
            friend class Querier;
            using ComponentContainer = std::unordered_map<ComponentID, void *>;

            World() = default;
            World(const World &) = delete;
            World &operator=(const World &) = delete;

            World &AddStartupSystem(StartupSystem sys) {
                this->startups.push_back(sys);
                return *this;
            }

            World &AddSystem(UpdateSystem sys) {
                this->updates.push_back(sys);
                return *this;
            }

            template<typename T>
            World &SetResource(T &&resource);

            void Startup();
            void Update();
            void Shutdown() {
                this->entities.clear();
                this->resources.clear();
                this->componentMap.clear();
            }

        private:
            struct Pool final {
                std::vector<void *> instances;
                std::vector<void *> cache;

                using Constructor = void *(*)();
                using Destructor = void (*)(void*);
                Constructor cfunc;
                Destructor dfunc;

                Pool(Constructor c, Destructor d) : cfunc(c), dfunc(d) {
                    //assert(c && "Pool constructor should be not-null");
                    //assert(d && "Pool destructor should be not-null");
                }
                
                void *Create() {
                    if (!cache.empty()) {
                        this->instances.push_back(this->cache.back());
                        this->cache.pop_back();
                    } else {
                        this->instances.push_back(this->cfunc());
                    }
                    return this->instances.back();
                }

                void Destroy(void *element) {
                    if (auto it = std::find(this->instances.begin(), this->instances.end(), element); it != this->instances.end()) {
                        this->cache.push_back(*it);
                        std::swap(*it, this->instances.back());
                        this->instances.pop_back();
                    } else {
                        assert(false && "Element not in pool");
                    }
                }
            };

            struct ComponentInfo {
                Pool pool;
                SparseSet<Entity, 32> sparseSet;
                ComponentInfo(Pool::Constructor c, Pool::Destructor d) : pool(c, d) {}
                ComponentInfo() : pool(nullptr, nullptr) {}
            
                void AddEntity(Entity entity) {
                    this->sparseSet.Add(entity);
                }

                void RemoveEntity(Entity entity) {
                    this->sparseSet.Remove(entity);
                }
            };

            using ComponentMap = std::unordered_map<ComponentID, ComponentInfo>;
            ComponentMap componentMap;
            std::unordered_map<Entity, ComponentContainer> entities;
            Events events;


            struct ResourceInfo {
                void *resource = nullptr;
                using Destructor = void (*)(void *);
                Destructor dtor = nullptr;

                ResourceInfo(Destructor d) : dtor(d) {
                    assert(d && "Resource destructor should be not-null");
                }

                ResourceInfo() = default;

                ~ResourceInfo() {
                    this->dtor(this->resource);
                }
            };

            std::unordered_map<ComponentID, ResourceInfo> resources;
            std::vector<StartupSystem> startups;
            std::vector<UpdateSystem> updates;
        };

        using EntityGenerator = IDGenerator<Entity>;
        class Commands final {
        public:
            Commands(World &world) : world(world) {}

            template<typename ...ComponentTypes>
            Commands &Spawn(ComponentTypes &&...components) {
                this->Spawned<ComponentTypes...>(std::forward<ComponentTypes>(components)...);
                return *this;
            }

            template<typename ...ComponentTypes>
            Entity Spawned(ComponentTypes &&...components) {
                EntitySpawnInfo info;
                info.entity = EntityGenerator::Generate();
                this->DoSpawn(info.entity, info.components, std::forward<ComponentTypes>(components)...);
                // FIXED ?
                this->spawnedEntities.push_back(info);
                return info.entity;
            }

            Commands &Destroy(Entity entity) {
                // if (auto it = this->world.entities.find(entity); it != this->world.entities.end()) {
                //     for (auto [id, component] : it->second) {
                //         auto &componentInfo = this->world.componentMap[id];
                //         componentInfo.pool.Destroy(component);
                //         componentInfo.sparseSet.Remove(entity);

                //     }
                //     this->world.entities.erase(it);
                // }
                destroyedEntities.push_back(entity);
                return *this;
            }

            template<typename T>
            Commands &SetResource(T &&resource) {
                auto index = IndexGetter<Resource>::Get<T>();
                if (auto it = this->world.resources.find(index); it != this->world.resources.end()) {
                    assert(it->second.resource && "Resource already exists");
                    it->second.resource = new T(std::forward<T>(resource));
                } else {
                    auto newIt = this->world.resources.emplace(index, World::ResourceInfo(
                        [](void *elem) { delete (T *) elem; } 
                    ));
                    newIt.first->second.resource = new T;
                    *(T *)(newIt.first->second.resource) = std::forward<T>(resource);
                }
                return *this;
            }

            template<typename T>
            Commands &RemoveResource() {
                auto index = IndexGetter<Resource>::Get<T>();
                // if (auto it = this->world.resources.find(index); it == this->world.resources.end()) {
                //     delete (T *) it->second.resource;
                // }
                this->destroyInfo.push_back(ResourceDestroyInfo(index, [](void *elem) { delete (T *) elem; }));
                return *this;
            }

            void Execute() {
                for (auto info : this->destroyInfo) {
                    this->ExecuteRemoveResource(info);
                }
                for (auto e : this->destroyedEntities) {
                    this->ExecuteDestroy(e);
                }
                for (auto &spawnInfo : spawnedEntities) {
                    auto it = this->world.entities.emplace(spawnInfo.entity, World::ComponentContainer {});
                    auto &compContainer = it.first->second;
                    for (auto &componentInfo : spawnInfo.components) {
                        compContainer[componentInfo.index] = this->DoSpawnWithoutComponentTypes(spawnInfo.entity, componentInfo);
                    }
                }
            }

        private:
            World &world;
            std::vector<Entity> destroyedEntities;

            using DestroyFunc = void (*)(void *);
            using AssignFunc = std::function<void (void *)>;

            struct ResourceDestroyInfo {
                uint32_t index;
                DestroyFunc Destroy;

                ResourceDestroyInfo(uint32_t index, DestroyFunc func) : index(index), Destroy(func) {}
            };
            std::vector<ResourceDestroyInfo> destroyInfo;

            struct ComponentSpawnInfo {
                AssignFunc assign;
                World::Pool::Constructor cfunc;
                World::Pool::Destructor dfunc;
                ComponentID index;
            };

            struct EntitySpawnInfo {
                Entity entity;
                std::vector<ComponentSpawnInfo> components;
            };
            std::vector<EntitySpawnInfo> spawnedEntities;

            template<typename T, typename ...Remains>
            void DoSpawn(Entity entity, std::vector<ComponentSpawnInfo> &spawnInfo, T &&component, Remains &&...remains) {
                ComponentSpawnInfo info;
                info.index = IndexGetter<Component>::Get<T>();
                info.cfunc = []() -> void * {
                    return new T;
                };
                info.dfunc = [](void *elem) {
                    delete (T *) elem;
                };
                info.assign = [=](void *elem) {
                    *((T *) elem) = component;
                };
                spawnInfo.push_back(info);

                if constexpr (sizeof...(Remains) != 0) {
                    DoSpawn<Remains...>(entity, spawnInfo, std::forward<Remains>(remains)...);
                }
            }

            void *DoSpawnWithoutComponentTypes(Entity entity, ComponentSpawnInfo &info) {
                if (auto it = this->world.componentMap.find(info.index); it == this->world.componentMap.end()) {
                    this->world.componentMap.emplace(
                        info.index,
                        World::ComponentInfo(info.cfunc, info.dfunc)
                    );
                }
                auto &componentInfo = this->world.componentMap[info.index];
                void *element = componentInfo.pool.Create();
                // *((T *) element) = std::forward<T>(component);
                info.assign(element);
                componentInfo.sparseSet.Add(entity);
                return element;
            }

            void ExecuteDestroy(Entity entity) {
                if (auto it = this->world.entities.find(entity); it != this->world.entities.end()) {
                    for (auto [id, component] : it->second) {
                        auto &componentInfo = this->world.componentMap[id];
                        componentInfo.pool.Destroy(component);
                        componentInfo.sparseSet.Remove(entity);

                    }
                    this->world.entities.erase(it);
                }
            }

            void ExecuteRemoveResource(ResourceDestroyInfo &info) {
                if (auto it = this->world.resources.find(info.index); it == this->world.resources.end()) {
                    info.Destroy(it->second.resource);
                }
            }
        };

        class Resources final {
        public:
            Resources(World &world) : world(world) {}
        
            template<typename T>
            bool Has() const {
                auto index = IndexGetter<Resource>::Get<T>();
                auto it = this->world.resources.find(index);
                return it != this->world.resources.end() && it->second.resource;
            }

            template<typename T>
            T &Get() {
                auto index = IndexGetter<Resource>::Get<T>();
                return *((T *) world.resources[index].resource);
            }
        private:
            World &world;
        };

        class Querier final {
        public:
            Querier(World &world) : world(world) {}
    
            template<typename ...Components>
            std::vector<Entity> Query() {
                std::vector<Entity> entities;
                DoQuery<Components...>(entities);
                return entities;
            }

            template<typename T>
            bool Has(Entity entity) {
                auto it = world.entities.find(entity);
                auto index = IndexGetter<Component>::Get<T>();
                return it != world.entities.end() && it->second.find(index) != it->second.end();
            }

            template<typename T>
            T &Get(Entity entity) {
                auto index = IndexGetter<Component>::Get<T>();
                return *((T *) world.entities[entity][index]);
            }

        private:
            World &world;

            template<typename T, typename ...Remains>
            void DoQuery(std::vector<Entity> &out) {
                auto index = IndexGetter<Component>::Get<T>();
                World::ComponentInfo &info = world.componentMap[index];

                for (auto e : info.sparseSet) {
                    if constexpr (sizeof...(Remains) != 0) {
                        // return DoQueryRemains<Remains...>(e, out);
                        DoQueryRemains<Remains...>(e, out);
                    } else {
                        out.push_back(e);
                    }
                }
                // return !out.empty();
            }

            template<typename T, typename ...Remains>
            void DoQueryRemains(Entity entity, std::vector<Entity> &out) {
                auto index = IndexGetter<Component>::Get<T>();
                auto &componentContainer = world.entities[entity];
                if (auto it = componentContainer.find(index); it == componentContainer.end()) {
                    //return false;
                    return;
                }

                if constexpr (sizeof...(Remains) == 0) {
                    out.push_back(entity);
                    // return true;
                } else {
                    //return DoQueryRemains<Remains...>(entity, out);
                    DoQueryRemains<Remains...>(entity, out);
                }
            }
        };

        inline void World::Startup() {
            std::vector<Commands> commandList;
            for (auto sys : this->startups) {
                Commands commands(*this);
                sys(commands);
                commandList.push_back(commands);
            }

            for (auto &command : commandList) {
                command.Execute();
            }
        }

        inline void World::Update() {
            std::vector<Commands> commandList;
            for (auto sys : this->updates) {
                Commands commands(*this);
                sys(commands, Querier(*this), Resources(*this), events);
                commandList.push_back(commands);
            }
            events.RemoveOldEvents();
            events.AddAllEvents();

            for (auto &command : commandList) {
                command.Execute();
            }
        }

        template<typename T>
        World &World::SetResource(T &&resource) {
            Commands command(*this);
            command.SetResource(std::forward<T>(resource));
            return *this;
        }


        template<typename T>
        auto Events::Reader() {
            return EventReader<T>();
        }

        template<typename T>
        auto Events::Writer() {
            return EventWriter<T> {*this};
        }

        template<typename T>
        void EventWriter<T>::Write(const T &t) {
            e.addEventFuncs.push_back([=]() {
                EventStaging<T>::Set(t);
            });
            e.removeEventFuncs.push_back([]() {
                EventStaging<T>::Clear();
            });
        }
    }
}

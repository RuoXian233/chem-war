#pragma once

#include <string>
#include <map>
#include "object.h"
#include "ecs.h"


namespace engine {

    class Scene {
    public:
        Scene(ecs::World &world, const std::string &prev, const std::string &next);
        Scene(ecs::World &world); 
        virtual void Enter();
        virtual void Exit();
        virtual void Update(float dt);
        virtual void Render();
        virtual void OnAdd();
        virtual void OnFirstEnter();
        virtual void OnRemove();
        virtual ~Scene();

        std::string prevScene;
        std::string nextScene;
        std::map<std::string, GameObject *> objects;
        std::map<std::string, GameObject *> borrowedObjects;
        bool firstEnter = true;

        template<typename T = GameObject>
        T *GetObject(const std::string &id) {
            if (this->objects.find(id) == this->objects.end()) {
                if (this->borrowedObjects.find(id)!= this->borrowedObjects.end()) {
                    assert(false && "Object not exists");
                }
                return reinterpret_cast<T *>(this->borrowedObjects[id]);
            } else {
                return reinterpret_cast<T *>(this->objects[id]);            
            }
        }

        void AddObject(GameObject *go);
        void AddBorrowedObject(GameObject *go);
        ecs::World &world;
    };

    class SceneManager final {
    public:
        static void Initialize();

        inline static std::string GetCurrentSceneName() { return currentScene; }
        inline int static GetSceneCount() { return scenes.size(); }
        static void AddScene(const std::string &sceneName, Scene *scene);
        static void SwitchScene(const std::string &sceneName);
        static void Proceed();
        static void Back();
        static void RemoveScene(const std::string &sceneName);
        static Scene *GetCurrentScene();
        static void DeleteScene(const std::string &sceneName);
        static Scene *GetScene(const std::string &sceneName);

        static void Finalize();
        static void Update(float dt);
        static void Render();

    private:
        static inline std::string currentScene = "none";
        static std::map<std::string, Scene *> scenes;

        // TODO: implement shared objects
        static std::vector<std::string, GameObject *> sharedObjects;
    };
}

#include "scene.h"
#include <cassert>

using namespace engine;
std::map<std::string, Scene*> SceneManager::scenes;
static engine::Logger logger("SceneManager");

Scene::Scene(ecs::World &world, const std::string &prev, const std::string &next) : world(world), prevScene(prev), nextScene(next) {}
Scene::Scene(ecs::World &world) : prevScene("none"), nextScene("none"), world(world) {}
Scene::~Scene() {
    for (auto &go : objects) {
        delete go.second;
    }
}


void Scene::Enter() {
    if (this->firstEnter) {
        this->firstEnter = false;
        this->OnFirstEnter();
    }
}
void Scene::Exit() {}
void Scene::OnFirstEnter() {}

void Scene::OnAdd() {}
void Scene::OnRemove() {}


void Scene::Update(float dt) {
    for (auto &go : objects) {
        go.second->Update(dt);
    }

    for (auto &go : borrowedObjects) {
        go.second->Update(dt);
    }
}

void Scene::Render() {
    for (auto &go : objects) {
        go.second->Render();
    }

    for (auto &go : borrowedObjects) {
        go.second->Render();
    }
}

void Scene::AddObject(GameObject *go) {
    this->objects.insert(std::make_pair(go->GetId(), go));
}

void Scene::AddBorrowedObject(GameObject *go) {
    this->borrowedObjects.insert(std::make_pair(go->GetId(), go));
}


void SceneManager::Initialize() {
    INFO("Initialized");
    logger.SetDisplayLevel(Logger::Level::Debug);
}
void SceneManager::Finalize() {
    INFO("Finalizing ...");
    std::vector<std::string> sceneNames;
    for (auto &[sceneName, s] : scenes) {
        DEBUG_F("Scene to release: `{}`", sceneName);
        sceneNames.push_back(sceneName);
    }

    for (auto &sceneName : sceneNames) {
        auto s = scenes[sceneName];
        RemoveScene(sceneName);
        delete s;
    }
    SceneManager::scenes.clear();
}

void SceneManager::AddScene(const std::string &name, Scene *scene) {
    if (scenes.find(name) == scenes.end()) {
        scenes[name] = scene;
        scene->OnAdd();
        DEBUG_F("Added scene `{}`", name);
    } else {
        assert(false && "Scene already exists");   
    }
}

Scene *SceneManager::GetCurrentScene() {
    if (!scenes.empty() && scenes.find(currentScene) != scenes.end()) {
        return scenes[currentScene];
    } else {
        return nullptr;
    }
}

Scene *SceneManager::GetScene(const std::string &name) {
    if (scenes.find(name) != scenes.end()) {
        return scenes[name];
    } else {
        return nullptr;
    }
}

void SceneManager::SwitchScene(const std::string &name) {
    auto s = GetCurrentScene();
    if (s) {
        s->Exit();
    }

    if (scenes.find(name) != scenes.end()) {
        currentScene = name;
        s = GetCurrentScene();
        s->Enter();
        DEBUG_F("Switched to scene `{}`", name);
    } else {
        assert(false && "Scene does not exist");
    }
}

void SceneManager::Proceed() {
    INFO("Proceeding to next scene");
    auto s = GetCurrentScene();
    if (s->nextScene != "none" && scenes.find(s->nextScene) != scenes.end()) {
        SwitchScene(s->nextScene);
    }
}

void SceneManager::Back() {
    auto s = GetCurrentScene();
    INFO("Try to roll back to previous scene");
    if (s->prevScene != "none" && scenes.find(s->prevScene) != scenes.end()) {
        SwitchScene(s->prevScene);
    }
}

void SceneManager::RemoveScene(const std::string &name) {
    if (scenes.find(name) != scenes.end()) {
        auto s = scenes[name];
        s->OnRemove();
        scenes.erase(name);
        DEBUG_F("Removed scene `{}`", name);
    } else {
        assert(false && "Scene does not exist");
    }
}

void SceneManager::DeleteScene(const std::string &name) {
    if (scenes.find(name) != scenes.end()) {
        auto s = scenes[name];
        RemoveScene(name);
        delete s;
    } else {
        assert(false && "Scene does not exist");
    }
}


void SceneManager::Update(float dt) {
    auto s = GetCurrentScene();
    if (s) {
        s->Update(dt);
    }
}

void SceneManager::Render() {
    auto s = GetCurrentScene();
    if (s) {
        s->Render();
    }
}


void SceneManager::PrintSceneHeirarchy() {
    logger.StartParagraph(Logger::Level::Debug);
    if (!SceneManager::GetCurrentScene()) {
        DEBUG("Scene is empty");
        logger.EndParagraph();
        return;
    }
    auto scene = SceneManager::GetCurrentScene();
    DEBUG_F("[Target scene: {}]", SceneManager::GetCurrentSceneName());
    
    std::function<void(int, GameObject*)> PrintGameObject = [&](int depth, GameObject *go) {
        std::cout << std::string(depth * 2, ' ') << "|-" << go->GetId();
        auto childrens = go->GetChildrens();
        if (!childrens.empty()) {
            for (auto child : childrens) {
                PrintGameObject(depth + 1, child);
            }
        } else {
            std::cout << std::endl;
        }
    };
    
    for (auto &go : scene->objects) {
        PrintGameObject(2, go.second);
    }
    logger.EndParagraph();
}

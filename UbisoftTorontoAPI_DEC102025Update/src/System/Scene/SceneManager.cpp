#include "SceneManager.h"
#include <iostream>

void SceneManager::RegisterScene(const std::string& name, std::unique_ptr<IScene> scene) {
    scenes[name] = std::move(scene);
}

void SceneManager::SwitchToScene(const std::string& name) {
    // Check if scene exists
    auto it = scenes.find(name);
    if (it == scenes.end()) {
        std::cerr << "Scene '" << name << "' not found!" << std::endl;
        return;
    }
    
    // Exit current scene if one is active
    if (currentScene) {
        currentScene->OnExit();
    }
    
    // Switch to new scene
    currentScene = it->second.get();
    currentSceneName = name;
    
    // Enter the new scene
    if (currentScene) {
        currentScene->OnEnter();
    }
}

void SceneManager::Update(float deltaTimeMs) {
    if (currentScene) {
        currentScene->Update(deltaTimeMs);
    }
}

void SceneManager::Render() {
    if (currentScene) {
        currentScene->Render();
    }
}

std::string SceneManager::GetCurrentSceneName() const {
    return currentSceneName;
}

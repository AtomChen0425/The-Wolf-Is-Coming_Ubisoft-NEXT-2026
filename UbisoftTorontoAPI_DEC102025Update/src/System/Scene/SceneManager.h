#pragma once
#include <string>
#include <memory>
#include <unordered_map>

// Forward declaration to avoid circular dependency
class UIManager;

// Forward declarations
class EntityManager;
struct Camera3D;

// Base Scene Interface
class IScene {
public:
    virtual ~IScene() = default;
    
    // Scene lifecycle methods
    virtual void OnEnter() = 0;      // Called when entering the scene
    virtual void OnExit() = 0;       // Called when exiting the scene
    virtual void Update(float deltaTimeMs) = 0;  // Update logic
    virtual void Render() = 0;       // Render logic
    
    // Get scene name
    virtual std::string GetName() const = 0;
};

// Scene Manager - manages scene transitions
class SceneManager {
public:
    SceneManager() = default;
    ~SceneManager() = default;
    
    // Register a scene with a name
    void RegisterScene(const std::string& name, std::unique_ptr<IScene> scene);
    
    // Switch to a different scene
    void SwitchToScene(const std::string& name);
    
    // Update current scene
    void Update(float deltaTimeMs);
    
    // Render current scene
    void Render();
    
    // Get current scene name
    std::string GetCurrentSceneName() const;
    
    // Get current scene (for direct access)
    IScene* GetCurrentScene() const { return currentScene; }
    
private:
    std::unordered_map<std::string, std::unique_ptr<IScene>> scenes;
    IScene* currentScene = nullptr;
    std::string currentSceneName;
};

#include "ControlSystem.h"
#include "../../ContestAPI/app.h"
#include "../../ContestAPI/AppSettings.h"
#include "../../System/Component/Component.h"
#include "GenerateSystem.h"
#include <cmath>
#include "Math/Math.h"

void PlayerControl3D(EntityManager& registry, float dt, float& nextSpawnZ, Camera3D& camera, const GameSettings& settings, const GameConfig& config) {
    float dtSec = dt / 1000.0f;
    float playerCurrentZ = 0.0f;
    
    // Get constants from config
    float jumpVelocity = config.jumpVelocity;
    float forwardSpeed = config.forwardSpeed;
    float strafeSpeed = config.strafeSpeed;
    const float rotationSpeed = config.rotationSpeed;
    float gravity = config.gravity;
    float bulletSpeed = config.bulletSpeed;
    
    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& playerTag = view.get<PlayerTag>(id);
        auto& playerTransform = view.get<Transform3D>(id);
        auto& vel = view.get<Velocity3D>(id).vel;
        Vec3& pos = playerTransform.pos;
        
        // Apply player stats upgrades if they have PlayerStats component
        if (view.has<PlayerStats>(id)) {
            auto& stats = view.get<PlayerStats>(id);
            forwardSpeed += stats.speedBonus;
            strafeSpeed += stats.speedBonus;
            jumpVelocity += stats.jumpBonus;
            gravity += stats.gravityBonus;  // Note: gravityBonus is negative, so this reduces gravity
            bulletSpeed += stats.bulletSpeedBonus;
        }
        
        forwardSpeed += playerTag.score;
        
        // === Camera Control Logic (moved from CameraSystem) ===
        if (settings.cameraControlMode == CameraControlMode::Mouse) {
            // Mouse control mode
            float mouseX, mouseY;
            App::GetMousePos(mouseX, mouseY);
            
            // Initialize mouse position on first frame
            if (!camera.mouseInitialized) {
                camera.lastMouseX = mouseX;
                camera.lastMouseY = mouseY;
                camera.mouseInitialized = true;
            }
            
            // Calculate mouse delta (sensitivity adjusted)
            const float mouseSensitivity = config.mouseSensitivity;
            float deltaX = (mouseX - camera.lastMouseX) * mouseSensitivity;
            float deltaY = (mouseY - camera.lastMouseY) * mouseSensitivity;
            
			playerTag.rotationYaw += deltaX; // Rotate player with camera
            // Update camera rotation based on mouse movement
			playerTag.rotationPitch -= deltaY; // Invert to match typical FPS controls
            // Vertical mouse movement controls pitch (up/down viewing)
            
            // Store current mouse position for next frame
            camera.lastMouseX = mouseX;
            camera.lastMouseY = mouseY;
        } else {
            // Arrow keys control mode
            if (App::IsKeyPressed(App::KEY_LEFT)) {
				playerTag.rotationYaw -= rotationSpeed * dtSec;
            }
            if (App::IsKeyPressed(App::KEY_RIGHT)) {
				playerTag.rotationYaw += rotationSpeed * dtSec;
            }
            if (App::IsKeyPressed(App::KEY_UP)) {
				playerTag.rotationPitch += rotationSpeed * dtSec;

            }
            if (App::IsKeyPressed(App::KEY_DOWN)) {
				playerTag.rotationPitch -= rotationSpeed * dtSec;
            }
        }
        // === End Camera Control Logic ===
        
        // 1. Handle horizontal input (forward/backward and strafe)
        // Use damping for smooth movement instead of hard reset
        float inputX = 0.0f;
        float inputZ = 0.0f;
        
        if (App::IsKeyPressed(App::KEY_A)) inputX = -1.0f;
        if (App::IsKeyPressed(App::KEY_D)) inputX = 1.0f;
        if (App::IsKeyPressed(App::KEY_W)) inputZ = 1.0f;
        if (App::IsKeyPressed(App::KEY_S)) inputZ = -1.0f;
        
        // Apply input with smooth acceleration/deceleration
        const float acceleration = config.acceleration;
        const float damping = config.damping;
        
        if (inputX != 0.0f) {
            vel.x += inputX * acceleration * dtSec;
            // Clamp to max speed
            if (vel.x > 1.0f) vel.x = 1.0f;
            if (vel.x < -1.0f) vel.x = -1.0f;
        } else {
            // Apply damping when no input
            vel.x *= (1.0f - damping * dtSec);
            if (std::abs(vel.x) < 0.01f) vel.x = 0.0f;
        }
        
        if (inputZ != 0.0f) {
            vel.z += inputZ * acceleration * dtSec;
            // Clamp to max speed
            if (vel.z > 1.0f) vel.z = 1.0f;
            if (vel.z < -1.0f) vel.z = -1.0f;
        } else {
            // Apply damping when no input
            vel.z *= (1.0f - damping * dtSec);
            if (std::abs(vel.z) < 0.01f) vel.z = 0.0f;
        }


        // 3. Handle jump input (only when on ground - collision system sets this flag)
        if (App::IsKeyPressed(App::KEY_SPACE) && playerTag.isOnGround) {
            vel.y = jumpVelocity;
        }

        // 4. Apply velocity to position
        // CollisionSystem will detect and resolve any collisions after this
		Vec3 dirSpeed = Vec3(strafeSpeed, 1.0f, forwardSpeed);
        float cosAngle = std::cos(playerTag.rotationYaw);
        float sinAngle = -std::sin(playerTag.rotationYaw);
		Vec3 localMove = vel * dirSpeed * dtSec;

        Vec4 worldMove4 = RotateByY(playerTag.rotationYaw, Vec4(localMove.x, localMove.y, localMove.z, 0.0f));
        pos += Vec3(worldMove4.x, worldMove4.y, worldMove4.z);
        // Record player's current Z position for map generation
            
        playerCurrentZ = pos.z;
    }
    
    // 6. Update map generation system based on player position
    GenerateSystem::MapGenerationSystem(registry, playerCurrentZ, nextSpawnZ, config);
}

void FireControl(EntityManager& registry, float dt, const GameConfig& config) {
    float dtSec = dt / 1000.0f;
	float bulletSpeed = config.bulletSpeed;
    View<PlayerTag, Transform3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& playerTag = view.get<PlayerTag>(id);
        playerTag.shootCooldown -= dt;
        auto& playerTransform = view.get<Transform3D>(id);
        
        // Apply player stats bonuses if they exist
        if (view.has<PlayerStats>(id)) {
            auto& stats = view.get<PlayerStats>(id);
            bulletSpeed += stats.bulletSpeedBonus;
        }

        if ((App::IsKeyPressed(App::KEY_J) || App::IsMousePressed(0)) && playerTag.shootCooldown <= 0.0f) {
            // Fire a bullet
            Vec3 bulletDirection = Vec3{std::cos(playerTag.rotationPitch) * std::sin(playerTag.rotationYaw),
                                        std::sin(playerTag.rotationPitch), 
                                        std::cos(playerTag.rotationPitch) * std::cos(playerTag.rotationYaw)};
            Vec3 bulletPosition = playerTransform.pos + bulletDirection * 2.0f; // Spawn bullet slightly in front of player

            // Create bullet entity
            Entity bullet = registry.createEntity();
			registry.addComponent(bullet, Bullet{ bulletDirection ,bulletSpeed, 1000,10,true});
            registry.addComponent(bullet, Transform3D{ bulletPosition, 5.0f, 5.0f, 5.0f, config.bulletColorR, config.bulletColorG, config.bulletColorB });
            registry.addComponent(bullet, Velocity3D{ bulletDirection * bulletSpeed });
            playerTag.shootCooldown = 100.0f;
        }
    }
}

void ControlSystem::Update(EntityManager& registry, float dt, float& nextSpawnZ, Camera3D& camera, const GameSettings& settings, const GameConfig& config)
{
    PlayerControl3D(registry, dt, nextSpawnZ, camera, settings, config);
    FireControl(registry, dt, config);
}

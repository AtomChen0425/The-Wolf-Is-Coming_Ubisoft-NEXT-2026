#include "RenderSystem.h"
#include "../../System/Render/RenderHelper.h"
#include "../../System/Component/Component.h"
#include "../../ContestAPI/app.h"
#include <cmath>
RenderHelper* gRenderHelper;

void DrawSprite(EntityManager& registry) {
    View<Position, SpriteComponent> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Position>(id);
        auto& spr = view.get<SpriteComponent>(id);

        if (spr.sprite) {
            spr.sprite->SetPosition(pos.pos.x, pos.pos.y);

            spr.sprite->Draw();
        }
    }
}
void DrawEnemies(EntityManager& registry) {
    View<EnemyTag, Position> enemyView(registry);
    for (EntityID id : enemyView) {
        auto& enemypos = enemyView.get<Position>(id);
        gRenderHelper->DrawQuad(enemypos.pos, 20.0f, 1.0f, 0.0f, 0.0f);
    }
}
void UpdateSpriteAnimation(EntityManager& registry, const float dt) {
    View<SpriteComponent> view(registry);
    for (EntityID id : view) {
        auto& spr = view.get<SpriteComponent>(id);
        auto& spriteVel = view.get<Velocity3D>(id);
        if (spr.sprite) {
            spr.sprite->Update(dt);
            if (spriteVel.vel.z > 0.1f) {
                if (spr.currentAnimID != ANIM_FORWARDS) {
                    spr.sprite->SetAnimation(ANIM_FORWARDS, true);
                    spr.currentAnimID = ANIM_FORWARDS;
                }
            }
            else if (spriteVel.vel.z < -0.1f) {
                if (spr.currentAnimID != ANIM_BACKWARDS) {
                    spr.sprite->SetAnimation(ANIM_BACKWARDS, true);
                    spr.currentAnimID = ANIM_BACKWARDS;
                }
            }
            else if (spriteVel.vel.x > 0.1f) {
                if (spr.currentAnimID != ANIM_RIGHT) {
                    spr.sprite->SetAnimation(ANIM_RIGHT, true);
                    spr.currentAnimID = ANIM_RIGHT;
                }
            }
            else if (spriteVel.vel.x < -0.1f) {
                if (spr.currentAnimID != ANIM_LEFT) {
                    spr.sprite->SetAnimation(ANIM_LEFT, true);
                    spr.currentAnimID = ANIM_LEFT;
                }
            }
            else {
                spr.sprite->SetAnimation(0, true);
                spr.currentAnimID = 0;

            }
        }
    }
}

//void RenderSystem25D(EntityManager& registry, Camera25D& camera) {
//    View<Position3D, SpriteComponent> view(registry);
//
//    std::vector<EntityID> sortedEntities;
//    for (EntityID id : view) sortedEntities.push_back(id);
//    std::sort(sortedEntities.begin(), sortedEntities.end(), [&](EntityID a, EntityID b) {
//        return view.get<Position3D>(a).z < view.get<Position3D>(b).z;
//        });
//
//    for (EntityID id : sortedEntities) {
//        auto& pos = view.get<Position3D>(id);
//        auto& spr = view.get<SpriteComponent>(id);
//
//        if (!spr.sprite) continue;
//
//        float screenGroundX = pos.x - camera.x;
//        float screenGroundY = pos.z - camera.y;
//
//        if (screenGroundX < -100 || screenGroundX > camera.width + 100 ||
//            screenGroundY < -100 || screenGroundY > camera.height + 100) {
//            continue;
//        }
//
//        float shadowSize = 20.0f;
//        gRenderHelper->DrawShadow(screenGroundX, screenGroundY, shadowSize);
//
//        float spriteScreenY = screenGroundY - pos.y;
//
//        spr.sprite->SetPosition(screenGroundX, spriteScreenY);
//        spr.sprite->Draw();
//        App::Print(screenGroundX, spriteScreenY - 30.0f, ("X:" + std::to_string((int)pos.x) + "Y:" + std::to_string((int)pos.z)).c_str(), 1.0f, 1.0f, 0.0f);
//
//    }
//}

// Calculate the minimum depth (closest point) of a cube in camera space
// This is used for proper z-sorting to avoid occlusion issues
static float MaxDepthInCameraSpace(const Transform3D& t, const Camera3D& camera) {
    const float halfW = t.width * 0.5f;
    const float halfH = t.height * 0.5f;
    const float halfD = t.depth * 0.5f;

    // 8 corners (world space)
    const Vec3 corners[8] = {
        Vec3(t.pos.x - halfW, t.pos.y - halfH, t.pos.z - halfD),
        Vec3(t.pos.x + halfW, t.pos.y - halfH, t.pos.z - halfD),
        Vec3(t.pos.x + halfW, t.pos.y + halfH, t.pos.z - halfD),
        Vec3(t.pos.x - halfW, t.pos.y + halfH, t.pos.z - halfD),
        Vec3(t.pos.x - halfW, t.pos.y - halfH, t.pos.z + halfD),
        Vec3(t.pos.x + halfW, t.pos.y - halfH, t.pos.z + halfD),
        Vec3(t.pos.x + halfW, t.pos.y + halfH, t.pos.z + halfD),
        Vec3(t.pos.x - halfW, t.pos.y + halfH, t.pos.z + halfD)
    };

    const float cosYaw = std::cos(camera.yawAngle);
    const float sinYaw = std::sin(camera.yawAngle);
    const float cosPitch = std::cos(camera.pitchAngle);
    const float sinPitch = std::sin(camera.pitchAngle);

    float maxZ = -1e30f;

    for (int i = 0; i < 8; ++i) {
        // to camera-relative
        const float rx = corners[i].x - camera.x;
        const float ry = corners[i].y - camera.y;
        const float rz = corners[i].z - camera.z + camera.followOffsetZ;

        // First rotate around Y (yaw)
        const float rotatedX = rx * cosYaw + rz * sinYaw;
        const float rotatedZ = -rx * sinYaw + rz * cosYaw;

        // Then apply pitch
        const float pitchedZ = ry * sinPitch + rotatedZ * cosPitch;

        if (pitchedZ > maxZ) maxZ = pitchedZ;
    }

    return maxZ;
}
void RenderRoad3D(EntityManager& registry, Camera3D& camera) {
    // Only render map blocks, not the player
    View<Transform3D, MapBlockTag> view(registry);
    std::vector<EntityID> sortedEntities;
    for (EntityID id : view) {
        sortedEntities.push_back(id);
    }

    //// Sort by minimum depth in camera space for proper rendering order
    //// Painter's algorithm: render far to near (larger depth values first)
    //std::sort(sortedEntities.begin(), sortedEntities.end(), [&](EntityID a, EntityID b) {
    //    auto& ta = view.get<Transform3D>(a);
    //    auto& tb = view.get<Transform3D>(b);

    //    float distA = MaxDepthInCameraSpace(ta, camera);
    //    float distB = MaxDepthInCameraSpace(tb, camera);
    //    
    //    // If blocks are at the same position, render taller blocks first
    //    if (ta.pos.x == tb.pos.x && ta.pos.z == tb.pos.z) { 
    //        return ta.pos.y > tb.pos.y; 
    //    }
    //    
    //    
    //    return distA < distB;
    //});

    for (EntityID id : sortedEntities) {
        auto& t = view.get<Transform3D>(id);
        //RenderCube(t, camera);
        gRenderHelper->RenderCube_inNDC(t, camera);
    }
}

void RenderPlayer3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, PlayerTag> view(registry);

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        //RenderPlayerCube(t, camera);
        gRenderHelper->RenderCube_inNDC(t, camera);
        // Display player position info
        App::Print(50, 50,
            ("Player Pos - X:" + std::to_string((int)t.pos.x) +
                " Y:" + std::to_string((int)t.pos.y) +
                " Z:" + std::to_string((int)t.pos.z)).c_str(),
            1.0f, 1.0f, 1.0f);

        // Display controls info
        //App::Print(50, 80, "Controls: WASD to move, SPACE to jump, Arrow keys to rotate", 1.0f, 1.0f, 0.0f);
    }
}
void RenderAnimal3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, AnimalTag> view(registry);

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        //RenderPlayerCube(t, camera);
        gRenderHelper->RenderCube_inNDC(t, camera);
    }
}

void RenderBullets3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, Bullet> bulletView(registry);

    for (EntityID id : bulletView) {
        auto& t = bulletView.get<Transform3D>(id);
        // Render bullets as small cubes
        gRenderHelper->RenderCube_inNDC(t, camera);
    }
}

void RenderParticles(EntityManager& registry, Camera3D& camera) {
    View<Transform3D, ParticleTag> view(registry);

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        gRenderHelper->RenderCube_inNDC(t, camera);
    }
}

// Render all characters (player, wolves, sheep) with proper depth sorting
void RenderSprite3D(EntityManager& registry, Camera3D& camera) {
    // Collect all characters into a single list with their transforms
    std::vector<std::tuple<EntityID, Transform3D*, SpriteComponent* >> allCharacters;

    View<Transform3D, SpriteComponent> SpriteView(registry);

    for (EntityID id : SpriteView) {
        allCharacters.push_back({ id, &SpriteView.get<Transform3D>(id) , &SpriteView.get<SpriteComponent>(id) });
    }

    // Sort by depth in camera space (far to near for proper occlusion)
    std::sort(allCharacters.begin(), allCharacters.end(), [&](const auto& a, const auto& b) {
        float distA = MaxDepthInCameraSpace(*std::get<1>(a), camera);
        float distB = MaxDepthInCameraSpace(*std::get<1>(b), camera);
        return distA > distB;  // Render far entities first (painter's algorithm)
        });

    // Render all characters in sorted order
    for (const auto& [id, transform, sprite] : allCharacters) {
        //if (SpriteView.has<SpriteComponent>(id)) {
    /*    auto& t = SpriteView.get<Transform3D>(id);
        auto& spr = SpriteView.get<SpriteComponent>(id);*/
        RenderSystem::RenderSpriteByPos(*transform, *sprite, camera);
        //RenderSystem::RenderPlayerSprite(registry, camera);
   /* }
    else {
        gRenderHelper->RenderCube_inNDC(*transform, camera);
    }*/
    }
}
void RenderSystem::Render(EntityManager& registry) {
    DrawSprite(registry);
    DrawEnemies(registry);
}
//void RenderSystem::Render(EntityManager& registry, Camera25D& camera) {
//    RenderSystem25D(registry, camera);
//}
void RenderSystem::Render(EntityManager& registry, Camera3D& camera) {
    // Render map blocks 
    RenderRoad3D(registry, camera);

    // Render bullets 
    RenderBullets3D(registry, camera);

    //Render Blocks3D(registry, camera);
    /*RenderPlayer3D(registry, camera);
    RenderAnimal3D(registry, camera);*/

    // To switch to sprite rendering,

    RenderSprite3D(registry, camera);

    // Render particles
    RenderParticles(registry, camera);

    // Render animals (wolves and sheep)
    // To switch to sprite rendering, comment out RenderAnimal3D and uncomment the sprite functions below

    // RenderWolvesSprite(registry, camera);
    // RenderSheepSprite(registry, camera);

    // Render crosshair on top of everything
    RenderCrosshair(registry, camera);

}
void RenderSystem::Update(EntityManager& registry, const float dt) {
    UpdateSpriteAnimation(registry, dt);
}


void RenderSystem::RenderCrosshair(EntityManager& registry, Camera3D& camera) {
    // Find the player to get their position and rotation
    View<Transform3D, PlayerTag> playerView(registry);

    bool playerFound = false;
    float crosshairScreenX = 512.0f;  // Default to screen center
    float crosshairScreenY = 384.0f;

    for (EntityID id : playerView) {
        auto& t = playerView.get<Transform3D>(id);
        auto& player = playerView.get<PlayerTag>(id);

        // Calculate dynamic aim distance based on camera pitch and player height
        const float baseDist = 500.0f;
        const float pitchFactor = std::tan(camera.pitchAngle);  // Positive when looking up, negative when looking down
        const float heightAboveGround = t.pos.y;

        // Calculate horizontal distance based on pitch angle and height
        float aimDistance = baseDist;
        if (pitchFactor < -0.01f) {
            // Looking down - calculate distance to ground intersection
            float distToGround = std::abs(heightAboveGround / pitchFactor);
            aimDistance = std::min(distToGround, baseDist * 2.0f);  // Cap at 2x base distance
        }
        else if (pitchFactor > 0.01f) {
            // Looking up - increase distance
            aimDistance = baseDist * (1.0f + pitchFactor * 2.0f);  // Scale up with pitch
            aimDistance = std::min(aimDistance, 1000.0f);  // Cap at reasonable max
        }

        // Calculate world position in front of player using player's yaw rotation
        const float playerYaw = player.rotationYaw;
        const float worldX = t.pos.x + aimDistance * std::sin(playerYaw);
        const float worldY = t.pos.y + aimDistance * pitchFactor;  // Height changes with pitch
        const float worldZ = t.pos.z + aimDistance * std::cos(playerYaw);

        // Project this 3D world point to screen using NDC projection
        // Camera-relative position
        const float rx = worldX - camera.x;
        const float ry = worldY - camera.y;
        const float rz = worldZ - camera.z;

        // Apply yaw rotation
        const float cosYaw = std::cos(camera.yawAngle);
        const float sinYaw = std::sin(camera.yawAngle);
        const float rotatedX = rx * cosYaw + rz * sinYaw;
        const float rotatedZ = -rx * sinYaw + rz * cosYaw;

        // Apply pitch rotation
        const float cosPitch = std::cos(camera.pitchAngle);
        const float sinPitch = std::sin(camera.pitchAngle);
        const float pitchedY = ry * cosPitch - rotatedZ * sinPitch;
        const float pitchedZ = ry * sinPitch + rotatedZ * cosPitch;

        // Check if point is in front of camera
        const float nearZ = 1.0f;
        if (pitchedZ > nearZ) {
            // Project to screen
            const float fov = camera.fov;
            const float centerX = camera.screenWidth / 2.0f;
            const float centerY = camera.screenHeight / 2.0f;

            crosshairScreenX = rotatedX * (fov / pitchedZ) + centerX;
            crosshairScreenY = pitchedY * (fov / pitchedZ) + centerY;
            playerFound = true;
        }

        break;
    }

    // Crosshair size and gap
    const float lineLength = 15.0f;  // Length of each line from center
    const float gap = 5.0f;          // Gap from center point

    // Crosshair color (bright green for visibility)
    const float r = 1.0f;
    const float g = 0.0f;
    const float b = 0.0f;

    // Draw horizontal line (left and right)
    App::DrawLine(crosshairScreenX - lineLength - gap, crosshairScreenY,
        crosshairScreenX - gap, crosshairScreenY, r, g, b);
    App::DrawLine(crosshairScreenX + gap, crosshairScreenY,
        crosshairScreenX + lineLength + gap, crosshairScreenY, r, g, b);

    // Draw vertical line (top and bottom)
    App::DrawLine(crosshairScreenX, crosshairScreenY - lineLength - gap,
        crosshairScreenX, crosshairScreenY - gap, r, g, b);
    App::DrawLine(crosshairScreenX, crosshairScreenY + gap,
        crosshairScreenX, crosshairScreenY + lineLength + gap, r, g, b);

    // Draw center dot for precision
    App::DrawLine(crosshairScreenX - 1, crosshairScreenY,
        crosshairScreenX + 1, crosshairScreenY, r, g, b);
    App::DrawLine(crosshairScreenX, crosshairScreenY - 1,
        crosshairScreenX, crosshairScreenY + 1, r, g, b);
}

void RenderSystem::RenderSpriteByPos(Transform3D& t, SpriteComponent& spr, Camera3D& camera) {
    const float rx = t.pos.x - camera.x;
    const float ry = t.pos.y - camera.y;
    const float rz = t.pos.z - camera.z;

    // Apply yaw rotation
    const float cosYaw = std::cos(camera.yawAngle);
    const float sinYaw = std::sin(camera.yawAngle);
    const float rotatedX = rx * cosYaw + rz * sinYaw;
    const float rotatedZ = -rx * sinYaw + rz * cosYaw;

    // Apply pitch rotation
    const float cosPitch = std::cos(camera.pitchAngle);
    const float sinPitch = std::sin(camera.pitchAngle);
    const float pitchedY = ry * cosPitch - rotatedZ * sinPitch;
    const float pitchedZ = ry * sinPitch + rotatedZ * cosPitch;

    // Skip if behind camera
    const float nearZ = 1.0f;
    if (pitchedZ <= nearZ) return;

    // Project to screen using NDC approach (same as RenderCube_inNDC)
    const float fov = camera.fov;
    const float centerX = camera.screenWidth / 2.0f;  // Fixed screen center
    const float centerY = camera.screenHeight / 2.0f;

    const float finalScreenX = rotatedX * (fov / pitchedZ) + centerX;
    float finalScreenY = pitchedY * (fov / pitchedZ) + centerY;

    float spriteNativeWidth = spr.sprite->GetWidth(); 
	float spriteNativeHeight = spr.sprite->GetHeight();
    float perspectiveFactor = fov / pitchedZ;
    if (spriteNativeWidth > 0.0f) {
        float scale = (t.width / spriteNativeWidth) * perspectiveFactor * 3;
		finalScreenY += spriteNativeHeight * 0.2f * scale; // Adjust Y to account for sprite height
        spr.sprite->SetScale(scale);
    }
    // Shadow on ground (y = 0 in world space)
    const float groundY = 15.0f - camera.y;
    const float groundPitchedY = groundY * cosPitch - rotatedZ * sinPitch;
    const float groundPitchedZ = groundY * sinPitch + rotatedZ * cosPitch;

    if (groundPitchedZ > nearZ) {
        const float shadowScreenY = groundPitchedY * (fov / groundPitchedZ) + centerY;
        const float shadowScreenX = finalScreenX; // Same X as sprite
        gRenderHelper->DrawShadow(shadowScreenX, shadowScreenY, t.width * 0.8f);
    }

    // Set sprite position and draw
    spr.sprite->SetPosition(finalScreenX, finalScreenY);
    spr.sprite->Draw();

}

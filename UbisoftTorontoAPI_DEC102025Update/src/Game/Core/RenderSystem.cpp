#include "RenderSystem.h"
#include "../System/Render/RenderHelper.h"
#include "../System/Component/Component.h"
#include "../ContestAPI/app.h"
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
    // 遍历所有有 SpriteComponent 和 Position 的实体
    View<SpriteComponent> view(registry);
    for (EntityID id : view) {
        auto& spr = view.get<SpriteComponent>(id);
        if (spr.sprite) {
            spr.sprite->Update(dt);
        }
    }
}
bool Project(float wx, float wy, float wz, const Camera3D& cam, float& outX, float& outY) {
    // 1. 将世界坐标转换为相对于相机的坐标 (View Space)
    float rx = wx - cam.x;
    float ry = wy - cam.y;
    float rz = wz - cam.z;

    // 2. 裁剪：如果点在相机后面或刚好在相机位置，不绘制 (防止除零错误和错误的投影)
    if (rz <= 1.0f) return false;

    // 3. 透视除法 (核心)
    // fov (Field of View) 焦距，决定视野有多广。值越大，看起来越近。
    float fov = 600.0f;
    // 屏幕中心点 (假设窗口 1024x768)
    float centerX = 1024.0f / 2.0f;
    float centerY = 768.0f / 2.0f;

    // 公式：屏幕坐标 = 相对坐标 * (焦距 / 相对深度) + 屏幕中心偏移
    outX = rx * (fov / rz) + centerX;
    // 注意这里的负号：3D世界Y向上是正，屏幕Y向下是正，所以要反转
    outY = -ry * (fov / rz) + centerY;

    return true;
}
void RenderSystem25D(EntityManager& registry, Camera25D& camera) {
    View<Position3D, SpriteComponent> view(registry);

    // 排序逻辑 (Y-Sorting) - 可选，但建议加上
    std::vector<EntityID> sortedEntities;
    for (EntityID id : view) sortedEntities.push_back(id);
    std::sort(sortedEntities.begin(), sortedEntities.end(), [&](EntityID a, EntityID b) {
        return view.get<Position3D>(a).z < view.get<Position3D>(b).z;
        });

    for (EntityID id : sortedEntities) {
        auto& pos = view.get<Position3D>(id);
        auto& spr = view.get<SpriteComponent>(id);

        if (!spr.sprite) continue;

        // ===========================================
        // 关键算式：减去 Camera 偏移
        // ===========================================

        // 1. 计算【地面】在屏幕上的位置
        // 屏幕X = 世界X - 相机X
        float screenGroundX = pos.x - camera.x;
        // 屏幕Y = 世界Z - 相机Y
        float screenGroundY = pos.z - camera.y;

        // 视锥剔除 (Culling): 如果在屏幕外就不画
        if (screenGroundX < -100 || screenGroundX > camera.width + 100 ||
            screenGroundY < -100 || screenGroundY > camera.height + 100) {
            continue;
        }

        // 2. 绘制阴影 (永远画在地面位置)
        float shadowSize = 20.0f;
        // 你的 DrawLine 可能需要 RGB，这里用黑色 (0,0,0)
		gRenderHelper->DrawShadow(screenGroundX, screenGroundY, shadowSize);

        // 3. 绘制人物 (加上跳跃高度偏移)
        // 只有这里用到了 pos.y！
        float spriteScreenY = screenGroundY - pos.y;

        spr.sprite->SetPosition(screenGroundX, spriteScreenY);
        spr.sprite->Draw();
		App::Print(screenGroundX, spriteScreenY - 30.0f, ("X:"+ std::to_string((int)pos.x) + "Y:" + std::to_string((int)pos.z)).c_str(), 1.0f, 1.0f, 0.0f);
		
    }
}

void RenderRoad3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D,MapBlockTag> view(registry);

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);

        // 我们用立方体的“顶面”来代表物体。顶面有 4 个角点。
        float halfW = t.width / 2; float halfD = t.depth / 2; float topY = t.y + t.height / 2;

        // 计算4个角的世界坐标
        float x1 = t.x - halfW; float z1 = t.z - halfD; // 左后
        float x2 = t.x + halfW; float z2 = t.z - halfD; // 右后
        float x3 = t.x + halfW; float z3 = t.z + halfD; // 右前
        float x4 = t.x - halfW; float z4 = t.z + halfD; // 左前

        float sx1, sy1, sx2, sy2, sx3, sy3, sx4, sy4;

        // 将4个点投影到屏幕上
        // 【关键】这里传入了当前的 camera 对象
        if (!Project(x1, topY, z1, camera, sx1, sy1)) continue;
        if (!Project(x2, topY, z2, camera, sx2, sy2)) continue;
        if (!Project(x3, topY, z3, camera, sx3, sy3)) continue;
        if (!Project(x4, topY, z4, camera, sx4, sy4)) continue;

        // 用两个三角形拼成一个矩形顶面
        // App::DrawTriangle 的 Z 参数这里传 1.0f 即可，因为我们已经手动计算了透视
        //App::DrawTriangle(sx1, sy1, 1, 1, sx2, sy2, 1, 1, sx3, sy3, 1, 1, t.r, t.g, t.b, t.r, t.g, t.b, t.r, t.g, t.b, true);
        //App::DrawTriangle(sx1, sy1, 1, 1, sx3, sy3, 1, 1, sx4, sy4, 1, 1, t.r, t.g, t.b, t.r, t.g, t.b, t.r, t.g, t.b, true);
        gRenderHelper->DrawQuad(Vec2((sx1 + sx2 + sx3 + sx4) / 4, (sy1 + sy2 + sy3 + sy4) / 4),t.width, t.r, t.g, t.b);
		App::Print((sx1 + sx2 + sx3 + sx4) / 4, (sy1 + sy2 + sy3 + sy4) / 4, ("X:" + std::to_string((int)t.x) + "Y:" + std::to_string((int)t.z)).c_str(), 255.0f, 1.0f, 0.0f);
    }
}
void RenderPlayer3D(EntityManager& registry, Camera3D& camera) {
    View<Transform3D,PlayerTag> view(registry);

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);

        // 我们用立方体的“顶面”来代表物体。顶面有 4 个角点。
        float halfW = t.width / 2; float halfD = t.depth / 2; float topY = t.y + t.height / 2;

        // 计算4个角的世界坐标
        float x1 = t.x - halfW; float z1 = t.z - halfD; // 左后
        float x2 = t.x + halfW; float z2 = t.z - halfD; // 右后
        float x3 = t.x + halfW; float z3 = t.z + halfD; // 右前
        float x4 = t.x - halfW; float z4 = t.z + halfD; // 左前

        float sx1, sy1, sx2, sy2, sx3, sy3, sx4, sy4;

        // 将4个点投影到屏幕上
        // 【关键】这里传入了当前的 camera 对象
        if (!Project(x1, topY, z1, camera, sx1, sy1)) continue;
        if (!Project(x2, topY, z2, camera, sx2, sy2)) continue;
        if (!Project(x3, topY, z3, camera, sx3, sy3)) continue;
        if (!Project(x4, topY, z4, camera, sx4, sy4)) continue;

        // 用两个三角形拼成一个矩形顶面
        // App::DrawTriangle 的 Z 参数这里传 1.0f 即可，因为我们已经手动计算了透视
        //App::DrawTriangle(sx1, sy1, 1, 1, sx2, sy2, 1, 1, sx3, sy3, 1, 1, t.r, t.g, t.b, t.r, t.g, t.b, t.r, t.g, t.b, true);
        //App::DrawTriangle(sx1, sy1, 1, 1, sx3, sy3, 1, 1, sx4, sy4, 1, 1, t.r, t.g, t.b, t.r, t.g, t.b, t.r, t.g, t.b, true);
        gRenderHelper->DrawQuad(Vec2((sx1 + sx2 + sx3 + sx4) / 4, (sy1 + sy2 + sy3 + sy4) / 4), t.width, t.r, t.g, t.b);
        App::Print(600, 600, ("X:" + std::to_string((int)t.x) + "Y:" + std::to_string((int)t.z)).c_str(), 0.0f, 255.0f, 0.0f);
        App::Print(600, 550, ("X:" + std::to_string((sx1 + sx2 + sx3 + sx4) / 4) + "Y:" + std::to_string((sy1 + sy2 + sy3 + sy4) / 4)).c_str(), 0.0f, 255.0f, 0.0f);
    }
}
void RenderSystem::Render(EntityManager& registry) {
    DrawSprite(registry);
    DrawEnemies(registry);
}
void RenderSystem::Render(EntityManager& registry, Camera25D& camera) {
    RenderSystem25D(registry, camera);
}
void RenderSystem::Render(EntityManager& registry, Camera3D& camera) {
    RenderRoad3D(registry, camera);
    RenderPlayer3D(registry, camera);
}
void RenderSystem::Update(EntityManager& registry, const float dt) {
	UpdateSpriteAnimation(registry, dt);
}
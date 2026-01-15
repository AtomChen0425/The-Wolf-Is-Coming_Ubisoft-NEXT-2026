#include "ParticleSystem.h"
#include <cmath>
#include "Math.h"


void ParticleSystem::Update(EntityManager& registry, float dt) {
    View<Transform3D, ParticlePhysics, ParticleTag> view(registry);
    std::vector<EntityID> toDestroy;

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        auto& p = view.get<ParticlePhysics>(id);

        // 减少寿命
        p.life -= dt;
        if (p.life <= 0) {
            toDestroy.push_back(id);
            continue;
        }

        // 应用重力 (简单的 Euler 积分)
        // 假设 dt 是毫秒，我们需要调整系数
        float dtSeconds = dt / 1000.0f;
        p.velocity.y -= p.gravity * 98.0f * dtSeconds; // 98.0 是个大概的重力缩放

        // 更新位置
        t.pos.x += p.velocity.x * dtSeconds;
        t.pos.y += p.velocity.y * dtSeconds;
        t.pos.z += p.velocity.z * dtSeconds;

        // 视觉效果：随寿命减少而变小
        float lifeRatio = p.life / p.maxLife;
        // 让粒子从初始大小缩小到 0
        // 注意：这里简单的将大小设为 ratio * 初始大小 (这就需要我们在 ParticlePhysics 里存初始大小，或者只做相对缩小)
        // 为了简单，我们让它根据 ratio 缩放一个基础值
        // 如果想更严谨，可以在 ParticlePhysics 里存 startSize
        float currentSize = t.width * (p.life / (p.life + dt)); // 每一帧按比例缩小一点点，或者直接重新赋值
        // 这里采用更简单的方式：直接根据生命周期重置大小有点麻烦，不如让它慢慢变小
        // 简单做法：每帧减小固定数值，直到消失
        // t.width = std::max(0.1f, t.width * 0.95f);
        // t.height = t.width;
        // t.depth = t.width;

        // 更好的做法：使用 sin 或 ratio 平滑过渡
        // 这里我们假设粒子创建时已经设定好 Transform3D，我们只负责更新位置。
        // 如果要缩放效果：
        t.width *= 0.95f; // 快速缩小
        t.height *= 0.95f;
        t.depth *= 0.95f;
    }

    // 销毁死亡粒子
    for (EntityID id : toDestroy) {
        registry.destroyEntity(id);
    }

    // --------------------------------------------------------
    // 2. 处理拖尾 (Trail Emitters)
    // --------------------------------------------------------
    View<Transform3D, TrailEmitter> emitterView(registry);
    for (EntityID id : emitterView) {
        auto& t = emitterView.get<Transform3D>(id);
        auto& emitter = emitterView.get<TrailEmitter>(id);

        emitter.timeSinceLast += dt;
        if (emitter.timeSinceLast >= emitter.interval) {
            emitter.timeSinceLast = 0; // 或者 -= interval

            // 生成一个原地不动的粒子 (或者稍微有点漂移)
            Entity e = registry.createEntity();
            registry.addComponent(e, ParticleTag{});
            registry.addComponent(e, Transform3D{ t.pos, emitter.size, emitter.size, emitter.size, emitter.r, emitter.g, emitter.b });
            // 拖尾粒子通常不需要重力，也不需要很快的速度
            registry.addComponent(e, ParticlePhysics{ Vec3{0, 0, 0}, emitter.particleLife, emitter.particleLife, 0.0f });
        }
    }
}

void ParticleSystem::CreateExplosion(EntityManager& registry, Vec3 pos, int count, Vec3 color, float speed) {
    for (int i = 0; i < count; ++i) {
        Entity e = registry.createEntity();
        registry.addComponent(e, ParticleTag{});

        float size = RandRange(5.0f, 10.0f);
        registry.addComponent(e, Transform3D{ pos, size, size, size, color.x, color.y, color.z });

        // 随机爆炸方向
        Vec3 vel;
        vel.x = RandRange(-1.0f, 1.0f);
        vel.y = RandRange(0.5f, 2.0f); // 倾向于向上炸
        vel.z = RandRange(-1.0f, 1.0f);

        // 归一化并乘以速度
        float len = std::sqrt(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);
        if (len > 0) { vel.x /= len; vel.y /= len; vel.z /= len; }

        float randomSpeed = RandRange(speed * 0.5f, speed * 1.5f);
        vel.x *= randomSpeed;
        vel.y *= randomSpeed;
        vel.z *= randomSpeed;

        float life = RandRange(1000.0f, 1500.0f); // 1.0 到 1.5 秒

        registry.addComponent(e, ParticlePhysics{ vel, life, life, 1.0f }); // 开启重力
    }
}
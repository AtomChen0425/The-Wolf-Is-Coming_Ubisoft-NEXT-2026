#include "ParticleSystem.h"
#include <cmath>
#include "Math.h"


void ParticleSystem::Update(EntityManager& registry, float dt) {
    View<Transform3D, ParticlePhysics, ParticleTag> view(registry);
    std::vector<EntityID> toDestroy;

    for (EntityID id : view) {
        auto& t = view.get<Transform3D>(id);
        auto& p = view.get<ParticlePhysics>(id);

        // ºı…Ÿ Ÿ√¸
        p.life -= dt;
        if (p.life <= 0) {
            toDestroy.push_back(id);
            continue;
        }

        float dtSeconds = dt / 1000.0f;
        p.velocity.y -= p.gravity * 98.0f * dtSeconds; 

        t.pos.x += p.velocity.x * dtSeconds;
        t.pos.y += p.velocity.y * dtSeconds;
        t.pos.z += p.velocity.z * dtSeconds;

        float lifeRatio = 1 - (dt / p.life);

        t.width *= lifeRatio; 
        t.height *= lifeRatio;
        t.depth *= lifeRatio;
    }


    for (EntityID id : toDestroy) {
        registry.destroyEntity(id);
    }

    View<Transform3D, TrailEmitter> emitterView(registry);
    for (EntityID id : emitterView) {
        auto& t = emitterView.get<Transform3D>(id);
        auto& emitter = emitterView.get<TrailEmitter>(id);

        emitter.timeSinceLast += dt;
        if (emitter.timeSinceLast >= emitter.interval) {
            emitter.timeSinceLast = 0;

            Entity e = registry.createEntity();
            registry.addComponent(e, ParticleTag{});
            registry.addComponent(e, Transform3D{ t.pos, emitter.size, emitter.size, emitter.size, emitter.r, emitter.g, emitter.b });
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

        Vec3 vel;
        vel.x = RandRange(-1.0f, 1.0f);
        vel.y = RandRange(0.5f, 2.0f);
        vel.z = RandRange(-1.0f, 1.0f);

        float len = std::sqrt(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);
        if (len > 0) { vel.x /= len; vel.y /= len; vel.z /= len; }

        float randomSpeed = RandRange(speed * 0.5f, speed * 1.5f);
        vel.x *= randomSpeed;
        vel.y *= randomSpeed;
        vel.z *= randomSpeed;

        float life = RandRange(1000.0f, 1500.0f); 

        registry.addComponent(e, ParticlePhysics{ vel, life, life, 1.0f }); 
    }
}
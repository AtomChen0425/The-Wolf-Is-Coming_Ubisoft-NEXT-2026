#include "PhysicsSystem.h"
#include "../../System/Component/Component.h"
void PhysicsSystem3D(EntityManager& registry, float dtMs) {
    float dtSec = dtMs / 1000.0f;
    float gravity = -9.8f * 100.0f; //
    View<Position3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Position3D>(id);
        auto& vel = view.get<Velocity3D>(id).vel;


        vel.y += gravity * dtSec;

        pos.x += vel.x * dtSec;
        pos.z += vel.z * dtSec;
        pos.y += vel.y * dtSec;


        if (pos.y < 0.0f) {
            pos.y = 0.0f;
            vel.y = 0.0f;
        }
    }
}
void PhysicsSystem::Update(EntityManager& registry, const float dtMs) {
    PhysicsSystem3D(registry, dtMs);
}
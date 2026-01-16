#include "PhysicsSystem.h"
#include "../../System/Component/Component.h"
void PhysicsSystem3D(EntityManager& registry, float dtMs) {
    float dtSec = dtMs / 1000.0f;
    float gravity = -9.8f * 100.0f; //
    View<Transform3D, Velocity3D, PhysicsTag> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Transform3D>(id).pos;
        auto& vel = view.get<Velocity3D>(id).vel;
		auto& tag = view.get<PhysicsTag>(id);
        vel.y += gravity * dtSec;
        if (tag.isOnGround && vel.y < 0.0f) {
            vel.y = 0.0f;
		}
        

        pos.x += vel.x * dtSec;
        pos.z += vel.z * dtSec;
        pos.y += vel.y * dtSec;
    }
}

void PhysicsSystem::Update(EntityManager& registry, const float dtMs) {
    PhysicsSystem3D(registry, dtMs);
}
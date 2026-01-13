#include "PhysicsSystem.h"
#include "../../System/Component/Component.h"
void PhysicsSystem25D(EntityManager& registry, float dtMs) {
    float dtSec = dtMs / 1000.0f;
    float gravity = -9.8f * 100.0f; // ����ϵ��

    View<Position3D, Velocity3D> view(registry);
    for (EntityID id : view) {
        auto& pos = view.get<Position3D>(id);
        auto& vel = view.get<Velocity3D>(id);

        // 1. Ӧ������ (ֻӰ��߶� Y)
        vel.vy += gravity * dtSec;

        // 2. ����λ��
        pos.x += vel.vx * dtSec;
        pos.z += vel.vz * dtSec; // �ذ��ϵ�ǰ���ƶ�
        pos.y += vel.vy * dtSec; // ���е�������Ծ

        // 3. ������ײ��� (�������߶��� 0)
        if (pos.y < 0.0f) {
            pos.y = 0.0f;
            vel.vy = 0.0f;
            // ����������� "CanJump" ���
        }
    }
}
void PhysicsSystem::Update(EntityManager& registry, const float dtMs) {
    PhysicsSystem25D(registry, dtMs);
}
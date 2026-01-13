#include "CollisionSystem.h"
#include "../System/Component/Component.h"
#include "../System/Physic/Collision.h"
extern Collision* gCollision;
void CheckPlayerEnemyCollision(EntityManager& registry) {
	View<PlayerTag, Position,Velocity, RigidBody,Health> playerView(registry);
	View<EnemyTag, Position> enemyView(registry);
	EntityID playerId{};
	Vec2 playerPos;
	float playerRadius = 0.0f;
	for (EntityID id : playerView) {
		playerId = id;
		playerPos = playerView.get<Position>(id).pos;
		playerRadius = playerView.get<RigidBody>(id).radius;
		break;
	}

	for (EntityID enemyId : enemyView) {
		auto& enemyPos = enemyView.get<Position>(enemyId);
		float enemyRadius = enemyView.get<RigidBody>(enemyId).radius;
		if (gCollision->Circle(playerPos, playerRadius, enemyPos.pos, enemyRadius)) {
			auto& playerHealth = playerView.get<Health>(playerId);
			playerHealth.currentHealth -= 10; // 假设碰撞扣10点血

			auto& playerRigidBody = playerView.get<RigidBody>(playerId);

			Vec2 dir = Normalize(playerPos - enemyPos.pos); // 从敌人指向玩家
			const float knockbackImpulse = 10.0f;              

			playerRigidBody.force = dir * knockbackImpulse;
			registry.destroyEntity(Entity{ enemyId, registry.getEntityVersion(enemyId) }); // 碰撞后销毁敌人实体
			break; // 碰撞后退出循环，避免多次扣血
		}
	}
}

void CollisionSystem::Update(EntityManager& registry) {
	CheckPlayerEnemyCollision(registry);
}
		
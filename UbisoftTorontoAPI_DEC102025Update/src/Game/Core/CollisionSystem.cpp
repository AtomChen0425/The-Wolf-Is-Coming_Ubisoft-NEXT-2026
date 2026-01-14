#include "CollisionSystem.h"
#include "../../System/Component/Component.h"
#include "../../System/Physic/Collision.h"

Collision* gCollision;
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
			playerHealth.currentHealth -= 10; 

			auto& playerRigidBody = playerView.get<RigidBody>(playerId);

			Vec2 dir = Normalize(playerPos - enemyPos.pos); 
			const float knockbackImpulse = 10.0f;              

			playerRigidBody.force = dir * knockbackImpulse;
			registry.destroyEntity(Entity{ enemyId, registry.getEntityVersion(enemyId) }); 
			break; 
		}
	}
}

void CheckPlayerOnTheFloor(EntityManager& registry) {
	View<PlayerTag, Transform3D> playerView(registry);
	EntityID playerId{};
	Transform3D playerTransform;
	for (EntityID id : playerView) {
		playerId = id;
		playerTransform = playerView.get<Transform3D>(id);
		break;
	}
	Vec3 playerPos = playerTransform.pos;
	Vec3 playerMin(playerPos.x - playerTransform.width / 2, playerPos.y - playerTransform.height / 2, playerPos.z - playerTransform.depth / 2);
	Vec3 playerMax(playerPos.x + playerTransform.width / 2, playerPos.y + playerTransform.height / 2, playerPos.z + playerTransform.depth / 2);
	View<Collider3D, Transform3D> floorView(registry);
	auto& player = playerView.get<PlayerTag>(playerId);
	//float groundY = 0.0f;
	for (EntityID floorId : floorView) {
		auto& floorCollider = floorView.get<Collider3D>(floorId);
		if (!floorCollider.isFloor) continue; // Skip non-floor objects

		auto& floorTransform = floorView.get<Transform3D>(floorId);
		Vec3 floorPos = floorTransform.pos;
		Vec3 floorMin(floorPos.x - floorTransform.width / 2, floorPos.y - floorTransform.height / 2, floorPos.z - floorTransform.depth / 2);
		Vec3 floorMax(floorPos.x + floorTransform.width / 2, floorPos.y + floorTransform.height / 2, floorPos.z + floorTransform.depth / 2);

		//if (playerMin.x < floorMax.x && playerMax.x > floorMin.x &&
		//	playerMin.z < floorMax.z && playerMax.z > floorMin.z) {
		//	// Player is horizontally aligned with this floor
		//	float floorTop = floorMax.y;
		//	if (floorTop > groundY) {
		//		groundY = floorTop;
		//	}
		//}
		// Check if player is standing on this floor
		if (playerMin.x < floorMax.x && playerMax.x > floorMin.x &&
			playerMin.z < floorMax.z && playerMax.z > floorMin.z &&
			playerMin.y <= floorMax.y + 0.1f && playerMin.y >= floorMax.y - 5.0f) {

			player.isOnGround = true;
			break;
		}
	}
	/*auto& playerVelocity = playerView.get<Velocity3D>(playerId);
	if (playerPos.y - playerTransform.height / 2 < groundY) {
		playerPos.y = groundY + playerTransform.height / 2;
		playerVelocity.vy = 0.0f;
	}*/
}

void CollisionSystem::Update(EntityManager& registry) {
	//CheckPlayerEnemyCollision(registry);
	CheckPlayerOnTheFloor(registry);
}
		
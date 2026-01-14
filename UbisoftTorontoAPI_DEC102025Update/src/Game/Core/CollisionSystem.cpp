#include "CollisionSystem.h"
#include "../System/Component/Component.h"
#include "../System/Physic/Collision.h"
#include <vector>
#include <algorithm>
extern Collision* gCollision;

// ========================================
// Collision System
// ========================================
// Handles all collision detection and response:
// - Bullet vs Enemy: Damage enemies, grant experience
// - Player vs Enemy: Damage player, apply knockback

// ========================================
// Player-Enemy Collision
// ========================================
void CheckPlayerEnemyCollision(EntityManager& registry) {
	View<PlayerTag, Position3D, Velocity, RigidBody, Health> playerView(registry);
	View<EnemyTag, Position3D, RigidBody> enemyView(registry);
	EntityID playerId{};
	Vec2 playerPos;
	float playerRadius = 0.0f;
	for (EntityID id : playerView) {
		playerId = id;
		auto& pos3d = playerView.get<Position3D>(id);
		playerPos = { pos3d.x, pos3d.z };
		playerRadius = playerView.get<RigidBody>(id).radius;
		break;
	}

	for (EntityID enemyId : enemyView) {
		auto& enemyPos3d = enemyView.get<Position3D>(enemyId);
		Vec2 enemyPos = { enemyPos3d.x, enemyPos3d.z };
		float enemyRadius = enemyView.get<RigidBody>(enemyId).radius;
		
		if (gCollision->Circle(playerPos, playerRadius, enemyPos, enemyRadius)) {
			auto& playerHealth = playerView.get<Health>(playerId);
			playerHealth.currentHealth -= 10; // ������ײ��10��Ѫ

			auto& playerRigidBody = playerView.get<RigidBody>(playerId);

			Vec2 dir = Normalize(playerPos - enemyPos); // �ӵ���ָ�����
			const float knockbackImpulse = 10.0f;              

			playerRigidBody.force = dir * knockbackImpulse;
			registry.destroyEntity(Entity{ enemyId, registry.getEntityVersion(enemyId) }); // ��ײ�����ٵ���ʵ��
			break; // ��ײ���˳�ѭ���������ο�Ѫ
		}
	}
}

// ========================================
// Bullet-Enemy Collision
// ========================================
// Detects when bullets hit enemies, applies damage, and grants experience
void CheckBulletEnemyCollision(EntityManager& registry) {
	View<BulletTag, Position3D, RigidBody, BulletComponent> bulletView(registry);
	View<EnemyTag, Position3D, RigidBody, Health, EnemyTypeComponent> enemyView(registry);
	
	std::vector<EntityID> bulletsToRemove;
	std::vector<EntityID> enemiesToRemove;
	
	for (EntityID bulletId : bulletView) {
		auto& bulletPos3d = bulletView.get<Position3D>(bulletId);
		Vec2 bulletPos = { bulletPos3d.x, bulletPos3d.z };
		float bulletRadius = bulletView.get<RigidBody>(bulletId).radius;
		auto& bulletComp = bulletView.get<BulletComponent>(bulletId);
		
		bool bulletHit = false;
		
		for (EntityID enemyId : enemyView) {
			// Skip if enemy already marked for removal
			if (std::find(enemiesToRemove.begin(), enemiesToRemove.end(), enemyId) != enemiesToRemove.end()) {
				continue;
			}
			
			auto& enemyPos3d = enemyView.get<Position3D>(enemyId);
			Vec2 enemyPos = { enemyPos3d.x, enemyPos3d.z };
			float enemyRadius = enemyView.get<RigidBody>(enemyId).radius;
			
			if (gCollision->Circle(bulletPos, bulletRadius, enemyPos, enemyRadius)) {
				auto& enemyHealth = enemyView.get<Health>(enemyId);
				auto& enemyType = enemyView.get<EnemyTypeComponent>(enemyId);
				
				// Damage enemy
				enemyHealth.currentHealth -= static_cast<int>(bulletComp.damage);
				
				// Check if enemy is dead
				if (enemyHealth.currentHealth <= 0) {
					enemiesToRemove.push_back(enemyId);
					
					// Grant experience to player
					View<PlayerTag, Experience> playerView(registry);
					for (EntityID playerId : playerView) {
						auto& exp = playerView.get<Experience>(playerId);
						exp.currentExp += enemyType.scoreValue;
						
						// Level up check
						while (exp.currentExp >= exp.expToNextLevel) {
							exp.currentExp -= exp.expToNextLevel;
							exp.level++;
							exp.expToNextLevel = static_cast<int>(exp.expToNextLevel * 1.2f); // 20% increase per level
						}
						break;
					}
				}
				
				bulletsToRemove.push_back(bulletId);
				bulletHit = true;
				break;
			}
		}
		
		if (bulletHit) continue;
	}
	
	// Remove dead enemies
	for (EntityID id : enemiesToRemove) {
		registry.destroyEntity(Entity{ id, registry.getEntityVersion(id) });
	}
	
	// Remove hit bullets
	for (EntityID id : bulletsToRemove) {
		registry.destroyEntity(Entity{ id, registry.getEntityVersion(id) });
	}
}

void CollisionSystem::Update(EntityManager& registry) {
	CheckBulletEnemyCollision(registry);
	CheckPlayerEnemyCollision(registry);
}
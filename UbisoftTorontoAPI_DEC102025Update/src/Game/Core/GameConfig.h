#pragma once
#include <string>
#include <unordered_map>
// Game configuration loaded from file
struct GameConfig {
    float gravity;
    float jumpVelocity;
    float forwardSpeed;
    float strafeSpeed;
    float acceleration;
    float damping;
    float rotationSpeed;

    // Camera controls
    float mouseSensitivity;
    float maxPitch;  // ~80 degrees
    float minPitch;

    // Map generation
    float blockSize;
    float renderDistance;
    float deleteDistance;
    int roadWidth;
    float wallHeight;

    // Map generation probabilities
    float normalFloorChance;
    float obstacleChance;
    float scorePointChance;
    float upgradePointChance;
    float gapChance;
    float gapFloorChance;

    // Chunk-based generation
    int chunkSize;           // Blocks per chunk (width and depth)
    int chunkRenderRadius;   // How many chunks to render in each direction

    // Block heights
    float floorHeight;
    float tallBlockHeight;
    float scorePointHeight;

    // Bullet physics
    float bulletSpeed;

    // Player spawn
    float playerSpawnX;
    float playerSpawnY;
    float playerSpawnZ;

    // Rendering
    float fov;
    float nearZ;
    float farZ;

    // Colors (RGB values, 0.0 to 1.0)
    // Player color
    float playerColorR;
    float playerColorG;
    float playerColorB;

    // Bullet color
    float bulletColorR;
    float bulletColorG;
    float bulletColorB;

    // Floor colors (alternating)
    float floorColor1R;
    float floorColor1G;
    float floorColor1B;
    float floorColor2R;
    float floorColor2G;
    float floorColor2B;

    // Wall color
    float wallColorR;
    float wallColorG;
    float wallColorB;

    // Tall block color
    float tallBlockColorR;
    float tallBlockColorG;
    float tallBlockColorB;

    // Score point color
    float scorePointColorR;
    float scorePointColorG;
    float scorePointColorB;

    // Upgrade point color
    float upgradePointColorR;
    float upgradePointColorG;
    float upgradePointColorB;

    // Upgrade values
    float healthUpgradeAmount;
    float speedUpgradeAmount;
    float jumpUpgradeAmount;
    float gravityUpgradeAmount;  // Negative makes you lighter
    float bulletSpeedUpgradeAmount;

    // Level/Round system parameters
    float roundDurationMs;           // Duration of each round (60 seconds)
    float baseWolfSpawnIntervalMs;    // Initial wolf spawn interval (1 second)
    float wolfSpawnReductionPerRound;  // Spawn interval reduction per round
    float minWolfSpawnIntervalMs;     // Minimum wolf spawn interval (1 second)
    int sheepAddedPerUpgrade;              // Number of sheep added with "Add Sheep" upgrade
    float sheepSpawnOffsetZ;           // Z offset for spawning new sheep

    // Wolf type spawn intervals (different wolves spawn at different rates)
    float sniperWolfSpawnIntervalMs;   // Sniper wolves spawn every 8 seconds
    float tankWolfSpawnIntervalMs;    // Tank wolves spawn every 12 seconds
    float fastWolfSpawnIntervalMs;     // Fast wolves spawn every 5 seconds
    float hunterWolfSpawnIntervalMs;  // Hunter wolves spawn every 10 seconds

    // Wolf stats - Basic
    int wolfBasicHealth;
    float wolfBasicSpeed;
    float wolfBasicChaseForce;
    float wolfBasicDetectionRange;
    float wolfBasicSize;

    // Wolf stats - Sniper
    int wolfSniperHealth;
    float wolfSniperSpeed;
    float wolfSniperChaseForce;
    float wolfSniperDetectionRange;
    float wolfSniperSize;

    // Wolf stats - Tank
    int wolfTankHealth;
    float wolfTankSpeed;
    float wolfTankChaseForce;
    float wolfTankDetectionRange;
    float wolfTankSize;

    // Wolf stats - Fast
    int wolfFastHealth;
    float wolfFastSpeed;
    float wolfFastChaseForce;
    float wolfFastDetectionRange;
    float wolfFastSize;

    // Wolf stats - Hunter
    int wolfHunterHealth;
    float wolfHunterSpeed;
    float wolfHunterChaseForce;
    float wolfHunterDetectionRange;
    float wolfHunterSize;
    float wolfHunterJumpCooldown;
    float wolfHunterJumpVelocity;
    float wolfHunterJumpBoost;
    float wolfHunterJumpMinDistance;
    float wolfHunterJumpMaxDistance;

    // Wolf stats - Magic
    int wolfMagicHealth;
    float wolfMagicSpeed;
    float wolfMagicChaseForce;
    float wolfMagicDetectionRange;
    float wolfMagicSize;

    // Wolf weapons - Sniper gun
    float wolfSniperGunDamage;
    float wolfSniperGunFireRate;
    float wolfSniperGunProjectileSpeed;
    float wolfSniperGunProjectileSize;
    float wolfSniperGunProjectileLife;
    float wolfSniperGunKnockback;

    // Wolf weapons - Magic wand
    float wolfMagicWandDamage;
    float wolfMagicWandFireRate;
    float wolfMagicWandProjectileSpeed;
    float wolfMagicWandProjectileSize;
    float wolfMagicWandProjectileLife;
    float wolfMagicWandKnockback;

    // Player weapons - Machine Gun
    float playerMachineGunDamage;
    float playerMachineGunFireRate;
    float playerMachineGunProjectileSpeed;
    float playerMachineGunProjectileSize;
    float playerMachineGunProjectileLife;
    float playerMachineGunExplosionRadius;
    float playerMachineGunKnockback;
    float playerMachineGunColorR;
    float playerMachineGunColorG;
    float playerMachineGunColorB;

    // Player weapons - Cannon
    float playerCannonDamage;
    float playerCannonFireRate;
    float playerCannonProjectileSpeed;
    float playerCannonProjectileSize;
    float playerCannonProjectileLife;
    float playerCannonExplosionRadius;
    float playerCannonKnockback;
    float playerCannonColorR;
    float playerCannonColorG;
    float playerCannonColorB;

    // Sheep weapons - Machine Gun
    float sheepMachineGunDamage;
    float sheepMachineGunFireRate;
    float sheepMachineGunProjectileSpeed;
    float sheepMachineGunProjectileSize;
    float sheepMachineGunProjectileLife;
    float sheepMachineGunExplosionRadius;
    float sheepMachineGunKnockback;
    float sheepMachineGunColorR;
    float sheepMachineGunColorG;
    float sheepMachineGunColorB;

    // Sheep weapons - Cannon
    float sheepCannonDamage;
    float sheepCannonFireRate;
    float sheepCannonProjectileSpeed;
    float sheepCannonProjectileSize;
    float sheepCannonProjectileLife;
    float sheepCannonExplosionRadius;
    float sheepCannonKnockback;
    float sheepCannonColorR;
    float sheepCannonColorG;
    float sheepCannonColorB;
    // Wolf spawn settings
    float wolfSpawnRadius;
    float wolfSpawnYPosition;
    float wolfSpawnMinX;
    float wolfSpawnMaxX;
    float wolfSpawnMinZ;
    float wolfSpawnMaxZ;
    // Sheep spawn settings
    float sheepSpawnRadius;
    int sheepInitialCount;
    float sheepSpawnYPosition;
    float sheepMoveSpeed;
    float sheepSpawnOffsetMin;
    float sheepSpawnOffsetMax;
    float sheepSize;
    float sheepColorR;
    float sheepColorG;
    float sheepColorB;
    float sheepSpriteScale;
    float sheepAnimationSpeed;

    // Sheep Boids behavior parameters (SheepComponent defaults)
    float sheepViewRadius;
    float sheepMaxSpeed;
    float sheepMaxForce;
    float sheepSeparationWeight;
    float sheepAlignmentWeight;
    float sheepCohesionWeight;
    float sheepTargetWeight;
    float sheepFearWeight;
    float sheepEnemyDetectRange;

    // Sheep shooting parameters
    float sheepWeaponArcAngle;  // Total arc for spreading weapons
    float sheepWeaponSpawnRadius;  // Radius for weapon spawn circle

    // Sheep targeting parameters
    float sheepTargetFarDistance;   // Distance at which sheep move toward player
    float sheepTargetNearDistance;  // Distance at which sheep move away from player

    // Sheep velocity damping
    float sheepVelocityDamping;

    // Sheep separation parameters
    float sheepSeparationSizeMultiplier;  // Minimum safe distance multiplier
    float sheepSeparationRepulsionStrength;  // Repulsion strength for very close neighbors

    // Particle trail parameters (for sheep bullets)
    float sheepBulletTrailEmissionRate;
    float sheepBulletTrailMinSize;
    float sheepBulletTrailMaxSize;
    float sheepBulletTrailLife;
    float sheepBulletTrailColorR;
    float sheepBulletTrailColorG;
    float sheepBulletTrailColorB;
    GameConfig();
    // Load config from file (returns true if successful)
    bool LoadFromFile(const std::string& filename);

    // Save config to file
    bool SaveToFile(const std::string& filename) const;

    // Load default values
    void LoadDefaults();
private:
    struct PropertyBase {
        std::string key;
        virtual void SetFromString(const std::string& value) = 0;
        virtual std::string ToString() const = 0;
        virtual ~PropertyBase() = default;
    };

   
    template <typename T>
    struct Property : PropertyBase {
        T* dataPtr;    

        Property(const std::string& k, T* ptr, T defVal) : dataPtr(ptr) {
            this->key = k;
            *dataPtr = defVal; 
        }

        void SetFromString(const std::string& value) override {
            std::stringstream ss(value);
            ss >> *dataPtr;
        }

        std::string ToString() const override {
            return std::to_string(*dataPtr);
        }
    };


    struct Item {
        bool isComment;
        std::string text; 
        PropertyBase* prop = nullptr;
    };


    std::vector<Item> savedItems; 
    std::unordered_map<std::string, PropertyBase*> propertyMap; 
    std::vector<PropertyBase*> garbageCollection; 


    void Header(const std::string& title) {
        savedItems.push_back({ true, "\n# " + title, nullptr });
    }

    template <typename T>
    void Bind(const std::string& key, T& variable, T defaultValue) {

        auto* p = new Property<T>(key, &variable, defaultValue);
        garbageCollection.push_back(p);

        propertyMap[key] = p;

        savedItems.push_back({ false, key, p });
    }
};
extern GameConfig config;
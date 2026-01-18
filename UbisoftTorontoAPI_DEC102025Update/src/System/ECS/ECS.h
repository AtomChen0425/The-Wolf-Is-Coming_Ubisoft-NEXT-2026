#pragma once
#include <vector>
#include <memory>
#include <bitset>
#include <algorithm>
#include <cassert>
#include <type_traits>

using EntityID = uint64_t;
using EntityVersion = uint64_t;
constexpr std::size_t MAX_ENTITY = 1 << 20;
constexpr uint64_t NULL_INDEX = static_cast<uint64_t>(-1); 

struct Entity {
    EntityID id;
    EntityVersion version;
};

// ==========================================
// Sparse Set
// ==========================================
class IComponentPool {
public:
    virtual ~IComponentPool() = default;
    virtual void removeIfExists(EntityID id) = 0;
    virtual void Clear() = 0;
    virtual std::size_t Size() = 0;
    virtual bool ContainsEntity(EntityID id) = 0;
};

template<typename T>
class ComponentPool : public IComponentPool {
public:
    std::vector<T> denseData;
    std::vector<EntityID> denseToEntity;
    std::vector<uint64_t> sparseIndices;

public:
    void add(EntityID id, const T& component) {
        if (id >= sparseIndices.size()) {
            sparseIndices.resize(id + 1, NULL_INDEX);
        }
        if (sparseIndices[id] != NULL_INDEX) {
            denseData[sparseIndices[id]] = component;
            return;
        }

        sparseIndices[id] = static_cast<uint64_t>(denseData.size());
        denseData.push_back(component);
        denseToEntity.push_back(id);
    }

    T* get(EntityID id) {
        if (id >= sparseIndices.size() || sparseIndices[id] == NULL_INDEX) {
            return nullptr;
        }
        return &denseData[sparseIndices[id]];
    }

    void removeIfExists(EntityID id) override {
        if (id >= sparseIndices.size() || sparseIndices[id] == NULL_INDEX) {
            return;
        }

        // Swap-and-Pop
        uint64_t removedIndex = sparseIndices[id];
        uint64_t lastIndex = static_cast<uint64_t>(denseData.size() - 1);

        if (removedIndex != lastIndex) {
			// move the last element to the removed spot
            EntityID lastEntity = denseToEntity[lastIndex];
            denseData[removedIndex] = denseData[lastIndex]; 
            denseToEntity[removedIndex] = lastEntity;       
            sparseIndices[lastEntity] = removedIndex;       
        }

        denseData.pop_back();
        denseToEntity.pop_back();
        sparseIndices[id] = NULL_INDEX;
    }

    void Clear() override {
        denseData.clear();
        denseToEntity.clear();
        sparseIndices.clear();
    }

    std::size_t Size() override {
        return denseData.size();
    }

    bool ContainsEntity(EntityID id) override {
        return id < sparseIndices.size() && sparseIndices[id] != NULL_INDEX;
    }
};

// ==========================================
// 3.Registry
// ==========================================
class EntityManager {
    EntityID nextEntityID = 0;
    std::vector<EntityVersion> entityVersions;
	std::vector<EntityID> freeIndices; // store freed entity IDs for reuse
    std::bitset<MAX_ENTITY> aliveEntities;
	std::vector<uint64_t> entityMasks; // store component bitmask for each entity
    std::vector<std::unique_ptr<IComponentPool>> m_componentPools;
private:
    static std::size_t getUniqueTypeID() {
        static std::size_t counter = 0;
        return counter++;
    }
public:
    Entity createEntity() {
        EntityID id;
        if (!freeIndices.empty()) {
            id = freeIndices.back();
            freeIndices.pop_back();
        }
        else {
            id = nextEntityID++;
            if (id >= entityVersions.size()) {
                entityVersions.resize(id + 1, 0);
                entityMasks.resize(id + 1, 0);
            }
        }
        aliveEntities.set(id);
        return { id, entityVersions[id] };
    }

    template<typename T>
    void addComponent(Entity e, T component) {
        if (!isValid(e)) return;

        std::size_t typeId = getComponentTypeID<T>();

        if (typeId >= m_componentPools.size()) {
            m_componentPools.resize(typeId + 1);
        }
        if (!m_componentPools[typeId]) {
            m_componentPools[typeId] = std::make_unique<ComponentPool<T>>();
        }
        static_cast<ComponentPool<T>*>(m_componentPools[typeId].get())->add(e.id, component);

        entityMasks[e.id] |= (1ULL << typeId);
    }

    template<typename T>
    T* getComponent(Entity e) {
        if (!isValid(e)) return nullptr;
        std::size_t typeId = getComponentTypeID<T>();

        if (typeId >= m_componentPools.size() || !m_componentPools[typeId]) {
            return nullptr;
        }
        return static_cast<ComponentPool<T>*>(m_componentPools[typeId].get())->get(e.id);
    }

    template<typename T>
    void removeComponent(Entity e) {
        if (!isValid(e)) return;
        std::size_t typeId = getComponentTypeID<T>();

        if (typeId < m_componentPools.size() && m_componentPools[typeId]) {
            m_componentPools[typeId]->removeIfExists(e.id);
        }
        entityMasks[e.id] &= ~(1ULL << typeId);
    }

    void destroyEntity(Entity e) {
        
        if (!isValid(e)) return;

        uint64_t mask = entityMasks[e.id];
		// Remove all components
        for (std::size_t typeId = 0; typeId < m_componentPools.size(); ++typeId) {
            if ((mask & (1ULL << typeId)) && m_componentPools[typeId]) {
                m_componentPools[typeId]->removeIfExists(e.id);
            }
        }

        aliveEntities.reset(e.id);
        entityMasks[e.id] = 0;
        entityVersions[e.id]++;
        freeIndices.push_back(e.id);
    }

    bool isValid(Entity e) {
        return e.id < nextEntityID && aliveEntities.test(e.id) && entityVersions[e.id] == e.version;
    }

    uint64_t getEntityMask(EntityID id) { return entityMasks[id]; }

    template<typename T>
    ComponentPool<T>* getPool() {
        std::size_t typeId = getComponentTypeID<T>();
        if (typeId >= m_componentPools.size()) return nullptr;
        return static_cast<ComponentPool<T>*>(m_componentPools[typeId].get());
    }

    template<typename T>
    static std::size_t getComponentTypeID() {
        static std::size_t typeID = getUniqueTypeID();
        return typeID;
    }

    EntityVersion getEntityVersion(EntityID id) const {
        return entityVersions[id];
    }
    std::size_t getAliveEntitiesCount() const {
        return aliveEntities.count();
    }
};

// ==========================================
// 4. View
// ==========================================
template<typename... Components>
class View {
    EntityManager& manager;
	IComponentPool* leaderPool = nullptr; // the pool with the least entities
	std::vector<EntityID>* leaderIndices = nullptr; // pointer to the denseToEntity of the leader pool
    uint64_t targetMask;

public:
    View(EntityManager& m) : manager(m) {
        targetMask = 0;
        ((targetMask |= (1ULL << EntityManager::getComponentTypeID<Components>())), ...);

        std::size_t minSize = static_cast<std::size_t>(-1);

        auto checkPool = [&](auto* pool) {
            if (pool && pool->Size() < minSize) {
                minSize = pool->Size();
                leaderPool = pool;
                leaderIndices = &pool->denseToEntity;
            }
            };

        (checkPool(manager.getPool<Components>()), ...);
    }

    struct Iterator {
        EntityManager& manager;
        const std::vector<EntityID>* indices; //  
        uint64_t mask;
		size_t index; // index in the dense array

        Iterator(EntityManager& m, const std::vector<EntityID>* idxs, uint64_t mk, size_t i)
            : manager(m), indices(idxs), mask(mk), index(i) {
            skipInvalid();
        }

        // Lazy Iteration
        void skipInvalid() {
            if (!indices) return;
            while (index < indices->size()) {
                EntityID id = (*indices)[index];
				// Check if entity matches the mask
                if ((manager.getEntityMask(id) & mask) == mask) {
                    return; 
                }
                index++; 
            }
        }

        Iterator& operator++() {
            index++;
            skipInvalid();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return index != other.index;
        }

        EntityID operator*() const {
            return (*indices)[index];
        }
    };

    Iterator begin() {
        if (!leaderPool) return Iterator(manager, nullptr, targetMask, 0);
        return Iterator(manager, leaderIndices, targetMask, 0);
    }

    Iterator end() {
        if (!leaderPool) return Iterator(manager, nullptr, targetMask, 0);
        return Iterator(manager, leaderIndices, targetMask, leaderIndices->size());
    }

    template<typename T>
    T& get(EntityID id) {
        return *manager.getComponent<T>(Entity{ id, manager.getEntityVersion(id) });
    }
    template<typename T>
    bool has(EntityID id) {
        uint64_t bit = 1ULL << EntityManager::getComponentTypeID<T>();
        return (manager.getEntityMask(id) & bit) != 0;
    }


};
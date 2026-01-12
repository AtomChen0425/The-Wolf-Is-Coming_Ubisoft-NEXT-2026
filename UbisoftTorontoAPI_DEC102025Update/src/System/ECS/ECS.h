#pragma once
#include <vector>
#include <memory>
#include <bitset>
#include <algorithm>
#include <cassert>
#include <type_traits>

// ==========================================
// 1. 基础类型定义
// ==========================================
using EntityID = uint64_t;
using EntityVersion = uint64_t;
constexpr std::size_t MAX_ENTITY = 2048;
constexpr uint64_t NULL_INDEX = static_cast<uint64_t>(-1); // 用于标记无效索引

struct Entity {
    EntityID id;
    EntityVersion version;
};

// ==========================================
// 2. 组件池接口与实现 (Sparse Set)
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
public: // 公开 denseData 以便 View 使用
    std::vector<T> denseData;
    std::vector<EntityID> denseToEntity;
    std::vector<uint64_t> sparseIndices;

public:
    void add(EntityID id, const T& component) {
        if (id >= sparseIndices.size()) {
            sparseIndices.resize(id + 1, NULL_INDEX);
        }
        // 如果已存在，直接更新
        if (sparseIndices[id] != NULL_INDEX) {
            denseData[sparseIndices[id]] = component;
            return;
        }

        // 新增：Sparse Set 标准插入流程
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

        // Swap-and-Pop 删除逻辑
        uint64_t removedIndex = sparseIndices[id];
        uint64_t lastIndex = static_cast<uint64_t>(denseData.size() - 1);

        if (removedIndex != lastIndex) {
            // 将最后一个元素搬到被删除的位置
            EntityID lastEntity = denseToEntity[lastIndex];
            denseData[removedIndex] = denseData[lastIndex]; // 移动组件数据
            denseToEntity[removedIndex] = lastEntity;       // 更新反向映射
            sparseIndices[lastEntity] = removedIndex;       // 更新稀疏索引
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
// 3. 实体管理器 (Registry)
// ==========================================
class EntityManager {
    EntityID nextEntityID = 0;
    std::vector<EntityVersion> entityVersions;
    std::vector<EntityID> freeIndices; // 回收站 ID
    std::bitset<MAX_ENTITY> aliveEntities;
    std::vector<uint64_t> entityMasks; // 存储每个实体的组件位掩码

    // 核心修复：索引是 ComponentTypeID，而不是 EntityID
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
            // 扩容检查
            if (id >= entityVersions.size()) {
                entityVersions.resize(id + 1, 0);
                entityMasks.resize(id + 1, 0);
            }
        }

        aliveEntities.set(id);
        // 复用 ID 时版本号不重置，而是继续增加，这里假设 destroy 时已增加
        return { id, entityVersions[id] };
    }

    template<typename T>
    void addComponent(Entity e, T component) {
        if (!isValid(e)) return;

        std::size_t typeId = getComponentTypeID<T>();

        // 确保 componentPools 足够大
        if (typeId >= m_componentPools.size()) {
            m_componentPools.resize(typeId + 1);
        }
        // 延迟创建池子
        if (!m_componentPools[typeId]) {
            m_componentPools[typeId] = std::make_unique<ComponentPool<T>>();
        }

        // 添加数据
        static_cast<ComponentPool<T>*>(m_componentPools[typeId].get())->add(e.id, component);

        // 更新位掩码
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

        // 1. 遍历掩码，从所有池子中移除该实体
        uint64_t mask = entityMasks[e.id];
        // 简单循环遍历每一位（实际应用可用 __builtin_ctz 优化）
        for (std::size_t typeId = 0; typeId < m_componentPools.size(); ++typeId) {
            if ((mask & (1ULL << typeId)) && m_componentPools[typeId]) {
                m_componentPools[typeId]->removeIfExists(e.id);
            }
        }

        // 2. 状态重置
        aliveEntities.reset(e.id);
        entityMasks[e.id] = 0;
        entityVersions[e.id]++; // 版本升级，使旧 Entity 句柄失效
        freeIndices.push_back(e.id); // 加入回收站
    }

    bool isValid(Entity e) {
        return e.id < nextEntityID && aliveEntities.test(e.id) && entityVersions[e.id] == e.version;
    }

    // 获取内部数据供 View 使用
    uint64_t getEntityMask(EntityID id) { return entityMasks[id]; }

    template<typename T>
    ComponentPool<T>* getPool() {
        std::size_t typeId = getComponentTypeID<T>();
        if (typeId >= m_componentPools.size()) return nullptr;
        return static_cast<ComponentPool<T>*>(m_componentPools[typeId].get());
    }

    template<typename T>
    static std::size_t getComponentTypeID() {
        // 对于每一个 T，这个静态变量只会初始化一次
        // 但它获取的值来自于全局唯一的 getUniqueTypeID()
        static std::size_t typeID = getUniqueTypeID();
        return typeID;
    }

    EntityVersion getEntityVersion(EntityID id) const {
        // 实际工程中可以加一个 assert(id < entityVersions.size());
        return entityVersions[id];
    }
};

// ==========================================
// 4. 视图 (View) 与 延迟迭代器
// ==========================================
template<typename... Components>
class View {
    EntityManager& manager;
    IComponentPool* leaderPool = nullptr; // 最短的池子，作为遍历基准
    std::vector<EntityID>* leaderIndices = nullptr; // 指向 denseToEntity 的指针
    uint64_t targetMask;

public:
    View(EntityManager& m) : manager(m) {
        targetMask = 0;
        // 折叠表达式计算目标掩码
        ((targetMask |= (1ULL << EntityManager::getComponentTypeID<Components>())), ...);

        // 寻找最短的池子 (Leader Optimization)
        std::size_t minSize = static_cast<std::size_t>(-1);

        // Lambda 用于检查并更新最小池子
        auto checkPool = [&](auto* pool) {
            if (pool && pool->Size() < minSize) {
                minSize = pool->Size();
                leaderPool = pool;
                leaderIndices = &pool->denseToEntity;
            }
            };

        // 对每个组件类型执行检查
        (checkPool(manager.getPool<Components>()), ...);
    }

    struct Iterator {
        EntityManager& manager;
        const std::vector<EntityID>* indices; // 引用 Leader 的密集数组
        uint64_t mask;
        size_t index; // 当前在 dense 数组中的下标

        Iterator(EntityManager& m, const std::vector<EntityID>* idxs, uint64_t mk, size_t i)
            : manager(m), indices(idxs), mask(mk), index(i) {
            // 构造时立即跳到第一个有效元素
            skipInvalid();
        }

        // 核心跳跃逻辑：Lazy Iteration
        void skipInvalid() {
            if (!indices) return;
            while (index < indices->size()) {
                EntityID id = (*indices)[index];
                // 检查位掩码是否完全匹配
                if ((manager.getEntityMask(id) & mask) == mask) {
                    return; // 找到了！
                }
                index++; // 不匹配，跳过
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

    // 辅助函数：在遍历中方便地获取组件
    template<typename T>
    T& get(EntityID id) {
        // 这里我们假设迭代器出来的 ID 肯定是有效的，所以可以直接构造 Entity 去查
        // 或者为了极致性能，View 初始化时就保存 componentPools 的指针，这里直接数组访问
        return *manager.getComponent<T>(Entity{ id, manager.getEntityVersion(id) });
    }
};

// 辅助：给 EntityManager 加一个创建 View 的接口
// (由于 View 是模板类，通常需要放在 EntityManager 定义之后，
// 或者将 View 定义在 EntityManager 内部)
// 这里我们假设用户手动创建 View view(manager);
#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <stdexcept>
#include <functional>
#include <vector>
#include <algorithm>
#include <set>

namespace engine::ECS {

using EntityID = uint32_t;

class IComponentStore {
public:
    virtual ~IComponentStore() = default;
    virtual void Remove(EntityID id) = 0;
};

template<typename T>
class ComponentStore : public IComponentStore {
public:
    void Add(EntityID id, const T& component) {
        components_[id] = component;
    }

    T* Get(EntityID id) {
        auto it = components_.find(id);
        return it != components_.end() ? &it->second : nullptr;
    }

    void Remove(EntityID id) override {
        components_.erase(id);
    }

    bool Has(EntityID id) const {
        return components_.find(id) != components_.end();
    }

    void ForEach(const std::function<void(EntityID, T&)>& fn) {
        for (auto& [id, comp] : components_) {
            fn(id, comp);
        }
    }

private:
    std::unordered_map<EntityID, T> components_;
};

class ComponentManager {
public:
    template<typename T>
    void AddComponent(EntityID id, const T& component) {
        GetOrCreateStore<T>()->Add(id, component);
    }

    template<typename T>
    T* GetComponent(EntityID id) {
        auto store = GetStore<T>();
        return store ? store->Get(id) : nullptr;
    }

    template<typename T>
    bool HasComponent(EntityID id) {
        auto store = GetStore<T>();
        return store && store->Has(id);
    }

    template<typename T>
    void RemoveComponent(EntityID id) {
        auto store = GetStore<T>();
        if (store) store->Remove(id);
    }

    template<typename T>
    void ForEachComponent(const std::function<void(EntityID, T&)>& fn) {
        auto store = GetStore<T>();
        if (store) store->ForEach(fn);
    }

    template<typename T>
    std::vector<EntityID> GetEntitiesWithComponent() {
        std::vector<EntityID> entities;
        auto store = GetStore<T>();
        if (store) {
            store->ForEach([&entities](EntityID id, T&) {
                entities.push_back(id);
            });
        }
        return entities;
    }

    template<typename T, typename U>
    std::vector<EntityID> GetEntitiesWithComponents() {
        auto entitiesT = GetEntitiesWithComponent<T>();
        auto entitiesU = GetEntitiesWithComponent<U>();
        
        std::vector<EntityID> result;
        std::sort(entitiesT.begin(), entitiesT.end());
        std::sort(entitiesU.begin(), entitiesU.end());
        
        std::set_intersection(entitiesT.begin(), entitiesT.end(),
                             entitiesU.begin(), entitiesU.end(),
                             std::back_inserter(result));
        return result;
    }

    template<typename T, typename U, typename V>
    std::vector<EntityID> GetEntitiesWithComponents() {
        auto entitiesT = GetEntitiesWithComponent<T>();
        auto entitiesU = GetEntitiesWithComponent<U>();
        auto entitiesV = GetEntitiesWithComponent<V>();

        std::vector<EntityID> result;
        std::sort(entitiesT.begin(), entitiesT.end());
        std::sort(entitiesU.begin(), entitiesU.end());
        std::sort(entitiesV.begin(), entitiesV.end());
        
        std::set_intersection(entitiesT.begin(), entitiesT.end(),
                             entitiesU.begin(), entitiesU.end(),
                             std::back_inserter(result));

        std::vector<EntityID> finalResult;
        std::sort(result.begin(), result.end());
        std::set_intersection(result.begin(), result.end(),
                             entitiesV.begin(), entitiesV.end(),
                             std::back_inserter(finalResult));
        return finalResult;
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStore>> stores_;

    template<typename T>
    ComponentStore<T>* GetOrCreateStore() {
        auto index = std::type_index(typeid(T));
        if (stores_.count(index) == 0) {
            stores_[index] = std::make_unique<ComponentStore<T>>();
        }
        return static_cast<ComponentStore<T>*>(stores_[index].get());
    }

    template<typename T>
    ComponentStore<T>* GetStore() {
        auto it = stores_.find(std::type_index(typeid(T)));
        if (it != stores_.end())
            return static_cast<ComponentStore<T>*>(it->second.get());
        return nullptr;
    }
};

} // namespace engine::ECS

// src/examples/zombie_survivor/ecs/systems/WeaponSystem.cpp

#include "WeaponSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include <iostream>
#include <algorithm>

namespace ZombieSurvivor::System {

void WeaponSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;

    auto& eventManager = world->GetEventManager();
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);

    std::cout << "[WeaponSystem] Initialized" << std::endl;
}

void WeaponSystem::Update(float deltaTime) {
    UpdateFireCooldowns(deltaTime);
    UpdateReloading(deltaTime);
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    auto entities = componentManager.GetEntitiesWithComponents<
        Component::WeaponComponent,
        Component::AmmoComponent,
        Component::InputComponent
    >();
    
    for (const auto& entityId : entities) {
        auto* input = componentManager.GetComponent<Component::InputComponent>(entityId);
        if (!input) continue;
        
        ProcessShootInput(entityId, input->shootButtonPressed);
        ProcessReloadInput(entityId, input->reloadButtonPressed);
    }
}

void WeaponSystem::Shutdown() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
    
    weaponStates_.clear();
    std::cout << "[WeaponSystem] Shutdown" << std::endl;
}

void WeaponSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (event && event->GetType() == engine::event::EventType::CUSTOM) {
        HandleGameEvent(event);
    }
}

bool WeaponSystem::CanShoot(uint32_t entityId) const {
    if (!ValidateEntityId(entityId, "CanShoot")) return false;
    
    return IsWeaponReady(entityId) && !IsReloading(entityId);
}

void WeaponSystem::TryShoot(uint32_t entityId) {
    if (!CanShoot(entityId)) return;
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    auto fireRequestData = std::make_shared<Events::WeaponFireRequestData>();
    fireRequestData->entityId = entityId;
    fireRequestData->inputPressed = true;
    
    auto fireRequestEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::WEAPON_FIRE_REQUESTED,
        std::static_pointer_cast<void>(fireRequestData)
    );
    eventManager.Publish(fireRequestEvent);
}

bool WeaponSystem::IsWeaponReady(uint32_t entityId) const {
    if (!ValidateEntityId(entityId, "IsWeaponReady")) return false;
    
    auto* weapon = GetWeaponComponent(entityId);
    if (!weapon) return false;
    
    uint32_t currentWeaponId = GetCurrentWeaponId(entityId);
    auto stateKey = GetWeaponStateKey(entityId, currentWeaponId);
    auto it = weaponStates_.find(stateKey);
    if (it == weaponStates_.end()) return true;
    
    float fireInterval = 1.0f / weapon->fireRate;
    return it->second.timeSinceLastShot >= fireInterval;
}

void WeaponSystem::StartReload(uint32_t entityId) {
    if (!ValidateEntityId(entityId, "StartReload")) return;
    
    auto* weapon = GetWeaponComponent(entityId);
    auto* ammo = GetAmmoComponent(entityId);
    if (!weapon || !ammo) return;
    
    // 检查是否已在装弹其他武器
    if (reloadingWeapons_.find(entityId) != reloadingWeapons_.end()) {
        std::cout << "[WeaponSystem] Entity " << entityId << " already reloading another weapon" << std::endl;
        return;
    }
    
    if (ammo->currentAmmo >= weapon->magazineCapacity) {
        std::cout << "[WeaponSystem] Entity " << entityId << " magazine already full" << std::endl;
        return;
    }
    
    if (ammo->totalAmmo <= 0) {
        std::cout << "[WeaponSystem] Entity " << entityId << " no ammo to reload" << std::endl;
        return;
    }
    
    uint32_t currentWeaponId = GetCurrentWeaponId(entityId);
    auto stateKey = GetWeaponStateKey(entityId, currentWeaponId);
    
    // 检查当前武器是否已在装弹
    auto stateIt = weaponStates_.find(stateKey);
    if (stateIt != weaponStates_.end() && stateIt->second.isReloading) {
        std::cout << "[WeaponSystem] Entity " << entityId << " already reloading current weapon" << std::endl;
        return;
    }
    
    // 开始装弹当前武器
    auto& state = GetOrCreateWeaponState(stateKey);
    state.isReloading = true;
    state.reloadTimer = 0.0f;
    state.weaponId = currentWeaponId;
    
    // 记录正在装弹的武器
    reloadingWeapons_[entityId] = currentWeaponId;
    
    PublishReloadStartedEvent(entityId, weapon->reloadTime, weapon->magazineCapacity, ammo->totalAmmo);
    
    std::cout << "[WeaponSystem] Entity " << entityId << " started reloading weapon " << currentWeaponId << std::endl;
}

bool WeaponSystem::IsReloading(uint32_t entityId) const {
    return reloadingWeapons_.find(entityId) != reloadingWeapons_.end();
}

float WeaponSystem::GetReloadProgress(uint32_t entityId) const {
    if (!IsReloading(entityId)) return 0.0f;
    
    auto* weapon = GetWeaponComponent(entityId);
    if (!weapon) return 0.0f;
    
    uint32_t currentWeaponId = GetCurrentWeaponId(entityId);
    auto stateKey = GetWeaponStateKey(entityId, currentWeaponId);
    auto it = weaponStates_.find(stateKey);
    if (it == weaponStates_.end()) return 0.0f;
    
    return std::min(1.0f, it->second.reloadTimer / weapon->reloadTime);
}

float WeaponSystem::GetFireCooldown(uint32_t entityId) const {
    uint32_t currentWeaponId = GetCurrentWeaponId(entityId);
    auto stateKey = GetWeaponStateKey(entityId, currentWeaponId);
    auto it = weaponStates_.find(stateKey);
    return it != weaponStates_.end() ? it->second.timeSinceLastShot : 0.0f;
}



void WeaponSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto gameEvent = std::dynamic_pointer_cast<Events::GameEvent>(event);
    if (!gameEvent) return;
    
    switch (gameEvent->GetGameEventType()) {
        case Events::GameEventType::RELOAD_REQUESTED:
            HandleReloadRequested(gameEvent->GetData());
            break;
        case Events::GameEventType::AMMO_CONSUMED:
            HandleAmmoConsumed(gameEvent->GetData());
            break;
        case Events::GameEventType::RELOAD_EXECUTED:  // 新增
            HandleReloadExecuted(gameEvent->GetData());
            break;
        default:
            break;
    }
}

void WeaponSystem::HandleReloadRequested(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::ReloadData>(eventData);
    if (!data) return;
    
    StartReload(data->entityId);
}

void WeaponSystem::HandleAmmoConsumed(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::AmmoConsumedData>(eventData);
    if (!data) return;
    
    ExecuteWeaponFire(data->entityId, data->currentAmmo, data->totalAmmo);
    
    if (data->currentAmmo <= 0) {
        StartReload(data->entityId);
    }
}

void WeaponSystem::ExecuteWeaponFire(uint32_t entityId, int currentAmmo, int totalAmmo) {
    auto* weapon = GetWeaponComponent(entityId);
    if (!weapon) return;
    
    uint32_t currentWeaponId = GetCurrentWeaponId(entityId);
    auto stateKey = GetWeaponStateKey(entityId, currentWeaponId);
    auto& state = GetOrCreateWeaponState(stateKey);
    state.timeSinceLastShot = 0.0f;
    
    PublishWeaponFiredEvent(entityId, weapon->damage, currentAmmo, totalAmmo);
    
    std::cout << "[WeaponSystem] Entity " << entityId << " fired weapon, ammo: " 
              << currentAmmo << "/" << totalAmmo << std::endl;
}

void WeaponSystem::UpdateFireCooldowns(float deltaTime) {
    for (auto& [entityId, state] : weaponStates_) {
        state.timeSinceLastShot += deltaTime;
    }
}

void WeaponSystem::UpdateReloading(float deltaTime) {
    auto it = reloadingWeapons_.begin();
    while (it != reloadingWeapons_.end()) {
        uint32_t entityId = it->first;
        uint32_t reloadingWeaponId = it->second;
        
        auto stateKey = GetWeaponStateKey(entityId, reloadingWeaponId);
        auto stateIt = weaponStates_.find(stateKey);
        
        if (stateIt != weaponStates_.end() && stateIt->second.isReloading) {
            stateIt->second.reloadTimer += deltaTime;
            
            auto* weapon = GetWeaponComponent(entityId);
            if (weapon && stateIt->second.reloadTimer >= weapon->reloadTime) {
                CompleteReload(entityId, reloadingWeaponId);
                // CompleteReload内部已经清除了reloadingWeapons_记录，不需要重复删除
                return;  // 重新开始迭代，因为map被修改了
            }
        }
        
        ++it;
    }
}

void WeaponSystem::CompleteReload(uint32_t entityId, uint32_t weaponId) {
    auto* weapon = GetWeaponComponent(entityId);
    auto* ammo = GetAmmoComponent(entityId);
    if (!weapon || !ammo) return;
    
    int neededAmmo = weapon->magazineCapacity - ammo->currentAmmo;
    
    // 清除武器状态
    auto stateKey = GetWeaponStateKey(entityId, weaponId);
    auto stateIt = weaponStates_.find(stateKey);
    if (stateIt != weaponStates_.end()) {
        stateIt->second.isReloading = false;
        stateIt->second.reloadTimer = 0.0f;
    }
    
    // 清除装弹记录 - 封装在这里确保一致性
    reloadingWeapons_.erase(entityId);
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    auto reloadExecuteData = std::make_shared<Events::ReloadExecuteData>();
    reloadExecuteData->entityId = entityId;
    reloadExecuteData->reloadAmount = neededAmmo;
    reloadExecuteData->magazineCapacity = weapon->magazineCapacity;
    
    auto reloadExecuteEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::RELOAD_EXECUTE,
        std::static_pointer_cast<void>(reloadExecuteData)
    );
    eventManager.Publish(reloadExecuteEvent);
    
    std::cout << "[WeaponSystem] Entity " << entityId << " requesting reload execution for weapon " 
              << weaponId << " with " << neededAmmo << " ammo" << std::endl;
}

void WeaponSystem::ProcessShootInput(uint32_t entityId, bool shootPressed) {
    if (shootPressed) {
        TryShoot(entityId);
    }
}

void WeaponSystem::ProcessReloadInput(uint32_t entityId, bool reloadPressed) {
    if (reloadPressed) {
        StartReload(entityId);
    }
}

bool WeaponSystem::ValidateEntityId(uint32_t entityId, const std::string& operation) const {
    auto* world = GetWorld();
    if (!world) {
        std::cerr << "[WeaponSystem] Error: No world available for " << operation << std::endl;
        return false;
    }
    
    auto& componentManager = world->GetComponentManager();
    if (!componentManager.HasComponent<Component::WeaponComponent>(entityId)) {
        std::cerr << "[WeaponSystem] Error: Entity " << entityId 
                  << " does not have WeaponComponent for " << operation << std::endl;
        return false;
    }
    
    return true;
}

Component::WeaponComponent* WeaponSystem::GetWeaponComponent(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return nullptr;
    
    return world->GetComponentManager().GetComponent<Component::WeaponComponent>(entityId);
}

Component::AmmoComponent* WeaponSystem::GetAmmoComponent(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return nullptr;
    
    return world->GetComponentManager().GetComponent<Component::AmmoComponent>(entityId);
}

void WeaponSystem::PublishWeaponFiredEvent(uint32_t entityId, float damage, int currentAmmo, int totalAmmo) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    auto firedData = std::make_shared<Events::WeaponFiredData>();
    firedData->entityId = entityId;
    firedData->damage = damage;
    firedData->currentAmmo = currentAmmo;
    firedData->totalAmmo = totalAmmo;
    
    auto firedEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::WEAPON_FIRED,
        std::static_pointer_cast<void>(firedData)
    );
    eventManager.Publish(firedEvent);
}

void WeaponSystem::PublishReloadStartedEvent(uint32_t entityId, float reloadTime, int magazineCapacity, int totalAmmo) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    auto reloadData = std::make_shared<Events::ReloadData>();
    reloadData->entityId = entityId;
    reloadData->reloadTime = reloadTime;
    reloadData->magazineCapacity = magazineCapacity;
    reloadData->totalAmmo = totalAmmo;
    
    auto reloadEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::RELOAD_STARTED,
        std::static_pointer_cast<void>(reloadData)
    );
    eventManager.Publish(reloadEvent);
}

void WeaponSystem::PublishReloadCompletedEvent(uint32_t entityId, int newAmmo, int totalAmmo) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    auto reloadData = std::make_shared<Events::ReloadData>();
    reloadData->entityId = entityId;
    reloadData->reloadTime = 0.0f;
    reloadData->magazineCapacity = newAmmo;
    reloadData->totalAmmo = totalAmmo;
    
    auto reloadEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::RELOAD_COMPLETED,
        std::static_pointer_cast<void>(reloadData)
    );
    eventManager.Publish(reloadEvent);
}

WeaponSystem::WeaponState& WeaponSystem::GetOrCreateWeaponState(const std::pair<uint32_t, uint32_t>& stateKey) {
    auto it = weaponStates_.find(stateKey);
    if (it == weaponStates_.end()) {
        auto [newIt, inserted] = weaponStates_.emplace(stateKey, WeaponState{});
        newIt->second.weaponId = stateKey.second;  // 设置weaponId
        return newIt->second;
    }
    return it->second;
}

void WeaponSystem::HandleReloadExecuted(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::ReloadExecutedData>(eventData);
    if (!data) return;
    
    // 获取当前武器ID并构造正确的state key
    uint32_t currentWeaponId = GetCurrentWeaponId(data->entityId);
    auto stateKey = GetWeaponStateKey(data->entityId, currentWeaponId);
    auto& state = GetOrCreateWeaponState(stateKey);
    
    state.isReloading = false;
    state.reloadTimer = 0.0f;
    
    PublishReloadCompletedEvent(data->entityId, data->newCurrentAmmo, data->newTotalAmmo);
    
    std::cout << "[WeaponSystem] Entity " << data->entityId << " reload completed with " 
              << data->actualReloadAmount << " ammo loaded, final state: " 
              << data->newCurrentAmmo << "/" << data->newTotalAmmo << std::endl;
}

// 新增辅助方法
uint32_t WeaponSystem::GetCurrentWeaponId(uint32_t entityId) const {
    auto it = currentWeapons_.find(entityId);
    return it != currentWeapons_.end() ? it->second : 0;  // 0表示默认武器
}

std::pair<uint32_t, uint32_t> WeaponSystem::GetWeaponStateKey(uint32_t entityId, uint32_t weaponId) const {
    return std::make_pair(entityId, weaponId);
}

bool WeaponSystem::IsReloading(uint32_t entityId) const {
    return reloadingWeapons_.find(entityId) != reloadingWeapons_.end();
}

void WeaponSystem::SetCurrentWeapon(uint32_t entityId, uint32_t weaponId) {
    if (IsReloading(entityId)) {
        StopReload(entityId);
    }
    
    currentWeapons_[entityId] = weaponId;
    std::cout << "[WeaponSystem] Entity " << entityId << " switched to weapon " << weaponId << std::endl;
}

void WeaponSystem::StopReload(uint32_t entityId) {
    auto it = reloadingWeapons_.find(entityId);
    if (it != reloadingWeapons_.end()) {
        uint32_t weaponId = it->second;
        auto stateKey = GetWeaponStateKey(entityId, weaponId);
        
        auto stateIt = weaponStates_.find(stateKey);
        if (stateIt != weaponStates_.end()) {
            stateIt->second.isReloading = false;
            stateIt->second.reloadTimer = 0.0f;
        }
        
        reloadingWeapons_.erase(it);
        std::cout << "[WeaponSystem] Entity " << entityId << " stopped reloading weapon " << weaponId << std::endl;
    }
}

} // namespace ZombieSurvivor::System 
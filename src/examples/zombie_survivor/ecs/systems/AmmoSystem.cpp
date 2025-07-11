// src/examples/zombie_survivor/ecs/systems/AmmoSystem.cpp

#include "AmmoSystem.hpp"
#include "engine/core/ecs/World.hpp"
#include "engine/core/event/EventManager.hpp"
#include "examples/zombie_survivor/events/GameEventData.hpp"
#include "examples/zombie_survivor/ecs/components/WeaponComponent.hpp"
#include <iostream>
#include <algorithm>
#include <unordered_map>

namespace ZombieSurvivor::System {

void AmmoSystem::Init() {
    auto* world = GetWorld();
    if (!world) return;

    auto& eventManager = world->GetEventManager();
    eventManager.Subscribe(engine::event::EventType::CUSTOM, this);

    std::cout << "[AmmoSystem] Initialized" << std::endl;
}

void AmmoSystem::Update(float deltaTime) {
    // AmmoSystem主要是事件驱动的，Update中可以处理弹药统计或其他定期检查
    // 目前保持空实现
    UpdateReloadProgress(deltaTime);
}

void AmmoSystem::Shutdown() {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    eventManager.Unsubscribe(engine::event::EventType::CUSTOM, this);
    
    std::cout << "[AmmoSystem] Shutdown" << std::endl;
}

void AmmoSystem::onEvent(const std::shared_ptr<engine::event::Event>& event) {
    if (event && event->GetType() == engine::event::EventType::CUSTOM) {
        HandleGameEvent(event);
    }
}

bool AmmoSystem::HasAmmo(uint32_t entityId) const {
    if (!ValidateEntityId(entityId, "HasAmmo")) return false;
    
    auto* ammo = GetAmmoComponent(entityId);
    return ammo && ammo->currentAmmo > 0;
}

bool AmmoSystem::CanConsume(uint32_t entityId, int amount) const {
    if (!ValidateEntityId(entityId, "CanConsume")) return false;
    if (!ValidateAmmoAmount(amount, "CanConsume")) return false;
    
    auto* ammo = GetAmmoComponent(entityId);
    return ammo && ammo->currentAmmo >= amount;
}

void AmmoSystem::ConsumeAmmo(uint32_t entityId, int amount) {
    if (!ValidateEntityId(entityId, "ConsumeAmmo")) return;
    if (!ValidateAmmoAmount(amount, "ConsumeAmmo")) return;
    
    auto* ammo = GetAmmoComponent(entityId);
    if (!ammo) return;
    
    int oldCurrent = ammo->currentAmmo;
    int oldTotal = ammo->totalAmmo;
    
    ammo->currentAmmo = std::max(0, ammo->currentAmmo - amount);
    
    PublishAmmoChangedEvent(entityId, oldCurrent, oldTotal, ammo->currentAmmo, ammo->totalAmmo);
    
    if (ammo->currentAmmo <= 0) {
        PublishAmmoDepletedEvent(entityId);
    }
    
    std::cout << "[AmmoSystem] Entity " << entityId << " consumed " << amount 
              << " ammo, remaining: " << ammo->currentAmmo << "/" << ammo->totalAmmo << std::endl;
}

void AmmoSystem::AddAmmo(uint32_t entityId, int amount) {
    if (!ValidateEntityId(entityId, "AddAmmo")) return;
    if (!ValidateAmmoAmount(amount, "AddAmmo")) return;
    
    auto* ammo = GetAmmoComponent(entityId);
    if (!ammo) return;
    
    int oldCurrent = ammo->currentAmmo;
    int oldTotal = ammo->totalAmmo;
    
    ammo->totalAmmo += amount;
    
    if (ammo->maxTotalAmmo > 0) {
        ammo->totalAmmo = std::min(ammo->totalAmmo, ammo->maxTotalAmmo);
    }
    
    // 发布弹药变化事件
    PublishAmmoChangedEvent(entityId, oldCurrent, oldTotal, ammo->currentAmmo, ammo->totalAmmo);
    
    std::cout << "[AmmoSystem] Entity " << entityId << " gained " << amount 
              << " ammo, total: " << ammo->currentAmmo << "/" << ammo->totalAmmo << std::endl;
}

int AmmoSystem::GetCurrentAmmo(uint32_t entityId) const {
    auto* ammo = GetAmmoComponent(entityId);
    return ammo ? ammo->currentAmmo : 0;
}

int AmmoSystem::GetTotalAmmo(uint32_t entityId) const {
    auto* ammo = GetAmmoComponent(entityId);
    return ammo ? ammo->totalAmmo : 0;
}

int AmmoSystem::GetMaxAmmo(uint32_t entityId) const {
    auto* ammo = GetAmmoComponent(entityId);
    return ammo ? ammo->maxTotalAmmo : 0;
}

void AmmoSystem::InitializeAmmoFromWeapon(uint32_t entityId) {
    if (!ValidateEntityId(entityId, "InitializeAmmoFromWeapon")) return;
    
    auto* ammo = GetAmmoComponent(entityId);
    if (!ammo) return;
    
    // Using default pistol value. Will replace with events or config
    ammo->currentAmmo = 12;
    ammo->totalAmmo = 120;
    ammo->maxTotalAmmo = 300;
    
    std::cout << "[AmmoSystem] Entity " << entityId << " initialized with default ammo: " 
              << ammo->currentAmmo << "/" << ammo->totalAmmo 
              << " (max: " << ammo->maxTotalAmmo << ")" << std::endl;
}

void AmmoSystem::SetAmmo(uint32_t entityId, int currentAmmo, int totalAmmo) {
    if (!ValidateEntityId(entityId, "SetAmmo")) return;
    
    auto* ammo = GetAmmoComponent(entityId);
    if (!ammo) return;
    
    int oldCurrent = ammo->currentAmmo;
    int oldTotal = ammo->totalAmmo;
    
    ammo->currentAmmo = std::max(0, currentAmmo);
    ammo->totalAmmo = std::max(0, totalAmmo);
    
    if (ammo->maxTotalAmmo > 0) {
        ammo->totalAmmo = std::min(ammo->totalAmmo, ammo->maxTotalAmmo);
    }
    
    PublishAmmoChangedEvent(entityId, oldCurrent, oldTotal, ammo->currentAmmo, ammo->totalAmmo);
    
    std::cout << "[AmmoSystem] Entity " << entityId << " ammo set to: " 
              << ammo->currentAmmo << "/" << ammo->totalAmmo << std::endl;
}

void AmmoSystem::SetMaxAmmo(uint32_t entityId, int maxAmmo) {
    if (!ValidateEntityId(entityId, "SetMaxAmmo")) return;
    
    auto* ammo = GetAmmoComponent(entityId);
    if (!ammo) return;
    
    ammo->maxTotalAmmo = std::max(0, maxAmmo);
    
    if (ammo->maxTotalAmmo > 0 && ammo->totalAmmo > ammo->maxTotalAmmo) {
        ammo->totalAmmo = ammo->maxTotalAmmo;
    }
    
    std::cout << "[AmmoSystem] Entity " << entityId << " max ammo set to: " << maxAmmo << std::endl;
}

void AmmoSystem::HandleGameEvent(const std::shared_ptr<engine::event::Event>& event) {
    auto gameEvent = std::dynamic_pointer_cast<Events::GameEvent>(event);
    if (!gameEvent) return;
    
    switch (gameEvent->GetGameEventType()) {
        case Events::GameEventType::WEAPON_FIRE_REQUESTED:
            HandleWeaponFireRequested(gameEvent->GetData());
            break;
        case Events::GameEventType::AMMO_CONSUME_REQUEST:
            HandleAmmoConsumeRequest(gameEvent->GetData());
            break;
        case Events::GameEventType::AMMO_CONSUMED:
            HandleAmmoConsumed(gameEvent->GetData());
            break;
        case Events::GameEventType::RELOAD_COMPLETED:
            HandleReloadCompleted(gameEvent->GetData());
            break;
        case Events::GameEventType::RELOAD_EXECUTE:
            HandleReloadExecute(gameEvent->GetData());
            break;
        case Events::GameEventType::RELOAD_STARTED:
            HandleReloadStarted(gameEvent->GetData());
            break;
        case Events::GameEventType::WEAPON_INITIALIZED:
            HandleWeaponInitialized(gameEvent->GetData());
            break;
        default:
            break;
    }
}

void AmmoSystem::HandleAmmoConsumeRequest(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::AmmoConsumeRequestData>(eventData);
    if (!data) return;
    
    if (!CanConsume(data->playerId, data->amount)) {
        std::cout << "[AmmoSystem] Player " << data->playerId << " cannot consume " 
                  << data->amount << " ammo - insufficient ammo" << std::endl;
        return;
    }
    
    ConsumeAmmo(data->playerId, data->amount);
    
    // Publish WEAPON_FIRED event to notify WeaponSystem to apply cooldown
    PublishWeaponFiredEvent(data->playerId);
    
    std::cout << "[AmmoSystem] Player " << data->playerId << " ammo consume request approved, " 
              << data->amount << " ammo consumed" << std::endl;
}

void AmmoSystem::HandleAmmoConsumed(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::AmmoConsumedData>(eventData);
    if (!data) return;
    
    // Handle ammo consumption statistics or other logic
    // Actual ammo quantity change has been handled in WeaponSystem
    
    std::cout << "[AmmoSystem] Handled ammo consumed event for entity " << data->entityId << std::endl;
}

void AmmoSystem::HandleReloadCompleted(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::ReloadData>(eventData);
    if (!data) return;
    
    // 🔧 修复：实际执行弹药补充！
    auto* world = GetWorld();
    if (world) {
        auto& componentManager = world->GetComponentManager();
        auto* weapon = componentManager.GetComponent<Component::WeaponComponent>(data->entityId);
        
        if (weapon) {
            // 使用武器的弹匣容量执行换弹
            ExecuteReload(data->entityId, weapon->magazineCapacity, weapon->magazineCapacity);
        }
    }
    
    auto* ammo = GetAmmoComponent(data->entityId);
    if (ammo) {
        // 清理换弹状态
        ammo->isReloading = false;
        ammo->reloadProgress = 0.0f;
    }
    
    // 清理换弹状态跟踪
    auto it = reloadStates_.find(data->entityId);
    if (it != reloadStates_.end()) {
        it->second.isActive = false;
    }
    
    std::cout << "[AmmoSystem] Reload completed for entity " << data->entityId << ", ammo replenished and state cleared" << std::endl;
}

void AmmoSystem::HandleWeaponFireRequested(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::WeaponFireRequestData>(eventData);
    if (!data) return;
    
    if (!CanConsume(data->entityId, 1)) {
        std::cout << "[AmmoSystem] Entity " << data->entityId << " cannot fire - no ammo" << std::endl;
        return;
    }
    
    ConsumeAmmo(data->entityId, 1);
    
    std::cout << "[AmmoSystem] Entity " << data->entityId << " fire request approved and ammo consumed" << std::endl;
}

bool AmmoSystem::ValidateEntityId(uint32_t entityId, const std::string& operation) const {
    auto* world = GetWorld();
    if (!world) {
        std::cerr << "[AmmoSystem] Error: No world available for " << operation << std::endl;
        return false;
    }
    
    auto& componentManager = world->GetComponentManager();
    if (!componentManager.HasComponent<Component::AmmoComponent>(entityId)) {
        std::cerr << "[AmmoSystem] Error: Entity " << entityId 
                  << " does not have AmmoComponent for " << operation << std::endl;
        return false;
    }
    
    return true;
}

bool AmmoSystem::ValidateAmmoAmount(int amount, const std::string& operation) const {
    if (amount < 0) {
        std::cerr << "[AmmoSystem] Error: Invalid ammo amount " << amount 
                  << " for " << operation << " (must be >= 0)" << std::endl;
        return false;
    }
    return true;
}

Component::AmmoComponent* AmmoSystem::GetAmmoComponent(uint32_t entityId) const {
    auto* world = GetWorld();
    if (!world) return nullptr;
    
    return world->GetComponentManager().GetComponent<Component::AmmoComponent>(entityId);
}

void AmmoSystem::PublishAmmoDepletedEvent(uint32_t entityId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    auto depletedData = std::make_shared<Events::AmmoConsumedData>();
    depletedData->entityId = entityId;
    depletedData->amountConsumed = 0;
    depletedData->currentAmmo = 0;
    depletedData->totalAmmo = GetTotalAmmo(entityId);
    
    auto depletedEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::AMMO_DEPLETED,
        std::static_pointer_cast<void>(depletedData)
    );
    depletedEvent->SetPriority(engine::event::EventPriority::HIGH);
    eventManager.Publish(depletedEvent);
}

void AmmoSystem::PublishAmmoChangedEvent(uint32_t entityId, int oldCurrent, int oldTotal, int newCurrent, int newTotal) {
    if (oldCurrent == newCurrent && oldTotal == newTotal) return;
    
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    auto changedData = std::make_shared<Events::AmmoConsumedData>();
    changedData->entityId = entityId;
    changedData->amountConsumed = oldCurrent - newCurrent;
    changedData->currentAmmo = newCurrent;
    changedData->totalAmmo = newTotal;
    
    auto changedEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::AMMO_CONSUMED,
        std::static_pointer_cast<void>(changedData)
    );
    eventManager.Publish(changedEvent);
}

void AmmoSystem::PublishWeaponFiredEvent(uint32_t entityId) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    
    auto* ammo = GetAmmoComponent(entityId);
    if (!ammo) return;
    
    auto firedData = std::make_shared<Events::WeaponFiredData>();
    firedData->entityId = entityId;
    firedData->damage = 0.0f;  // Damage will be handled by weapon system
    firedData->currentAmmo = ammo->currentAmmo;
    firedData->totalAmmo = ammo->totalAmmo;
    
    auto firedEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::WEAPON_FIRED,
        std::static_pointer_cast<void>(firedData)
    );
    eventManager.Publish(firedEvent);
}

void AmmoSystem::ExecuteReload(uint32_t entityId, int reloadAmount, int magazineCapacity) {
    if (!ValidateEntityId(entityId, "ExecuteReload")) return;
    
    auto* ammo = GetAmmoComponent(entityId);
    if (!ammo) return;
    
    int neededAmmo = magazineCapacity - ammo->currentAmmo;
    int availableAmmo = ammo->totalAmmo;
    int actualReload = std::min({reloadAmount, neededAmmo, availableAmmo});
    
    if (actualReload <= 0) {
        std::cout << "[AmmoSystem] Entity " << entityId << " - no ammo to reload" << std::endl;
        PublishReloadExecutedEvent(entityId, 0, ammo->currentAmmo, ammo->totalAmmo);
        return;
    }
    
    int oldCurrent = ammo->currentAmmo;
    int oldTotal = ammo->totalAmmo;
    
    ammo->currentAmmo += actualReload;
    ammo->totalAmmo -= actualReload;
    
    PublishAmmoChangedEvent(entityId, oldCurrent, oldTotal, ammo->currentAmmo, ammo->totalAmmo);
    PublishReloadExecutedEvent(entityId, actualReload, ammo->currentAmmo, ammo->totalAmmo);
    
    std::cout << "[AmmoSystem] Entity " << entityId << " reloaded " << actualReload 
              << " ammo, new state: " << ammo->currentAmmo << "/" << ammo->totalAmmo << std::endl;
}

void AmmoSystem::HandleReloadExecute(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::ReloadExecuteData>(eventData);
    if (!data) return;
    
    ExecuteReload(data->entityId, data->reloadAmount, data->magazineCapacity);
}

void AmmoSystem::HandleWeaponInitialized(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::WeaponInitData>(eventData);
    if (!data) return;
    
    SetAmmo(data->entityId, data->magazineCapacity, data->defaultTotalAmmo);
    SetMaxAmmo(data->entityId, data->maxTotalAmmo);
    
    std::cout << "[AmmoSystem] Entity " << data->entityId << " initialized with weapon config: " 
              << data->magazineCapacity << "/" << data->defaultTotalAmmo 
              << " (max: " << data->maxTotalAmmo << ")" << std::endl;
}

void AmmoSystem::HandleReloadStarted(const std::shared_ptr<void>& eventData) {
    auto data = std::static_pointer_cast<Events::ReloadData>(eventData);
    if (!data) return;
    
    auto* ammo = GetAmmoComponent(data->entityId);
    if (!ammo) return;
    
    // 设置AmmoComponent状态
    ammo->isReloading = true;
    ammo->reloadProgress = 0.0f;
    
    // 初始化换弹状态跟踪
    ReloadState& reloadState = reloadStates_[data->entityId];
    reloadState.reloadTime = data->reloadTime;
    reloadState.elapsedTime = 0.0f;
    reloadState.isActive = true;
    
    std::cout << "[AmmoSystem] Entity " << data->entityId << " started reloading (time: " 
              << data->reloadTime << "s), AmmoComponent state updated" << std::endl;
}

void AmmoSystem::PublishReloadExecutedEvent(uint32_t entityId, int actualReload, int newCurrent, int newTotal) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& eventManager = world->GetEventManager();
    auto executedData = std::make_shared<Events::ReloadExecutedData>();
    executedData->entityId = entityId;
    executedData->actualReloadAmount = actualReload;
    executedData->newCurrentAmmo = newCurrent;
    executedData->newTotalAmmo = newTotal;
    
    auto executedEvent = std::make_shared<Events::GameEvent>(
        Events::GameEventType::RELOAD_EXECUTED,
        std::static_pointer_cast<void>(executedData)
    );
    eventManager.Publish(executedEvent);
}

void AmmoSystem::UpdateReloadProgress(float deltaTime) {
    auto* world = GetWorld();
    if (!world) return;
    
    auto& componentManager = world->GetComponentManager();
    
    for (auto& [entityId, reloadState] : reloadStates_) {
        if (!reloadState.isActive) continue;
        
        auto* ammo = componentManager.GetComponent<Component::AmmoComponent>(entityId);
        if (!ammo || !ammo->isReloading) {
            // 换弹已结束，清理状态
            reloadState.isActive = false;
            continue;
        }
        
        // 更新进度
        reloadState.elapsedTime += deltaTime;
        float progress = std::min(1.0f, reloadState.elapsedTime / reloadState.reloadTime);
        ammo->reloadProgress = progress;
        
        // 检查是否完成（这里主要是更新进度，实际完成由WeaponSystem处理）
        if (progress >= 1.0f) {
            ammo->isReloading = false;
            ammo->reloadProgress = 0.0f;
            reloadState.isActive = false;
        }
    }
    
    // 清理非活跃状态
    auto it = reloadStates_.begin();
    while (it != reloadStates_.end()) {
        if (!it->second.isActive) {
            it = reloadStates_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace ZombieSurvivor::System 
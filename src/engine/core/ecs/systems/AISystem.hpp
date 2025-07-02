// src/engine/core/ecs/systems/AISystem.hpp
#pragma once

#include "engine/core/ecs/System.hpp"
#include "engine/core/ecs/components/AIComponent.hpp"
#include "engine/core/Types.hpp"

namespace engine::ECS {

/**
 * AISystem - 极简的 AI 系统基类
 * 
 * 提供最基础的 AI 框架，游戏继承并实现具体逻辑
 */
class AISystem : public System {
public:
    AISystem() = default;
    virtual ~AISystem() = default;

    // System 接口
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

protected:
    // 基础工具函数 - 供子类使用
    
    // 获取实体位置
    Vector2 GetEntityPosition(EntityID entity) const;
    
    // 设置实体速度（通过 Velocity2D 组件）
    void SetEntityVelocity(EntityID entity, float vx, float vy);
    
    // 移动到目标
    void MoveTowards(EntityID entity, const Vector2& target, float speed);
    
    // 停止移动
    void StopMovement(EntityID entity);
    
    // 获取两个实体间的距离
    float GetDistance(EntityID from, EntityID to) const;
    
    // 获取方向向量
    Vector2 GetDirection(const Vector2& from, const Vector2& to) const;
    
    // 状态管理
    void SetAIState(EntityID entity, AIState newState);
    
    // 核心虚函数 - 游戏必须实现
    virtual void ProcessAI(EntityID entity, AIComponent& ai, float deltaTime) = 0;
    
    // 可选的通知钩子 - 游戏可以重写
    virtual void OnStateChanged(EntityID entity, AIState oldState, AIState newState) {}

private:
    // 获取AI组件
    AIComponent* GetAIComponent(EntityID entity);
};

} // namespace engine::ECS
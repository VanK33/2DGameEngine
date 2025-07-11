# Zombie Survivor Game - TODO & Development Plan

## 🎯 Project Current Status

### ✅ **Completed Core Systems**
- **ECS Architecture**: Complete Entity-Component-System architecture
- **Player Systems**: Health, experience, upgrades, movement, input, aiming
- **Weapon Systems**: Event-driven weapon and ammunition management (recently refactored)
- **Event Systems**: Comprehensive event-driven architecture
- **Testing Framework**: Complete test scenes and debugging functionality

### ❌ **Critical Missing Components**
- Enemy AI system
- Projectile system (bullets)
- Rendering integration and visual feedback
- Actual gameplay loop

---

## 🚀 Next Development Plan (Prioritized)

### **1. Enemy System (Enemy System)** - 🔥 Highest Priority

**Goal**: Transform "test scene" into "real game"

**Components to implement**:
```cpp
struct EnemyComponent {
    EnemyType type = EnemyType::ZOMBIE_BASIC;
    float detectionRadius = 150.0f;
    float attackRange = 30.0f;
    float attackDamage = 20.0f;
    float attackCooldown = 1.5f;
    float expValue = 10.0f;
    bool isAggressive = true;
};

struct EnemyAIComponent {
    AIState state = AIState::PATROLLING;
    EntityID targetPlayer = 0;
    Vector2 lastKnownPlayerPos{0, 0};
    float aggroTimer = 0.0f;
};
```

**Systems to implement**:
- `ZombieAISystem` (inherit from engine::ECS::AISystem)
- `EnemySpawnSystem`
- Collision attack system (utilize existing CollisionSystem)

**Implementation steps**:
1. Create enemy components
2. Implement ZombieAISystem inheriting from engine::ECS::AISystem
3. Create enemy entities in ZombieTestScene
4. Configure collision rules (enemy vs player)
5. Handle collision attacks in DamageSystem

### **2. Projectile System (Projectile System)** - 🔥 High Priority

**Goal**: Implement actual bullets and shooting mechanics

**Components to implement**:
```cpp
struct ProjectileComponent {
    float damage = 15.0f;
    float speed = 400.0f;
    float lifetime = 3.0f;
    Vector2 direction{1.0f, 0.0f};
    EntityID shooterId = 0;
    bool hasHit = false;
};
```

**Systems to implement**:
- `ProjectileSystem` (handle projectile movement)
- `ProjectileCollisionSystem` (handle bullet collisions)
- Integration with existing `WeaponFireSystem`

**Implementation steps**:
1. Modify WeaponFireSystem to create bullet entities
2. Implement ProjectileSystem for trajectory handling
3. Integrate collision detection: bullets vs enemies
4. Test shooting and hit feedback

### **3. Rendering Integration and Visual Feedback** - 🔥 Medium-High Priority

**Goal**: Transform "tech demo" into "playable game"

**To implement**:
- Integrate existing entities with engine's RenderSystem
- Add sprite components to players, enemies, bullets
- Basic UI display (health, ammo, experience)
- Simple visual effects (muzzle flash, impact effects)

**Implementation steps**:
1. Add Sprite2D components to all entities
2. Integrate engine's RenderSystem
3. Add basic UI display
4. Implement simple particle effects

---

## 🎮 Attack System Design Decision

### **Phase 1: Collision Attack (Recommended for initial implementation)**

**Advantages**:
- ✅ Quick implementation: Utilize existing CollisionSystem
- ✅ Simple and direct: Zombie touches player = immediate damage
- ✅ Minimum viable product: Aligns with rapid prototyping goals

**Implementation approach**:
```cpp
// Listen to collision events in DamageSystem
void DamageSystem::HandleCollisionEvent(const CollisionEventData& data) {
    // Check if it's enemy vs player collision
    if (IsEnemy(data.entityA) && IsPlayer(data.entityB)) {
        // Add cooldown mechanism to avoid damage every frame
        static float lastAttackTime = 0.0f;
        float currentTime = SDL_GetTicks() / 1000.0f;
        
        if (currentTime - lastAttackTime > 1.0f) {
            DealDamage(data.entityB, data.entityA, 20, "zombie_melee");
            lastAttackTime = currentTime;
        }
    }
}
```

### **Phase 2: Dedicated Attack System (Future refactoring)**

When more complex attack logic is needed, create:
- `MeleeAttackSystem`
- `RangedAttackSystem`
- `AttackComponent`

---

## 📋 Specific Task Checklist

### **This Week's Tasks (Enemy System)**
- [ ] Create EnemyComponent and EnemyAIComponent
- [ ] Implement ZombieAISystem inheriting from engine::ECS::AISystem
- [ ] Create enemy entities in ZombieTestScene
- [ ] Configure collision rules (enemy vs player)
- [ ] Modify DamageSystem to handle collision attacks
- [ ] Test enemy tracking and attack behavior

### **Next Week's Tasks (Projectile System)**
- [ ] Create ProjectileComponent
- [ ] Implement ProjectileSystem
- [ ] Modify WeaponFireSystem to create bullet entities
- [ ] Integrate bullet collision detection
- [ ] Test shooting and hit feedback

### **Following Week's Tasks (Rendering Integration)**
- [ ] Add Sprite2D components to all entities
- [ ] Integrate engine's RenderSystem
- [ ] Add basic UI display
- [ ] Implement simple visual effects

---

## 🚨 Technical Debt and Considerations

### **Performance Considerations**
- Consider spatial partitioning optimization before implementing large numbers of enemies
- Use engine's QuadTree for collision detection optimization

### **Code Organization**
- Simplify test scenes: Current tests are too complex, consider creating dedicated game scenes
- Event system is already well-implemented, continue using current architecture

### **Extensibility**
- Reserve interfaces for future weapon types, enemy types, upgrade systems
- Maintain ECS architecture's modular design

---

## 🎯 Success Criteria

After completing these 3 directions, you'll have a basic but complete shooting game:
- ✅ Player can move and aim
- ✅ Enemies will track and attack the player
- ✅ Bullets can hit enemies
- ✅ Basic visual feedback and UI

This will be a truly playable game prototype, laying a solid foundation for future content expansion (different weapons, enemy types, upgrade systems, etc.).

---

## 📝 Development Log

### **2025-07-11**
- ✅ Completed weapon system refactoring
- ✅ Fixed event processing timing issues
- ✅ Improved ammunition and reload systems
- 🔄 Started enemy system development

---

*Last updated: 2025-07-11
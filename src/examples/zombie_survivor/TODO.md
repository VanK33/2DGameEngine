# Zombie Survivor Game - TODO & Development Plan

## 🎯 Project Current Status

### ✅ **Completed Core Systems**
- **ECS Architecture**: Complete Entity-Component-System architecture
- **Player Systems**: Health, experience, upgrades, movement, input, aiming
- **Weapon Systems**: Event-driven weapon and ammunition management (recently refactored)
- **Event Systems**: Comprehensive event-driven architecture
- **Testing Framework**: Complete test scenes and debugging functionality
- **🎉 Enemy System**: **NEWLY COMPLETED** - Basic enemy AI and spawning system
  - ✅ EnemyComponent and related components
  - ✅ EnemySpawnSystem (spawn control, positioning, component configuration)
  - ✅ ZombieAISystem architecture (inherits from engine::ECS::AISystem)
  - ✅ Collision-based attack system
  - ✅ Performance optimizations (spawn limits, cleanup logic)

### ❌ **Critical Missing Components**
- **Projectile system (bullets)** - 🔥 **NOW HIGHEST PRIORITY**
- **Rendering integration and visual feedback** - 🔥 **SECOND PRIORITY**
- **Actual gameplay loop** - 🔥 **THIRD PRIORITY**

---

## 🚀 Next Development Plan (Prioritized)

### **1. Projectile System (Bullet System)** - 🔥 **HIGHEST PRIORITY**

**Goal**: Implement actual bullets and shooting mechanics to complete the core gameplay loop

**Components to implement**:
```cpp
struct ProjectileComponent {
    float damage = 15.0f;
    float speed = 400.0f;
    float lifetime = 3.0f;
    Vector2 direction{1.0f, 0.0f};
    EntityID shooterId = 0;
    bool hasHit = false;
    ProjectileType type = ProjectileType::BULLET;
};
```

**Systems to implement**:
- `ProjectileSystem` (handle projectile movement and lifetime)
- `ProjectileCollisionSystem` (handle bullet vs enemy collisions)
- Integration with existing `WeaponFireSystem`

**Implementation steps**:
1. Create ProjectileComponent
2. Modify WeaponFireSystem to create bullet entities instead of direct damage
3. Implement ProjectileSystem for trajectory handling
4. Integrate collision detection: bullets vs enemies
5. Test shooting accuracy and hit feedback

### **2. Rendering Integration and Visual Feedback** - 🔥 **HIGH PRIORITY**

**Goal**: Transform "console output game" into "visual game"

**To implement**:
- Integrate existing entities with engine's RenderSystem
- Add sprite components to players, enemies, bullets
- Basic UI display (health, ammo, experience)
- Simple visual effects (muzzle flash, impact effects)

**Implementation steps**:
1. Add Sprite2D components to all entities
2. Configure engine's RenderSystem to display game entities
3. Add basic UI overlay (health bars, ammo counters)
4. Implement simple particle effects for bullets and impacts

### **3. Gameplay Loop Enhancement** - 🔥 **MEDIUM PRIORITY**

**Goal**: Complete the game experience with proper game states and progression

**To implement**:
- Wave system (increasing difficulty)
- Game state management (start, playing, game over)
- Victory/defeat conditions
- Progressive enemy spawning

**Implementation steps**:
1. Create WaveSystem for managing enemy waves
2. Implement GameStateManager
3. Add victory/defeat conditions
4. Balance difficulty progression

---

## 🎮 Current System Status

### **✅ Working Systems**
- **Player Movement**: Full 360-degree movement with WASD
- **Weapon Systems**: Firing, reloading, ammo management
- **Enemy AI**: Basic zombie tracking and collision attacks
- **Enemy Spawning**: Controlled spawn rates with proper cleanup
- **Health System**: Damage dealing and health management
- **Experience System**: XP gain and leveling up
- **Event System**: All systems communicate through events

### **🔄 Partially Working**
- **Combat System**: Collision-based attacks work, but missing ranged combat
- **Visual Feedback**: All logic works but no visual representation

### **❌ Missing**
- **Projectile Combat**: No bullets yet - enemies can attack player but player can't attack enemies
- **Visual Rendering**: Game runs in console only
- **Game Loop**: No win/lose conditions or progression

---

## 📋 Specific Task Checklist

### **This Week's Tasks (Projectile System)**
- [ ] Create ProjectileComponent
- [ ] Implement ProjectileSystem for movement and lifetime
- [ ] Modify WeaponFireSystem to create bullet entities
- [ ] Implement bullet vs enemy collision detection
- [ ] Test shooting mechanics and damage application
- [ ] Add bullet cleanup and performance optimization

### **Next Week's Tasks (Rendering Integration)**
- [ ] Add Sprite2D components to all entities
- [ ] Configure RenderSystem to display game entities
- [ ] Create basic UI overlay (health, ammo, experience)
- [ ] Test visual feedback and entity rendering

### **Following Week's Tasks (Gameplay Loop)**
- [ ] Implement WaveSystem for progressive difficulty
- [ ] Add GameStateManager for game states
- [ ] Create victory/defeat conditions
- [ ] Balance enemy spawn rates and difficulty

---

## 🚨 Technical Debt and Considerations

### **Performance Status**
- ✅ **EnemySpawnSystem**: All performance issues resolved
  - Fixed spawn limits (maxEnemies_ = 5)
  - Proper entity cleanup with ClearAllEnemies()
  - Resolved counting logic bugs
  - No more excessive zombie spawning

### **Code Organization**
- Consider creating dedicated game scenes separate from test scenes
- Event system architecture is solid, continue using current approach
- ECS architecture is clean and extensible

### **Extensibility**
- Current architecture supports future weapon types, enemy types, upgrade systems
- Maintain ECS modular design for easy content expansion

---

## 🎯 Success Criteria

After completing the next 3 priorities, you'll have a complete playable game:
- ✅ Player can move and aim (DONE)
- ✅ Enemies track and attack the player (DONE)
- ⏳ Player can shoot and kill enemies (NEXT: Projectile System)
- ⏳ Visual feedback and UI (NEXT: Rendering Integration)
- ⏳ Proper game loop with progression (NEXT: Gameplay Loop)

**Current Progress: ~70% Complete** 🎉

---

## 📝 Development Log

### **2025-07-11**
- ✅ Completed weapon system refactoring
- ✅ Fixed event processing timing issues
- ✅ Improved ammunition and reload systems
- ✅ **COMPLETED Enemy System implementation**
  - ✅ EnemySpawnSystem with proper spawn control
  - ✅ ZombieAISystem with basic tracking behavior
  - ✅ Collision-based attack system
  - ✅ Performance optimizations (spawn limits, cleanup)
- 🔄 **NEXT: Starting Projectile System development**

---

*Last updated: 2025-07-11*
*Project Status: Ready for Projectile System implementation*
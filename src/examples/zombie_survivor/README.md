# 2D Post-Apocalyptic Survival Shooter Development Plan - Manual Shooting Version

## Project Overview

This document outlines the development plan for a post-apocalyptic survival shooter game based on the CS5850 2D Game Engine. The game features a **manual aim and shoot** mechanism where players control shooting direction and timing with mouse input.

**Game Type**: 2D Top-Down Survival Shooter
**Core Mechanics**: Manual aiming and shooting, survival, upgrades, ammunition management
**Controls**: WASD movement + Mouse aiming and shooting
**Tech Stack**: Custom 2D Game Engine (ECS Architecture, SDL3, C++20)

---

## 🎯 Core Game Mechanics

### Manual Shooting System
- **Mouse Aiming**: Mouse position determines shooting direction
- **Click to Shoot**: Left mouse button click for single shots
- **Hold to Spray**: Left mouse button hold for continuous shooting
- **Ammunition Management**: Limited ammo, requires manual reload
- **Skill Requirement**: Requires accurate aiming and timing

### Survival Loop
1. **Move and Dodge** - WASD controls character movement
2. **Aim and Shoot** - Mouse aiming and shooting at enemies
3. **Ammo Management** - Monitor ammunition and reload timely
4. **Collect and Upgrade** - Gain experience from kills, upgrade abilities
5. **Survival Challenge** - Face increasingly numerous enemies

---

## Technical Architecture Design

### 🏗️ Core Component System

```cpp
// Player Component
struct PlayerComponent {
    float health = 100.0f;
    float maxHealth = 100.0f;
    float moveSpeed = 150.0f;
    int level = 1;
    int experience = 0;
    int experienceToNext = 10;
    bool isAlive = true;
};

// Manual Shooting Weapon Component
struct WeaponComponent {
    WeaponType type = WeaponType::PISTOL;
    float damage = 15.0f;
    float fireRate = 3.0f;          // Max shots per second
    float range = 250.0f;
    int ammo = 12;                  // Current ammo
    int maxAmmo = 12;               // Magazine capacity
    int totalAmmo = 120;            // Total ammo reserves
    float reloadTime = 2.0f;        // Reload time
    float timeSinceLastShot = 0.0f;
    float reloadTimer = 0.0f;
    bool isReloading = false;
    bool canShoot = true;
};

// Aiming System Component
struct AimingComponent {
    Vector2 aimDirection{1.0f, 0.0f};  // Aim direction
    Vector2 mouseWorldPos{0.0f, 0.0f}; // Mouse world coordinates
    float aimRange = 250.0f;            // Aim range
    bool isAiming = false;
    bool showAimLine = true;            // Show aim line
};

// Input State Component
struct InputStateComponent {
    bool isMovingUp = false;
    bool isMovingDown = false;
    bool isMovingLeft = false;
    bool isMovingRight = false;
    bool isShootingHeld = false;        // Mouse held state
    bool shootPressed = false;          // Mouse click event
    bool reloadPressed = false;         // R key reload
};

// Enemy Component
struct EnemyComponent {
    EnemyType type = EnemyType::ZOMBIE_BASIC;
    float health = 30.0f;
    float maxHealth = 30.0f;
    float moveSpeed = 60.0f;
    float attackDamage = 20.0f;
    float expValue = 3.0f;
    float detectionRange = 120.0f;
    float attackRange = 25.0f;
    float attackCooldown = 1.5f;
    float timeSinceLastAttack = 0.0f;
};

// Drop Item Component
struct DropItemComponent {
    ItemType type = ItemType::HEALTH_PACK;
    float value = 25.0f;
    float lifetime = 30.0f;
    bool isCollected = false;
    float collectRange = 15.0f;        // Collection range
};

// Upgrade Component
struct UpgradeComponent {
    std::vector<UpgradeType> availableUpgrades;
    std::map<UpgradeType, int> upgradeLevel;
    bool isLevelingUp = false;
};
```

---

## 📋 Phase 1: Manual Shooting Core Systems

### 🎮 1.1 Player Control and Aiming System

**Technical Implementation**:
```cpp
class PlayerControlSystem : public System {
    // Handle WASD movement input
    // Handle mouse aiming
    // Integrate movement and aiming
};

class AimingSystem : public System {
    // Mouse position to world coordinates
    // Calculate aim direction
    // Aim range limitation
    // Aiming visual feedback
};
```

**Core Features**:
- **Movement Control**:
  - WASD continuous movement control
  - Movement doesn't affect aiming
  - Upgradeable movement speed
- **Aiming Mechanism**:
  - Real-time aim direction calculation from mouse position
  - Visual aim line display
  - Aim range limitation (no infinite range shooting)
- **Visual Feedback**:
  - Player character orientation follows mouse
  - Aim line display
  - Shooting range indicator

### 🔫 1.2 Manual Shooting System

**Technical Implementation**:
```cpp
class ShootingSystem : public System {
    // Handle mouse shooting input
    // Shooting rate control
    // Bullet generation and trajectory
    // Ammunition consumption management
};

class ReloadSystem : public System {
    // Reload mechanism
    // Reload animation and timing
    // Ammunition inventory management
};
```

**Core Features**:
- **Shooting Mechanism**:
  - Left mouse button click = Single shot
  - Left mouse button hold = Continuous shooting
  - Shooting rate limitation (prevent too rapid firing)
- **Ammunition System**:
  - Limited ammo (pistol 12 rounds)
  - R key manual reload
  - Forced reload when ammo depleted
  - Total ammunition reserves limitation
- **Shooting Feedback**:
  - Shooting recoil (visual)
  - Muzzle flash effect
  - Bullet trajectory display

### 🎯 1.3 Bullet and Ballistics System

**Technical Implementation**:
```cpp
class ProjectileSystem : public System {
    // Bullet flight trajectory
    // Bullet collision detection
    // Bullet lifecycle management
};

struct ProjectileComponent {
    Vector2 velocity;
    float damage = 15.0f;
    float speed = 400.0f;
    float lifetime = 3.0f;          // Maximum bullet flight time
    float remainingTime = 3.0f;
    EntityID shooter = 0;           // Shooter ID
    bool hasHit = false;
};
```

**Core Features**:
- **Ballistics Calculation**:
  - Straight-line flight based on aim direction
  - Bullet speed and flight time
  - Auto-destroy when exceeding range
- **Collision System**:
  - Bullet-enemy collision detection
  - Bullet-wall collision (later phase)
  - One bullet can only hit one target
- **Visual Effects**:
  - Bullet flight trajectory
  - Spark effects on impact

### 🧟 1.4 Enemy AI and Tracking System

**Technical Implementation**:
```cpp
class EnemyAISystem : public engine::ECS::AISystem {
    // Inherit existing AI system
    // Intelligent player tracking
    // Attack behavior
};

class EnemySpawnSystem : public System {
    // Enemy spawn management
    // Time-based difficulty increase
    // Spawn position control
};
```

**Core Features**:
- **AI Behavior**:
  - Detect and track player
  - Attack when close to player
  - Reaction when damaged
  - Drops when killed
- **Spawn Mechanism**:
  - Spawn at screen edges
  - Time-based increasing spawn frequency
  - Avoid spawning near player
- **Attack Mechanism**:
  - Melee attack (contact damage)
  - Attack cooldown time
  - Attack animation

### 📊 1.5 Experience and Upgrade System

**Technical Implementation**:
```cpp
class ExperienceSystem : public System {
    // Experience collection and calculation
    // Level up trigger
    // Upgrade selection interface
};

class UpgradeSystem : public System {
    // Upgrade effect application
    // Upgrade option generation
    // Upgrade interface management
};
```

**Core Features**:
- **Experience Mechanism**:
  - Gain experience from enemy kills
  - Experience bar display
  - Progressive level-up thresholds
- **Upgrade Selection**:
  - Pause game during level up
  - 3 random upgrade options
  - Instant upgrade effect application
- **Basic Upgrades**:
  - Weapon damage increase
  - Shooting rate increase
  - Movement speed increase
  - Maximum health increase
  - Ammunition capacity increase
  - Reload speed increase

### 🎨 1.6 UI and User Interface

**Technical Implementation**:
```cpp
class GameUISystem : public System {
    // In-game UI display
    // Health and ammo display
    // Upgrade interface
};
```

**Core Features**:
- **Survival Information**:
  - Health bar
  - Current ammo/total ammo display
  - Survival time counter
  - Kill count statistics
- **Aiming UI**:
  - Crosshair display
  - Aim line
  - Shooting range indicator
- **Upgrade Interface**:
  - Upgrade option cards
  - Upgrade descriptions
  - Selection confirmation
- **Game State**:
  - Pause menu
  - Game over screen
  - Restart functionality

---

## 🔄 Phase 2: Extended Mechanics and Depth

### 💊 2.1 Drop System and Supply Management

**Technical Implementation**:
```cpp
class DropSystem : public System {
    // Enemy death drops
    // Drop item management
    // Collection mechanism
};
```

**Core Features**:
- **Drop Types**:
  - Health packs (restore health)
  - Ammo packs (replenish ammunition)
  - Experience gems (extra experience)
- **Drop Mechanism**:
  - Random drops from killed enemies
  - Drop items have lifetime
  - Auto-collect when nearby
- **Balance Design**:
  - Ammo drop rate based on current ammo level
  - Health pack drop rate based on current health
  - Experience gems rare drops

### 🔫 2.2 Weapon Upgrades and Diversification

**Technical Implementation**:
```cpp
class WeaponUpgradeSystem : public System {
    // Weapon attribute upgrades
    // Special ammunition types
    // Shooting mode changes
};
```

**Core Features**:
- **Weapon Attribute Upgrades**:
  - Damage increase
  - Fire rate increase
  - Ammunition capacity increase
  - Range extension
  - Reload speed increase
- **Special Ammunition**:
  - Piercing bullets (hit multiple enemies)
  - Explosive bullets (area damage)
  - Incendiary bullets (damage over time)
  - Freeze bullets (slow effect)
- **Shooting Modes**:
  - Scatter mode (shotgun effect)
  - Rapid fire mode (machine gun effect)
  - Precision mode (high damage single shot)

### 🛡️ 2.3 Defense and Survival Mechanisms

**Technical Implementation**:
```cpp
class DefenseSystem : public System {
    // Shield system
    // Dodge mechanism
    // Defense upgrades
};
```

**Core Features**:
- **Shield System**:
  - Extra health protection
  - Delayed recovery after damage
  - Shield upgrade options
- **Dodge Mechanism**:
  - Brief invincibility time
  - Cooldown limitation
  - Upgradeable dodge distance
- **Survival Skills**:
  - Health regeneration rate increase
  - Damage reduction
  - Movement speed increase

### 🎯 2.4 Enemy Diversification

**Technical Implementation**:
```cpp
class AdvancedEnemySystem : public System {
    // Multiple enemy types
    // Special enemy behaviors
    // Enemy upgrade mechanism
};
```

**Core Features**:
- **Enemy Types**:
  - Fast zombie (high speed, low health)
  - Tank zombie (low speed, high health)
  - Ranged zombie (throwing attacks)
  - Exploding zombie (suicide attacks)
- **Special Behaviors**:
  - Group assaults
  - Flanking maneuvers
  - Dodging shots
  - Coordinated attacks
- **Difficulty Scaling**:
  - Time-based attribute increases
  - New types gradually unlocked
  - Elite variants appear

---

## 🌟 Phase 3: Boss Battles and Complete Experience

### 🏆 3.1 Boss Enemy System

**Technical Implementation**:
```cpp
class BossSystem : public System {
    // Boss AI behavior
    // Special attack patterns
    // Multi-phase boss battles
};
```

**Core Features**:
- **Boss Types**:
  - Giant zombie (high health melee)
  - Zombie king (special abilities)
  - Mutant boss (random ability combinations)
- **Special Attacks**:
  - Charge attacks
  - Area attacks
  - Minion summoning
  - Projectile attacks
- **Boss Mechanics**:
  - Multiple health bar system
  - Phase-based behavior changes
  - Special drop rewards

### 🎵 3.2 Audio and Atmosphere

**Technical Implementation**:
```cpp
class GameAudioSystem : public System {
    // Shooting sound effects
    // Environmental audio
    // Dynamic music
};
```

**Core Features**:
- **Shooting Audio**:
  - Gunshot sounds (based on weapon type)
  - Reload sound effects
  - Hit sound effects
  - Empty chamber sounds
- **Environmental Audio**:
  - Zombie groaning sounds
  - Footstep sounds
  - Ambient background audio
  - Drop item sounds
- **Dynamic Music**:
  - Calm background music
  - Intense combat music
  - Boss battle exclusive music
  - Danger warning sounds

### 🎮 3.3 Advanced Gameplay Mechanics

**Technical Implementation**:
```cpp
class AdvancedGameplaySystem : public System {
    // Environmental interaction
    // Tactical mechanics
    // Achievement system
};
```

**Core Features**:
- **Environmental Interaction**:
  - Destructible cover
  - Explosive barrel traps
  - Terrain advantage utilization
- **Tactical Elements**:
  - Ammunition management strategy
  - Positional choice importance
  - Upgrade path planning
- **Achievement System**:
  - Precision shooting achievements
  - Survival time achievements
  - Special kill achievements

---

## 🎯 Controls and Input Design

### 🎮 Control Scheme

```cpp
// Input mapping
struct ControlScheme {
    // Movement controls
    SDL_Keycode moveUp = SDLK_w;
    SDL_Keycode moveDown = SDLK_s;
    SDL_Keycode moveLeft = SDLK_a;
    SDL_Keycode moveRight = SDLK_d;
    
    // Shooting controls
    Uint8 shoot = SDL_BUTTON_LEFT;      // Left mouse button
    SDL_Keycode reload = SDLK_r;        // R key reload
    
    // Special controls
    SDL_Keycode pause = SDLK_ESCAPE;    // Pause game
    SDL_Keycode inventory = SDLK_TAB;   // Open inventory/stats
};
```

### 🎯 Mouse Controls

**Aiming System**:
- **Mouse Position**: Determines aim direction
- **Left Click**: Single shot
- **Left Hold**: Continuous shooting
- **Right Click**: Future use (special abilities)

**Visual Feedback**:
- **Crosshair**: Dynamic crosshair that changes based on weapon
- **Aim Line**: Shows shooting direction and range
- **Reticle**: Indicates accuracy and weapon state

---

## 📊 Development Phases Summary

### Phase 1: Core Foundation (Basic Playable Game)
- ✅ Manual shooting mechanics
- ✅ Basic enemy AI
- ✅ Player movement and aiming
- ✅ Ammunition management
- ✅ Basic UI elements
- ✅ Experience and upgrade system

### Phase 2: Enhanced Gameplay (Depth and Variety)
- 🔄 Drop system and item collection
- 🔄 Weapon upgrades and special ammo
- 🔄 Multiple enemy types
- 🔄 Defense mechanisms
- 🔄 Improved balance and progression

### Phase 3: Polished Experience (Complete Game)
- 🔄 Boss battles and special encounters
- 🔄 Audio system and atmosphere
- 🔄 Advanced gameplay mechanics
- 🔄 Achievement system
- 🔄 Final polish and optimization

---

## 🎯 Technical Considerations

### Performance Optimization
- **Object Pooling**: Reuse bullet and enemy objects
- **Spatial Partitioning**: Efficient collision detection
- **Level of Detail**: Reduce processing for distant enemies
- **Batch Rendering**: Optimize drawing calls

### Memory Management
- **Component Allocation**: Efficient ECS memory usage
- **Asset Loading**: Smart texture and audio loading
- **Garbage Collection**: Minimize memory allocations

### Scalability
- **Modular Design**: Easy to add new weapons and enemies
- **Configuration Files**: Tweakable game balance
- **Save System**: Progress persistence
- **Difficulty Scaling**: Adaptive challenge system

---

*This development plan provides a comprehensive roadmap for creating a manual-aim survival shooter game using the CS5850 2D Game Engine. The phased approach ensures steady progress while maintaining flexibility for feature adjustments and improvements.*
```

</rewritten_file>
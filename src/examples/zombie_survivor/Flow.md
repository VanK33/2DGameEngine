# GameScene Flow Architecture

## GameScene Lifecycle

```
┌─────────────────────────────────────────────────────────────────────┐
│                           GAMESCENE LIFECYCLE                       │
└─────────────────────────────────────────────────────────────────────┘

┌─────────────┐
│  Load()     │
│ (Scene Init)│
└─────┬───────┘
      │
      ▼
┌─────────────┐     ┌─────────────────────────────────────────────────┐
│ Get World   │────▶│ InitializeSystems() - Priority Order:           │
│ & Setup     │     │ • GroundRenderSystem      (15)                  │
│ Factory     │     │ • InputSystem             (25)                  │
└─────┬───────┘     │ • MovementSystem          (30)                  │
      │             │ • WeaponFollowSystem      (35)                  │
      ▼             │ • AimingSystem            (38)                  │
┌──────────────┐    │ • RotationSystem          (40)                  │
│CreateEntities│    │ • WeaponInputSystem       (42)                  │
│ • Player     │    │ • WeaponFireSystem        (45)                  │
│ • Weapon     │    │ • ProjectileSystem        (50)                  │
└─────┬────────┘    └─────────────────────────────────────────────────┘
      │
      ▼
┌─────────────────────────────────────────────────────────────────────┐
│                         GAME LOOP                                   │
└─────────────────────────────────────────────────────────────────────┘

┌─────────────┐
│ HandleEvent │
│ (ESC→Quit)  │
└─────────────┘
      │
      ▼
┌─────────────┐    ┌─────────────────────────────────────────────────┐
│ Update()    │───▶│ SystemManager.Update() - Executes in Order:     │
│ (deltaTime) │    │                                                 │
└─────────────┘    │ [15] GroundRenderSystem                         │
      │            │ [25] InputSystem ─────────┐                     │
      ▼            │ [30] MovementSystem       │                     │
┌─────────────┐    │ [35] WeaponFollowSystem   │                     │
│ Render()    │    │ [38] AimingSystem         │                     │
│ • Debug     │    │ [40] RotationSystem       │                     │
│   Aiming    │    │ [42] WeaponInputSystem    │                     │
│ • Mouse     │    │ [45] WeaponFireSystem     │                     │
│   Crosshair │    │ [50] ProjectileSystem     │                     │
└─────────────┘    └─────────────────────────────────────────────────┘
```

## Current Data Flow

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Player    │    │   Weapon    │    │ Projectile  │    │   Debug     │
│ (Entity 1)  │    │ (Entity 2)  │    │ (Spawned)   │    │ Rendering   │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
│                  │                  │                  │
│ • Transform2D    │ • Transform2D    │ • Transform2D    │ • Mouse Cross
│ • Sprite2D       │ • Sprite2D       │ • Sprite2D       │ • Aim Line
│ • InputComponent │ • FollowComponent│ • Velocity2D     │ • Weapon Center
│ • Movement...    │ • InputComponent │ • Projectile...  │
│                  │ • AimingComponent│                  │
└─────────────────┬┘                  │                  │
                  │                   │                  │
                  ▼                   ▼                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│                    SYSTEM EXECUTION FLOW                            │
│                                                                     │
│ Input → Movement → WeaponFollow → Aiming → Rotation → WeaponInput   │
│                                                    ↓                │
│                              WeaponFire → ProjectileSystem          │
└─────────────────────────────────────────────────────────────────────┘
```

## System Priority & Dependencies

| Priority | System | Dependencies | Purpose |
|----------|--------|--------------|---------|
| 15 | GroundRenderSystem | None | Render background tiles |
| 25 | InputSystem | InputManager | Capture keyboard/mouse input |
| 30 | MovementSystem | InputComponent | Process WASD movement |
| 35 | WeaponFollowSystem | Player Transform | Position weapon at player |
| 38 | AimingSystem | Mouse Input | Calculate aim direction |
| 40 | RotationSystem | AimingComponent | Apply rotation to weapon |
| 42 | WeaponInputSystem | InputComponent | Process weapon input |
| 45 | WeaponFireSystem | WeaponInput | Create projectiles |
| 50 | ProjectileSystem | Projectile entities | Move bullets |

## Entity Component Architecture

### Player Entity (ID: 1)
```
┌──────────────────┐
│ Transform2D      │ ← Position, rotation
│ Sprite2D         │ ← 64x64 red square, ENTITIES layer
│ InputComponent   │ ← WASD keys, mouse position
│ MovementComponent│ ← Movement speed and direction
│ Velocity2D       │ ← Physics velocity
│ PhysicsMode...   │ ← Physics integration
└──────────────────┘
```

### Weapon Entity (ID: 2)
```
┌─────────────────┐
│ Transform2D     │ ← Position at player center-5px
│ Sprite2D        │ ← 40x12 green rectangle, WEAPON layer
│ FollowComponent │ ← Follow player with {-5, 0} offset
│ InputComponent  │ ← Copied from player
│ AimingComponent │ ← Aim direction to mouse
└─────────────────┘
```

### Projectile Entity (Spawned)
```
┌─────────────────┐
│ Transform2D     │ ← Position at weapon muzzle
│ Sprite2D        │ ← Small bullet sprite
│ Velocity2D      │ ← Movement direction & speed
│ ProjectileComp..│ ← Lifetime, damage, etc.
└─────────────────┘
```

## Current Issues Identified

### 1. Projectile System Issue 🔫
- **Problem**: WeaponFireSystem exists but bullets not spawning
- **Location**: `WeaponFireSystem.cpp` line ~45
- **Status**: Needs debugging

### 2. Missing Enemy System 🧟
- **Problem**: No zombie entities or AI
- **Needed**: ZombieComponent, spawning system, basic AI
- **Status**: Not implemented

### 3. Missing Collision Consequences 💥
- **Problem**: No health/damage system
- **Needed**: Health components, damage events
- **Status**: Not implemented

### 4. Missing Animation System 🎬
- **Problem**: Static sprites only
- **Needed**: AnimationComponent, sprite sheets
- **Status**: Not implemented

## Debug Features

The GameScene includes debug rendering in `RenderDebugAiming()`:
- **Red Cross**: Mouse cursor position
- **Green Line**: Weapon aim direction (50px long)
- **Yellow Cross**: Weapon center position

## Next Sprint Goals

1. **Day 1**: Fix projectile spawning and movement
2. **Day 2-3**: Add zombie enemies with collision detection  
3. **Day 3-4**: Implement animation system for all entities

## File Structure

```
src/examples/zombie_survivor/
├── GameScene.hpp/.cpp          # Main scene controller
├── ecs/
│   ├── GameEntityFactory.hpp   # Entity creation
│   ├── components/             # Game-specific components
│   │   ├── FollowComponent.hpp
│   │   ├── AimingComponent.hpp
│   │   └── InputComponent.hpp
│   └── systems/               # Game-specific systems
│       ├── InputSystem.cpp
│       ├── MovementSystem.cpp
│       ├── WeaponFollowSystem.cpp
│       ├── AimingSystem.cpp
│       ├── RotationSystem.cpp
│       ├── WeaponInputSystem.cpp
│       ├── WeaponFireSystem.cpp
│       └── ProjectileSystem.cpp
```
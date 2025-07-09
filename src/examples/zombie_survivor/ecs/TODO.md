# Zombie Survivor ECS Architecture TODO

This file serves as a tracking record for the components and systems needed for the zombie survivor game.

## Player Entity Components

The following components are required for the Player Entity:

1. HealthComponent [✓]
2. MovementComponent [✓]
3. UpgradeComponent [✓]
4. InputComponent [✓]
5. WeaponComponent [✓]
6. AmmoComponent [✓]
7. AimingComponent [✓]

## Engine Components (Reused)

The following engine-provided components will be used:

1. Transform2D (position, rotation, scale)
2. Velocity2D (movement velocity)
3. Sprite2D (visual representation)

## Required Systems

The following systems need to be implemented:

1. InputSystem (handle player input) [✓]
2. MovementSystem (handle movement and steering) [✓]  
3. WeaponSystem (handle shooting logic, use weapon blueprint) [✓] 
4. AmmoSystem (handle ammo management and reloading, initialize from weapon config) [✓] 
5. AimingSystem (calculate aiming direction, manage aiming state) 
6. HealthSystem (handle health changes and damage) [✓]
7. DamageSystem (handle damage calculation and distribution) [✓]
8. ExperienceSystem (handle experience gaining and leveling) [✓]
9. UpgradeSystem (handle skill upgrades and player progression) [✓]
10. PlayerStatsSystem (handle player statistics tracking and management) [✓]

## Engine Systems (Reused)

The following engine-provided systems will be used:

1. RenderSystem (rendering entities)
2. PhysicsSystem (physics simulation)
3. CollisionSystem (collision detection)

## Design Notes

- **WeaponComponent**: Static weapon configuration (blueprint) - defines weapon capabilities
- **AmmoComponent**: Dynamic ammo state - gets initialized from WeaponComponent when weapon is equipped
- **Weapon Switching Flow**: WeaponComponent provides config → AmmoComponent gets initialized with weapon's default values
- **AimingComponent**: Under consideration - might be handled entirely within AimingSystem
- **DamageSystem**: Event-driven damage processing - listens to collision events, calculates damage, and publishes damage events for HealthSystem to process. This decouples damage calculation from health management.
- **Event Communication**: DamageSystem → HealthSystem communication happens via DAMAGE_TAKEN events, eliminating direct dependencies between systems.

## System Architecture Separation

**Player-related functionality is split into focused systems:**

- **HealthSystem**: Manages entity health, death processing, and health-related events
- **ExperienceSystem**: Handles experience gaining, level calculations, and level-up events  
- **PlayerStatsSystem**: Tracks player statistics (kills, survival time, damage dealt/taken, etc.)
- **UpgradeSystem**: Manages skill trees, upgrades, and player progression choices

This separation follows the Single Responsibility Principle and improves maintainability.
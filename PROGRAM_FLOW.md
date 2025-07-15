# Program Flow Documentation

## ZombieSurvivor Game - Function Call Sequence

This document shows the execution flow of the ZombieSurvivor game from startup to shutdown, including the sequence of function calls between `main.cpp` and `GameScene.cpp`.

## ASCII Flow Chart

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                   MAIN.CPP                                      │
└─────────────────────────────────────────────────────────────────────────────────┘
                                        │
                                        │
                                        ▼
    ┌─────────────────────────────────────────────────────────────────────────────┐
    │ main()                                                                      │
    │ ├─ Create engine::Engine gameEngine                                         │
    │ ├─ Configure EngineConfig (800x600, 60 FPS)                                 │
    │ ├─ gameEngine.Initialize(config)                                            │
    │ │  │                                                                        │
    │ │  ├─ renderer_.Init()                                                      │
    │ │  ├─ spriteRenderer_ = new SpriteRenderer()                                │
    │ │  ├─ resourceManager_ = new ResourceManager()                              │
    │ │  └─ InitializeSystems()                                                   │
    │ │     ├─ Add CollisionSystem (priority 10)                                  │
    │ │     ├─ Add PhysicsSystem (priority 20)                                    │
    │ │     ├─ Add LifetimeSystem (priority 30)                                   │
    │ │     ├─ Add RenderSystem (priority 50) ← CORE RENDERING                    │
    │ │     └─ Add DebugRenderSystem (priority 100)                               │
    │ │                                                                           │
    │ ├─ RegisterScene<GameScene>("game_scene", inputManager, resourceManager)    │ 
    │ ├─ sceneManager.RequestSceneChange("game_scene")                            │
    │ └─ gameEngine.Run() ────────────────────────────────────────────────────────┼─┐
    └─────────────────────────────────────────────────────────────────────────────┘ │
                                                                                    │
                                                                                    │
┌───────────────────────────────────────────────────────────────────────────────────┘
│
│   ┌─────────────────────────────────────────────────────────────────────────────┐
│   │                            GAME LOOP (Engine::Run)                          │
│   │                                                                             │
│   │  while (isRunning_) {                                                       │
│   │    ├─ UpdateTiming()                                                        │
│   │    ├─ HandleEvents()                                                        │
│   │    ├─ UpdateSystems()                                                       │
│   │    │  ├─ inputManager_.Update()                                             │
│   │    │  ├─ eventManager_.Update()                                             │
│   │    │  ├─ world_->Update(deltaTime) ← Updates ALL ECS Systems                │
│   │    │  └─ sceneManager_.Update(deltaTime) ───────────────────────────────────┼─┐
│   │    │                                                                        │ │
│   │    └─ Render()                                                              │ │
│   │       ├─ renderer_.BeginFrame()                                             │ │
│   │       ├─ sceneManager_.Render(renderer) ────────────────────────────────────┼─┼─┐
│   │       └─ renderer_.EndFrame()                                               │ │ │
│   │  }                                                                          │ │ │
│   └─────────────────────────────────────────────────────────────────────────────┘ │ │
│                                                                                   │ │
└───────────────────────────────────────────────────────────────────────────────────┘ │
                                                                                      │
┌─────────────────────────────────────────────────────────────────────────────────────┘
│
│   ┌─────────────────────────────────────────────────────────────────────────────┐
│   │                              GAMESCENE.CPP                                  │
│   └─────────────────────────────────────────────────────────────────────────────┘
│                                        │
│                                        │
│                                        ▼
│    ┌─────────────────────────────────────────────────────────────────────────────┐
│    │ Scene Loading Phase                                                         │
│    │                                                                             │
│    │ GameScene::GameScene(id, inputManager, resourceManager)                     │
│    │ └─ Store references to inputManager_ and resourceManager_                   │
│    │                                                                             │
│    │ SceneManager calls:                                                         │
│    │ └─ GameScene::Load()                                                        │
│    │    ├─ world_ = GetWorld() ← Get World from Engine                           │
│    │    ├─ gameEntityFactory_ = new GameEntityFactory(world_, resourceManager_)  │
│    │    ├─ InitializeSystems()                                                   │
│    │    │  └─ Add GroundRenderSystem (priority 15)                               │
│    │    │     └─ Creates background and decoration entities                      │
│    │    │                                                                        │
│    │    └─ CreateEntities()                                                      │
│    │       └─ gameEntityFactory_->CreatePlayer()                                 │
│    │          ├─ Create Entity ID                                                │
│    │          ├─ Add Transform2D component (position: 400, 300)                  │
│    │          ├─ Add Sprite2D component                                          │
│    │          │  ├─ texture: "pixel.png"                                         │
│    │          │  ├─ size: 64x64                                                  │
│    │          │  ├─ tint: RED (255, 0, 0, 255)                                   │
│    │          │  └─ renderLayer: UI (20)                                         │
│    │          └─ Add Collider2D component                                        │
│    │                                                                             │
│    └─────────────────────────────────────────────────────────────────────────────┘
│                                        │
│                                        │
└────────────────────────────────────────┘
                                         │
                                         │
                                         ▼
     ┌─────────────────────────────────────────────────────────────────────────────┐
     │ Update Phase (called every frame)                                           │
     │                                                                             │
     │ GameScene::Update(deltaTime)                                                │
     │ └─ world_->GetSystemManager().Update(deltaTime)                             │
     │    ├─ CollisionSystem::Update() (priority 10)                               │
     │    ├─ GroundRenderSystem::Update() (priority 15)                            │
     │    ├─ PhysicsSystem::Update() (priority 20)                                 │
     │    ├─ LifetimeSystem::Update() (priority 30)                                │
     │    ├─ RenderSystem::Update() (priority 50) ← SPRITE RENDERING               │
     │    │  ├─ CollectRenderableSprites()                                         │
     │    │  │  ├─ Query entities with Transform2D + Sprite2D                      │
     │    │  │  ├─ Filter by visible = true                                        │
     │    │  │  └─ Found: Player entity (1)                                        │
     │    │  ├─ Sort by renderLayer (background=0, UI=20)                          │
     │    │  └─ For each sprite:                                                   │
     │    │     ├─ Get texture from ResourceManager                                │
     │    │     ├─ Apply color tint (RED for player)                               │
     │    │     └─ SpriteRenderer::Draw()                                          │
     │    │                                                                        │
     │    └─ DebugRenderSystem::Update() (priority 100)                            │
     │                                                                             │
     └─────────────────────────────────────────────────────────────────────────────┘
                                         │
                                         │
                                         ▼
     ┌─────────────────────────────────────────────────────────────────────────────┐
     │ Render Phase (called every frame)                                           │
     │                                                                             │
     │ GameScene::Render(renderer)                                                 │
     │ └─ (Currently empty - all rendering handled by RenderSystem)                │
     │                                                                             │
     │ Note: Engine's RenderSystem handles all sprite rendering automatically      │
     │ Background: Sky blue (135, 206, 235, 180) from GroundRenderSystem           │
     │ Player: Red square (255, 0, 0, 255) at center (400, 300)                    │
     │                                                                             │
     └─────────────────────────────────────────────────────────────────────────────┘
                                         │
                                         │
                                         ▼
     ┌─────────────────────────────────────────────────────────────────────────────┐
     │ Event Handling                                                              │
     │                                                                             │
     │ GameScene::HandleEvent(event)                                               │
     │ └─ if (event.type == SDL_EVENT_KEY_DOWN)                                    │
     │    └─ if (event.key.key == SDLK_ESCAPE)                                     │
     │       └─ Request engine shutdown                                            │
     │                                                                             │
     └─────────────────────────────────────────────────────────────────────────────┘
                                         │
                                         │
                                         ▼
     ┌─────────────────────────────────────────────────────────────────────────────┐
     │ Shutdown Phase                                                              │
     │                                                                             │
     │ GameScene::Unload()                                                         │
     │ ├─ world_->GetSystemManager().ClearAllSystems()                             │
     │ └─ world_->ClearAllEntities()                                               │
     │                                                                             │
     │ Engine::Shutdown()                                                          │
     │ ├─ world_->GetSystemManager().ClearAllSystems()                             │
     │ ├─ world_->ClearAllEntities()                                               │
     │ ├─ spriteRenderer_.reset()                                                  │
     │ ├─ resourceManager_.reset()                                                 │
     │ └─ renderer_.Shutdown()                                                     │
     │                                                                             │
     └─────────────────────────────────────────────────────────────────────────────┘
```

## Key Function Call Sequence

### 1. Initialization Sequence
1. `main()` → `Engine::Initialize()` → Core ECS systems setup
2. `main()` → `Engine::RegisterScene<GameScene>()` → GameScene constructor
3. `main()` → `SceneManager::RequestSceneChange()` → `GameScene::Load()`
4. `GameScene::Load()` → `InitializeSystems()` → `CreateEntities()`

### 2. Runtime Loop (60 FPS)
1. `Engine::Run()` → `UpdateSystems()` → `SceneManager::Update()` → `GameScene::Update()`
2. `GameScene::Update()` → `World::Update()` → All ECS systems update
3. `Engine::Run()` → `Render()` → `SceneManager::Render()` → `GameScene::Render()`
4. `RenderSystem::Update()` → Renders all sprites including player

### 3. Current State
- **Player Entity**: Created with red tint, 64x64 size, at center (400, 300)
- **Background**: Sky blue with transparency from GroundRenderSystem
- **Rendering**: Engine's RenderSystem processes all Sprite2D components
- **Input**: ESC key exits the game

## System Architecture Notes

- **Engine owns core systems** (RenderSystem, PhysicsSystem, etc.)
- **GameScene adds game-specific systems** (GroundRenderSystem)
- **Entity creation** happens in GameScene::CreateEntities()
- **Rendering pipeline** is automatic through ECS RenderSystem
- **No manual rendering** needed in GameScene::Render()
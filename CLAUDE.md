# CS5850 2D Game Engine Development Partnership

We're building a modular 2D game engine focused on **educational game development** for CS5850 assignments. Your role is to create maintainable, efficient solutions while leveraging our existing ECS architecture and completed systems.

When you seem stuck or overly complex, I'll redirect you - my guidance helps you stay on track.

## <® PROJECT CONTEXT: 2D Game Engine Architecture
**Current Status**: Core engine systems COMPLETED  (ECS, Collision, Physics, Input, Events)
**Focus**: Building games that demonstrate engine capabilities
**Tech Stack**: C++20, SDL3, ECS Architecture, CMake

## =¨ BUILD SYSTEM VALIDATION 
**ALL build issues are BLOCKING - EVERYTHING must compile and run!**  
No compilation errors. No CMake issues. No linking problems. Zero tolerance.  
Test your changes by building the project before continuing.

## CRITICAL WORKFLOW - ALWAYS FOLLOW THIS!

### Research ’ Plan ’ Implement
**NEVER JUMP STRAIGHT TO CODING!** Always follow this sequence:
1. **Research**: Explore the codebase, understand existing ECS patterns and engine systems
2. **Plan**: Create a detailed implementation plan considering our modular architecture  
3. **Implement**: Execute the plan with validation checkpoints

When asked to implement any feature, you'll first say: "Let me research the codebase and create a plan before implementing."

For complex architectural decisions or challenging problems, use **"ultrathink"** to engage maximum reasoning capacity. Say: "Let me ultrathink about this architecture before proposing a solution."

### USE MULTIPLE AGENTS!
*Leverage subagents aggressively* for better results:

* Spawn agents to explore different parts of the codebase in parallel
* Use one agent to write tests while another implements features
* Delegate research tasks: "I'll have an agent investigate the database schema while I analyze the API structure"
* For complex refactors: One agent identifies changes, another implements them

Say: "I'll spawn agents to tackle different aspects of this problem" whenever a task has multiple independent parts.

### Reality Checkpoints
**Stop and validate** at these moments:
- After implementing a complete ECS system or component
- Before starting a new game feature  
- When something feels wrong
- Before declaring "done"
- **WHEN BUILD FAILS** L

Run: `cmake --build build && ./build/debug_scene` (or equivalent for your target)

> Why: You can lose track of what's actually working. These checkpoints prevent cascading failures in our complex engine architecture.

### =¨ CRITICAL: Build Failures Are BLOCKING
**When build reports ANY issues, you MUST:**
1. **STOP IMMEDIATELY** - Do not continue with other tasks
2. **FIX ALL ISSUES** - Address every compilation and linking error
3. **VERIFY THE FIX** - Re-run the build to confirm it compiles and runs
4. **CONTINUE ORIGINAL TASK** - Return to what you were doing before the interrupt
5. **NEVER IGNORE** - There are NO warnings in our engine, only requirements

This includes:
- Compilation errors (C++ syntax, missing includes, etc.)
- Linking errors (missing libraries, undefined symbols)
- CMake configuration issues
- Runtime crashes during basic testing

Your code must compile and run successfully. No exceptions.

**Recovery Protocol:**
- When interrupted by a build failure, maintain awareness of your original task
- After fixing all issues and verifying the fix, continue where you left off
- Use the todo list to track both the fix and your original task

## Working Memory Management

### When context gets long:
- Re-read this CLAUDE.md file
- Summarize progress in a PROGRESS.md file
- Document current state before major changes

### Maintain TODO.md:
```
## Current Task
- [ ] What we're doing RIGHT NOW

## Completed  
- [x] What's actually done and tested

## Next Steps
- [ ] What comes next
```

## C++20 Engine-Specific Rules

### FORBIDDEN - NEVER DO THESE:
- **NO raw pointers** for owned resources - use smart pointers!
- **NO manual memory management** - use RAII and smart pointers!
- **NO** keeping old and new code together
- **NO** migration functions or compatibility layers  
- **NO** versioned function names (processV2, handleNew)
- **NO** complex inheritance hierarchies without clear justification
- **NO** TODOs in final code

> **BUILD ENFORCEMENT**: The compiler will catch most violations.  
> When you see compilation errors, you MUST fix them immediately!

### Required Standards:
- **Delete** old code when replacing it
- **Meaningful names**: `entityID` not `id`, `playerComponent` not `pc`
- **Early returns** to reduce nesting
- **RAII throughout**: All resources managed automatically
- **Smart pointers**: `std::unique_ptr`, `std::shared_ptr` for ownership
- **ECS patterns**: Components are data, Systems are behavior
- **Engine/Game separation**: Engine provides systems, games provide specific components
- **SDL3 integration**: Use SDL3 features consistently
- **CMake organization**: Follow existing build structure

## Implementation Standards

### Our code is complete when:
-  Project builds successfully with CMake
-  All engine systems integrate properly  
-  Feature works end-to-end in a test game
-  Old code is deleted
-  Comments on all public engine interfaces

### Testing Strategy
- Complex ECS systems ’ Write tests first (use DebugScene)
- Simple components ’ Write tests after
- Performance-critical systems ’ Add benchmarks  
- Skip tests for simple data structures and basic components

### Project Structure
```
src/engine/         # Core engine systems
src/examples/       # Test games and demos
build/             # CMake build output
.claude/           # Claude development guidance
```

## Problem-Solving Together

When you're stuck or confused:
1. **Stop** - Don't spiral into complex solutions
2. **Delegate** - Consider spawning agents for parallel investigation
3. **Ultrathink** - For complex problems, say "I need to ultrathink through this challenge" to engage deeper reasoning
4. **Step back** - Re-read the requirements
5. **Simplify** - The simple solution is usually correct
6. **Ask** - "I see two approaches: [A] vs [B]. Which do you prefer?"

My insights on better approaches are valued - please ask for them!

## Performance & Game Development

### **Measure First**:
- No premature optimization
- Benchmark before claiming something is faster
- Use profilers for real bottlenecks (especially for ECS systems)
- Target 60 FPS for game performance

### **Game Engine Best Practices**:
- Validate all user input (especially for file loading)
- Use SDL3 security practices
- Efficient ECS queries (cache results when possible)
- Smart resource management (texture loading, etc.)

## Communication Protocol

### Progress Updates:
```
 Implemented PlayerComponent (ECS integration complete)
 Added shooting system (collision detection working)  
 Found issue with enemy spawning - investigating
```

### Suggesting Improvements:
"The current ECS approach works, but I notice [observation].
Would you like me to [specific improvement]?"

## Working Together

- This is always a feature branch - no backwards compatibility needed  
- When in doubt, we choose clarity over cleverness
- **Single Game Focus**: ZombieSurvivor demonstrates ALL engine capabilities
- **REMINDER**: If this file hasn't been referenced in 30+ minutes, RE-READ IT!

Avoid complex abstractions or "clever" code. The simple, obvious solution is probably better, and my guidance helps you stay focused on what matters.

## <¯ Engine-Specific Guidelines

### **Remember Our ECS Architecture:**
- **Engine provides:** Core systems (ECS, Collision, Physics, Input, Events, Rendering)
- **ZombieSurvivor provides:** Game-specific components (Health, Weapon, Enemy, PowerUp)
- **Engine handles:** How things work (movement, collision, rendering, AI framework)
- **Game handles:** What things are (player stats, enemy types, weapon properties)

### **Current Engine Status:**
-  **ECS Core**: Complete entity-component-system with World management
-  **Collision System**: AABB detection with layer-based filtering
-  **Physics System**: Velocity-based movement with constraints
-  **Input System**: Keyboard/mouse with state tracking and events
-  **Event System**: Publisher-subscriber with filtering and priorities
-  **AI Framework**: Base system for game-specific AI implementations
- =Ë **UI System**: Planned for ZombieSurvivor interface
- =Ë **Audio System**: Planned for ZombieSurvivor sound effects and music

### **Development Focus:**
- Build ZombieSurvivor game that showcases ALL engine capabilities
- Manual aim-and-shoot mechanics with WASD movement
- Survival gameplay with enemy AI, upgrades, and progression
- Single comprehensive game validates entire engine architecture
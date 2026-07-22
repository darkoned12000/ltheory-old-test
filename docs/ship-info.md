# Ship System Documentation

> Complete reference for how ships, stations, weapons, AI, and combat work
> in the Limit Theory engine. Reverse-engineered from C++ source and LTSL scripts.

---

## 1. Ship Creation Pipeline

Ships are created in a **two-step process**: define a blueprint (`Item_ShipType`),
then instantiate a live object from it.

### Step 1: `Item_ShipType` — The Blueprint

```ltsl
var shipType (Item_ShipType value seed capacity compactness integrity propulsion systems turrets)
```

| Parameter      | Type    | Meaning                                      | Default |
|----------------|---------|----------------------------------------------|---------|
| `value`        | Float   | Total budget in credits (determines size)    | —       |
| `seed`         | Int     | RNG seed for procedural mesh generation      | —       |
| `capacity`     | Float   | Cargo capacity multiplier                    | 1.0     |
| `compactness`  | Float   | Mass density multiplier                      | 1.0     |
| `integrity`    | Float   | Health multiplier                            | 1.0     |
| `propulsion`   | Float   | Thrust multiplier                            | 1.0     |
| `systems`      | Float   | Generator power multiplier                   | 1.0     |
| `turrets`      | Float   | Turret count multiplier                      | 1.0     |

**Convenience overload** (defaults all tuning to 1.0):
```ltsl
Item_ShipType value seed
```

#### What happens under the hood

1. **Budget allocation** (`ShipType.cpp:188-197`):
   - 60% → hull value
   - Remaining 40% splits between thruster and generator value

2. **Derives stats from hull value**:
   - `capacity` = `Constant_ValueToCapacity(hullValue, capacity)`
   - `integrity` = `Constant_ValueToIntegrity(hullValue, integrity)`
   - `mass` = `Constant_ValueToMass(hullValue, compactness)`
   - `scale` = `Constant_MassToScale(mass)`

3. **Procedural mesh generation** — calls LTSL script `Item/ShipType/Generate:Main`:
   - Creates a `PlateMesh` (quality level 12)
   - Starts with 2 base boxes (wide-flat + tall-deep)
   - Iteratively adds `2 + sqrt(scale)` plates
   - Each plate: picks a random existing box, picks a random axis, creates a
     smaller box adjacent to it, repeats 1-5 times
   - For larger ships (`scale >= 10`): adds mirrored bottom panels
   - Applies warps: `VerticalCompress` (squish Y) and `HExpand` (widen X at tail)
   - Centers mesh, computes occlusion, creates `Model` with `Material_Metal`

4. **Socket placement**:
   - **Thruster sockets**: Up to 10 pairs on hull surface (rear, lateral, top, bottom, forward)
   - **Turret sockets**: 4 pairs at random surface positions (line-of-sight checked)
   - **Generator sockets**: 1-3 based on `logScale`
   - **Interior sockets**: Proportional to `logScale`

5. **Standard fitted components**:
   - `standardThruster` — default thruster (plugs into all thruster sockets)
   - `standardGenerator` — power generator
   - `standardScanner` — scanner

### Step 2: `.Instantiate` — Create the Live Ship

```ltsl
var ship shipType.Instantiate
```

This creates an `Object_Ship` with **22 components**:

| Component       | Purpose                                    |
|-----------------|--------------------------------------------|
| Affectable      | Runs affector list (e.g. player input)     |
| Asset           | Ownership tracking (which Player owns it)  |
| BoundingBox     | AABB for spatial queries                   |
| Cargo           | Item inventory (Map of Item→Quantity)       |
| Collidable      | Collision detection and resolution          |
| Crew            | Crew slots                                 |
| Cullable        | Distance-based rendering culling           |
| Database        | Data storage                               |
| Detectable      | Detection time tracking (for sensors)      |
| Drawable        | Holds Renderable (mesh) and draws it       |
| Explodable      | Creates explosion effect on death          |
| Integrity       | Health/maxHealth, ApplyDamage, OnDeath     |
| Motion          | Force/torque/velocity/mass/inertia         |
| MotionControl   | SDF-based autopilot steering               |
| Nameable        | Ship name                                  |
| Orientation     | Transform (position, look, up, scale)      |
| Pilotable       | Holds Player reference; calls pilot->Update |
| Scriptable      | Attaches LTSL scripts with Update/Receive  |
| Sockets         | Array of Socket slots for child objects    |
| Supertyped      | Links to the Item_ShipType blueprint       |
| Targets         | List of target objects (for weapons)       |
| Tasks           | Stack of TaskInstance objects (AI behavior) |

`.Instantiate` also auto-plugs:
- Standard thruster (fills all thruster sockets)
- Standard generator
- Standard scanner

---

## 2. Ship Size Tiers

From the existing apps, ships use these value ranges:

| Tier    | Value     | Description          | Example ships               |
|---------|-----------|----------------------|-----------------------------|
| Fighter | 10,000    | Small, fast          | `war.lts` light fighters    |
| Corvette| 100,000   | Medium               | `dogfight.lts` enemies      |
| Frigate | 1,000,000 | Large                | `dogfight.lts` player ship  |
| Capital | 10,000,000| Massive              | `war.lts` capital ships     |

The value directly determines mass, scale, health, and cargo capacity through
the `Constant_ValueTo*` functions in `ShipType.cpp`.

---

## 3. Weapons

### Creating Weapons

```ltsl
var weaponType (Item_WeaponType seed)
```

`Item_WeaponType(seed)` creates a procedural weapon with:
- **Class**: Random from Pulse, Missile, Beam, Rail
- **Stats**: Damage, range, speed, fire rate — all seed-driven
- **Visual**: Color, scale, sound effects
- **Socket type**: `SocketType_Turret`

### Weapon Classes

| Class    | Behavior                                      |
|----------|-----------------------------------------------|
| Pulse    | Fast-firing projectiles, hitscan-like          |
| Missile  | Slow, homing, high damage                      |
| Beam     | Continuous damage (currently `#if 0` disabled) |
| Rail     | Fast, high-spread projectiles                  |

### Equipping Weapons

```ltsl
# Fill all turret sockets with the same weapon type
while (ship.Plug weaponType) 0

# Or equip different weapons (one per socket)
ship.Plug (Item_WeaponType 46)
ship.Plug (Item_WeaponType 74)
ship.Plug (Item_WeaponType 12)
ship.Plug (Item_WeaponType 99)
```

`ship.Plug(item)` iterates sockets, finds the first free matching slot, and
instantiates the item there. Returns false when no free sockets remain.

---

## 4. AI / Pilot System

### Players

```ltsl
var player Player_Human        # Human-controlled player
var ai (Player_AI traits)      # AI player with personality traits
```

`Player_AI` takes a `Traits` struct with 7 float dimensions (0.0–1.0):
- `Aggressive` — combat tendency
- `Creative` — problem-solving
- `Explorative` — exploration drive
- `Greedy` — economic focus
- `Intellectual` — research focus
- `Lawless` — criminal behavior
- `Sociable` — cooperation tendency

### Piloting

```ltsl
player.Pilot ship       # Player takes control of the ship
player.Unpilot           # Release control
player.GetPiloting       # Get the ship being piloted
```

### Task System

Tasks are a **LIFO stack** — only the topmost task runs. When it finishes,
the next one resumes.

```ltsl
ship.PushTask (Task_Destroy target)         # Attack a target
ship.PushTask (Task_Goto destination 512)   # Navigate somewhere
ship.PushTask (Task_Patrol zone)            # Patrol an area
ship.PushTask (Task_Mine zone)              # Mine asteroids
ship.PushTask (Task_Dock station)           # Dock at station
ship.PushTask (Task_Wait 5.0)               # Wait 5 seconds
ship.PushTask (Task_Buy station item qty)   # Buy items
ship.PushTask (Task_Sell station item qty)  # Sell items
ship.PushTask (Task_Play player)            # Default AI behavior
ship.PushTask (Task_Pirate zone)            # Pirate behavior

ship.GetCurrentTask         # Get the active task
ship.ClearTasks              # Remove all tasks
```

### Task Details

#### `Task_Destroy target`
- If target is far away, pushes `Task_Goto(target, destroyDistance)` first
- Orbits target at `radius = 1000 + target.radius + self.radius`
- Pushes `SDF_Sphere` toward orbit point for steering
- Iterates all turret sockets, checks weapon range
- Computes intercept point via `ComputeImpact`
- Broadcasts `MessageTargetPosition`, `MessageTargetObject`, `MessageFire`
- Finished when target is dead

#### `Task_Goto target distance`
- Handles same-container travel via Dijkstra pathfinding through `ComponentNavigable` nodes
- For direct flight, pushes `SDF_Sphere(nextNode, 1.0)` into MotionControl
- Broadcasts `MessageBoost()` during flight
- Handles docking/undocking for sub-container navigation

#### `Task_Patrol zone`
- Picks random positions within zone bounds
- Steers via `SDF_Sphere(target, 1.0)`

#### `Task_Mine zone`
- Navigates to mining zone
- Engages mining behavior

#### `Task_Play player`
- Default "do something useful" behavior
- Spawns new ships when affordable
- Manages economy and fleet

---

## 5. Ship Movement / Physics

### Motion Component

State per ship:
- `force` — accumulated linear force this frame
- `torque` — accumulated angular force this frame
- `velocity` — linear velocity
- `velocityA` — angular velocity
- `mass` — from ship supertype + cargo
- `inertia` — `mass^3.75 / 3`
- `speed` — `Length(velocity)`

Each frame:
```
force -= velocity * (mass * 0.8)          # linear drag
torque -= velocityA * (inertia * 2.0)     # angular drag
velocity += force * (dt / mass)            # F = ma
velocityA += torque * (dt / inertia)       # alpha = torque / I
position += velocity * dt                  # integrate position
rotation += velocityA * dt                 # integrate rotation
force = 0; torque = 0                      # reset
```

**Top speed**: `GetMaxThrust() / (kLinearDrag * mass)` — equilibrium where drag = thrust.

### Thruster Force Application

Each thruster receives `MessageThrustLinear` and `MessageThrustAngular`:

**Linear**: `thrust = Saturate(2.0 * Saturate(-Dot(look, dir)) - 0.5)`
- Thrust activation = how well the desired direction aligns with thruster's look direction
- Force pushes in the **negative look direction** (thrusters face backward)

**Angular**: `torque = Normalize(dir) * maxTorque * Saturate(amount)`

### SDF-Based Steering (AI)

Tasks don't directly move ships. They push `SDF` (Signed Distance Field) elements
into `MotionControl.elements`. Each frame:
1. Compute SDF gradient at ship's predicted position
2. Gradient points toward nearest attractor
3. `thrustDir = Normalize(gradient)`, attenuated by heading alignment
4. Broadcast `MessageThrustLinear`, `MessageThrustAngular`

---

## 6. Combat

### Messages

| Message                    | Purpose                          |
|----------------------------|----------------------------------|
| `MessageFire`              | Fire all weapons                 |
| `MessageTargetPosition`    | Where weapons should aim         |
| `MessageTargetObject`      | What weapons should track        |
| `MessageReload`            | Reload weapons                   |

### Weapon Firing

Each weapon:
- Tracks `targetPos` and `targetObject` via messages
- Auto-aims (tracks target heading at 2π rad/sec)
- Checks line-of-fire (raycast from origin, skip if hits parent)
- Magazine reload with timers
- Cooldown system

### Projectile Types

| Type    | Visual            | Behavior                          |
|---------|-------------------|-----------------------------------|
| Pulse   | Fast projectile   | Hitscan-like, raycast hit detect  |
| Missile | Homing projectile | Thrust + guidance toward target   |
| Beam    | (disabled)        | Continuous damage                 |
| Rail    | Rail shot         | Hitscan-like, high damage         |

### Damage

- `Damager.type` and `Damager.source` set on projectiles
- `Event_Damage(source, dest, damage)` triggers on hit
- When `health <= 0`: `OnDeath()` → `Explodable` creates explosion effect

---

## 7. Rendering

### Procedural Mesh
- Generated by `Item/ShipType/Generate:Main` (LTSL script)
- Uses `PlateMesh` with quality level 12
- Material: `Material_Metal`
- Mesh is stored as `Renderable` on the ShipType item
- Copied to Ship's `Drawable` component on instantiation

### Runtime Drawing
```
ComponentDrawable.Draw:
  Set transform from Orientation
  renderable->Render(state)    # draws the ship's model
```

### Thruster Effects
- Each Thruster renders its mesh + billboard trail
- Trail uses `billboard_axis.jsl` + `thruster_trail.jsl` shaders
- Additive blending, size/color vary by activation level
- Creates a `Light` object for glow

### Weapon Effects
- Weapons create `Light` for muzzle flash
- Flash decays from 1→0 via exponential
- `light->color = 4.0 * flash * weapon_color`

---

## 8. Station System

### Creating Stations

```ltsl
var stationType (Item_StationType value seed capacity integrity compactness systems)
var station stationType.Instantiate
station.SetPos position
system.AddInterior station
```

### Station Components (22 total)

Same as ships plus:
- `Dockable` — hangars and ports for ships to dock
- `Interior` — interior view (first-person rendering)
- `Market` — buy/sell orders
- `MissionBoard` — mission generation
- `Storage` — item storage
- `Zoned` — zone management

### Station AI Manager

```ltsl
var manager (Player_AI traits)
manager.SetName station.GetName + " Manager"
manager.AddAsset station
manager.AddCredits 1000000
```

Stations are given an AI manager player, stocked with items, and have
market sell orders placed.

### Station Marketplace

```ltsl
# Stock the station with items
for i 0 i < 32 i.++
  manager.AddToStorageLocker (Item_CommodityType rng.Int)

# Place sell orders
for each item in storage
  station.AddMarketSellOrder item quantity price
```

---

## 9. Object Hierarchy

```
Universe
  └─ Region (recursive)
       └─ System (Object_System)
            ├─ Star
            ├─ Planet
            │    ├─ Colony
            │    └─ OrbitalStation (disabled)
            ├─ Zone (asteroid field)
            │    ├─ Asteroid
            │    └─ Station (outpost)
            ├─ Ship
            │    ├─ Turret (plugged into sockets)
            │    │    └─ Weapon (plugged into turret)
            │    ├─ Thruster (plugged into sockets)
            │    ├─ PowerGenerator (plugged into sockets)
            │    └─ Scanner (plugged into sockets)
            ├─ WarpNode / WarpRail
            └─ DustFlecks
```

Containment: `system.AddInterior ship` makes the ship a child of the system.

---

## 10. Complete Examples

### Minimal Ship (no weapons, no AI)

```ltsl
var shipType (Item_ShipType 100000 42)
var ship shipType.Instantiate
ship.SetPos (Vec3 1000 0 0)
system.AddInterior ship
```

### Armed Player Ship

```ltsl
var shipType (Item_ShipType 1000000 55 1 1 1 1 1 1)
var ship shipType.Instantiate
ship.SetPos kOrigin

var weaponType (Item_WeaponType 74)
for i 0 i < 4 i.++
  ship.Plug weaponType

system.AddInterior ship

var player Player_Human
player.AddAsset ship
player.Pilot ship
```

### AI Combat Fleet

```ltsl
var ships List

for i 0 i < 10 i.++
  var shipType (Item_ShipType 100000 rng.Int 1 1 1 1 1 1)
  var ship shipType.Instantiate
  ship.SetPos 5000.0 * rng.Direction
  system.AddInterior ship

  var weaponType (Item_WeaponType rng.Int)
  for j 0 j < 4 j.++
    ship.Plug weaponType

  # Each ship attacks a random other ship
  if ships.Size > 0
    ship.PushTask (Task_Destroy (Object (ships.Get (rng.Int ships.Size))))

  ships += ship
```

### AI with Patrol and Patrol Combat

```ltsl
# Create a patrol zone
var zone Object
# ... (zone creation code)

# Create a patrol ship
var patrolShip shipType.Instantiate
patrolShip.SetPos zone.GetCenter
system.AddInterior patrolShip

# Create a pirate
var pirate shipType2.Instantiate
pirate.SetPos zone.GetCenter + 1000 * rng.Direction
system.AddInterior pirate

# They fight each other
patrolShip.PushTask (Task_Destroy pirate)
pirate.PushTask (Task_Destroy patrolShip)
```

---

## 11. Key File Reference

### C++ Core

| File | Role |
|------|------|
| `src/liblt/Game/Object/Ship.cpp` | Ship object creation, component stack |
| `src/liblt/Game/Item/ShipType.cpp` | ShipType: budget, mesh gen, sockets |
| `src/liblt/Game/Item/WeaponType.cpp` | Weapon: class, damage, firing |
| `src/liblt/Game/Item/ThrusterType.cpp` | Thruster: thrust output, power drain |
| `src/liblt/Game/Object/Thruster.cpp` | Thruster: force, trail, messages |
| `src/liblt/Game/Object/Weapon.cpp` | Weapon: aiming, firing, projectiles |
| `src/liblt/Game/Object/Station.cpp` | Station: docks, market, interior |
| `src/liblt/Game/Player.cpp` | Player_Human, Player_AI, Pilot/Unpilot |
| `src/liblt/Game/Widget/HUD.cpp` | Human input: ShipAffector, camera |
| `src/liblt/Game/Messages.h` | All message types |
| `src/liblt/Game/Task/Destroy.cpp` | Attack task |
| `src/liblt/Game/Task/Goto.cpp` | Navigation task |
| `src/liblt/Game/Task/Patrol.cpp` | Patrol task |
| `src/liblt/Game/Task/Play.cpp` | Default AI behavior |
| `src/liblt/Component/Motion.cpp` | Physics integration |
| `src/liblt/Component/MotionControl.cpp` | SDF steering |
| `src/liblt/Component/Tasks.cpp` | Task stack execution |

### LTSL Scripts

| File | Role |
|------|------|
| `resource/script/Item/ShipType/Generate.lts` | Procedural mesh generation |
| `resource/script/Object/Ship.lts` | Ship init, ambient sounds |
| `resource/script/App/war.lts` | 32-ship free-for-all |
| `resource/script/App/dogfight.lts` | 10 enemies vs player |
| `resource/script/App/ltheory-main.lts` | Universe sandbox with ships |

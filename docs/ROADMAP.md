# Roadmap

This roadmap is intentionally implementation-focused and may change as prototypes are tested.

## Milestone 1 — Locomotion foundation

- [x] UE 5.8 project
- [x] ALS-based C++ player class
- [x] Enhanced Input
- [x] third-person camera
- [x] basic gait / stance / aim hooks
- [x] tap crouch / hold prone stance transitions
- [x] prone collision and overhead-clearance checks
- [x] stance-preserving aim behavior
- [x] stance-aware shoulder camera and FOV transitions
- [x] prone movement/noise gameplay behavior
- [x] prone left/right roll gameplay state
- [x] chest-down / supine orientation state
- [x] supine/on-back aiming camera and firearm path
- [ ] dedicated prone/supine animation set and animation layer
- [ ] final player Blueprint/content wiring
- [ ] locomotion regression test map

## Milestone 2 — Weapon / inventory core

- [x] combat coordinator/state component
- [x] weapon base actor
- [x] firearm magazine model
- [x] reserve-ammo inventory pools
- [x] fire-mode data and runtime selection
- [x] four physical weapon carry slots + knife slot
- [x] grenade / utility explosive carry limits
- [x] data-driven weapon configuration
- [x] attachment Data Assets and compatibility
- [x] suppressor-aware acoustic modifiers
- [x] attachment visual socket spawning
- [ ] pickup / swap / drop interaction presentation
- [ ] final weapon Data Assets for the catalog
- [ ] final player/weapon skeletal sockets and meshes

## Milestone 3 — Third-person shooting

- [ ] hip fire
- [ ] ADS camera/weapon alignment
- [ ] recoil application
- [ ] spread application
- [ ] hitscan/projectile implementation
- [ ] impact feedback
- [ ] muzzle flash presentation
- [ ] upper-body animation layer
- [ ] reload animation/montage integration

## Milestone 4 — Melee / CQC

- [ ] melee state machine
- [ ] light/heavy attacks
- [ ] combo windows
- [ ] hit traces
- [ ] stagger / interruption
- [ ] contextual takedowns
- [ ] transition between firearm and CQC states

## Milestone 5 — Damage & reactions

- [ ] health component
- [ ] directional hit reactions
- [ ] death
- [ ] knockdown / recovery
- [ ] damage tags/types

## Milestone 6 — Stealth & AI

- [x] BDFR AI plugin integration
- [x] player footprint emission
- [x] player scent emission
- [x] physical surface mapping for tracks
- [x] difficulty-gated human footprint tracking
- [x] canine scent tracking controller
- [x] base sight/hearing/damage perception via BDFR
- [x] movement footstep acoustic events
- [x] carried-gear/load acoustic multiplier
- [x] weapon equip/reload/attachment acoustic events
- [x] suppressed gunshot integration
- [x] landing acoustic event
- [ ] precise animation-notify footstep timing
- [ ] vault/mantle acoustic hook
- [ ] visible footprint decals / presentation
- [ ] Behavior Tree tasks for trail navigation
- [ ] suspicion/alert game-state integration
- [ ] cover behavior
- [ ] search behavior
- [ ] squad coordination
- [ ] stealth takedown integration

## Milestone 7 — Tactical gameplay

- [ ] interaction framework
- [ ] mission objectives
- [x] inventory/equipment foundation
- [ ] smoke air-support / rescue mission hooks
- [ ] C4 / Claymore placement and detonation behavior
- [ ] checkpoints/save data
- [ ] UI/HUD
- [ ] polish and optimization

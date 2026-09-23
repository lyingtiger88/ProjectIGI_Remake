# Build & Recovery Guide

## Normal build

1. Install Unreal Engine 5.8 and Visual Studio 2022.
2. Make sure the ALS and BDFR AI submodules exist:
   ```bash
   git submodule update --init --recursive
   ```
3. Right-click `ProjectIGI_Remake.uproject`.
4. Choose **Generate Visual Studio project files**.
5. Open the generated solution.
6. Select:
   - Configuration: `Development Editor`
   - Platform: `Win64`
   - Target: `ProjectIGI_RemakeEditor`
7. Build the solution.
8. Open the project in Unreal Engine.

## Clean rebuild

Run:

```text
Tools\Clean_ProjectIGI_Remake.bat
```

The script removes generated project and plugin build products. It also offers to remove `Saved/`.

After cleaning:

1. Generate Visual Studio project files again.
2. Rebuild `ProjectIGI_RemakeEditor`.
3. Launch Unreal Editor.

## Common ALS-related checks

If the editor crashes when the player is possessed:

- verify the player's Skeletal Mesh is valid,
- verify the ALS Animation Blueprint is loaded,
- verify the ALS Animation Instance exists before ALS refresh logic runs,
- verify ALS character and movement settings assets are valid,
- verify the ALS plugin was built for the exact Unreal Engine version in use.

The project player class is intended to refresh its ALS animation instance before and after possession so the ALS base class never operates on an uninitialized animation instance.

## Rebuild order after plugin changes

When ALS or BDFR C++ source changes:

1. close Unreal Editor,
2. run the cleanup batch,
3. regenerate project files,
4. rebuild the editor target,
5. launch the project.

Avoid relying on Hot Reload for major engine/plugin class-layout changes.


## BDFR tracking integration checks

If footprint/scent code does not compile or run:

- verify `Plugins/BDFR_InteractiveAI` is populated by Git submodules,
- verify `BDFR_InteractiveAI` is enabled in `ProjectIGI_Remake.uproject`,
- verify the game module depends on `BDFR_InteractiveAI`,
- verify Physical Surfaces 1-8 are present in Project Settings -> Physics,
- verify materials that need tracking use a Physical Material with the expected Surface Type,
- regenerate Visual Studio project files after adding/updating the plugin.

After pulling tracking changes, prefer a clean rebuild instead of Hot Reload because new reflected
UCLASS/UActorComponent types and plugin module dependencies are involved.

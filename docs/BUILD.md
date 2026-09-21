# Build & Recovery Guide

## Normal build

1. Install Unreal Engine 5.8 and Visual Studio 2022.
2. Make sure the ALS submodule exists:
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

When ALS C++ source changes:

1. close Unreal Editor,
2. run the cleanup batch,
3. regenerate project files,
4. rebuild the editor target,
5. launch the project.

Avoid relying on Hot Reload for major engine/plugin class-layout changes.

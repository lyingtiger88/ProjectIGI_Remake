# Build & Recovery Guide

## Normal build

1. Install Unreal Engine 5.8 and Visual Studio 2022.
2. Make sure the ALS and BDFR AI submodules are synchronized:
   ```bash
   git submodule sync --recursive
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

1. synchronize/update submodules,
2. generate Visual Studio project files again,
3. rebuild `ProjectIGI_RemakeEditor`,
4. launch Unreal Editor.

## Common ALS-related checks

If the editor crashes when the player is possessed:

- verify the player's Skeletal Mesh is valid,
- verify the ALS Animation Blueprint is loaded,
- verify the ALS Animation Instance exists before ALS refresh logic runs,
- verify ALS character and movement settings assets are valid,
- verify the ALS plugin was built for the exact Unreal Engine version in use.

The project player class refreshes its ALS animation instance before and after possession so the ALS base class does not operate on an uninitialized animation instance.

## Rebuild order after plugin or reflected-type changes

When ALS/BDFR C++ source changes, or after pulling new `UCLASS` / `UActorComponent` / `UDataAsset` types:

1. close Unreal Editor,
2. run the cleanup batch,
3. run:
   ```bash
   git submodule sync --recursive
   git submodule update --init --recursive
   ```
4. regenerate project files,
5. rebuild the editor target,
6. launch the project.

Avoid Hot Reload for this class of change.

## BDFR integration checks

If tracking/acoustic code does not compile or run:

- verify `Plugins/BDFR_InteractiveAI` is populated and at the revision pinned by the project,
- verify `BDFR_InteractiveAI` is enabled in `ProjectIGI_Remake.uproject`,
- verify the game module depends on `BDFR_InteractiveAI`,
- verify the pinned BDFR revision exposes `UBDFRAcousticEventLibrary::ReportAcousticEvent`,
- verify Physical Surfaces 1-8 are present in Project Settings -> Physics,
- verify tracked materials use the expected Physical Material / Surface Type.

## Weapon/inventory content checks

The C++ framework does not create binary Unreal assets in Git.

In the editor, create `UIGIWeaponDataAsset` and `UIGIWeaponAttachmentDataAsset` assets, then assign meshes, compatible carry slots, ammo values, acoustic values, and supported attachments.

The player/weapon skeletal meshes also need the documented carry/attachment socket names. Missing sockets will not provide the intended physical placement even though the C++ types compile.

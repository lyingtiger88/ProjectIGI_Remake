using UnrealBuildTool;

public class ProjectIGI_Remake : ModuleRules
{
	public ProjectIGI_Remake(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;

		// This module intentionally keeps gameplay headers in feature folders directly
		// under Source/ProjectIGI_Remake (AI, Combat, Inventory, Stealth, Tracking, Weapons).
		// UE 5.8 does not implicitly add the module root for these quoted feature-path
		// includes, so expose it explicitly for includes such as "Weapons/IGIWeaponBase.h".
		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags",
			"PhysicsCore",
			"AIModule",
			"ALS",
			"BDFR_InteractiveAI"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}

using UnrealBuildTool;

public class ProjectIGI_Remake : ModuleRules
{
	public ProjectIGI_Remake(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags",
			"ALS",
			"BDFR_InteractiveAI"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}

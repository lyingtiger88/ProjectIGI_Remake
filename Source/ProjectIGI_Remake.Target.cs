using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectIGI_RemakeTarget : TargetRules
{
	public ProjectIGI_RemakeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		ExtraModuleNames.AddRange(new string[] { "ProjectIGI_Remake" });
	}
}

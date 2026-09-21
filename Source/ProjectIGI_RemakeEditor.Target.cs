using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectIGI_RemakeEditorTarget : TargetRules
{
	public ProjectIGI_RemakeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		ExtraModuleNames.AddRange(new string[] { "ProjectIGI_Remake" });
	}
}

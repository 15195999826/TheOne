// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheOne : ModuleRules
{
	public TheOne(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		// 暴露给其他模块使用的公共依赖头文件路径（如果是在Public目录下则不需要添加），这样比如public/xxx目录，添加到这里的之后，代码总包含目录可以去掉相对路径xxx
		PublicIncludePaths.AddRange(
			new string[] {
				"TheOne"
			}
		);

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "UMG",
			"NavigationSystem", "FastNoise", "FastNoiseGenerator", "GeometryScriptingCore", "AIModule", "IKRig",
			"Slate", "SlateCore", "LomoCaptureActor"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
			{ "GameplayTags", "GameplayTasks", "Niagara", "DeveloperSettings", "MoviePlayer", "PreLoadScreen", "UnLua", "Lua" });

		if (Target.bBuildEditor) {
			PrivateDependencyModuleNames.AddRange(new string[]{"SubobjectEditor"});
		}
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

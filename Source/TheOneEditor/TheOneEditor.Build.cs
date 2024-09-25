using UnrealBuildTool;

public class TheOneEditor : ModuleRules
{
    public TheOneEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        // 暴露给其他模块使用的公共依赖头文件路径（如果是在Public目录下则不需要添加），这样比如public/xxx目录，添加到这里的之后，代码总包含目录可以去掉相对路径xxx
        PublicIncludePaths.AddRange(
            new string[] {
                "TheOneEditor"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "EditorFramework",
                "UnrealEd",
                "PhysicsCore",
                "GameplayTagsEditor",
                "GameplayTasksEditor",
                "GameplayAbilities",
                "GameplayAbilitiesEditor",
                "PropertyEditor",
                "TheOne",
                "Slate",
                "SlateCore"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "InputCore",
             
                "ToolMenus",
                "EditorStyle",
                "DataValidation",
                "MessageLog",
                "Projects",
                "DeveloperToolSettings",
                "CollectionManager",
                "SourceControl",
                "Chaos",
                "GameplayTags"
            }
        );
        
        // Generate compile errors if using DrawDebug functions in test/shipping builds.
        PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
    }
}
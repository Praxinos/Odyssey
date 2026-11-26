// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

using System;

namespace UnrealBuildTool.Rules
{
    /** Converts the legacy raster assets of a project to the tiled image storage, in one pass. Transitional. */
    public class OdysseyTiledImageMigration : ModuleRules
    {
        public OdysseyTiledImageMigration(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            if (Target.Type == TargetType.Game)
            {
                throw new InvalidOperationException("ERROR in OdysseyTiledImageMigration Module : Target.Type == TargetType.Game");
            }

            // Not unity-built in debug configurations, to catch missing includes.
            if (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.DebugGame)
            {
                bMergeUnityFiles = false;
                PCHUsage = ModuleRules.PCHUsageMode.NoPCHs;
                bUseUnity = false;
                OptimizeCode = CodeOptimization.Never;
            }

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "AssetRegistry",
                    "Core",
                    "CoreUObject",
                    "DeveloperSettings",
                    "Engine",
                    "Projects",
                    "Slate",
                    "SlateCore",
                    "SourceControl",
                    "ToolMenus",
                    "UnrealEd",

                    "OdysseyAnimationEditor",
                    "OdysseyImaging",
                    "OdysseyTexture",
                }
                );
        }
    }
}

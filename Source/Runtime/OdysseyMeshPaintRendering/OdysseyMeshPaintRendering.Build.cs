// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using UnrealBuildTool;

public class OdysseyMeshPaintRendering : ModuleRules
{
    public OdysseyMeshPaintRendering(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "RenderCore",
                "Engine",
                "Projects",
                "RHI"
            }
        );
    }
}

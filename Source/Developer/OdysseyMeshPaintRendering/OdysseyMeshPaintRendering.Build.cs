// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

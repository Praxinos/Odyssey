// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using UnrealBuildTool;

public class OdysseyImaging : ModuleRules
{
    public OdysseyImaging(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "ImageCore",
                "RenderCore",
                "RHI",
                "TargetPlatform",
                "TextureCompressor"
             }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
                "ULISLoader",
             }
        );

    }
}

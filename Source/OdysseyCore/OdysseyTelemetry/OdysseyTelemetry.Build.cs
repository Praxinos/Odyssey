// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

namespace UnrealBuildTool.Rules
{
    public class OdysseyTelemetry : ModuleRules
    {
        public OdysseyTelemetry(ReadOnlyTargetRules Target) : base(Target)
        {
            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    //[RUNTIME]
                    "Analytics",
                    "AnalyticsET",
                    "HTTP",
                    "Json",
                    "Projects",
                    "Engine",
                    "BuildSettings",
                    "CoreUObject",
                    "AssetRegistry",
                    // ... add private dependencies that you statically link with here ...

                    "OdysseyAnalytics",
                }
                );

            PublicIncludePathModuleNames.Add("Analytics");
            CppCompileWarningSettings.UnsafeTypeCastWarningLevel = WarningLevel.Error;
        }
    }
}

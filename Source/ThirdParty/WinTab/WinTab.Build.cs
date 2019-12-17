// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class WinTab : ModuleRules
{
	public WinTab(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

        //if (Target.Platform == UnrealTargetPlatform.Win32 || Target.Platform == UnrealTargetPlatform.Win64)
        //{
        //    PublicIncludePaths.AddRange(
        //        new string[] {
        //            Path.Combine( ModuleDirectory, "Windows" )
        //        }
        //    );
        //}

        // It's more 'safe/readable' to have 
        // #include "Windows/wintab.h" 
        // inside calling modules than only 
        // #include "wintab.h"
        // in case there are files with same name for windows/macosx/linux/...
        PublicIncludePaths.AddRange(
            new string[] {
                ModuleDirectory
            }
        );
    }
}

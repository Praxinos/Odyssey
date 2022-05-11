// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "StoryboardCreationDialog/StoryboardImportImageSequenceSettings.h"
#include "StoryboardCreationDialog/StoryboardSettings.h"

class FTabManager;

//---

class NewStoryboardDialog
{
public:
    /** Open dialog for creating an empty storyboard */
    static void OpenCreationDialog( const TSharedRef<FTabManager>& TabManager );

    /** Open dialog for creating a storyboard from a sequence of images */
    static void OpenImportImageSequenceDialog( const TSharedRef<FTabManager>& TabManager );
};

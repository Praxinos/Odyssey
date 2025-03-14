// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

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

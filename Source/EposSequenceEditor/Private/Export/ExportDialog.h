// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FTabManager;
class UMovieSceneSequence;

//---

class ExportStoryboardDialog
{
public:
    /** Open dialog for exporting a storyboard to a sequence of images */
    static void OpenExportImageSequenceDialog( const TSharedRef<FTabManager>& TabManager, UMovieSceneSequence* iSequence );

    /** Open dialog for exporting a storyboard to a PDF */
    static void OpenExportPDFDialog( const TSharedRef<FTabManager>& TabManager, UMovieSceneSequence* iSequence );
};

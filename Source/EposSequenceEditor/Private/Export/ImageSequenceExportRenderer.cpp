// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ImageSequenceExportRenderer.h"

#include "IMovieRenderPipelineEditorModule.h"
#include "LevelEditor.h"
#include "LevelSequence.h"

#include "EposMovieSceneSequence.h"
#include "Export/SImageSequenceExportDialog.h"

#define LOCTEXT_NAMESPACE "ImageSequenceExportRenderer"

//---

FName FImageSequenceExportRenderer::ImageSequenceExportTabName = "ImageSequenceExport";
FText FImageSequenceExportRenderer::ImageSequenceExportTabLabel = LOCTEXT( "StoryboardImageSequenceExportRenderTab_Label", "Storyboard Export Image Sequence" );

void
FImageSequenceExportRenderer::RenderMovie( UMovieSceneSequence* iSequence, const TArray<UMovieSceneCinematicShotSection*>& iSections ) //override
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );

    ExportStoryboardDialog::OpenExportImageSequenceDialog( LevelEditorModule.GetLevelEditorTabManager().ToSharedRef(), iSequence );
};

FString
FImageSequenceExportRenderer::GetDisplayName() const //override
{
    return ImageSequenceExportTabLabel.ToString();
}

//---

#undef LOCTEXT_NAMESPACE

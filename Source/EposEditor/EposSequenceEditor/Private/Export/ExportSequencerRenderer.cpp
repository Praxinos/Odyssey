// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/ExportSequencerRenderer.h"

#include "IMovieRenderPipelineEditorModule.h"
#include "LevelEditor.h"
#include "LevelSequence.h"

#include "EposMovieSceneSequence.h"
#include "Export/ExportDialog.h"

#define LOCTEXT_NAMESPACE "ExportSequencerRenderer"

//---

FName FExportSequencerRenderer::ExportTabName = "StoryboardExport";
FText FExportSequencerRenderer::ExportTabLabel = LOCTEXT( "StoryboardExportRenderTab_Label", "Storyboard Export (PDF, PNG,JPG,...)" );

void
FExportSequencerRenderer::RenderMovie( UMovieSceneSequence* iSequence, const TArray<UMovieSceneCinematicShotSection*>& iSections ) //override
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );

    ExportStoryboardDialog::OpenExportImageSequenceDialog( LevelEditorModule.GetLevelEditorTabManager().ToSharedRef(), iSequence );
};

FString
FExportSequencerRenderer::GetDisplayName() const //override
{
    return ExportTabLabel.ToString();
}

//---

#undef LOCTEXT_NAMESPACE

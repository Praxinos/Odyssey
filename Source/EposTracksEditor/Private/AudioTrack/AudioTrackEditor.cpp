// Copyright Epic Games, Inc. All Rights Reserved.

#include "AudioTrack/AudioTrackEditor.h"
#include "Textures/SlateTextureData.h"
#include "Rendering/RenderingCommon.h"
#include "Rendering/DrawElements.h"
#include "Widgets/SBoxPanel.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "RenderUtils.h"
#include "Modules/ModuleManager.h"
#include "Audio.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundWave.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "SequencerSectionPainter.h"
#include "EditorStyleSet.h"
#include "Editor/UnrealEdEngine.h"
#include "Sound/SoundCue.h"
#include "UnrealEdGlobals.h"
#include "Tracks/MovieSceneAudioTrack.h"
#include "Sections/MovieSceneAudioSection.h"
#include "CommonMovieSceneTools.h"
#include "AudioDevice.h"
#include "Sound/SoundNodeWavePlayer.h"
#include "Slate/SlateTextures.h"
#include "AudioDecompress.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "SequencerUtilities.h"
#include "AssetRegistryModule.h"
#include "MatineeImportTools.h"
#include "Matinee/InterpTrackSound.h"
#include "ISectionLayoutBuilder.h"

#include "DragAndDrop/AssetDragDropOp.h"
#include "Misc/QualifiedFrameTime.h"


//#define LOCTEXT_NAMESPACE "EposAudioTrackEditor"



FAudioTrackEditor::FAudioTrackEditor( TSharedRef<ISequencer> iSequencer )
	: FAudioTrackEditorOriginal( iSequencer )
{
}

FAudioTrackEditor::~FAudioTrackEditor()
{
}


TSharedRef<ISequencerTrackEditor> FAudioTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> iSequencer )
{
	return MakeShareable( new FAudioTrackEditor( iSequencer ) );
}


bool FAudioTrackEditor::SupportsSequence( UMovieSceneSequence* iSequence ) const
{
    return ( iSequence != nullptr ) && ( ( iSequence->GetClass()->GetName() == TEXT( "BoardSequence" ) ) || ( iSequence->GetClass()->GetName() == TEXT( "ShotSequence" ) ) );
}


//#undef LOCTEXT_NAMESPACE

// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/ResourceAssetTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "EditorAssetLibrary.h"
#include "Engine/Texture2D.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Factories/Texture2dFactoryNew.h"
#include "IMovieScenePlayer.h"
#include "ISequencer.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneSequence.h"
#include "ObjectTools.h"

#include "EposMovieSceneSequence.h"
#include "NamingConvention.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationFactory.h"
#include "StoryNote.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "ResourceAssetTools"

//---

//static
UOdysseyAnimation*
ProjectAssetTools::CreateAnimation( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FIntPoint iSize, EOdysseyAnimationFormat iFormat, FFrameRate iFrameRate, TSubclassOf<class UOdysseyAnimationLayer> iDefaultLayerClass, TOptional<FLinearColor> iLayerBackgroundColor )
{
    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence );
    if( !epos_sequence )
    {
        checkf( false, TEXT( "iSequence is certainly a LevelSequence, manage it" ) );

        return nullptr;
    }

    FString animation_path;
    FString animation_name;
    FString animation_pathname = NamingConvention::GenerateAnimationAssetPathName( iPlayer, *epos_sequence, iSequenceID, animation_path, animation_name );

    //---

    UOdysseyAnimationFactory* factory = NewObject<UOdysseyAnimationFactory>();
    factory->Width = iSize.X;
    factory->Height = iSize.Y;
    factory->Format = iFormat;
    factory->FrameRate = iFrameRate;
    factory->DefaultLayerClass = iDefaultLayerClass;
    factory->LayerBackgroundColor = iLayerBackgroundColor;

    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    UObject* new_object = assetToolsModule.Get().CreateAsset( animation_name, animation_path, UOdysseyAnimation::StaticClass(), factory );

    UOdysseyAnimation* new_animation = Cast<UOdysseyAnimation>( new_object );

    //---

    //new_animation-> = ...;

    return new_animation;
}

//---

//static
UStoryNote*
ProjectAssetTools::CreateNote( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    if( UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence ) )
    {
        FString note_path;
        FString note_name;
        FString note_pathname = NamingConvention::GenerateNoteAssetPathName( iSequencer, *epos_sequence, iSequenceID, note_path, note_name );

        //---

        FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
        UObject* new_object = assetToolsModule.Get().CreateAsset( note_name, note_path, UStoryNote::StaticClass(), nullptr );
        UStoryNote* new_note = Cast<UStoryNote>( new_object );
        check( new_note );

        new_note->Text = TEXT( "Write a note here" ); // default text

        return new_note;
    }

    checkf( false, TEXT( "iSequence is certainly a LevelSequence, manage it" ) );

    return nullptr;
}

//static
UStoryNote*
ProjectAssetTools::CloneNote( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UStoryNote* iNoteToClone )
{
    if( UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence ) )
    {
        FString note_path;
        FString note_name;
        FString note_pathname = NamingConvention::GenerateNoteAssetPathName( iSequencer, *epos_sequence, iSequenceID, note_path, note_name );

        //---

        UObject* new_object = UEditorAssetLibrary::DuplicateLoadedAsset( iNoteToClone, note_pathname );

        UStoryNote* new_note = Cast<UStoryNote>( new_object );

        return new_note;
    }

    checkf( false, TEXT( "iSequence is certainly a LevelSequence, manage it" ) );

    return nullptr;
}


#undef LOCTEXT_NAMESPACE

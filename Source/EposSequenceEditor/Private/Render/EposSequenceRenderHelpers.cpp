// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Render/EposSequenceRenderHelpers.h"

//#include "AssetToolsModule.h"
//#include "EditorAssetLibrary.h"
#include "Factories/Factory.h"
#include "IMovieRenderPipelineEditorModule.h"
#include "LevelEditor.h"
#include "LevelSequence.h"
#include "MovieSceneTimeHelpers.h"
#include "Sections/MovieSceneCinematicShotSection.h"
#include "Tracks/MovieSceneCinematicShotTrack.h"

#include "EposMovieSceneSequence.h"
#include "Render/EposSequencePipelineRenderer.h"

#define LOCTEXT_NAMESPACE "EposSequenceRenderHelpers"

//---

static
UFactory*
GetLevelSequenceFactory()
{
    for( TObjectIterator<UClass> It; It; ++It )
    {
        UClass* Class = *It;
        if( Class->IsChildOf( UFactory::StaticClass() ) &&
            !Class->HasAnyClassFlags( CLASS_Abstract ) )
        {
            UFactory* Factory = Class->GetDefaultObject<UFactory>();

            if( Factory->GetSupportedClass() == ULevelSequence::StaticClass() )
            {
                return Factory;
            }
        }
    }

    return nullptr;
}

//static
ULevelSequence*
EposSequenceRenderHelpers::CreateLevelSequenceTransient( TArray<UEposMovieSceneSequence*> iSequences )
{
    if( iSequences.Num() <= 0 )
        return nullptr;

    //Sort in alphabetical order
    iSequences.Sort( []( UEposMovieSceneSequence& iA, UEposMovieSceneSequence& iB )
                     {
                         return iA.GetDisplayName().ToString() < iB.GetDisplayName().ToString();
                     } );

    //---

    FString sequence_pathname = iSequences[0]->GetPathName();
    FString sequence_path = FPaths::GetPath( sequence_pathname );
    FString sequence_name = FPaths::GetBaseFilename( sequence_pathname );

    //IAssetTools& AssetTools = FModuleManager::LoadModuleChecked< FAssetToolsModule >( "AssetTools" ).Get();

    //FString asset_name = TEXT( "LS_" ) + sequence_name;
    //UEditorAssetLibrary::DeleteAsset( sequence_path / asset_name );
    //UObject* object = AssetTools.CreateAsset( asset_name, sequence_path, nullptr, GetLevelSequenceFactory() );
    //if( !object )
    //    return nullptr;

    ULevelSequence* levelSequence = NewObject<ULevelSequence>( GetTransientPackage(), ULevelSequence::StaticClass(), FName( TEXT( "Transient_" ) + sequence_name ), RF_Transient );
    levelSequence->Initialize();

    iSequences[0]->mLevelSequenceRender = levelSequence;

    //---

    //ULevelSequence* levelSequence = Cast<ULevelSequence>( object );
    UMovieScene* levelMovieScene = levelSequence->GetMovieScene();

    //Compute values to use in the LevelSequence
    // Tick Resolution, DisplayRate
    FFrameRate tickResolution = iSequences[0]->GetMovieScene()->GetTickResolution();
    FFrameRate displayRate = iSequences[0]->GetMovieScene()->GetDisplayRate();
    for( int i = 1; i < iSequences.Num(); i++ )
    {
        UEposMovieSceneSequence* eposSequence = iSequences[i];
        UMovieScene* eposMovieScene = eposSequence->GetMovieScene();

        tickResolution = eposMovieScene->GetTickResolution().AsInterval() < tickResolution.AsInterval() ? eposMovieScene->GetTickResolution() : tickResolution;
        displayRate = eposMovieScene->GetDisplayRate().AsInterval() < displayRate.AsInterval() ? eposMovieScene->GetDisplayRate() : displayRate;
    }

    levelMovieScene->SetTickResolutionDirectly( tickResolution );
    levelMovieScene->SetDisplayRate( displayRate );

    // Add CinematicShotTrack
    UMovieSceneCinematicShotTrack* shotTrack = levelMovieScene->AddMasterTrack<UMovieSceneCinematicShotTrack>();

    // Add Shot Sections
    for( int i = 0; i < iSequences.Num(); i++ )
    {
        UEposMovieSceneSequence* eposSequence = iSequences[i];
        UMovieScene* eposMovieScene = eposSequence->GetMovieScene();

        // Add Section to Cinematic shot track
        UMovieSceneCinematicShotSection* shotSection = Cast<UMovieSceneCinematicShotSection>( shotTrack->CreateNewSection() );
        if( shotSection )
        {
            shotTrack->Modify();
            shotTrack->AddSection( *shotSection );
        }

        // Add boardSequence to section
        shotSection->SetSequence( eposSequence );

        // Compute new shot range
        TRange<FFrameNumber> eposPlaybackRange = eposMovieScene->GetPlaybackRange();
        TRange<FFrameNumber> sectionRange = TRange<FFrameNumber>( FFrameRate::TransformTime( eposPlaybackRange.GetLowerBoundValue(), eposMovieScene->GetTickResolution(), tickResolution ).GetFrame(),
                                                                  FFrameRate::TransformTime( eposPlaybackRange.GetUpperBoundValue(), eposMovieScene->GetTickResolution(), tickResolution ).GetFrame() );
        if( i == 0 )
        {
            levelMovieScene->SetPlaybackRange( sectionRange );
        }
        else
        {
            sectionRange = UE::MovieScene::TranslateRange( sectionRange, levelMovieScene->GetPlaybackRange().GetUpperBoundValue() - sectionRange.GetLowerBoundValue() );

            TRange<FFrameNumber> levelPlaybackRange( levelMovieScene->GetPlaybackRange() );
            levelPlaybackRange.SetUpperBoundValue( sectionRange.GetUpperBoundValue() );
            levelMovieScene->SetPlaybackRange( levelPlaybackRange );
        }

        shotSection->SetRange( sectionRange );
    }

    return levelSequence;
}

//---

////static
//void
//EposSequenceRenderHelpers::RenderMovie( TArray<UEposMovieSceneSequence*> iSequences, UMoviePipelineMasterConfig* iMasterConfig )
//{
//    ULevelSequence* levelSequence = CreateLevelSequence( iSequences );
//    if( !levelSequence )
//        return;
//
//    //--- from FSequencer::RenderMovieInternal()#3678
//
//    FEposSequencePipelineRenderer renderer;
//    renderer.mMasterConfig = iMasterConfig;
//    renderer.RenderMovie( levelSequence, TArray<UMovieSceneCinematicShotSection*>() );
//}

#undef LOCTEXT_NAMESPACE

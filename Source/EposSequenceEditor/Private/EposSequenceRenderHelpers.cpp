// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposSequenceRenderHelpers.h"

#include "AssetToolsModule.h"
#include "AutomatedLevelSequenceCapture.h"
#include "Factories/Factory.h"
#include "FileHelpers.h"
#include "FrameNumberNumericInterface.h"
#include "ISequencerModule.h"
#include "LevelEditor.h"
#include "LevelSequence.h"
#include "LevelEditorSequencerIntegration.h"
#include "MovieSceneCaptureDialogModule.h"
#include "MovieSceneTimeHelpers.h"
#include "Sections/MovieSceneCinematicShotSection.h"
#include "SequencerSettings.h"
#include "Tracks/MovieSceneCinematicShotTrack.h"

//#include "Board/BoardSequence.h"
#include "EposMovieSceneSequence.h"

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

static
void
RenderSequencesByCreatingLevelSequenceAsset( TArray<UEposMovieSceneSequence*> iSequences )
{
    if( iSequences.Num() <= 0 )
        return;

    // Prompt the user to save their changes so that they'll be in the movie, since we're not saving temporary copies of the level.
    bool bPromptUserToSave = true;
    bool bSaveMapPackages = true;
    bool bSaveContentPackages = true;
    if( !FEditorFileUtils::SaveDirtyPackages( bPromptUserToSave, bSaveMapPackages, bSaveContentPackages ) )
        return;

    //---

    //Sort in alphabetical order
    iSequences.Sort( []( UEposMovieSceneSequence& iA, UEposMovieSceneSequence& iB )
                     {
                         return iA.GetDisplayName().ToString() < iB.GetDisplayName().ToString();
                     } );

    //---

    FString sequence_pathname = iSequences[0]->GetPathName();
    FString sequence_path = FPaths::GetPath( sequence_pathname );
    FString sequence_name = FPaths::GetBaseFilename( sequence_pathname );

    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked< FAssetToolsModule >( "AssetTools" ).Get();

    FString PackageName;
    FString AssetName;
    AssetTools.CreateUniqueAssetName( sequence_path / TEXT("LS_") + sequence_name, TEXT( "" ), PackageName, AssetName );

    UObject* object = AssetTools.CreateAssetWithDialog( AssetName, sequence_path, ULevelSequence::StaticClass(), GetLevelSequenceFactory() );
    if( !object )
        return;

    //---

    ULevelSequence* levelSequence = Cast<ULevelSequence>( object );
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

    //---

    TArray<TWeakPtr<ISequencer>> sequencers = FLevelEditorSequencerIntegration::Get().GetSequencers();
    if( !sequencers.Num() )
        return;

    ISequencer* sequencer = sequencers[0].Pin().Get();
    if( !sequencer )
        return;

    USequencerSettings* sequencerSettings = sequencer->GetSequencerSettings();

    //--- from FSequencer::RenderMovieInternal()#3678

    //ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );
    //if( IMovieRendererInterface* MovieRenderer = SequencerModule.GetMovieRenderer( sequencer->GetMovieRendererName() ) )
    //{
    //    MovieRenderer->RenderMovie( sequencer->GetRootMovieSceneSequence(), TArray<UMovieSceneCinematicShotSection*>() );
    //    return;
    //}

    //TRange<FFrameNumber> Range = levelPlaybackRange;
    //if( Range.GetLowerBound().IsOpen() || Range.GetUpperBound().IsOpen() )
    //{
    //    Range = TRange<FFrameNumber>::Hull( Range, GetPlaybackRange() );
    //}
    TRange<FFrameNumber> Range = levelMovieScene->GetPlaybackRange();

    //// If focused on a subsequence, transform the playback range to the root in order to always render from the root
    //if( sequencer->GetRootMovieSceneSequence() != sequencer->GetFocusedMovieSceneSequence() )
    //{
    //    //bSetFrameOverrides = true;

    //    if( const FMovieSceneSubSequenceData* SubSequenceData = RootTemplateInstance.FindSubData( GetFocusedTemplateID() ) )
    //    {
    //        Range = Range * SubSequenceData->RootToSequenceTransform.InverseLinearOnly();
    //    }
    //}

    // Create a new movie scene capture object for an automated level sequence, and open the tab
    UAutomatedLevelSequenceCapture* MovieSceneCapture = NewObject<UAutomatedLevelSequenceCapture>( GetTransientPackage(), UAutomatedLevelSequenceCapture::StaticClass(), UMovieSceneCapture::MovieSceneCaptureUIName, RF_Transient );
    MovieSceneCapture->LoadFromConfig();

    // Always render from the root
    MovieSceneCapture->LevelSequenceAsset = levelMovieScene->GetOuter()->GetPathName();

    FFrameRate DisplayRate = levelMovieScene->GetDisplayRate();
    FFrameRate TickResolution = levelMovieScene->GetTickResolution();

    MovieSceneCapture->Settings.FrameRate = DisplayRate;
    MovieSceneCapture->Settings.ZeroPadFrameNumbers = sequencerSettings->GetZeroPadFrames();
    MovieSceneCapture->Settings.bUseRelativeFrameNumbers = false;

    FFrameNumber StartFrame = UE::MovieScene::DiscreteInclusiveLower( Range );
    FFrameNumber EndFrame = UE::MovieScene::DiscreteExclusiveUpper( Range );

    FFrameNumber RoundedStartFrame = FFrameRate::TransformTime( StartFrame, TickResolution, DisplayRate ).CeilToFrame();
    FFrameNumber RoundedEndFrame = FFrameRate::TransformTime( EndFrame, TickResolution, DisplayRate ).CeilToFrame();

    //if( bSetFrameOverrides )
    {
        MovieSceneCapture->SetFrameOverrides( RoundedStartFrame, RoundedEndFrame );
    }
    //else
    //{
    //    if( !MovieSceneCapture->bUseCustomStartFrame )
    //    {
    //        MovieSceneCapture->CustomStartFrame = RoundedStartFrame;
    //    }

    //    if( !MovieSceneCapture->bUseCustomEndFrame )
    //    {
    //        MovieSceneCapture->CustomEndFrame = RoundedEndFrame;
    //    }
    //}

    // We create a new Numeric Type Interface that ties it's Capture/Resolution rates to the Capture Object so that it converts UI entries
    // to the correct resolution for the capture, and not for the original sequence.
    USequencerSettings* LocalSettings = sequencerSettings;

    TAttribute<EFrameNumberDisplayFormats> GetDisplayFormatAttr = MakeAttributeLambda(
        [LocalSettings]
        {
            if( LocalSettings )
            {
                return LocalSettings->GetTimeDisplayFormat();
            }
            return EFrameNumberDisplayFormats::Frames;
        }
        );

    TAttribute<uint8> GetZeroPadFramesAttr = MakeAttributeLambda(
        [LocalSettings]()->uint8
        {
            if( LocalSettings )
            {
                return LocalSettings->GetZeroPadFrames();
            }
            return 0;
        }
    );

    // By using a TickResolution/DisplayRate that match the numbers entered via the numeric interface don't change frames of reference.
    // This is used here because the movie scene capture works entirely on play rate resolution and has no knowledge of the internal resolution
    // so we don't need to convert the user's input into internal resolution.
    TAttribute<FFrameRate> GetFrameRateAttr = MakeAttributeLambda(
        [MovieSceneCapture]
        {
            if( MovieSceneCapture )
            {
                return MovieSceneCapture->GetSettings().FrameRate;
            }
            return FFrameRate( 30, 1 );
        }
        );

    // Create our numeric type interface so we can pass it to the time slider below.
    TSharedPtr<INumericTypeInterface<double>> MovieSceneCaptureNumericInterface = MakeShareable( new FFrameNumberInterface( GetDisplayFormatAttr, GetZeroPadFramesAttr, GetFrameRateAttr, GetFrameRateAttr ) );

    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );

    IMovieSceneCaptureDialogModule::Get().OpenDialog( LevelEditorModule.GetLevelEditorTabManager().ToSharedRef(), MovieSceneCapture, MovieSceneCaptureNumericInterface );
}

//static
void
RenderSequencesByCreatingLevelSequenceTransient( TArray<UEposMovieSceneSequence*> iSequences )
{
    //The following code works but, we need to have the Render as a modal window to make it work properly

    return;

    for (int i = 0; i < iSequences.Num(); i++)
    {
        ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
        FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

        UEposMovieSceneSequence* eposSequence = iSequences[i];
        UMovieScene* eposMovieScene = eposSequence->GetMovieScene();

        ULevelSequence* levelSequence = NewObject<ULevelSequence>(GetTransientPackage(), ULevelSequence::StaticClass(), FName(TEXT("TransientRenderLevelSequence")), RF_Transient);
        levelSequence->Initialize();

        UMovieScene* levelMovieScene = levelSequence->GetMovieScene();

        TRange<FFrameNumber> eposPlaybackRange = eposMovieScene->GetPlaybackRange();

        //--- Prepare LevelSequence
        // Copy DisplayRate and TickResolution
        levelMovieScene->SetTickResolutionDirectly(eposMovieScene->GetTickResolution());
        levelMovieScene->SetDisplayRate(eposMovieScene->GetDisplayRate());
        // Set StartFrame and EndFrame
        TRange<FFrameNumber> levelPlaybackRange(FFrameNumber(0), eposPlaybackRange.GetUpperBoundValue() - eposPlaybackRange.GetLowerBoundValue());
        levelMovieScene->SetPlaybackRange(levelPlaybackRange);
        // Add CinematicShotTrack
        UMovieSceneCinematicShotTrack* shotTrack = levelMovieScene->AddMasterTrack<UMovieSceneCinematicShotTrack>();
        // Add Section to Cinematic shot track
        UMovieSceneSection* section = shotTrack->CreateNewSection();
        if (section)
        {
            shotTrack->Modify();
            shotTrack->AddSection(*section);
        }
        UMovieSceneCinematicShotSection* shotSection = Cast<UMovieSceneCinematicShotSection>(section);

        // Add boardSequence to section
        shotSection->SetSequence(eposSequence);

        // Set Section StartFrame and EndFrame
        shotSection->SetStartFrame(levelPlaybackRange.GetLowerBound());
        shotSection->SetEndFrame(levelPlaybackRange.GetUpperBound());

        // Render
        USequencerSettings* Settings = USequencerSettingsContainer::GetOrCreate<USequencerSettings>(*FString("LevelSequenceEditor"));
        FString MovieRendererName = Settings->GetMovieRendererName();
        if (MovieRendererName.IsEmpty() && SequencerModule.GetMovieRendererNames().Num() > 0)
        {
            MovieRendererName = SequencerModule.GetMovieRendererNames()[0];

            Settings->SetMovieRendererName(MovieRendererName);
        }

        if (IMovieRendererInterface* MovieRenderer = SequencerModule.GetMovieRenderer(MovieRendererName))
        {
            MovieRenderer->RenderMovie(levelSequence, TArray<UMovieSceneCinematicShotSection*>());
            return;
        }

        // Create a new movie scene capture object for an automated level sequence, and open the tab
        UAutomatedLevelSequenceCapture* MovieSceneCapture = NewObject<UAutomatedLevelSequenceCapture>(GetTransientPackage(), UAutomatedLevelSequenceCapture::StaticClass(), UMovieSceneCapture::MovieSceneCaptureUIName, RF_Transient);
        MovieSceneCapture->LoadFromConfig();

        // Always render from the root
        MovieSceneCapture->LevelSequenceAsset = levelSequence->GetMovieScene()->GetOuter()->GetPathName();

        FFrameRate DisplayRate = eposMovieScene->GetDisplayRate();
        FFrameRate TickResolution = eposMovieScene->GetTickResolution();

        MovieSceneCapture->Settings.FrameRate = DisplayRate;
        MovieSceneCapture->Settings.ZeroPadFrameNumbers = Settings->GetZeroPadFrames();
        MovieSceneCapture->Settings.bUseRelativeFrameNumbers = false;

        FFrameNumber StartFrame = UE::MovieScene::DiscreteInclusiveLower(eposPlaybackRange);
        FFrameNumber EndFrame = UE::MovieScene::DiscreteExclusiveUpper(eposPlaybackRange);

        FFrameNumber RoundedStartFrame = FFrameRate::TransformTime(StartFrame, TickResolution, DisplayRate).CeilToFrame();
        FFrameNumber RoundedEndFrame = FFrameRate::TransformTime(EndFrame, TickResolution, DisplayRate).CeilToFrame();

        if (!MovieSceneCapture->bUseCustomStartFrame)
        {
            MovieSceneCapture->CustomStartFrame = RoundedStartFrame;
        }

        if (!MovieSceneCapture->bUseCustomEndFrame)
        {
            MovieSceneCapture->CustomEndFrame = RoundedEndFrame;
        }

        // We create a new Numeric Type Interface that ties it's Capture/Resolution rates to the Capture Object so that it converts UI entries
        // to the correct resolution for the capture, and not for the original sequence.
        USequencerSettings* LocalSettings = Settings;

        TAttribute<EFrameNumberDisplayFormats> GetDisplayFormatAttr = MakeAttributeLambda(
            [LocalSettings]
            {
                if (LocalSettings)
                {
                    return LocalSettings->GetTimeDisplayFormat();
                }
                return EFrameNumberDisplayFormats::Frames;
            }
            );

        TAttribute<uint8> GetZeroPadFramesAttr = MakeAttributeLambda(
            [LocalSettings]()->uint8
            {
                if (LocalSettings)
                {
                    return LocalSettings->GetZeroPadFrames();
                }
                return 0;
            }
        );

        // By using a TickResolution/DisplayRate that match the numbers entered via the numeric interface don't change frames of reference.
        // This is used here because the movie scene capture works entirely on play rate resolution and has no knowledge of the internal resolution
        // so we don't need to convert the user's input into internal resolution.
        TAttribute<FFrameRate> GetFrameRateAttr = MakeAttributeLambda(
            [MovieSceneCapture]
            {
                if (MovieSceneCapture)
                {
                    return MovieSceneCapture->GetSettings().FrameRate;
                }
                return FFrameRate(30, 1);
            }
            );

        // TODO: That line should be called only when the capture has started
        // but for now we cannot know when it has started
        GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(levelSequence);

        // Create our numeric type interface so we can pass it to the time slider below.
        TSharedPtr<INumericTypeInterface<double>> MovieSceneCaptureNumericInterface = MakeShareable(new FFrameNumberInterface(GetDisplayFormatAttr, GetZeroPadFramesAttr, GetFrameRateAttr, GetFrameRateAttr));
        IMovieSceneCaptureDialogModule::Get().OpenDialog(LevelEditorModule.GetLevelEditorTabManager().ToSharedRef(), MovieSceneCapture, MovieSceneCaptureNumericInterface);

        // TODO: The following is not workiing as it should be called only when the capture has finished
        // And we cannot know when it has finished
        TSharedPtr<FMovieSceneCaptureBase> currentCapture = IMovieSceneCaptureDialogModule::Get().GetCurrentCapture();
        if (currentCapture)
        {
            currentCapture->CaptureStoppedDelegate.AddLambda([levelSequence](bool iSuccess)
                {
                    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(levelSequence);
                }
            );
        }
    }
}

//static
void
EposSequenceRenderHelpers::RenderMovie( TArray<UEposMovieSceneSequence*> iSequences )
{
    RenderSequencesByCreatingLevelSequenceAsset( iSequences );
    //RenderSequencesByCreatingLevelSequenceTrandient( iSequences );
}

#undef LOCTEXT_NAMESPACE

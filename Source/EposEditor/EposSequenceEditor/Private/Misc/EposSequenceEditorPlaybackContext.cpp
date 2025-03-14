// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Misc/EposSequenceEditorPlaybackContext.h"
#include "Editor.h"

#include "EposMovieSceneSequence.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorPlaybackContext"

FEposSequenceEditorPlaybackContext::FEposSequenceEditorPlaybackContext( UEposMovieSceneSequence* iEposSequence )
    : mEposSequence( iEposSequence )
{
    //FEditorDelegates::MapChange.AddRaw( this, &FLevelSequencePlaybackContext::OnMapChange );
    //FEditorDelegates::PreBeginPIE.AddRaw( this, &FLevelSequencePlaybackContext::OnPieEvent );
    //FEditorDelegates::BeginPIE.AddRaw( this, &FLevelSequencePlaybackContext::OnPieEvent );
    //FEditorDelegates::PostPIEStarted.AddRaw( this, &FLevelSequencePlaybackContext::OnPieEvent );
    //FEditorDelegates::PrePIEEnded.AddRaw( this, &FLevelSequencePlaybackContext::OnPieEvent );
    //FEditorDelegates::EndPIE.AddRaw( this, &FLevelSequencePlaybackContext::OnPieEvent );
}

FEposSequenceEditorPlaybackContext::~FEposSequenceEditorPlaybackContext()
{
    //FEditorDelegates::MapChange.RemoveAll( this );
    //FEditorDelegates::PreBeginPIE.RemoveAll( this );
    //FEditorDelegates::BeginPIE.RemoveAll( this );
    //FEditorDelegates::PostPIEStarted.RemoveAll( this );
    //FEditorDelegates::PrePIEEnded.RemoveAll( this );
    //FEditorDelegates::EndPIE.RemoveAll( this );
}

UEposMovieSceneSequence*
FEposSequenceEditorPlaybackContext::GetEposSequence() const
{
    return mEposSequence.Get();
}

UObject*
FEposSequenceEditorPlaybackContext::GetPlaybackContext() const
{
    UpdateCachedContextAndClient();

    //if( ALevelSequenceActor* Client = GetPlaybackClient() )
    //{
    //    return Client;
    //}

    return mWeakCurrentContext.Get();
}

UObject*
FEposSequenceEditorPlaybackContext::GetPlaybackContextAsObject() const
{
    return GetPlaybackContext();

}

FEposSequenceEditorPlaybackContext::FContextAndClient
FEposSequenceEditorPlaybackContext::ComputePlaybackContextAndClient( const UEposMovieSceneSequence* iEposSequence )
{
    //const ULevelSequenceEditorSettings* Settings = GetDefault<ULevelSequenceEditorSettings>();
    //IMovieSceneCaptureDialogModule* CaptureDialogModule = FModuleManager::GetModulePtr<IMovieSceneCaptureDialogModule>( "MovieSceneCaptureDialog" );

    //// Some plugins may not want us to automatically attempt to bind to the world where it doesn't make sense,
    //// such as movie rendering.
    bool bAllowPlaybackContextBinding = true;
    //ILevelSequenceEditorModule* LevelSequenceEditorModule = FModuleManager::GetModulePtr<ILevelSequenceEditorModule>( "LevelSequenceEditor" );
    //if( LevelSequenceEditorModule )
    //{
    //    LevelSequenceEditorModule->OnComputePlaybackContext().Broadcast( bAllowPlaybackContextBinding );
    //}

    UWorld* RecordingWorld = nullptr; // CaptureDialogModule ? CaptureDialogModule->GetCurrentlyRecordingWorld() : nullptr;

    // Only allow PIE and Simulate worlds if the settings allow them
    const bool bIsSimulatingInEditor = GEditor && GEditor->bIsSimulatingInEditor;
    const bool bIsPIEValid = ( !bIsSimulatingInEditor && /*Settings->bAutoBindToPIE*/ true ) || ( bIsSimulatingInEditor && /*Settings->bAutoBindToSimulate*/ true );

    UWorld* EditorWorld = nullptr;

    // Return PIE worlds if there are any
    for( const FWorldContext& Context : GEngine->GetWorldContexts() )
    {
        if( Context.WorldType == EWorldType::PIE )
        {
            UWorld* ThisWorld = Context.World();
            if( bIsPIEValid && bAllowPlaybackContextBinding && RecordingWorld != ThisWorld )
            {
                //TArray<ALevelSequenceActor*> LevelSequenceActors;
                //UE::MovieScene::FindLevelSequenceActors( ThisWorld, iEposSequence, LevelSequenceActors );
                //return FContextAndClient( ThisWorld, ( LevelSequenceActors.Num() > 0 ? LevelSequenceActors[0] : nullptr ) );
                return FContextAndClient( ThisWorld, nullptr );
            }
        }
        else if( Context.WorldType == EWorldType::Editor )
        {
            EditorWorld = Context.World();
        }
    }

    if( ensure( EditorWorld ) )
    {
        //// First search selected actors. If one is selected, use that as the client.
        //for( FSelectionIterator It( GEditor->GetSelectedActorIterator() ); It; ++It )
        //{
        //    if( ALevelSequenceActor* LSA = Cast<ALevelSequenceActor>( *It ) )
        //    {
        //        return FContextAndClient( EditorWorld, LSA );
        //    }
        //}

        // Otherwise, attempt to find one in the world.
        //TArray<ALevelSequenceActor*> LevelSequenceActors;
        //UE::MovieScene::FindLevelSequenceActors( EditorWorld, iEposSequence, LevelSequenceActors );
        //return FContextAndClient( EditorWorld, ( LevelSequenceActors.Num() > 0 ? LevelSequenceActors[0] : nullptr ) );
        return FContextAndClient( EditorWorld, nullptr );
    }

    return FContextAndClient( nullptr, nullptr );
}

void
FEposSequenceEditorPlaybackContext::UpdateCachedContextAndClient() const
{
    if( mWeakCurrentContext.Get() != nullptr )
    {
        return;
    }

    FContextAndClient ContextAndClient = ComputePlaybackContextAndClient( mEposSequence.Get() );
    check( ContextAndClient.Key );
    mWeakCurrentContext = ContextAndClient.Key;
    //WeakCurrentClient = ContextAndClient.Value;
}

#undef LOCTEXT_NAMESPACE

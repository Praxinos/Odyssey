// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Misc/EposSequenceEditorPlaybackContext.h"

#include "Editor.h"
#include "Engine/NetDriver.h"
#include "IMovieScenePlaybackClient.h"
#include "LevelInstance/LevelInstanceInterface.h"
#include "LevelInstance/LevelInstanceSubsystem.h"
#include "Selection.h"

#include "EposMovieSceneSequence.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorPlaybackContext"

//TODO: if needed one day
//class SLevelSequenceContextPicker : public SCompoundWidget
//{
//  ...
//}

namespace UE
{
    namespace MovieScene2
    {
        /**
         * Finds all actors that implement IMovieScenePlaybackClient in the given world, and return those that can preview the given sequence.
         */
        static void FindPlaybackClientActors( const UWorld* InWorld, const UEposMovieSceneSequence* InEposSequence, TArray<IMovieScenePlaybackClient*>& OutClients )
        {
            if( !InWorld )
            {
                return;
            }

            ULevelInstanceSubsystem* LevelInstanceSubsystem = InWorld->GetSubsystem<ULevelInstanceSubsystem>();

            for( const ULevel* Level : InWorld->GetLevels() )
            {
                ILevelInstanceInterface* LevelInstance = LevelInstanceSubsystem ? LevelInstanceSubsystem->GetOwningLevelInstance( Level ) : nullptr;
                if( LevelInstance && !LevelInstance->IsEditing() )
                {
                    continue;
                }

                for( AActor* Actor : Level->Actors )
                {
                    IMovieScenePlaybackClient* PlaybackClient = Cast<IMovieScenePlaybackClient>( Actor );
                    if( InEposSequence && PlaybackClient && PlaybackClient->CanPreview( *InEposSequence ) )
                    {
                        OutClients.Add( PlaybackClient );
                    }
                }
            }
        }

    }
}

FEposSequenceEditorPlaybackContext::FEposSequenceEditorPlaybackContext( UEposMovieSceneSequence* iEposSequence )
    : EposSequence( iEposSequence )
{
    FEditorDelegates::MapChange.AddRaw( this, &FEposSequenceEditorPlaybackContext::OnMapChange );
    FEditorDelegates::PreBeginPIE.AddRaw( this, &FEposSequenceEditorPlaybackContext::OnPieEvent );
    FEditorDelegates::BeginPIE.AddRaw( this, &FEposSequenceEditorPlaybackContext::OnPieEvent );
    FEditorDelegates::PostPIEStarted.AddRaw( this, &FEposSequenceEditorPlaybackContext::OnPieEvent );
    FEditorDelegates::PrePIEEnded.AddRaw( this, &FEposSequenceEditorPlaybackContext::OnPieEvent );
    FEditorDelegates::EndPIE.AddRaw( this, &FEposSequenceEditorPlaybackContext::OnPieEvent );

    if( GEngine )
    {
        GEngine->OnWorldAdded().AddRaw( this, &FEposSequenceEditorPlaybackContext::OnWorldListChanged );
        GEngine->OnWorldDestroyed().AddRaw( this, &FEposSequenceEditorPlaybackContext::OnWorldListChanged );
    }
}

FEposSequenceEditorPlaybackContext::~FEposSequenceEditorPlaybackContext()
{
    FEditorDelegates::MapChange.RemoveAll( this );
    FEditorDelegates::PreBeginPIE.RemoveAll( this );
    FEditorDelegates::BeginPIE.RemoveAll( this );
    FEditorDelegates::PostPIEStarted.RemoveAll( this );
    FEditorDelegates::PrePIEEnded.RemoveAll( this );
    FEditorDelegates::EndPIE.RemoveAll( this );

    if( GEngine )
    {
        GEngine->OnWorldAdded().RemoveAll( this );
        GEngine->OnWorldDestroyed().RemoveAll( this );
    }
}

void FEposSequenceEditorPlaybackContext::OnPieEvent( bool )
{
    WeakCurrentContext = nullptr;
}

void FEposSequenceEditorPlaybackContext::OnMapChange( uint32 )
{
    WeakCurrentContext = nullptr;
}

void FEposSequenceEditorPlaybackContext::OnWorldListChanged( UWorld* )
{
    WeakCurrentContext = nullptr;
}

UEposMovieSceneSequence* FEposSequenceEditorPlaybackContext::GetEposSequence() const
{
    return EposSequence.Get();
}

UObject* FEposSequenceEditorPlaybackContext::GetPlaybackContext() const
{
    UpdateCachedContextAndClient();

    if( UObject* Client = GetPlaybackClientAsUObject() )
    {
        return Client;
    }

    return WeakCurrentContext.Get();
}

UObject* FEposSequenceEditorPlaybackContext::GetPlaybackContextAsObject() const
{
    return GetPlaybackContext();

}

UObject* FEposSequenceEditorPlaybackContext::GetPlaybackClientAsUObject() const
{
    UpdateCachedContextAndClient();
    return WeakCurrentClient.GetObject();
}

IMovieScenePlaybackClient* FEposSequenceEditorPlaybackContext::GetPlaybackClientAsInterface() const
{
    UpdateCachedContextAndClient();
    return WeakCurrentClient.Get();
}

//TArray<UObject*> FLevelSequencePlaybackContext::GetEventContexts() const
//{
//    TArray<UObject*> Contexts;
//    if( UObject* PlaybackContext = GetPlaybackContext() )
//    {
//        if( UWorld* ContextWorld = PlaybackContext->GetWorld() )
//        {
//            ULevelSequencePlayer::GetEventContexts( *ContextWorld, Contexts );
//        }
//    }
//    return Contexts;
//}
//
//void FLevelSequencePlaybackContext::OverrideWith( UWorld* InNewContext, IMovieScenePlaybackClient* InNewClient )
//{
//    // InNewContext may be null to force an auto update
//    WeakCurrentContext = InNewContext;
//    WeakCurrentClient = InNewClient;
//}
//
//TSharedRef<SWidget> FLevelSequencePlaybackContext::BuildWorldPickerCombo()
//{
//    return SNew( SLevelSequenceContextPicker )
//        .Owner( this, &FLevelSequencePlaybackContext::GetLevelSequence )
//        .OnGetPlaybackContextAndClient( this, &FLevelSequencePlaybackContext::GetPlaybackContextAndClient )
//        .OnSetPlaybackContextAndClient( this, &FLevelSequencePlaybackContext::OverrideWith );
//}

FEposSequenceEditorPlaybackContext::FContextAndClient
FEposSequenceEditorPlaybackContext::ComputePlaybackContextAndClient( const UEposMovieSceneSequence* iEposSequence )
{
    //TODO
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
            const bool bIsServerWorld = ( ThisWorld && ThisWorld->GetNetDriver() && ThisWorld->GetNetDriver()->IsServer() );
            if( bIsPIEValid && bAllowPlaybackContextBinding && RecordingWorld != ThisWorld && !bIsServerWorld )
            {
                TArray<IMovieScenePlaybackClient*> PlaybackClients;
                UE::MovieScene2::FindPlaybackClientActors( ThisWorld, iEposSequence, PlaybackClients );
                return FContextAndClient( ThisWorld, ( PlaybackClients.Num() > 0 ? PlaybackClients[0] : nullptr ) );
            }
        }
        else if( Context.WorldType == EWorldType::Editor )
        {
            EditorWorld = Context.World();
        }
    }

    if( ensure( EditorWorld ) )
    {
        // First search selected actors. If one is selected, use that as the client.
        for( FSelectionIterator It( GEditor->GetSelectedActorIterator() ); It; ++It )
        {
            if( IMovieScenePlaybackClient* Client = Cast<IMovieScenePlaybackClient>( *It ) )
            {
                return FContextAndClient( EditorWorld, Client );
            }
        }

        // Otherwise, attempt to find one in the world.
        TArray<IMovieScenePlaybackClient*> PlaybackClients;
        UE::MovieScene2::FindPlaybackClientActors( EditorWorld, iEposSequence, PlaybackClients );
        return FContextAndClient( EditorWorld, ( PlaybackClients.Num() > 0 ? PlaybackClients[0] : nullptr ) );
    }

    return FContextAndClient( nullptr, nullptr );
}

void
FEposSequenceEditorPlaybackContext::UpdateCachedContextAndClient() const
{
    if( WeakCurrentContext.Get() != nullptr )
    {
        return;
    }

    FContextAndClient ContextAndClient = ComputePlaybackContextAndClient( EposSequence.Get() );
    check( ContextAndClient.Key );
    WeakCurrentContext = ContextAndClient.Key;
    WeakCurrentClient = ContextAndClient.Value;
}

FEposSequenceEditorPlaybackContext::FContextAndClient FEposSequenceEditorPlaybackContext::GetPlaybackContextAndClient() const
{
    UpdateCachedContextAndClient();
    return FContextAndClient( WeakCurrentContext.Get(), WeakCurrentClient.Get() );
}

//TODO: if needed one day
//Construct SLevelSequenceContextPicker

#undef LOCTEXT_NAMESPACE

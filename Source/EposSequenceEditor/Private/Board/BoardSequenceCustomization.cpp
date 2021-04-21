// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Board/BoardSequenceCustomization.h"

#include "CineCameraActor.h"

#include "ArrangeSectionsType.h"
#include "Board/BoardSequence.h"
#include "Board/BoardSequenceEditorCommands.h"
#include "CinematicBoardTrack/CinematicBoardTrackHelpers.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposTracksModule.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/EposTracksSettings.h"
#include "Shot/ShotSequence.h"
#include "Shot/ShotSequenceEditorCommands.h"
#include "ShotHelpers/ShotSequenceHelpers.h"

#define LOCTEXT_NAMESPACE "BoardSequenceCustomization"

//---

void
FBoardSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder ) // This is called each time the focused sequence changed (ie. when double-clicking on a section to go inside its subsequence)
{
    mSequencer = &ioBuilder.GetSequencer();
    mBoardSequence = Cast<UBoardSequence>( &ioBuilder.GetFocusedSequence() );

    //---

    // Listen for actor/component movement
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw( this, &FBoardSequenceCustomization::OnPrePropertyChanged );
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw( this, &FBoardSequenceCustomization::OnPostPropertyChanged );

    mPatchNotifySequencerHandle = FEposTracksModule::GetTracksCustomizationManager().Register( FOnNotifySequencer::CreateLambda( [this](){ mSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::Unknown ); } ) );

    ProcessCommands( mSequencer->GetCommandBindings(), kMap );

    //---

    FSequencerCustomizationInfo customization;

    TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();
    ToolbarExtender->AddToolBarExtension( "Curve Editor", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw( this, &FBoardSequenceCustomization::ExtendSequencerToolbar ) );
    customization.ToolbarExtender = ToolbarExtender;

    //customization.OnReceivedDragOver.BindRaw( this, &FBoardSequenceCustomization::OnSequencerReceiveDragOver );
    //customization.OnReceivedDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerReceiveDrop );

    customization.OnAssetsDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerAssetsDrop );
    customization.OnClassesDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerClassesDrop );
    customization.OnActorsDrop.BindRaw( this, &FBoardSequenceCustomization::OnSequencerActorsDrop );

    ioBuilder.AddCustomization( customization );
}

void
FBoardSequenceCustomization::UnregisterSequencerCustomization()
{
    FEposTracksModule::GetTracksCustomizationManager().Unregister( mPatchNotifySequencerHandle );
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.RemoveAll( this );
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll( this );

    ProcessCommands( mSequencer->GetCommandBindings(), kUnmap );

    //---

    mSequencer = nullptr;
    mBoardSequence = nullptr;
}

//---

void
FBoardSequenceCustomization::ProcessCommands( TSharedPtr<FUICommandList> CommandList, EMapping iMap )
{
    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().ArrangeShotsManually,
            FExecuteAction::CreateLambda( [this]{ CinematicBoardTrackHelpers::SetArrangeSections( mSequencer, EArrangeSections::Manually ); } ),
            FCanExecuteAction::CreateLambda([] { return true; }),
            FIsActionChecked::CreateLambda([] { return GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots == EArrangeSections::Manually; } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().ArrangeShotsManually );

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow,
            FExecuteAction::CreateLambda( [this]{ CinematicBoardTrackHelpers::SetArrangeSections( mSequencer, EArrangeSections::OnOneRow ); } ),
            FCanExecuteAction::CreateLambda([] { return true; }),
            FIsActionChecked::CreateLambda([] { return GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots == EArrangeSections::OnOneRow; } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow );

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows,
            FExecuteAction::CreateLambda( [this]{ CinematicBoardTrackHelpers::SetArrangeSections( mSequencer, EArrangeSections::OnTwoRowsShifted ); } ),
            FCanExecuteAction::CreateLambda([] { return true; }),
            FIsActionChecked::CreateLambda([] { return GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots == EArrangeSections::OnTwoRowsShifted; } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows );

    //---

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().NewSectionWithBoardAtCurrentFrame,
            FExecuteAction::CreateLambda( [this]{ CinematicBoardTrackHelpers::InsertBoard( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().NewSectionWithBoardAtCurrentFrame );

    if( iMap == kMap )
        CommandList->MapAction(
            FBoardSequenceEditorCommands::Get().NewSectionWithShotAtCurrentFrame,
            FExecuteAction::CreateLambda( [this]{ CinematicBoardTrackHelpers::InsertShot( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FBoardSequenceEditorCommands::Get().NewSectionWithShotAtCurrentFrame );

    //---

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().CreateCamera,
            FExecuteAction::CreateLambda( [this]{ BoardSequenceHelpers::CreateCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this]{ return BoardSequenceHelpers::CanCreateCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().CreateCamera );

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().CreatePlane,
            FExecuteAction::CreateLambda( [this]{ BoardSequenceHelpers::CreatePlane( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this]{ return !!BoardSequenceHelpers::GetCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().CreatePlane );

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().SnapCameraToViewport,
            FExecuteAction::CreateLambda( [this]{ BoardSequenceHelpers::SnapCameraToViewport( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this]{ return !!BoardSequenceHelpers::GetCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().SnapCameraToViewport );
}

//---

void
FBoardSequenceCustomization::ExtendSequencerToolbar( FToolBarBuilder& ToolbarBuilder )
{
    ToolbarBuilder.AddSeparator();

    TAttribute<FText> ArrangeShotsName;
    ArrangeShotsName.Bind( TAttribute<FText>::FGetter::CreateLambda( []
    {
        switch( GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots )
        {
            case EArrangeSections::OnOneRow:            return FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow->GetLabel();
            case EArrangeSections::OnTwoRowsShifted:    return FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows->GetLabel();
            default:
            case EArrangeSections::Manually:            return FBoardSequenceEditorCommands::Get().ArrangeShotsManually->GetLabel();
        }
    } ) );

    TAttribute<FSlateIcon> ArrangeShotsIcon;
    ArrangeShotsIcon.Bind( TAttribute<FSlateIcon>::FGetter::CreateLambda( []
    {
        switch( GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots )
        {
            case EArrangeSections::OnOneRow:            return FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow->GetIcon();
            case EArrangeSections::OnTwoRowsShifted:    return FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows->GetIcon();
            default:
            case EArrangeSections::Manually:            return FBoardSequenceEditorCommands::Get().ArrangeShotsManually->GetIcon();
        }
    } ) );

    TAttribute<FText> ArrangeShotsToolTip;
    ArrangeShotsToolTip.Bind( TAttribute<FText>::FGetter::CreateLambda( []
    {
        switch( GetDefault<UEposTracksSettings>()->BoardTrackSettings.ArrangeShots )
        {
            case EArrangeSections::OnOneRow:            return FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow->GetDescription();
            case EArrangeSections::OnTwoRowsShifted:    return FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows->GetDescription();
            default:
            case EArrangeSections::Manually:            return FBoardSequenceEditorCommands::Get().ArrangeShotsManually->GetDescription();
        }
    } ) );

    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeArrangeShotsMenu ),
        ArrangeShotsName,
        ArrangeShotsToolTip,
        ArrangeShotsIcon );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FBoardSequenceEditorCommands::Get().NewSectionWithBoardAtCurrentFrame );
    ToolbarBuilder.AddToolBarButton( FBoardSequenceEditorCommands::Get().NewSectionWithShotAtCurrentFrame );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().CreateCamera );
    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FBoardSequenceCustomization::MakeCameraMenu ),
        LOCTEXT( "CameraOptions", "Options" ),
        LOCTEXT( "CameraOptionsToolTip", "Camera Options" ),
        TAttribute<FSlateIcon>(),
        true );

    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().SnapCameraToViewport );
    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().CreatePlane );

    ToolbarBuilder.AddSeparator();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeArrangeShotsMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    //MenuBuilder.BeginSection( NAME_None, LOCTEXT( "ArrangeShotsHeader", "Arrange Shots" ) );
    {
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsManually );
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow );
        MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows );
    }
    //MenuBuilder.EndSection();

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FBoardSequenceCustomization::MakeCameraMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    MenuBuilder.BeginSection( NAME_None, LOCTEXT( "CameraSettingsTitle", "Default Camera Settings" ) );
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

        // Create a detail view
        FDetailsViewArgs Args( false, false, false, FDetailsViewArgs::HideNameArea );
        Args.ColumnWidth = .5f;
        TSharedRef<IDetailsView> DetailView = PropertyModule.CreateDetailView( Args );

        // Filter properties to only get CameraSettings ones
        auto visible_property = []( const FPropertyAndParent& iPropertyChain )
        {
            FName root_name = iPropertyChain.ParentProperties.Num() ? iPropertyChain.ParentProperties.Last()->GetFName() : iPropertyChain.Property.GetFName();
            return root_name == GET_MEMBER_NAME_CHECKED( UEposSequenceEditorSettings, CameraSettings );
        };
        DetailView->GetIsPropertyVisibleDelegate() = FIsPropertyVisible::CreateLambda( visible_property );
        // Set the object to view
        DetailView->SetObject( GetMutableDefault<UEposSequenceEditorSettings>() );

        MenuBuilder.AddWidget( DetailView, FText(), true );
    }
    MenuBuilder.EndSection();

    return MenuBuilder.MakeWidget();
}

//---

void
FBoardSequenceCustomization::OnPreTransformChanged( UObject& InObject )
{
    if( !mSequencer->IsAllowedToChange() )
        return;

    ACineCameraActor* Actor = Cast<ACineCameraActor>( &InObject );
    // If Sequencer is allowed to autokey and we are clicking on an Actor that can't be autokeyed
    if( !Actor || Actor->IsEditorOnly() )
        return;

    USceneComponent* SceneComponentThatChanged = Actor->GetRootComponent();
    check( SceneComponentThatChanged );

    // Cache off the existing transform so we can detect which components have changed
    // and keys only when something has changed
    FTransformData Transform( SceneComponentThatChanged );

    mObjectToExistingTransform.Add( &InObject, Transform );

    // Do not manage track creation as it should already exist, and otherwise, do nothing (for the moment)
}

void
FBoardSequenceCustomization::OnTransformChanged( UObject& InObject )
{
    if( !mSequencer->IsAllowedToChange() )
        return;

    ACineCameraActor* Actor = Cast<ACineCameraActor>( &InObject );
    // If the Actor that just finished transforming doesn't have autokey disabled
    if( !Actor || Actor->IsEditorOnly() )
        return;

    USceneComponent* SceneComponentThatChanged = Actor->GetRootComponent();
    check( SceneComponentThatChanged );

    // Find an existing transform if possible.  If one exists we will compare against the new one to decide what components of the transform need keys
    TOptional<FTransformData> ExistingTransform;
    if( const FTransformData* Found = mObjectToExistingTransform.Find( &InObject ) )
    {
        ExistingTransform = *Found;
    }

    // Remove it from the list of cached transforms.
    // @todo sequencer livecapture: This can be made much for efficient by not removing cached state during live capture situation
    mObjectToExistingTransform.Remove( &InObject );

    // Build new transform data
    FTransformData NewTransformData( SceneComponentThatChanged );

    //---

    BoardSequenceHelpers::StopPilotingCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, Actor, ExistingTransform, NewTransformData );
}

void
FBoardSequenceCustomization::OnPrePropertyChanged( UObject* InObject, const FEditPropertyChain& InPropertyChain )
{
    FProperty* PropertyAboutToChange = InPropertyChain.GetActiveMemberNode()->GetValue();
    const FName MemberPropertyName = PropertyAboutToChange != nullptr ? PropertyAboutToChange->GetFName() : NAME_None;
    const bool bTransformationToChange =
        ( MemberPropertyName == USceneComponent::GetRelativeLocationPropertyName() ||
          MemberPropertyName == USceneComponent::GetRelativeRotationPropertyName() ||
          MemberPropertyName == USceneComponent::GetRelativeScale3DPropertyName() );

    if( InObject && bTransformationToChange )
    {
        OnPreTransformChanged( *InObject );
    }
}

void
FBoardSequenceCustomization::OnPostPropertyChanged( UObject* InObject, FPropertyChangedEvent& InPropertyChangedEvent )
{
    const FName MemberPropertyName = InPropertyChangedEvent.MemberProperty != nullptr ? InPropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
    const bool bTransformationChanged =
        ( MemberPropertyName == USceneComponent::GetRelativeLocationPropertyName() ||
          MemberPropertyName == USceneComponent::GetRelativeRotationPropertyName() ||
          MemberPropertyName == USceneComponent::GetRelativeScale3DPropertyName() );

    if( InObject && bTransformationChanged )
    {
        OnTransformChanged( *InObject );
    }
}

//---

//bool
//FBoardSequenceCustomization::OnSequencerReceiveDragOver( const FGeometry& iGeometry, const FDragDropEvent& iEvent, FReply& oReply )
//{
//    oReply = FReply::Unhandled();
//
//    return false;
//}
//
//bool
//FBoardSequenceCustomization::OnSequencerReceiveDrop( const FGeometry& iGeometry, const FDragDropEvent& iEvent, FReply& oReply )
//{
//    oReply = FReply::Unhandled();
//
//    return false;
//}

ESequencerDropResult
FBoardSequenceCustomization::OnSequencerAssetsDrop( const TArray<UObject*>& iAssets, const FAssetDragDropOp& iDragDropOp )
{
    // Maybe we can store data, and then get them inside tracks through (like in sequencer.cpp ?)
    //   ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );
    //   TSharedPtr<FSequencerCustomizationManager> Manager = SequencerModule.GetSequencerCustomizationManager();
    //   ...
    // because once an asset is dropped on a NOT supported track, it goes through here, where we can store iDragDropOp.X/Y,
    // and get them again when we go inside CinematicBoardTrackEditor:HandleAssetAdded() where the information is not given

    return ESequencerDropResult::Unhandled; // Process the default behavior for assets
}

ESequencerDropResult
FBoardSequenceCustomization::OnSequencerClassesDrop( const TArray<TWeakObjectPtr<UClass>>& iClasses, const FClassDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::DropDenied;    // Don't accept classes
}

ESequencerDropResult
FBoardSequenceCustomization::OnSequencerActorsDrop( const TArray<TWeakObjectPtr<AActor>>& iActors, const FActorDragDropGraphEdOp& iDragDropOp )
{
    return ESequencerDropResult::DropDenied;    // Don't accept actors
}

#undef LOCTEXT_NAMESPACE

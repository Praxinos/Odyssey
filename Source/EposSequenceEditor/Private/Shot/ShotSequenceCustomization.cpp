// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shot/ShotSequenceCustomization.h"

#include "CineCameraActor.h"

#include "EposSequenceEditorCommands.h"
#include "EposSequenceToolbarHelpers.h"
#include "PlaneActor.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposSequenceEditorStyle.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"
#include "Tools/LighttableTools.h"

#define LOCTEXT_NAMESPACE "ShotSequenceCustomization"

//---

FShotSequenceCustomization::~FShotSequenceCustomization()
{
    //PATCH: unregister is not called if
    // - a board sequence is opened
    // - open a level sequence which replace the board sequence
    // - UnregisterSequencerCustomization() of the board sequence is not called
    // So, call it inside the destructor and check is wasn't called before
    if( mSequencer )
        UnregisterSequencerCustomization();
}

void
FShotSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder )
{
    mSequencer = &ioBuilder.GetSequencer();
    mShotSequence = Cast<UShotSequence>( &ioBuilder.GetFocusedSequence() );

    //---

    // Listen for actor/component movement
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw( this, &FShotSequenceCustomization::OnPrePropertyChanged );
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw( this, &FShotSequenceCustomization::OnPostPropertyChanged );

    ProcessCommands( mSequencer->GetCommandBindings(), kMap );

    //---

    FSequencerCustomizationInfo customization;

    // customization.AddMenuExtender ...

    TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();
    ToolbarExtender->AddToolBarExtension( "Curve Editor", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw( this, &FShotSequenceCustomization::ExtendSequencerToolbar ) );
    customization.ToolbarExtender = ToolbarExtender;

    // customization.OnReceivedDragOver ...
    // customization.OnReceivedDrop ...

    customization.OnAssetsDrop.BindRaw( this, &FShotSequenceCustomization::OnSequencerAssetsDrop );
    customization.OnClassesDrop.BindRaw( this, &FShotSequenceCustomization::OnSequencerClassesDrop );
    customization.OnActorsDrop.BindRaw( this, &FShotSequenceCustomization::OnSequencerActorsDrop );

    ioBuilder.AddCustomization( customization );
}

void
FShotSequenceCustomization::UnregisterSequencerCustomization()
{
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.RemoveAll( this );
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll( this );

    ProcessCommands( mSequencer->GetCommandBindings(), kUnmap );

    mSequencer = nullptr;
    mShotSequence = nullptr;
}

//---

void
FShotSequenceCustomization::ProcessCommands( TSharedPtr<FUICommandList> CommandList, EMapping iMap )
{
    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().CreateCameraAtCurrentTime,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::CreateCamera( mSequencer ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::CanCreateCamera( mSequencer ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().CreateCameraAtCurrentTime );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().SnapCameraToViewportAtCurrentTime,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::SnapCameraToViewport( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            // It doesn't work due to strange stuff between FMovieSceneSequenceID and FMovieSceneSequenceIDRef ...
            //FExecuteAction::CreateStatic( &ShotSequenceTools::SnapCameraToViewport, mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID() ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::CanSnapCameraToViewport( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().SnapCameraToViewportAtCurrentTime );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().PilotCameraAtCurrentTime,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::PilotCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::CanPilotCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().PilotCameraAtCurrentTime );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().EjectCameraAtCurrentTime,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::EjectCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::CanEjectCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().EjectCameraAtCurrentTime );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().GotoPreviousCameraPosition,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::GotoPreviousCameraPosition( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::HasPreviousCameraPosition( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().GotoPreviousCameraPosition );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().GotoNextCameraPosition,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::GotoNextCameraPosition( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::HasNextCameraPosition( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().GotoNextCameraPosition );

    //---

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().CreatePlaneAtCurrentTime,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::CreatePlane( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::CanCreatePlane( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().CreatePlaneAtCurrentTime );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().DetachPlaneAtCurrentTime,
            FExecuteAction::CreateLambda( [this]()
                                          {
                                              TArray<FGuid> plane_bindings;
                                              int32 plane_count = ShotSequenceTools::GetAttachedPlanes( mSequencer, nullptr, &plane_bindings );
                                              if( plane_count != 1 )
                                                  return;
                                              ShotSequenceTools::DetachPlane( mSequencer, plane_bindings[0] );
                                          } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::GetAttachedPlanes( mSequencer ) == 1; } ),
            FIsActionChecked(),
            FIsActionButtonVisible::CreateLambda( [this](){ return ShotSequenceTools::GetAttachedPlanes( mSequencer ) <= 1; } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().DetachPlaneAtCurrentTime );

    //---

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().CreateDrawingAtCurrentTime,
            FExecuteAction::CreateLambda( [this]()
                                          {
                                              TArray<FGuid> plane_bindings;
                                              int32 plane_count = ShotSequenceTools::GetAllPlanes( mSequencer, nullptr, &plane_bindings );
                                              if( plane_count != 1 )
                                                  return;
                                              ShotSequenceTools::CreateDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, plane_bindings[0] );
                                          } ),
            FCanExecuteAction::CreateLambda( [this]()
                                             {
                                                 TArray<FGuid> plane_bindings;
                                                 int32 plane_count = ShotSequenceTools::GetAllPlanes( mSequencer, nullptr, &plane_bindings );
                                                 if( plane_count != 1 )
                                                     return false;
                                                 return ShotSequenceTools::CanCreateDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, plane_bindings[0] );
                                             } ),
            FIsActionChecked(),
            FIsActionButtonVisible::CreateLambda( [this](){ return ShotSequenceTools::GetAllPlanes( mSequencer ) <= 1; } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().CreateDrawingAtCurrentTime );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().GotoPreviousDrawing,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::GotoPreviousDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::HasPreviousDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().GotoPreviousDrawing );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().GotoNextDrawing,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::GotoNextDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::HasNextDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().GotoNextDrawing );
}

//---

void
FShotSequenceCustomization::ExtendSequencerToolbar( FToolBarBuilder& ToolbarBuilder )
{
    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().CreateCameraAtCurrentTime );
    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeCameraMenu ),
        LOCTEXT( "CameraOptions", "Options" ),
        LOCTEXT( "CameraOptionsToolTip", "Camera Options" ),
        TAttribute<FSlateIcon>(),
        true );
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().SnapCameraToViewportAtCurrentTime );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().CreatePlaneAtCurrentTime );
    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeTextureMenu ),
        LOCTEXT( "TextureOptions", "Options" ),
        LOCTEXT( "TextureOptionsToolTip", "Texture Options" ),
        TAttribute<FSlateIcon>(),
        true );
    // The 2 following buttons should be exclusive visible:
    // - the first button is displayed when there is only 1 plane (or 0) available
    // - the second button is displayed when there are more than 2 planes available
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().DetachPlaneAtCurrentTime );
    ToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this](){ return ShotSequenceTools::GetAttachedPlanes( mSequencer ) > 1; } )
        ),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakePlaneMenu ),
        FEposSequenceEditorCommands::Get().DetachPlaneAtCurrentTime->GetLabel(),
        FEposSequenceEditorCommands::Get().DetachPlaneAtCurrentTime->GetDescription(),
        FEposSequenceEditorCommands::Get().DetachPlaneAtCurrentTime->GetIcon() );

    auto GetLighttableTooltip = [this]() -> FText
    {
        TArray<FGuid> plane_bindings;
        int32 plane_count = ShotSequenceTools::GetAllPlanes( mSequencer, nullptr, &plane_bindings );
        check( plane_count == 1 );
        //if( plane_count != 1 )
        //    return;

        if( LighttableTools::IsOn( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), plane_bindings[0] ) )
            return LOCTEXT( "disable-lighttable-tooltip", "Disable the lighttable" );
        else
            return LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable" );
    };

    auto GetLighttableIcon = [this]() -> FSlateIcon
    {
        TArray<FGuid> plane_bindings;
        int32 plane_count = ShotSequenceTools::GetAllPlanes( mSequencer, nullptr, &plane_bindings );
        check( plane_count == 1 );
        //if( plane_count != 1 )
        //    return;

        if( LighttableTools::IsOn( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), plane_bindings[0] ) )
            return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOn" );
        else
            return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );
    };

    // The 2 following buttons should be exclusive visible:
    // - the first button is displayed when there is only 1 plane (or 0) available
    // - the second button is displayed when there are more than 2 planes available
    ToolbarBuilder.AddToolBarButton( FUIAction(
                                         FExecuteAction::CreateLambda( [this]()
                                                                       {
                                                                           TArray<FGuid> plane_bindings;
                                                                           int32 plane_count = ShotSequenceTools::GetAllPlanes( mSequencer, nullptr, &plane_bindings );
                                                                           if( plane_count != 1 )
                                                                               return;

                                                                           if( LighttableTools::IsOn( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), plane_bindings[0] ) )
                                                                               LighttableTools::Deactivate( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), plane_bindings[0] );
                                                                           else
                                                                               LighttableTools::Activate( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), plane_bindings[0] );
                                                                       } ),
                                         FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::GetAllPlanes( mSequencer ) == 1; } ),
                                         FIsActionChecked(),
                                         FIsActionButtonVisible::CreateLambda( [this](){ return ShotSequenceTools::GetAllPlanes( mSequencer ) <= 1; } ) ),
                                     NAME_None,
                                     FText::GetEmpty(),
                                     MakeAttributeLambda( GetLighttableTooltip ),
                                     MakeAttributeLambda( GetLighttableIcon )
                                     );
    ToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this](){ return ShotSequenceTools::GetAllPlanes( mSequencer ) > 1; } )
        ),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeLighttableMenu ),
        FText::GetEmpty(),
        LOCTEXT( "LighttableOptionsTooltip", "Activate/Deactivate lighttable on planes" ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" ) );

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoPreviousCameraPosition );
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoNextCameraPosition );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoPreviousDrawing );
    // The 2 following buttons should be exclusive visible:
    // - the first button is displayed when there is only 1 plane (or 0) available
    // - the second button is displayed when there are more than 2 planes available
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().CreateDrawingAtCurrentTime );
    ToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this](){ return ShotSequenceTools::GetAllPlanes( mSequencer ) > 1; } )
        ),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeDrawingMenu ),
        FEposSequenceEditorCommands::Get().CreateDrawingAtCurrentTime->GetLabel(),
        FEposSequenceEditorCommands::Get().CreateDrawingAtCurrentTime->GetDescription(),
        FEposSequenceEditorCommands::Get().CreateDrawingAtCurrentTime->GetIcon() );
    ToolbarBuilder.AddToolBarButton( FEposSequenceEditorCommands::Get().GotoNextDrawing );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeSettingsMenu ),
        LOCTEXT( "Settings", "Settings" ),
        LOCTEXT( "SettingsToolTip", "Set sequence settings" ),
        FSlateIcon( FEposSequenceEditorStyle::Get().GetStyleSetName(), "Settings" ) );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeHelpMenu ),
        LOCTEXT( "Help", "Help" ),
        LOCTEXT( "HelpToolTip", "Help" ),
        FSlateIcon( FEposSequenceEditorStyle::Get().GetStyleSetName(), "Help" ) );
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeCameraMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    EposSequenceToolbarHelpers::MakeCameraSettingsEntries( MenuBuilder );

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakePlaneMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    TArray<APlaneActor*> planes;
    TArray<FGuid> plane_bindings;
    int32 plane_count = ShotSequenceTools::GetAttachedPlanes( mSequencer, &planes, &plane_bindings );
    if( !plane_count )
        return SNullWidget::NullWidget;

    for( int i = 0; i < plane_count; i++ )
    {
        APlaneActor* plane = planes[i];
        FGuid plane_binding = plane_bindings[i];

        MenuBuilder.AddMenuEntry(
            FText::FromString( plane->GetActorLabel() ),
            FText::GetEmpty(),
            FSlateIcon(),
            FUIAction(
                FExecuteAction::CreateLambda( [this, plane_binding](){ ShotSequenceTools::DetachPlane( mSequencer, plane_binding ); } )
            )
        );
    }

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeLighttableMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    TArray<APlaneActor*> planes;
    TArray<FGuid> plane_bindings;
    int32 plane_count = ShotSequenceTools::GetAllPlanes( mSequencer, &planes, &plane_bindings );
    if( !plane_count )
        return SNullWidget::NullWidget;

    for( int i = 0; i < plane_count; i++ )
    {
        APlaneActor* plane = planes[i];
        FGuid plane_binding = plane_bindings[i];

        FText tooltip;
        if( LighttableTools::IsOn( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), plane_binding ) )
            tooltip = LOCTEXT( "disable-lighttable-tooltip", "Disable the lighttable" );
        else
            tooltip = LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable" );

        FSlateIcon icon;
        if( LighttableTools::IsOn( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), plane_binding ) )
            icon = FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOn" );
        else
            icon = FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );

        MenuBuilder.AddMenuEntry(
            FText::FromString( plane->GetActorLabel() ),
            tooltip,
            icon,
            FUIAction(
                FExecuteAction::CreateLambda( [this, plane_binding]()
                                              {
                                                  if( LighttableTools::IsOn( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), plane_binding ) )
                                                      LighttableTools::Deactivate( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), plane_binding );
                                                  else
                                                      LighttableTools::Activate( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), plane_binding );
                                              } )
            )
        );
    }

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeDrawingMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    TArray<APlaneActor*> planes;
    TArray<FGuid> plane_bindings;
    int32 plane_count = ShotSequenceTools::GetAllPlanes( mSequencer, &planes, &plane_bindings );
    if( !plane_count )
        return SNullWidget::NullWidget;

    for( int i = 0; i < plane_count; i++ )
    {
        APlaneActor* plane = planes[i];
        FGuid plane_binding = plane_bindings[i];

        MenuBuilder.AddMenuEntry(
            FText::FromString( plane->GetActorLabel() ),
            //LOCTEXT( "LockPlayback", "Lock to Display Rate at Runtime" ),
            FText::GetEmpty(),
            //LOCTEXT( "LockPlayback_Description", "When enabled, causes all runtime evaluation and the engine FPS to be locked to the current display frame rate" ),
            FSlateIcon(),
            FUIAction(
                FExecuteAction::CreateLambda( [this, plane_binding](){ ShotSequenceTools::CreateDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, plane_binding ); } ),
                FCanExecuteAction::CreateLambda( [this, plane_binding](){ return ShotSequenceTools::CanCreateDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, plane_binding ); } )
            )/*,
            NAME_None,
            EUserInterfaceActionType::ToggleButton*/ //TODO: I don't know how, but there should be something to multi-select planes and create plane on them
        );
    }

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeSettingsMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    EposSequenceToolbarHelpers::MakeSettingsEntries( MenuBuilder, mSequencer );

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeTextureMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    EposSequenceToolbarHelpers::MakeTextureSettingsEntries( MenuBuilder );

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeHelpMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    EposSequenceToolbarHelpers::MakeHelpEntries( MenuBuilder );

    return MenuBuilder.MakeWidget();
}

//---

void
FShotSequenceCustomization::OnPreTransformChanged( UObject& InObject )
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
FShotSequenceCustomization::OnTransformChanged( UObject& InObject )
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

    ShotSequenceTools::StopPilotingCamera( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, Actor, ExistingTransform, NewTransformData );
}

void
FShotSequenceCustomization::OnPrePropertyChanged( UObject* InObject, const FEditPropertyChain& InPropertyChain )
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
FShotSequenceCustomization::OnPostPropertyChanged( UObject* InObject, FPropertyChangedEvent& InPropertyChangedEvent )
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
//FShotSequenceCustomization::OnSequencerReceivedDragOver( const FGeometry& iMyGeometry, const FDragDropEvent& iDragDropEvent, FReply& oReply )
//{
//    bool bIsDragSupported = false;
//
//    TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
//    if( Operation.IsValid() && (
//        ( Operation->IsOfType<FAssetDragDropOp>() && StaticCastSharedPtr<FAssetDragDropOp>( Operation )->GetAssetPaths().Num() <= 1 ) ||
//        ( Operation->IsOfType<FClassDragDropOp>() && StaticCastSharedPtr<FClassDragDropOp>( Operation )->ClassesToDrop.Num() <= 1 ) ||
//        ( Operation->IsOfType<FActorDragDropGraphEdOp>() && StaticCastSharedPtr<FActorDragDropGraphEdOp>( Operation )->Actors.Num() <= 1 ) ) )
//    {
//        bIsDragSupported = true;
//    }
//
//    OutReply = ( bIsDragSupported ? FReply::Handled() : FReply::Unhandled() );
//    return true;
//}

ESequencerDropResult
FShotSequenceCustomization::OnSequencerAssetsDrop( const TArray<UObject*>& iAssets, const FAssetDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::Unhandled; // Process the default behavior for assets
}

ESequencerDropResult
FShotSequenceCustomization::OnSequencerClassesDrop( const TArray<TWeakObjectPtr<UClass>>& iClasses, const FClassDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::DropDenied;    // Don't accept classes
}

ESequencerDropResult
FShotSequenceCustomization::OnSequencerActorsDrop( const TArray<TWeakObjectPtr<AActor>>& iActors, const FActorDragDropOp& iDragDropOp )
{
    return ESequencerDropResult::Unhandled; // Process the default behavior for actors
}

#undef LOCTEXT_NAMESPACE

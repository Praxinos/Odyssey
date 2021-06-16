// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Shot/ShotSequenceCustomization.h"

#include "CineCameraActor.h"

#include "EposSequenceEditorCommands.h"
#include "Misc/SAboutWindow.h"
#include "PlaneActor.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "ShotSequenceEditorCommands.h"
#include "Styles/EposSequenceEditorStyle.h"
#include "Tools/EposSequenceTools.h"

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
            FShotSequenceEditorCommands::Get().CreateCamera,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::CreateCamera( mSequencer ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return !ShotSequenceTools::GetCamera( mSequencer ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().CreateCamera );

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().SnapCameraToViewport,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::SnapCameraToViewport( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            // It doesn't work due to strange stuff between FMovieSceneSequenceID and FMovieSceneSequenceIDRef ...
            //FExecuteAction::CreateStatic( &ShotSequenceTools::SnapCameraToViewport, mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID() ),
            FCanExecuteAction::CreateLambda( [this](){ return !!ShotSequenceTools::GetCamera( mSequencer ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().SnapCameraToViewport );

    //---

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().CreatePlane,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::CreatePlane( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return !!ShotSequenceTools::GetCamera( mSequencer ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().CreatePlane );

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().DetachPlane,
            FExecuteAction::CreateLambda( [this]()
                {
                    TArray<APlaneActor*> planes;
                    int32 plane_count = ShotSequenceTools::GetAttachedPlanes( mSequencer, &planes, nullptr );
                    if( !plane_count )
                        return;
                    ShotSequenceTools::DetachPlane( mSequencer, planes[0] );
                } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::GetAttachedPlanes( mSequencer ) == 1; } ),
            FIsActionChecked(),
            FIsActionButtonVisible::CreateLambda( [this](){ return ShotSequenceTools::GetAttachedPlanes( mSequencer ) <= 1; } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().DetachPlane );

    //---

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().CreateDrawing,
            FExecuteAction::CreateLambda( [this]()
                {
                    TArray<FGuid> plane_bindings;
                    int32 plane_count = ShotSequenceTools::GetAllPlanes( mSequencer, nullptr, &plane_bindings );
                    if( !plane_count )
                        return;
                    ShotSequenceTools::CreateDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, plane_bindings[0] );
                } ),
            FCanExecuteAction::CreateLambda( [this]()
                {
                    TArray<FGuid> plane_bindings;
                    int32 plane_count = ShotSequenceTools::GetAllPlanes( mSequencer, nullptr, &plane_bindings );
                    if( !plane_count )
                        return false;
                    return ShotSequenceTools::CanCreateDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber, plane_bindings[0] );
                } ),
            FIsActionChecked(),
            FIsActionButtonVisible::CreateLambda( [this](){ return ShotSequenceTools::GetAllPlanes( mSequencer ) <= 1; } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().CreateDrawing );

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().GotoPreviousDrawing,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::GotoPreviousDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::HasPreviousDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().GotoPreviousDrawing );

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().GotoNextDrawing,
            FExecuteAction::CreateLambda( [this](){ ShotSequenceTools::GotoNextDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return ShotSequenceTools::HasNextDrawing( mSequencer, mSequencer->GetLocalTime().Time.FrameNumber ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().GotoNextDrawing );

    //---

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().GotoPraxinos,
            FExecuteAction::CreateLambda( [this](){ FPlatformProcess::LaunchURL( TEXT("https://praxinos.coop"), nullptr, nullptr ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().GotoPraxinos );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().GotoForum,
            FExecuteAction::CreateLambda( [this](){ FPlatformProcess::LaunchURL( TEXT("https://praxinos.coop/forum/"), nullptr, nullptr ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().GotoForum );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().GotoUserDocumentation,
            FExecuteAction::CreateLambda( [this](){ FPlatformProcess::LaunchURL( TEXT("https://praxinos.coop/Documentation/Epos/User/html/"), nullptr, nullptr ); } ),
            FCanExecuteAction::CreateLambda( [this](){ return false; } ) // Not available now
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().GotoUserDocumentation );

    if( iMap == kMap )
        CommandList->MapAction(
            FEposSequenceEditorCommands::Get().OpenAboutWindow,
            FExecuteAction::CreateLambda( [this](){ TSharedPtr<SWindow> root = FGlobalTabmanager::Get()->GetRootWindow(); SAboutWindow::Open( root ); } )
        );
    else
        CommandList->UnmapAction( FEposSequenceEditorCommands::Get().OpenAboutWindow );
}

//---

void
FShotSequenceCustomization::ExtendSequencerToolbar( FToolBarBuilder& ToolbarBuilder )
{
    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().CreateCamera );
    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeCameraMenu ),
        LOCTEXT( "CameraOptions", "Options" ),
        LOCTEXT( "CameraOptionsToolTip", "Camera Options" ),
        TAttribute<FSlateIcon>(),
        true );
    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().SnapCameraToViewport );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().CreatePlane );
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
    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().DetachPlane );
    ToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this](){ return ShotSequenceTools::GetAttachedPlanes( mSequencer ) > 1; } )
        ),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakePlaneMenu ),
        FShotSequenceEditorCommands::Get().DetachPlane->GetLabel(),
        FShotSequenceEditorCommands::Get().DetachPlane->GetDescription(),
        FShotSequenceEditorCommands::Get().DetachPlane->GetIcon() );


    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().GotoPreviousDrawing );
    // The 2 following buttons should be exclusive visible:
    // - the first button is displayed when there is only 1 plane (or 0) available
    // - the second button is displayed when there are more than 2 planes available
    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().CreateDrawing );
    ToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this](){ return ShotSequenceTools::GetAllPlanes( mSequencer ) > 1; } )
        ),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeDrawingMenu ),
        FShotSequenceEditorCommands::Get().CreateDrawing->GetLabel(),
        FShotSequenceEditorCommands::Get().CreateDrawing->GetDescription(),
        FShotSequenceEditorCommands::Get().CreateDrawing->GetIcon() );
    ToolbarBuilder.AddToolBarButton( FShotSequenceEditorCommands::Get().GotoNextDrawing );

    ToolbarBuilder.AddSeparator();

    ToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &FShotSequenceCustomization::MakeHelpMenu ),
        LOCTEXT( "Help", "Help" ),
        LOCTEXT( "HelpToolTip", "Help" ),
        FSlateIcon( FEposSequenceEditorStyle::Get()->GetStyleSetName(), "EposSequenceEditor.Help" ) );

    //TSharedRef<SHorizontalBox> Widget = SNew(SHorizontalBox)
    //  +SHorizontalBox::Slot()
    //  .AutoWidth()
    //  .VAlign(VAlign_Center)
    //  [
    //      SNew(STextBlock)
    //      .Text(LOCTEXT("BoundActorClassPicker", "Bound Actor Class"))
        //]
        //+SHorizontalBox::Slot()
        //.AutoWidth()
        //.VAlign(VAlign_Center)
        //[
        //  SNew(SComboButton)
        //  .OnGetMenuContent_Raw(this, &FTemplateSequenceCustomization::GetBoundActorClassMenuContent)
        //  .ButtonContent()
        //  [
        //      SNew(STextBlock)
        //      .Text_Raw(this, &FTemplateSequenceCustomization::GetBoundActorClassName)
        //  ]
        //];

    //ToolbarBuilder.AddWidget(Widget);
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeCameraMenu()
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
            return root_name == GET_MEMBER_NAME_CHECKED( UEposTracksEditorSettings, CameraSettings );
        };
        DetailView->GetIsPropertyVisibleDelegate() = FIsPropertyVisible::CreateLambda( visible_property );
        // Set the object to view
        DetailView->SetObject( GetMutableDefault<UEposTracksEditorSettings>() );

        MenuBuilder.AddWidget( DetailView, FText(), true );
    }
    MenuBuilder.EndSection();

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
                FExecuteAction::CreateLambda( [this, plane](){ ShotSequenceTools::DetachPlane( mSequencer, plane ); } )
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
FShotSequenceCustomization::MakeTextureMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    MenuBuilder.BeginSection( NAME_None, LOCTEXT( "TextureSettingsTitle", "Default Texture Settings" ) );
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
            return root_name == GET_MEMBER_NAME_CHECKED( UEposTracksEditorSettings, TextureSettings );
        };
        DetailView->GetIsPropertyVisibleDelegate() = FIsPropertyVisible::CreateLambda( visible_property );
        // Set the object to view
        DetailView->SetObject( GetMutableDefault<UEposTracksEditorSettings>() );

        MenuBuilder.AddWidget( DetailView, FText(), true );
    }
    MenuBuilder.EndSection();

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
FShotSequenceCustomization::MakeHelpMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer->GetCommandBindings() );

    MenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().GotoPraxinos );
    MenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().GotoForum );

    MenuBuilder.AddSeparator();

    MenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().GotoUserDocumentation );

    MenuBuilder.AddSeparator();

    MenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().OpenAboutWindow );

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
FShotSequenceCustomization::OnSequencerActorsDrop( const TArray<TWeakObjectPtr<AActor>>& iActors, const FActorDragDropGraphEdOp& iDragDropOp )
{
    return ESequencerDropResult::Unhandled; // Process the default behavior for actors
}

#undef LOCTEXT_NAMESPACE

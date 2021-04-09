// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Shot/ShotSequenceCustomization.h"

#include "Settings/EposSequenceEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "ShotHelpers/ShotSequenceHelpers.h"
#include "ShotSequenceEditorCommands.h"

#define LOCTEXT_NAMESPACE "ShotSequenceCustomization"

//---

void
FShotSequenceCustomization::RegisterSequencerCustomization( FSequencerCustomizationBuilder& ioBuilder )
{
    mSequencer = &ioBuilder.GetSequencer();
    mShotSequence = Cast<UShotSequence>( &ioBuilder.GetFocusedSequence() );

    //---

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
            FExecuteAction::CreateLambda( [this]{ ShotSequenceHelpers::CreateCamera( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID() ); } ),
            FCanExecuteAction::CreateLambda( [this]{ return !ShotSequenceHelpers::GetCamera( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), nullptr ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().CreateCamera );

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().SnapCameraToViewport,
            FExecuteAction::CreateLambda( [this]{ ShotSequenceHelpers::SnapCameraToViewport( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID() ); } ),
            // It doesn't work due to strange stuff between FMovieSceneSequenceID and FMovieSceneSequenceIDRef ...
            //FExecuteAction::CreateStatic( &ShotSequenceHelpers::SnapCameraToViewport, mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID() ),
            FCanExecuteAction::CreateLambda( [this]{ return !!ShotSequenceHelpers::GetCamera( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), nullptr ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().SnapCameraToViewport );

    if( iMap == kMap )
        CommandList->MapAction(
            FShotSequenceEditorCommands::Get().CreatePlane,
            FExecuteAction::CreateLambda( [this]{ ShotSequenceHelpers::CreatePlane( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID() ); } ),
            FCanExecuteAction::CreateLambda( [this]{ return !!ShotSequenceHelpers::GetCamera( *mSequencer, mSequencer->GetFocusedMovieSceneSequence(), mSequencer->GetFocusedTemplateID(), nullptr ); } )
        );
    else
        CommandList->UnmapAction( FShotSequenceEditorCommands::Get().CreatePlane );
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

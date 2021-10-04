// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "NoteTrack/NoteTrackEditor.h"

#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "Rendering/DrawElements.h"
#include "SequencerSectionPainter.h"
#include "DragAndDrop/AssetDragDropOp.h"
#include "EditorStyleSet.h"
#include "ISequencerSection.h"
#include "CommonMovieSceneTools.h"
#include "SequencerUtilities.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

#include "NoteTrack/MovieSceneNoteTrack.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "NoteTrack/NoteSection.h"
#include "StoryNote.h"

#define LOCTEXT_NAMESPACE "FNoteTrackEditor"

//---

FNoteTrackEditor::FNoteTrackEditor( TSharedRef<ISequencer> InSequencer )
    : FMovieSceneTrackEditor( InSequencer )
{
}

FNoteTrackEditor::~FNoteTrackEditor()
{
}

TSharedRef<ISequencerTrackEditor> FNoteTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> InSequencer )
{
    return MakeShareable( new FNoteTrackEditor( InSequencer ) );
}

void FNoteTrackEditor::BuildAddTrackMenu( FMenuBuilder& MenuBuilder )
{
    MenuBuilder.AddMenuEntry(
        LOCTEXT( "AddTrack", "Note Track" ),
        LOCTEXT( "AddTooltip", "Adds a new master note track that can play sounds." ),
        FSlateIcon( FEditorStyle::GetStyleSetName(), "Sequencer.Tracks.Note" ),
        FUIAction(
            FExecuteAction::CreateRaw( this, &FNoteTrackEditor::HandleAddNoteTrackMenuEntryExecute )
        )
    );
}

void FNoteTrackEditor::BuildObjectBindingTrackMenu( FMenuBuilder& MenuBuilder, const TArray<FGuid>& ObjectBindings, const UClass* ObjectClass )
{
    //TODO: doesn't work when pressing Enter key for new note

    //if( ObjectClass != nullptr && ObjectClass->IsChildOf( AActor::StaticClass() ) )
    //{
    //    MenuBuilder.AddSubMenu(
    //        LOCTEXT( "AddAttachedNoteTrack", "Note" ),
    //        LOCTEXT( "AddAttachedNoteTooltip", "Adds an note track attached to the object." ),
    //        FNewMenuDelegate::CreateSP( this, &FNoteTrackEditor::HandleAddAttachedNoteTrackMenuEntryExecute, ObjectBindings ) );
    //}
}

bool FNoteTrackEditor::SupportsType( TSubclassOf<UMovieSceneTrack> Type ) const
{
    return Type == UMovieSceneNoteTrack::StaticClass();
}

bool FNoteTrackEditor::SupportsSequence( UMovieSceneSequence* InSequence ) const
{
    ETrackSupport TrackSupported = InSequence ? InSequence->IsTrackSupported( UMovieSceneNoteTrack::StaticClass() ) : ETrackSupport::NotSupported;
    return TrackSupported == ETrackSupport::Supported;
}

void FNoteTrackEditor::BuildTrackContextMenu( FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track )
{
}

const FSlateBrush* FNoteTrackEditor::GetIconBrush() const
{
    return FEditorStyle::GetBrush( "Sequencer.Tracks.Note" );
}

bool FNoteTrackEditor::IsResizable( UMovieSceneTrack* InTrack ) const
{
    return true;
}

void FNoteTrackEditor::Resize( float NewSize, UMovieSceneTrack* InTrack )
{
    UMovieSceneNoteTrack* NoteTrack = Cast<UMovieSceneNoteTrack>( InTrack );
    if( NoteTrack )
    {
        NoteTrack->Modify();

        int32 MaxNumRows = 1;
        for( UMovieSceneSection* Section : NoteTrack->GetAllSections() )
        {
            MaxNumRows = FMath::Max( MaxNumRows, Section->GetRowIndex() + 1 );
        }

        NoteTrack->SetRowHeight( FMath::RoundToInt( NewSize ) / MaxNumRows );
    }
}

bool FNoteTrackEditor::OnAllowDrop( const FDragDropEvent& DragDropEvent, FSequencerDragDropParams& DragDropParams )
{
    if( !DragDropParams.Track->IsA( UMovieSceneNoteTrack::StaticClass() ) )
    {
        return false;
    }

    TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();

    if( !Operation.IsValid() || !Operation->IsOfType<FAssetDragDropOp>() )
    {
        return false;
    }

    TSharedPtr<FAssetDragDropOp> DragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( Operation );

    for( const FAssetData& AssetData : DragDropOp->GetAssets() )
    {
        if( UStoryNote* Note = Cast<UStoryNote>( AssetData.GetAsset() ) )
        {
            FFrameRate TickResolution = GetSequencer()->GetFocusedTickResolution();
            FFrameNumber LengthInFrames = TickResolution.AsFrameNumber( 8.f ); //TODO: but what ? find the corresponding board section ? and its start frame ?
            //FFrameNumber LengthInFrames = TickResolution.AsFrameNumber( Note->GetDuration() );
            DragDropParams.FrameRange = TRange<FFrameNumber>( DragDropParams.FrameNumber, DragDropParams.FrameNumber + LengthInFrames );
            return true;
        }
    }

    return false;
}

FReply FNoteTrackEditor::OnDrop( const FDragDropEvent& DragDropEvent, const FSequencerDragDropParams& DragDropParams )
{
    if( !DragDropParams.Track->IsA( UMovieSceneNoteTrack::StaticClass() ) )
    {
        return FReply::Unhandled();
    }

    TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();

    if( !Operation.IsValid() || !Operation->IsOfType<FAssetDragDropOp>() )
    {
        return FReply::Unhandled();
    }

    UMovieSceneNoteTrack* NoteTrack = Cast<UMovieSceneNoteTrack>( DragDropParams.Track );

    const FScopedTransaction Transaction( LOCTEXT( "DropAssets", "Drop Assets" ) );

    TSharedPtr<FAssetDragDropOp> DragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( Operation );

    FMovieSceneTrackEditor::BeginKeying( DragDropParams.FrameNumber );

    bool bAnyDropped = false;
    for( const FAssetData& AssetData : DragDropOp->GetAssets() )
    {
        UStoryNote* Note = Cast<UStoryNote>( AssetData.GetAsset() );

        if( Note )
        {
            if( DragDropParams.TargetObjectGuid.IsValid() )
            {
                TArray<TWeakObjectPtr<>> OutObjects;
                for( TWeakObjectPtr<> Object : GetSequencer()->FindObjectsInCurrentSequence( DragDropParams.TargetObjectGuid ) )
                {
                    OutObjects.Add( Object );
                }

                AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FNoteTrackEditor::AddNewAttachedNote, Note, NoteTrack, OutObjects ) );
            }
            else
            {
                AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FNoteTrackEditor::AddNewMasterNote, Note, NoteTrack, DragDropParams.RowIndex ) );
            }

            bAnyDropped = true;
        }
    }

    FMovieSceneTrackEditor::EndKeying();

    return bAnyDropped ? FReply::Handled() : FReply::Unhandled();
}

TSharedRef<ISequencerSection> FNoteTrackEditor::MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding )
{
    check( SupportsType( SectionObject.GetOuter()->GetClass() ) );
    return MakeShareable( new FNoteSection( SectionObject, GetSequencer() ) );
}

TSharedPtr<SWidget> FNoteTrackEditor::BuildOutlinerEditWidget( const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params )
{
    // Create a container edit box
    return SNew( SHorizontalBox )

        // Add the note combo box
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign( VAlign_Center )
        [
            FSequencerUtilities::MakeAddButton( LOCTEXT( "NoteText", "Note" ), FOnGetContent::CreateSP( this, &FNoteTrackEditor::BuildNoteSubMenu, FOnAssetSelected::CreateRaw( this, &FNoteTrackEditor::OnNoteAssetSelected, Track ), FOnAssetEnterPressed::CreateRaw( this, &FNoteTrackEditor::OnNoteAssetEnterPressed, Track ), FOnTextCommitted::CreateRaw( this, &FNoteTrackEditor::OnNoteTextCommited, Track ) ), Params.NodeIsHovered, GetSequencer() )
        ];
}

bool FNoteTrackEditor::HandleAssetAdded( UObject* Asset, const FGuid& TargetObjectGuid )
{
    if( Asset->IsA<UStoryNote>() )
    {
        auto Note = Cast<UStoryNote>( Asset );
        UMovieSceneNoteTrack* DummyTrack = nullptr;

        const FScopedTransaction Transaction( LOCTEXT( "AddNote_Transaction", "Add Note" ) );

        if( TargetObjectGuid.IsValid() )
        {
            TArray<TWeakObjectPtr<>> OutObjects;
            for( TWeakObjectPtr<> Object : GetSequencer()->FindObjectsInCurrentSequence( TargetObjectGuid ) )
            {
                OutObjects.Add( Object );
            }

            AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FNoteTrackEditor::AddNewAttachedNote, Note, DummyTrack, OutObjects ) );
        }
        else
        {
            int32 RowIndex = INDEX_NONE;
            AnimatablePropertyChanged( FOnKeyProperty::CreateRaw( this, &FNoteTrackEditor::AddNewMasterNote, Note, DummyTrack, RowIndex ) );
        }

        return true;
    }
    return false;
}

FKeyPropertyResult FNoteTrackEditor::AddNewMasterNote( FFrameNumber KeyTime, UStoryNote* iNote, UMovieSceneNoteTrack* NoteTrack, int32 RowIndex )
{
    FKeyPropertyResult KeyPropertyResult;

    UMovieScene* FocusedMovieScene = GetFocusedMovieScene();
    if( FocusedMovieScene->IsReadOnly() )
    {
        return KeyPropertyResult;
    }

    FocusedMovieScene->Modify();

    FFindOrCreateMasterTrackResult<UMovieSceneNoteTrack> TrackResult;
    TrackResult.Track = NoteTrack;
    if( !NoteTrack )
    {
        TrackResult = FindOrCreateMasterTrack<UMovieSceneNoteTrack>();
        NoteTrack = TrackResult.Track;
    }

    if( ensure( NoteTrack ) )
    {
        NoteTrack->Modify();

        UMovieSceneSection* NewSection = NoteTrack->AddNewNoteOnRow( iNote, KeyTime, RowIndex );

        if( TrackResult.bWasCreated )
        {
            NoteTrack->SetDisplayName( LOCTEXT( "NoteTrackName", "Note" ) );

            if( GetSequencer().IsValid() )
            {
                GetSequencer()->OnAddTrack( NoteTrack, FGuid() );
            }
        }

        KeyPropertyResult.bTrackModified = true;
        KeyPropertyResult.SectionsCreated.Add( NewSection );
    }

    return KeyPropertyResult;
}

FKeyPropertyResult FNoteTrackEditor::AddNewAttachedNote( FFrameNumber KeyTime, UStoryNote* iNote, UMovieSceneNoteTrack* NoteTrack, TArray<TWeakObjectPtr<UObject>> ObjectsToAttachTo )
{
    FKeyPropertyResult KeyPropertyResult;

    for( int32 ObjectIndex = 0; ObjectIndex < ObjectsToAttachTo.Num(); ++ObjectIndex )
    {
        UObject* Object = ObjectsToAttachTo[ObjectIndex].Get();

        FFindOrCreateHandleResult HandleResult = FindOrCreateHandleToObject( Object );
        FGuid ObjectHandle = HandleResult.Handle;
        KeyPropertyResult.bHandleCreated |= HandleResult.bWasCreated;

        if( ObjectHandle.IsValid() )
        {
            FFindOrCreateTrackResult TrackResult;
            TrackResult.Track = NoteTrack;
            if( !NoteTrack )
            {
                TrackResult = FindOrCreateTrackForObject( ObjectHandle, UMovieSceneNoteTrack::StaticClass() );
                NoteTrack = Cast<UMovieSceneNoteTrack>( TrackResult.Track );
            }

            KeyPropertyResult.bTrackCreated |= TrackResult.bWasCreated;

            if( ensure( NoteTrack ) )
            {
                NoteTrack->Modify();

                UMovieSceneSection* NewSection = NoteTrack->AddNewNote( iNote, KeyTime );
                NoteTrack->SetDisplayName( LOCTEXT( "NoteTrackName", "Note" ) );
                KeyPropertyResult.bTrackModified = true;
                KeyPropertyResult.SectionsCreated.Add( NewSection );

                GetSequencer()->EmptySelection();
                GetSequencer()->SelectSection( NewSection );
                GetSequencer()->ThrobSectionSelection();
            }
        }
    }

    return KeyPropertyResult;
}

/* FAudioTrackEditor callbacks
 *****************************************************************************/

void FNoteTrackEditor::HandleAddNoteTrackMenuEntryExecute()
{
    UMovieScene* FocusedMovieScene = GetFocusedMovieScene();

    if( FocusedMovieScene == nullptr )
    {
        return;
    }

    if( FocusedMovieScene->IsReadOnly() )
    {
        return;
    }

    const FScopedTransaction Transaction( NSLOCTEXT( "Sequencer", "AddNoteTrack_Transaction", "Add Note Track" ) );
    FocusedMovieScene->Modify();

    auto NewTrack = FocusedMovieScene->AddMasterTrack<UMovieSceneNoteTrack>();
    ensure( NewTrack );

    NewTrack->SetDisplayName( LOCTEXT( "NoteTrackName", "Note" ) );

    if( GetSequencer().IsValid() )
    {
        GetSequencer()->OnAddTrack( NewTrack, FGuid() );
    }
}

void FNoteTrackEditor::HandleAddAttachedNoteTrackMenuEntryExecute( FMenuBuilder& MenuBuilder, TArray<FGuid> ObjectBindings )
{
    //BuildNoteSubMenu( MenuBuilder, FOnAssetSelected::CreateRaw( this, &FNoteTrackEditor::OnAttachedNoteAssetSelected, ObjectBindings ), FOnAssetEnterPressed::CreateRaw( this, &FNoteTrackEditor::OnAttachedNoteEnterPressed, ObjectBindings ), FOnTextCommitted::CreateRaw( this, &FNoteTrackEditor::OnAttachedNoteTextCommited, ObjectBindings ) );
    MenuBuilder.AddWidget( BuildNoteSubMenu( FOnAssetSelected::CreateRaw( this, &FNoteTrackEditor::OnAttachedNoteAssetSelected, ObjectBindings ), FOnAssetEnterPressed::CreateRaw( this, &FNoteTrackEditor::OnAttachedNoteEnterPressed, ObjectBindings ), FOnTextCommitted::CreateRaw( this, &FNoteTrackEditor::OnAttachedNoteTextCommited, ObjectBindings ) ), FText::GetEmpty(), true );
}

TSharedRef<SWidget> FNoteTrackEditor::BuildNoteSubMenu( FOnAssetSelected OnAssetSelected, FOnAssetEnterPressed OnAssetEnterPressed, FOnTextCommitted OnTextCommited )
{
    FMenuBuilder MenuBuilder( true, nullptr );

    //-

    //MenuBuilder.AddEditableText( LOCTEXT( "text-label", "Text" ), FText::GetEmpty(), FSlateIcon(), FText::GetEmpty(), OnTextCommited );

    //MenuBuilder.AddWidget( SNew( SEditableText )
    //                       .OnTextCommitted( OnTextCommited ),
    //                       LOCTEXT( "text-label", "Text" ) );

    //MenuBuilder.AddWidget( SNew( SEditableTextBox )
    //                       .OnTextCommitted( OnTextCommited ),
    //                       LOCTEXT( "textbox-label", "TextBox" ) );
    //
    //MenuBuilder.AddWidget( SNew( SMultiLineEditableText )
    //                       .OnTextCommitted( OnTextCommited ),
    //                       LOCTEXT( "multitext-label", "MultiText" ) );

    // D:\Epic Games\UE_4.27\Engine\Source\Developer\OutputLog\Private\SOutputLog.cpp

    TSharedPtr<SBox> MenuEntry = SNew( SBox )
        .MinDesiredWidth( 400 )
        .MinDesiredHeight( 50 )
        [
            SNew( SMultiLineEditableTextBox )
            .ModiferKeyForNewLine( EModifierKey::Shift )
            .OnTextCommitted( OnTextCommited )
        ];

    MenuBuilder.AddWidget( MenuEntry.ToSharedRef(), LOCTEXT( "menu-new-note-label", "New Note Text" ) );

    //-

    auto OnExistingNotes = [=]( FMenuBuilder& iMenuBuilder )
    {
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );
        TArray<FName> ClassNames;
        ClassNames.Add( UStoryNote::StaticClass()->GetFName() );
        TSet<FName> DerivedClassNames;
        AssetRegistryModule.Get().GetDerivedClassNames( ClassNames, TSet<FName>(), DerivedClassNames );

        FAssetPickerConfig AssetPickerConfig;
        {
            AssetPickerConfig.OnAssetSelected = OnAssetSelected;
            AssetPickerConfig.OnAssetEnterPressed = OnAssetEnterPressed;
            AssetPickerConfig.bAllowNullSelection = false;
            AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
            for( auto ClassName : DerivedClassNames )
            {
                AssetPickerConfig.Filter.ClassNames.Add( ClassName );
            }
        }

        FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );

        TSharedPtr<SBox> MenuEntry = SNew( SBox )
            .WidthOverride( 300.0f )
            .HeightOverride( 300.f )
            [
                ContentBrowserModule.Get().CreateAssetPicker( AssetPickerConfig )
            ];

        iMenuBuilder.AddWidget( MenuEntry.ToSharedRef(), FText::GetEmpty(), true );
    };

    MenuBuilder.AddSubMenu( LOCTEXT( "submenu-existing-notes-label", "Existing Notes" ), LOCTEXT( "submenu-existing-notes-tooltip", "Link to one of the existing notes" ), FNewMenuDelegate::CreateLambda( OnExistingNotes ) );

    return MenuBuilder.MakeWidget();
}

void FNoteTrackEditor::OnNoteTextCommited( const FText& iText, ETextCommit::Type iType, UMovieSceneTrack* Track )
{
    if( iType != ETextCommit::OnEnter )
        return;

    const FScopedTransaction Transaction( NSLOCTEXT( "Sequencer", "AddNote_Transaction", "Add Note" ) );

    auto NoteTrack = Cast<UMovieSceneNoteTrack>( Track );
    NoteTrack->Modify();

    //TODO:
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );

    UPackage* package = GetSequencer()->GetRootMovieSceneSequence()->GetPackage();
    FString package_pathname = package->GetName(); // ie. /Game/MyStoryboard2
    FString package_name = FPaths::GetBaseFilename( GetSequencer()->GetFocusedMovieSceneSequence()->GetPackage()->GetName() ); // ie. shot_0002_01

    FString note_package_name;
    FString note_asset_name;
    assetToolsModule.Get().CreateUniqueAssetName( FPaths::Combine( package_pathname, TEXT("Notes"), package_name ), "_N_01", note_package_name, note_asset_name );

    FString package_path = FPackageName::GetLongPackagePath( note_package_name );
    UObject* new_object = assetToolsModule.Get().CreateAsset( note_asset_name, package_path, UStoryNote::StaticClass(), nullptr );
    UStoryNote* new_note = Cast<UStoryNote>( new_object );
    check( new_note );

    new_note->Text = iText.ToString();
    //

    FFrameTime KeyTime = GetSequencer()->GetLocalTime().Time;
    UMovieSceneSection* NewSection = NoteTrack->AddNewNote( new_note, KeyTime.FrameNumber );

    GetSequencer()->EmptySelection();
    GetSequencer()->SelectSection( NewSection );
    GetSequencer()->ThrobSectionSelection();

    GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

void FNoteTrackEditor::OnNoteAssetSelected( const FAssetData& AssetData, UMovieSceneTrack* Track )
{
    FSlateApplication::Get().DismissAllMenus();

    UObject* SelectedObject = AssetData.GetAsset();

    if( SelectedObject )
    {
        UStoryNote* NewNote = CastChecked<UStoryNote>( AssetData.GetAsset() );
        if( NewNote != nullptr )
        {
            const FScopedTransaction Transaction( NSLOCTEXT( "Sequencer", "AddNote_Transaction", "Add Note" ) );

            auto NoteTrack = Cast<UMovieSceneNoteTrack>( Track );
            NoteTrack->Modify();

            FFrameTime KeyTime = GetSequencer()->GetLocalTime().Time;
            UMovieSceneSection* NewSection = NoteTrack->AddNewNote( NewNote, KeyTime.FrameNumber );

            GetSequencer()->EmptySelection();
            GetSequencer()->SelectSection( NewSection );
            GetSequencer()->ThrobSectionSelection();

            GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
        }
    }
}

void FNoteTrackEditor::OnNoteAssetEnterPressed( const TArray<FAssetData>& AssetData, UMovieSceneTrack* Track )
{
    if( AssetData.Num() > 0 )
    {
        OnNoteAssetSelected( AssetData[0].GetAsset(), Track );
    }
}

void FNoteTrackEditor::OnAttachedNoteAssetSelected( const FAssetData& AssetData, TArray<FGuid> ObjectBindings )
{
    FSlateApplication::Get().DismissAllMenus();

    UObject* SelectedObject = AssetData.GetAsset();

    if( SelectedObject )
    {
        const FScopedTransaction Transaction( NSLOCTEXT( "Sequencer", "AddNote_Transaction", "Add Note" ) );

        for( FGuid ObjectBinding : ObjectBindings )
        {
            HandleAssetAdded( SelectedObject, ObjectBinding );
        }
    }
}

void FNoteTrackEditor::OnAttachedNoteEnterPressed( const TArray<FAssetData>& AssetData, TArray<FGuid> ObjectBindings )
{
    if( AssetData.Num() > 0 )
    {
        OnAttachedNoteAssetSelected( AssetData[0].GetAsset(), ObjectBindings );
    }
}

void FNoteTrackEditor::OnAttachedNoteTextCommited( const FText& iText, ETextCommit::Type iType, TArray<FGuid> ObjectBindings )
{
    //const FScopedTransaction Transaction( NSLOCTEXT( "Sequencer", "AddNote_Transaction", "Add Note" ) );

    //auto NoteTrack = Cast<UMovieSceneNoteTrack>( Track );
    //NoteTrack->Modify();

    //TODO: create note asset here ?

    //FFrameTime KeyTime = GetSequencer()->GetLocalTime().Time;
    //UMovieSceneSection* NewSection = NoteTrack->AddNewNote( iText.ToString(), KeyTime.FrameNumber );

    //GetSequencer()->EmptySelection();
    //GetSequencer()->SelectSection( NewSection );
    //GetSequencer()->ThrobSectionSelection();

    //GetSequencer()->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
}

#undef LOCTEXT_NAMESPACE

// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorTopTab.h"

#include "Models/OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyBrushOptions.h"
#include "OdysseyBlendParameters.h"

#include "FileHelpers.h"
#include "ObjectEditorUtils.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorTopTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_TopBar"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

class FOdysseyPainterEditor;

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorTopTab::~FOdysseyPainterEditorTopTab()
{
    mEditor->OnCurrentToolChanged().RemoveAll(this);
}

FOdysseyPainterEditorTopTab::FOdysseyPainterEditorTopTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "top-tab.name", "Top Bar" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Spark16" ))
    , mEditor( iEditor )
    , mToolWidgetSlot(nullptr)
    , mCurrentTool(nullptr)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Spawner callback

void
FOdysseyPainterEditorTopTab::Init()
{
    mEditor->OnCurrentToolChanged().AddSP(SharedThis<FOdysseyPainterEditorTopTab>(this), &FOdysseyPainterEditorTopTab::OnCurrentToolChanged);
    FOdysseyEditorTab::Init();
}

TSharedRef< SDockTab >
FOdysseyPainterEditorTopTab::SpawnTab( const FSpawnTabArgs& iArgs )
{
    return SNew( SDockTab )
        .Label( DisplayName() )
        .ShouldAutosize( true )
        [
            Widget().ToSharedRef()
        ];
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ Public Getter

bool
FOdysseyPainterEditorTopTab::IsPackageEdited() const
{
    UObject* editedObject = mEditor->GetEditedObject();
    if (editedObject && editedObject->GetOutermost()->IsDirty())
        return true;

    TArray<UObject*> additionalEditedObjects = mEditor->GetAdditionalEditedObjects();
    for( UObject* additionalEditedObject : additionalEditedObjects )
    {
        if( additionalEditedObject->GetOutermost()->IsDirty() )
            return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorTopTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorTopTab::CreateWidget()
{
    TSharedPtr<SWidget> widget = SAssignNew( mWrapBox, SWrapBox )
        .UseAllottedSize(true)
        .InnerSlotPadding(FVector2D(2.f, 2.f))
        +SWrapBox::Slot()
        [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("top-tab.save", "Save the edited asset.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked( this, &FOdysseyPainterEditorTopTab::OnSaveButtonClicked )
            .IsEnabled( this, &FOdysseyPainterEditorTopTab::IsPackageEdited )
            [
                SNew( SImage )
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Save32" ) )
            ]
        ]
        + SWrapBox::Slot()
        [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("top-tab.undo", "Undo the previous action.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked(this, &FOdysseyPainterEditorTopTab::OnUndoButtonClicked )
            [
                SNew( SImage )
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Undo32" ) )
            ]
        ]
        + SWrapBox::Slot()
        .Padding( 0.f, 0.f, 30.f, 0.f )
        [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("top-tab.redo", "Redo the next action.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked(this, &FOdysseyPainterEditorTopTab::OnRedoButtonClicked )
            [
                SNew( SImage )
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Redo32" ) )
            ]
        ]

        + SWrapBox::Slot()
        .Padding( 0.f, 0.f, 30.f, 0.f )
        [
            SNew( SButton )
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .ToolTipText( LOCTEXT("top-tab.clear", "Clear the whole canvas.") )
            .VAlign( VAlign_Center )
            .ContentPadding( FMargin( 0.0, 0.0 ) )
            .OnClicked(this, &FOdysseyPainterEditorTopTab::OnClearButtonClicked )
            [
                SNew( SImage )
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.TopBar.Clear32" ) )
            ]
        ]

        + SWrapBox::Slot()
        .FillEmptySpace(true)
        .VAlign(VAlign_Center)
        .Expose(mToolWidgetSlot)
        [
            SNullWidget::NullWidget
        ];

    UpdateToolWidget();
    return widget;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorTopTab::UpdateToolWidget()
{
    //Clear the tool widget content
    //mToolWidgetSlot->DetachWidget();
    if (mCurrentTool == mEditor->GetCurrentTool())
        return; //Fix jumping UI when moving in animation timeline

    mCurrentTool = mEditor->GetCurrentTool();
    if(!mCurrentTool)
    {
        mToolWidgetSlot->AttachWidget(SNullWidget::NullWidget);
        return;
    }

    mToolWidgetSlot->AttachWidget(mCurrentTool->GetTopTabWidget());
}

void
FOdysseyPainterEditorTopTab::OnCurrentToolChanged()
{
    UpdateToolWidget();
}

FReply
FOdysseyPainterEditorTopTab::OnSaveButtonClicked()
{
    TArray<UPackage*> packages;
    UObject* editedObject = mEditor->GetEditedObject();
    if (editedObject)
        packages.Add(editedObject->GetOutermost());

    TArray<UObject*> additionalEditedObjects = mEditor->GetAdditionalEditedObjects();
    for( UObject* additionalEditedObject : additionalEditedObjects )
    {
        packages.Add( additionalEditedObject->GetOutermost() );
    }

    FEditorFileUtils::PromptForCheckoutAndSave(packages, true, false);

    return FReply::Handled();
}

FReply
FOdysseyPainterEditorTopTab::OnUndoButtonClicked()
{
    GEditor->UndoTransaction(true);
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorTopTab::OnRedoButtonClicked()
{
    GEditor->RedoTransaction();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorTopTab::OnClearButtonClicked()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source)
        return FReply::Unhandled();

    source->Clear();
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners


#undef LOCTEXT_NAMESPACE

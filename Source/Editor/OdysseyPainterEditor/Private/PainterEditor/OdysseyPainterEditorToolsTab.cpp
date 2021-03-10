// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorToolsTab.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolsTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolsTab::~FOdysseyPainterEditorToolsTab()
{
}

FOdysseyPainterEditorToolsTab::FOdysseyPainterEditorToolsTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_Tools"),
                            LOCTEXT( "OdysseyPainterEditorToolsTab", "Tools" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorToolsTab::CreateWidget()
{
	return SNew( SScrollBox )
		.Orientation( Orient_Vertical )
		.ScrollBarAlwaysVisible( false )
		+SScrollBox::Slot()
		[
			SNew( SExpandableArea )
			.HeaderContent()
			[
				SNew( STextBlock )
				.Text( LOCTEXT( "Utils", "Utils" ) )
				.Font( FEditorStyle::GetFontStyle( "DetailsView.CategoryFontStyle" ) )
				.ShadowOffset( FVector2D( 1.0f, 1.0f ) )
			]
			.BodyContent()
			[
				SNew( SWrapBox )
				.UseAllottedWidth( true )
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					.OnClicked_Raw(this, &FOdysseyPainterEditorToolsTab::OnClear)
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Shredder32" ) )
					]
				]
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					.OnClicked_Raw(this, &FOdysseyPainterEditorToolsTab::OnFill)
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket32" ) )
					]
				]
			]
		]
		+SScrollBox::Slot()
		[
			SNew( SExpandableArea )
			.HeaderContent()
			[
				SNew( STextBlock )
				.Text( LOCTEXT( "UndoRedo", "UndoRedo" ) )
				.Font( FEditorStyle::GetFontStyle( "DetailsView.CategoryFontStyle" ) )
				.ShadowOffset( FVector2D( 1.0f, 1.0f ) )
			]
			.BodyContent()
			[
				SNew( SWrapBox )
				.UseAllottedWidth( true )
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					.OnClicked_Raw(this, &FOdysseyPainterEditorToolsTab::OnUndo)
					[
						SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Undo32"))
					]
				]
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					.OnClicked_Raw(this, &FOdysseyPainterEditorToolsTab::OnRedo)
					[
						SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Redo32"))
					]
				]
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.Text( LOCTEXT( "Clear Undo History", "Clear Undo History" ) )
					.ToolTipText( LOCTEXT( "Clear Undos tooltip", "If the undo/redo is slow, clear the cache by clicking this button" ))
                    .VAlign(EVerticalAlignment::VAlign_Center)
					.OnClicked_Raw(this, &FOdysseyPainterEditorToolsTab::OnClearUndo)
				]
			]
		];
}

void
FOdysseyPainterEditorToolsTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolsTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(painterEditorCommands.FillCurrentLayer, Fill )
	MAP_ACTION(painterEditorCommands.ClearCurrentLayer, Clear )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

FReply
FOdysseyPainterEditorToolsTab::OnClear()
{
    Clear();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorToolsTab::OnFill()
{
    Fill();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorToolsTab::OnUndo()
{
    mEditor->Undo();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorToolsTab::OnRedo()
{
    mEditor->Redo();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorToolsTab::OnClearUndo()
{
    mEditor->ClearUndo();
    return FReply::Handled();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditorToolsTab::Clear()
{
    //TODO: PaintEngine->Clear() should generate its own undo 
    mEditor->PaintEngine()->Clear();
}

void
FOdysseyPainterEditorToolsTab::Fill()
{
    //TODO: PaintEngine->Fill() should generate its own undo 
	mEditor->PaintEngine()->Fill();
}

#undef LOCTEXT_NAMESPACE

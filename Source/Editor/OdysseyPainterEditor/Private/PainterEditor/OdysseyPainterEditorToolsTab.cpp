// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorToolsTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyTool.h"
#include "OdysseyToolFreeHand.h"
#include "OdysseyToolPolygon.h"
#include "OdysseyToolCircle.h"
#include "OdysseyToolEllipse.h"
#include "OdysseyToolBezier.h"
#include "OdysseyToolRectangle.h"
#include "OdysseyToolLine.h"

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
//---------------------------------------------------------- FOdysseyEditorTab interface

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
                .Text( LOCTEXT( "Tools", "Tools" ) )
                .Font( FEditorStyle::GetFontStyle( "DetailsView.CategoryFontStyle" ) )
                .ShadowOffset( FVector2D( 1.0f, 1.0f ) )
            ]
            .BodyContent()
            [
                SNew( SWrapBox )
                .UseAllottedWidth( true )
                +SWrapBox::Slot()
                [
                    SNew( SCheckBox )
                    .Style( &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.TransparentCheckBox") )
                    .OnCheckStateChanged( this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, kTool_FreeHand )
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return Cast<UOdysseyToolFreeHand>(mEditor->GetSelectedTool()) != nullptr ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    [
                        SNew( SImage )
                        .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.FreeHand32" ) )
                    ]
                ]
                + SWrapBox::Slot()
                [
                    SNew( SCheckBox )
                    .Style(&FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.TransparentCheckBox"))
                    .OnCheckStateChanged(this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, kTool_Line )
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return Cast<UOdysseyToolLine>(mEditor->GetSelectedTool()) != nullptr ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    [
                        SNew( SImage )
                        .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Line32" ) )
                    ]
                ]
                + SWrapBox::Slot()
                [
                    SNew( SCheckBox )
                    .Style(&FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.TransparentCheckBox"))
                    .OnCheckStateChanged(this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, kTool_Rectangle)
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return Cast<UOdysseyToolRectangle>(mEditor->GetSelectedTool()) != nullptr ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    [
                        SNew( SImage )
                        .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Square32" ) )
                    ]
                ]
                +SWrapBox::Slot()
                [
                    SNew( SCheckBox )
                    .Style(&FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.TransparentCheckBox"))
                    .OnCheckStateChanged(this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, kTool_Polygon)
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return Cast<UOdysseyToolPolygon>(mEditor->GetSelectedTool()) != nullptr ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    [
                        SNew(SImage)
                        .Image(FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Polygon32" ))
                    ]
                ]
                +SWrapBox::Slot()
                [
                    SNew( SCheckBox )
                    .Style(&FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.TransparentCheckBox"))
                    .OnCheckStateChanged(this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, kTool_Circle)
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return Cast<UOdysseyToolCircle>(mEditor->GetSelectedTool()) != nullptr ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    [
                        SNew(SImage)
                        .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Circle32"))
                    ]
                ]
                +SWrapBox::Slot()
                [
                    SNew( SCheckBox )
                    .Style(&FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.TransparentCheckBox"))
                    .OnCheckStateChanged(this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, kTool_Ellipse)
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return Cast<UOdysseyToolEllipse>(mEditor->GetSelectedTool()) != nullptr ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    [
                        SNew(SImage)
                        .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Ellipse32"))
                    ]
                ]
                +SWrapBox::Slot()
                [
                    SNew( SCheckBox )
                    .Style(&FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.TransparentCheckBox"))
                    .OnCheckStateChanged(this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, kTool_Bezier)
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return Cast<UOdysseyToolBezier>(mEditor->GetSelectedTool()) != nullptr ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    [
                        SNew(SImage)
                        .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Bezier32"))
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
FOdysseyPainterEditorToolsTab::OnClearUndo()
{
    mEditor->ClearUndo();
    return FReply::Handled();
}

void 
FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked(ECheckBoxState iCheckBoxState, eToolType iToolType)
{
    //mEditor->SetSelectedTool( iTool );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditorToolsTab::Clear()
{
	//Do the fill
	::ULIS::FBlock* paintBlock = mEditor->PaintEngine().PaintBlock();
	::ULIS::FColor color = ::ULIS::FColor::Black;
	::ULIS::FRectI rect = paintBlock->Rect();
	::ULIS::eFormat format = paintBlock->Format();

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
	ctx.Fill(*paintBlock, color);
	ctx.Finish();

	paintBlock->Dirty();
	mEditor->PaintEngine().Commit(FOdysseyBlendParameters(true, EOdysseyBlendingMode::kNormal, EOdysseyAlphaMode::kNormal, 100.0f));
}

void
FOdysseyPainterEditorToolsTab::Fill()
{
	//Do the fill
	::ULIS::FBlock* paintBlock = mEditor->PaintEngine().PaintBlock();
	::ULIS::FColor color = mEditor->PaintColor().GetValue();
	::ULIS::FRectI rect = paintBlock->Rect();
	::ULIS::eFormat format = paintBlock->Format();

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
	ctx.Fill(*paintBlock, color);
	ctx.Finish();

	paintBlock->Dirty();
	mEditor->PaintEngine().Commit(FOdysseyBlendParameters());
}

#undef LOCTEXT_NAMESPACE

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorToolsTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
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
                    .OnCheckStateChanged( this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, eGUISelectedTool::kBrush )
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return mEditor->GetGUISelectedTool() == eGUISelectedTool::kBrush ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    [
                        SNew( SImage )
                        .Image( FOdysseyStyle::GetBrush( "TODO Paint Brush Icon" ) )
                    ]
                ]
                +SWrapBox::Slot()
                [
                    SNew( SCheckBox )
                    .Style(&FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.TransparentCheckBox"))
                    .OnCheckStateChanged(this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, eGUISelectedTool::kLine )
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return mEditor->GetGUISelectedTool() == eGUISelectedTool::kLine ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    [
                        SNew( SImage )
                        .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Line32" ) )
                    ]
                ]
                +SWrapBox::Slot()
                [
                    SNew( SCheckBox )
                    .Style(&FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.TransparentCheckBox"))
                    .OnCheckStateChanged(this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, eGUISelectedTool::kCircle )
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return mEditor->GetGUISelectedTool() == eGUISelectedTool::kCircle ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
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
                    .OnCheckStateChanged(this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, eGUISelectedTool::kEllipse )
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return mEditor->GetGUISelectedTool() == eGUISelectedTool::kEllipse ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
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
                    .OnCheckStateChanged(this, &FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked, eGUISelectedTool::kPolygon)
                    .IsChecked_Lambda([&]() -> ECheckBoxState
                    {
                        return mEditor->GetGUISelectedTool() == eGUISelectedTool::kPolygon ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    })
                    [
                        SNew(SImage)
                        .Image(FOdysseyStyle::GetBrush("TODO PainterEditor.ToolsTab.Polygon32"))
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
FOdysseyPainterEditorToolsTab::OnToolCheckBoxClicked(ECheckBoxState iCheckBoxState, eGUISelectedTool iTool)
{
    mEditor->SetGUISelectedTool( iTool );
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

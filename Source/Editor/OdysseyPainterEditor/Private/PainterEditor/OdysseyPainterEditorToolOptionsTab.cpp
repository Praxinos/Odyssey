// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorToolOptionsTab.h"

#include "Tools/Widgets/SOdysseyToolOptions.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolOptionsTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolOptionsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolOptionsTab::~FOdysseyPainterEditorToolOptionsTab()
{
}

FOdysseyPainterEditorToolOptionsTab::FOdysseyPainterEditorToolOptionsTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_ToolOptions"),
                            LOCTEXT( "OdysseyPainterEditorToolOptionsTab", "Tool Options" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.BrushSelector16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorToolOptionsTab::CreateWidget()
{
    return SNew(SOdysseyToolOptions)
            .Tool(this, &FOdysseyPainterEditorToolOptionsTab::GetSelectedTool);
        /* + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SOdysseyBrushSelector )
                .Brush( this, &FOdysseyPainterEditorToolOptionsTab::Brush )
                .OnBrushChanged_Raw( this, &FOdysseyPainterEditorToolOptionsTab::OnBrushSelected )
            ]
        + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SNew( SOdysseyBrushExposedParameters )
                .BrushInstance( this, &FOdysseyPainterEditorToolOptionsTab::BrushInstance )
                .OnParameterChanged_Raw( this, &FOdysseyPainterEditorToolOptionsTab::OnParameterChanged )
            ];*/
}

void
FOdysseyPainterEditorToolOptionsTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolOptionsTab::__VA_ARGS__ ), FCanExecuteAction() );

	//MAP_ACTION(painterEditorCommands.RefreshBrush, RefreshBrush )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyTool*
FOdysseyPainterEditorToolOptionsTab::GetSelectedTool() const
{
    return mEditor->GetSelectedTool();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters


#undef LOCTEXT_NAMESPACE

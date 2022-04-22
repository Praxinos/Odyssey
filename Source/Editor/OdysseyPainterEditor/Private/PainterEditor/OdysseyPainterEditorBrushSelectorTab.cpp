// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorBrushSelectorTab.h"
#include "Tools/DrawingTool/Widgets/SOdysseyDrawingToolBrushSelector.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditor.h"
#include "SOdysseyPaintModifiers.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include "Tools/DrawingTool/OdysseyDrawingTool.h"
#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorBrushSelectorTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorBrushSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorBrushSelectorTab::~FOdysseyPainterEditorBrushSelectorTab()
{
}

FOdysseyPainterEditorBrushSelectorTab::FOdysseyPainterEditorBrushSelectorTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_BrushSelector"),
                            LOCTEXT( "OdysseyPainterEditorBrushSelectorTab", "Brush Selector" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.BrushSelector16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorBrushSelectorTab::CreateWidget()
{
    return SNew(SOdysseyDrawingToolBrushSelector)
        .Tool(Cast<UOdysseyDrawingTool>(mEditor->GetSelectedTool())); //TODO: Replace by the DrawingTool instead of the SelctedTool
}

void
FOdysseyPainterEditorBrushSelectorTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorBrushSelectorTab::__VA_ARGS__ ), FCanExecuteAction() );

	MAP_ACTION(painterEditorCommands.RefreshBrush, RefreshBrush )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditorBrushSelectorTab::RefreshBrush()
{
    UOdysseyDrawingTool* drawingTool = Cast<UOdysseyDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return;
    drawingTool->RefreshBrushInstance();
}

#undef LOCTEXT_NAMESPACE

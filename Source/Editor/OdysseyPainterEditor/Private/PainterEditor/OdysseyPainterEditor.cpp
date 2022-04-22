// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditor.h"

#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditorTopTab.h"
#include "SOdysseyPaintModifiers.h"
#include "OdysseyHUDSystem.h"
#include "IOdysseyTool.h"
#include "OdysseyToolFreeHand.h"
#include "ULISLoaderModule.h"
#include "OdysseyPainterEditorGUI.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyPainterEditor::~FOdysseyPainterEditor()
{
    delete mPaintEngine;
}

FOdysseyPainterEditor::FOdysseyPainterEditor()
    : mPaintEngine( new FOdysseyPaintEngine() )
	, mSelectedTool( NewObject<UOdysseyToolFreeHand>() )
    , mHUDSystem(new FOdysseyHUDSystem())
	, mPaintColor( ::ULIS::FColor::RGBA8( 0, 0, 0 ) )
    , mDrawBrushPreview( true )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyPainterEditor::InitData()
{
    //Set Default Brush
    const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
    FOdysseyPainterEditorDrawingState* drawingState = new FOdysseyPainterEditorDrawingState(this);

    mPaintEngine->AddDrawingState(drawingState);
    mPaintEngine->Brush(settings.BrushDefaults.DefaultBrush);
}

void
FOdysseyPainterEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyEditor::BindShortcuts(iToolkit);

	//---

	const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

	#define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateRaw( this, &FOdysseyPainterEditor::__VA_ARGS__ ), FCanExecuteAction() );

	MAP_ACTION(painterEditorCommands.Undo, Undo )
	MAP_ACTION(painterEditorCommands.Redo, Redo )
    MAP_ACTION(painterEditorCommands.ClearUndo, ClearUndo )

	#undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Undo

void
FOdysseyPainterEditor::Undo()
{
	//End stroke before undoing, allows to manage PaintEngine->OnTick Undo
	PaintEngine()->Flush();
}

void
FOdysseyPainterEditor::Redo()
{
	//End stroke before redoing, allows to manage PaintEngine->OnTick Redo
	PaintEngine()->Flush();
}

void
FOdysseyPainterEditor::ClearUndo()
{
	//End stroke before undoing, just to be perfectly clean
	PaintEngine()->Flush();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyPaintEngine*
FOdysseyPainterEditor::PaintEngine() const
{
    return mPaintEngine;
}

FOdysseyHUDSystem* 
FOdysseyPainterEditor::HUDSystem() const
{
	return mHUDSystem;
}

FOdysseyUndoHistory*
FOdysseyPainterEditor::UndoHistory() const
{
	return mUndoHistory;
}

bool
FOdysseyPainterEditor::DrawBrushPreview() const
{
	return mDrawBrushPreview;
}

::ULIS::FColor
FOdysseyPainterEditor::PaintColor() const
{
	return mPaintColor;
}

IOdysseyTool*
FOdysseyPainterEditor::GetSelectedTool() const
{
    return mSelectedTool;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyPainterEditor::DrawBrushPreview(bool iDrawBrushPreview)
{
	mDrawBrushPreview = iDrawBrushPreview;
}

void
FOdysseyPainterEditor::PaintColor(::ULIS::FColor iColor)
{
	mPaintColor = iColor;
}

void
FOdysseyPainterEditor::SetSelectedTool(IOdysseyTool* iSelectedTool)
{
    mSelectedTool = iSelectedTool;
}


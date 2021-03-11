// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditor.h"

#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditorTopTab.h"
#include "SOdysseyPaintModifiers.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyPainterEditor::~FOdysseyPainterEditor()
{
    delete mPaintEngine;
    delete mUndoHistory;
}

FOdysseyPainterEditor::FOdysseyPainterEditor()
    : mUndoHistory( new FOdysseyUndoHistory() )
    , mPaintEngine( new FOdysseyPaintEngine(mUndoHistory) )
	, mPaintColor( ::ul3::FPixelValue::FromRGBA8( 0, 0, 0 ) )
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
FOdysseyPainterEditor::PaintEngine()
{
    return mPaintEngine;
}

FOdysseyUndoHistory*
FOdysseyPainterEditor::UndoHistory()
{
	return mUndoHistory;
}

bool
FOdysseyPainterEditor::DrawBrushPreview()
{
	return mDrawBrushPreview;
}

::ul3::FPixelValue
FOdysseyPainterEditor::PaintColor() const
{
	return mPaintColor;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyPainterEditor::DrawBrushPreview(bool iDrawBrushPreview)
{
	mDrawBrushPreview = iDrawBrushPreview;
}

void
FOdysseyPainterEditor::PaintColor(::ul3::FPixelValue iColor)
{
	mPaintColor = iColor;
}

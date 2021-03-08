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

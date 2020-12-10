// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorData.h"

#include "ULISLoaderModule.h"


/////////////////////////////////////////////////////
// FOdysseyPainterEditorData
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorData::~FOdysseyPainterEditorData()
{
	if (mBrushInstance)
	{
		mBrushInstance->RemoveFromRoot();
		mBrushInstance = NULL;
	}

    delete mPaintEngine;
    delete mUndoHistory;
}

FOdysseyPainterEditorData::FOdysseyPainterEditorData()
    : mUndoHistory( new FOdysseyUndoHistory() )
    , mPaintEngine( new FOdysseyPaintEngine(mUndoHistory) )
    , mBrush( NULL )
    , mBrushInstance( NULL )
	, mPaintColor( ::ul3::FPixelValue::FromRGBA8( 0, 0, 0 ) )
    , mDrawBrushPreview( true )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyPainterEditorData::Init()
{
    // Setup Paint Engine
	mPaintEngine->Block( NULL );
    mPaintEngine->SetBrushInstance( NULL );
    mPaintEngine->SetColor( mPaintColor );
    mPaintEngine->SetSizeModifier( 20.f );
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

FOdysseyPaintEngine*
FOdysseyPainterEditorData::PaintEngine()
{
    return mPaintEngine;
}

FOdysseyUndoHistory*
FOdysseyPainterEditorData::UndoHistory()
{
	return mUndoHistory;
}

UOdysseyBrush*
FOdysseyPainterEditorData::Brush()
{
	return mBrush;
}

UOdysseyBrushAssetBase*
FOdysseyPainterEditorData::BrushInstance()
{
	return mBrushInstance;
}

bool
FOdysseyPainterEditorData::DrawBrushPreview()
{
	return mDrawBrushPreview;
}

::ul3::FPixelValue
FOdysseyPainterEditorData::PaintColor() const
{
	return mPaintColor;
}

void
FOdysseyPainterEditorData::Brush(UOdysseyBrush* iBrush)
{
	mBrush = iBrush;
}

void
FOdysseyPainterEditorData::BrushInstance(UOdysseyBrushAssetBase* iBrushInstance)
{
	mBrushInstance = iBrushInstance;
}

void
FOdysseyPainterEditorData::DrawBrushPreview(bool iDrawBrushPreview)
{
	mDrawBrushPreview = iDrawBrushPreview;
}

void
FOdysseyPainterEditorData::PaintColor(::ul3::FPixelValue iColor)
{
	mPaintColor = iColor;
}

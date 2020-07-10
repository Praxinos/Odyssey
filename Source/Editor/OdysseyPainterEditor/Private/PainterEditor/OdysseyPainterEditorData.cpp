// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
		mBrushInstance->CleansePool(ECacheLevel::kSuper);
		mBrushInstance->CleansePools();
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
    mPaintEngine->SetColor( ::ul3::FPixelValue::FromRGBA8( 0, 0, 0 ) );
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

// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditor.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyPainterEditor::~FOdysseyPainterEditor()
{
    delete mPaintEngine;
    delete mUndoHistory;
}

FOdysseyPainterEditor::FOdysseyPainterEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit)
    : mToolkit(iToolkit)
    , mUndoHistory( new FOdysseyUndoHistory() )
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
FOdysseyPainterEditor::Init()
{
    // Setup Paint Engine
	mPaintEngine->Block( NULL );
    mPaintEngine->SetBrushInstance( NULL );
    mPaintEngine->SetColor( mPaintColor );
    mPaintEngine->SetSizeModifier( 20.f );
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

UOdysseyBrush*
FOdysseyPainterEditor::Brush()
{
	return mBrush;
}

UOdysseyBrushAssetBase*
FOdysseyPainterEditor::BrushInstance()
{
	return mBrushInstance;
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

TSharedPtr<FOdysseyPainterEditorToolkit>
FOdysseyPainterEditor::Toolkit()
{
    return mToolkit.Pin();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyPainterEditor::Brush(UOdysseyBrush* iBrush)
{
	mBrush = iBrush;
}

void
FOdysseyPainterEditor::BrushInstance(UOdysseyBrushAssetBase* iBrushInstance)
{
	mBrushInstance = iBrushInstance;
}

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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditor::OnToolkitInitialized()
{
    GetGUI()->OnToolkitInitialized();
}

bool
FOdysseyPainterEditor::OnCloseRequested()
{
	PaintEngine()->Flush();
    return true;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ FGCObject interface

void FOdysseyPainterEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
    if (mBrushInstance)
        Collector.AddReferencedObject(mBrushInstance);
        
    if (mBrush)
        Collector.AddReferencedObject(mBrush);
}

FString FOdysseyPainterEditor::GetReferencerName() const
{
	return TEXT("FOdysseyPainterEditor");
}

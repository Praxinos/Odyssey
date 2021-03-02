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

FOdysseyPainterEditor::FOdysseyPainterEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit)
    : mToolkit(iToolkit)
    , mUndoHistory( new FOdysseyUndoHistory() )
    , mPaintEngine( new FOdysseyPaintEngine(mUndoHistory) )
	, mPaintColor( ::ul3::FPixelValue::FromRGBA8( 0, 0, 0 ) )
    , mDrawBrushPreview( true )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyPainterEditor::Init()
{
    InitData();
    InitGUI();
}

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
FOdysseyPainterEditor::InitGUI()
{
    GetGUI()->Init();
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

TSharedPtr<FOdysseyPainterEditorToolkit>
FOdysseyPainterEditor::Toolkit()
{
    return mToolkit.Pin();
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
    return true;
}

void
FOdysseyPainterEditor::FillExtender(TSharedPtr<FExtender>& ioExtender)
{
	return GetGUI()->FillExtender(ioExtender);
}

TSharedRef<FTabManager::FLayout>
FOdysseyPainterEditor::GetLayout()
{
    return GetGUI()->GetLayout();
}

void
FOdysseyPainterEditor::UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager )
{
    GetGUI()->UnregisterTabSpawners(iTabManager);
}

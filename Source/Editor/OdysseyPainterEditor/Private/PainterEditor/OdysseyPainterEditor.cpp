// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditor.h"

#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditorTopTab.h"
#include "SOdysseyPaintModifiers.h"
#include "ULISLoaderModule.h"

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

FOdysseyPaintEngineHUD* 
FOdysseyPainterEditor::PaintEngineHUD() const
{
	return mPaintEngineHUD;
}

IOdysseySurfaceEditable* 
FOdysseyPainterEditor::HUDSurface() const
{
    return mHUDSurface;
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

void 
FOdysseyPainterEditor::RefreshHUDSurface(FVector2D iSizeHUD)
{
    //TODO: check, maybe we don't need a paintEngineHUD at all, a surface is enough.
    //But we'll need to pass a new block to it, making it responsible for the life of both its block and texture,
    //not just the texture as it is right now

    if( mHUDSurface )
    {
        delete mHUDSurface;
    }

    PaintEngineHUD()->SetHUDBlock( new ::ULIS::FBlock(iSizeHUD.X, iSizeHUD.Y, ::ULIS::Format_BGRA8) );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
    ctx.Clear(*(PaintEngineHUD()->GetHUDBlock()));
    ctx.Finish();

    mHUDSurface = new FOdysseySurfaceTexture2DEditable( PaintEngineHUD()->GetHUDBlock() );
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

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
}

FOdysseyPainterEditor::FOdysseyPainterEditor()
    : mPaintEngine()
	, mSelectedTool( NewObject<UOdysseyToolFreeHand>() )
    , mHUDSystem(new FOdysseyHUDSystem())
	, mStrokeEngine(nullptr)
	, mBrushContexts()
	, mPaintColor(::ULIS::FColor::Black)
{
	/* mBrushContexts.Add(new FOdysseyPainterEditorBrushContext(this)); */
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyPainterEditor::InitData()
{
	mStrokeEngine = NewObject<UOdysseyStrokeEngine>();
	mStrokeEngine->Initialize(&mPaintEngine);
	mStrokeEngine->Activate();
    //---
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
	//PaintEngine()->Flush();
}

void
FOdysseyPainterEditor::Redo()
{
	//End stroke before redoing, allows to manage PaintEngine->OnTick Redo
	//PaintEngine()->Flush();
}

void
FOdysseyPainterEditor::ClearUndo()
{
	//End stroke before undoing, just to be perfectly clean
	//PaintEngine()->Flush();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyPaintEngine&
FOdysseyPainterEditor::PaintEngine()
{
	return mPaintEngine;
}

UOdysseyStrokeEngine*
FOdysseyPainterEditor::StrokeEngine()
{
    return mStrokeEngine;
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

FOdysseyBrushColor&
FOdysseyPainterEditor::PaintColor()
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
FOdysseyPainterEditor::PaintColor(const FOdysseyBrushColor& iColor)
{
	mPaintColor = iColor;
}

void
FOdysseyPainterEditor::SetSelectedTool(IOdysseyTool* iSelectedTool)
{
    mSelectedTool = iSelectedTool;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyPainterEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FOdysseyEditor::AddReferencedObjects(Collector);

	if (mStrokeEngine)
        Collector.AddReferencedObject(mStrokeEngine);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------- FTickableEditorObject implementation

void
FOdysseyPainterEditor::Tick(float iDeltaTime)
{
	FOdysseyEditor::Tick(iDeltaTime);

	/* if (mActiveTool)
		mActiveTool->Tick(iDeltaTime); */
		
	if (mStrokeEngine)
		mStrokeEngine->Tick(iDeltaTime);
}

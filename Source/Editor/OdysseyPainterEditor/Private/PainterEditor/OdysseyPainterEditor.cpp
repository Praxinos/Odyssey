// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditor.h"

#include "ObjectEditorUtils.h"

#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditorTopTab.h"
#include "SOdysseyPaintModifiers.h"
#include "OdysseyHUDSystem.h"
#include "OdysseyTool.h"
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
	, mSelectedTool(nullptr)
    , mHUDSystem(new FOdysseyHUDSystem())
	//, mStrokeEngine(nullptr)
	, mBrushContexts()
	, mPaintColor(::ULIS::FColor::Black)
{
	mBrushContexts.Add(new FOdysseyPainterEditorBrushContext(this));
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyPainterEditor::InitData()
{
	//Init DrawingTool
	UOdysseyDrawingTool* drawingTool = NewObject<UOdysseyDrawingTool>();
	drawingTool->OnApplyOverridesDelegate().AddRaw(this, &FOdysseyPainterEditor::OnApplyOverrides);
	drawingTool->Initialize(&mPaintEngine);
	drawingTool->SetBrushContexts(mBrushContexts);
	FObjectEditorUtils::SetPropertyValue(drawingTool->GetBrushOptions(), "Color", FOdysseyBrushColor(mPaintColor)); //Set the paint color in the brushOptions at startup for synchronization


	mSelectedTool = drawingTool;
	mSelectedTool->Activate();
    //---
}

void
FOdysseyPainterEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyEditor::BindShortcuts(iToolkit);

	//---

	//TODO: BindShortcuts from mTools instead of mSelectedTool
	mSelectedTool->BindShortcuts(iToolkit);

	//---

	const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

	#define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateRaw( this, &FOdysseyPainterEditor::__VA_ARGS__ ), FCanExecuteAction() );

	MAP_ACTION(painterEditorCommands.Undo, Undo )
	MAP_ACTION(painterEditorCommands.Redo, Redo )
    MAP_ACTION(painterEditorCommands.ClearUndo, ClearUndo )

	#undef MAP_ACTION
}

void
FOdysseyPainterEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	FOdysseyEditor::ExtendMenu(iOwner, iMenuName);

	//---

	//TODO: ExtendMenu from mTools instead of mSelectedTool
	mSelectedTool->ExtendMenu(iOwner, iMenuName);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Undo

void
FOdysseyPainterEditor::Undo()
{
	//End stroke before undoing, allows to manage PaintEngine->OnTick Undo
	mSelectedTool->Flush();
	mSelectedTool->Commit();
}

void
FOdysseyPainterEditor::Redo()
{
	//End stroke before redoing, allows to manage PaintEngine->OnTick Redo
	mSelectedTool->Flush();
	mSelectedTool->Commit();
}

void
FOdysseyPainterEditor::ClearUndo()
{
	//End stroke before clearing undo
	mSelectedTool->Flush();
	mSelectedTool->Commit();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyPaintEngine&
FOdysseyPainterEditor::PaintEngine()
{
	return mPaintEngine;
}

/* UOdysseyStrokeEngine*
FOdysseyPainterEditor::StrokeEngine()
{
    return mStrokeEngine;
} */

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

UOdysseyTool*
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
FOdysseyPainterEditor::SetSelectedTool(UOdysseyTool* iSelectedTool)
{
    mSelectedTool = iSelectedTool;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Callbacks

void
FOdysseyPainterEditor::OnApplyOverrides(const TMap<FName, UObject*>& iOverrides)
{
	//TODO: Apply Overrides for Paint Color
	//TODO: Apply Overrides for Other things like HUDs, Mesh Selector, or anything else
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyPainterEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FOdysseyEditor::AddReferencedObjects(Collector);

	if (mSelectedTool)
		Collector.AddReferencedObject(mSelectedTool);

	/* if (mStrokeEngine)
        Collector.AddReferencedObject(mStrokeEngine); */
}

//--------------------------------------------------------------------------------------
//------------------------------------------------- FTickableEditorObject implementation

void
FOdysseyPainterEditor::Tick(float iDeltaTime)
{
	FOdysseyEditor::Tick(iDeltaTime);

	if (mSelectedTool)
		mSelectedTool->Tick(iDeltaTime);
		
	/* if (mStrokeEngine)
		mStrokeEngine->Tick(iDeltaTime); */
}

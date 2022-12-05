// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include "Tools/PaintBucketTool/OdysseyPaintBucketTool.h"
#include "UObject/OdysseyObjectEditorUtils.h"

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
	//Init Tools
	InitTools();
	SetSelectedTool(mTools[0]);
}

void
FOdysseyPainterEditor::InitTools()
{
	UOdysseyDrawingTool* drawingTool = NewObject<UOdysseyDrawingTool>();
	drawingTool->OnApplyOverridesDelegate().AddRaw(this, &FOdysseyPainterEditor::OnApplyOverrides);
	drawingTool->SetBrushContexts(mBrushContexts);
	drawingTool->Initialize(&mPaintEngine);
	FOdysseyObjectEditorUtils::SetPropertyValue(drawingTool->GetBrushOptions(), "Color", FOdysseyBrushColor(mPaintColor)); //Set the paint color in the brushOptions at startup for synchronization

	UOdysseyPaintBucketTool* paintBucketTool = NewObject<UOdysseyPaintBucketTool>();
	paintBucketTool->Initialize(&mPaintEngine);

	mTools.Add(drawingTool);
	mTools.Add(paintBucketTool);
}

void
FOdysseyPainterEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyEditor::BindShortcuts(iToolkit);

	//---

	//TODO: BindShortcuts from mTools instead of mSelectedTool
	for (UOdysseyTool* tool : mTools)
		tool->BindShortcuts(iToolkit);

	//---

	const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

	#define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateRaw( this, &FOdysseyPainterEditor::__VA_ARGS__ ), FCanExecuteAction() );

	#undef MAP_ACTION
}

void
FOdysseyPainterEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	FOdysseyEditor::ExtendMenu(iOwner, iMenuName);

	//---
	
	for (UOdysseyTool* tool : mTools)
		tool->ExtendMenu(iOwner, iMenuName);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyPaintEngine&
FOdysseyPainterEditor::PaintEngine()
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

const TArray<UOdysseyTool*>&
FOdysseyPainterEditor::GetTools() const
{
    return mTools;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyPainterEditor::PaintColor(const FOdysseyBrushColor& iColor)
{
	mPaintColor = iColor;
}

void
FOdysseyPainterEditor::SetSelectedTool(UOdysseyTool* iTool)
{
	if (!mTools.Contains(iTool))
		return;

	if (mSelectedTool)
		mSelectedTool->Inactivate();
		
    mSelectedTool = iTool;

	if (mSelectedTool)
		mSelectedTool->Activate();
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
	Collector.AddReferencedObjects(mTools);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------- FTickableEditorObject implementation

void
FOdysseyPainterEditor::Tick(float iDeltaTime)
{
	FOdysseyEditor::Tick(iDeltaTime);

	if (mSelectedTool)
		mSelectedTool->Tick(iDeltaTime);
}

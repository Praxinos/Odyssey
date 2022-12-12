// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditor.h"

#include "ObjectEditorUtils.h"

#include "OdysseyPainterEditorTopTab.h"
#include "SOdysseyPaintModifiers.h"
#include "OdysseyHUDSystem.h"
#include "ULISLoaderModule.h"
#include "OdysseyPainterEditorGUI.h"
#include "UObject/OdysseyObjectEditorUtils.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyPainterEditor::~FOdysseyPainterEditor()
{
}

FOdysseyPainterEditor::FOdysseyPainterEditor()
    : mSelectedTool(nullptr)
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
	//SetSelectedTool(mTools[0]);
}

void
FOdysseyPainterEditor::InitTools()
{
}

void
FOdysseyPainterEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyEditor::BindShortcuts(iToolkit);

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
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyHUDSystem* 
FOdysseyPainterEditor::HUDSystem() const
{
	return mHUDSystem;
}

FOdysseyBrushColor&
FOdysseyPainterEditor::PaintColor()
{
	return mPaintColor;
}

UOdysseyPainterEditorTool*
FOdysseyPainterEditor::GetSelectedTool() const
{
    return mSelectedTool;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyPainterEditor::PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit)
{
	mPaintColor = iColor;

	//PATCH: should be automatic in the new drawing Tool, fix it asap
	FOdysseyObjectEditorUtils::SetPropertyValue(GetRasterDrawingTool()->GetBrushOptions(), "Color", iColor);
}

void
FOdysseyPainterEditor::SetSelectedTool(UOdysseyPainterEditorTool* iTool)
{
	if (mSelectedTool)
		mSelectedTool->Inactivate();
		
    mSelectedTool = iTool;

	if (mSelectedTool)
		mSelectedTool->Activate();
}

void
FOdysseyPainterEditor::ActivateDefaultTool()
{
	for ( UOdysseyPainterEditorTool* tool : mTools )
	{
		if ( tool->IsActivable() )
		{
			tool->Activate();
			return;
		}
	}
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
//------------------------------------------------- FTickableEditorObject implementation

void
FOdysseyPainterEditor::Tick(float iDeltaTime)
{
	FOdysseyEditor::Tick(iDeltaTime);

	if (mSelectedTool)
		mSelectedTool->Tick(iDeltaTime);
}

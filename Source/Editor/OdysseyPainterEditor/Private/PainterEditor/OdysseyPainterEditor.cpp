// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditor.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include "BrushContext/OdysseyPainterEditorBrushContext.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "ObjectEditorUtils.h"

#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorTopTab.h"
#include "OdysseyHUDSystem.h"
#include "ULISLoaderModule.h"
#include "OdysseyPainterEditorGUI.h"
#include "OdysseyPainterEditorExtension.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyEditorLayoutBuilder.h"
#include "BrushContext/OdysseyPainterEditorBrushContext.h"
#include "Models/OdysseyPainterEditorCommands.h"

#include "OdysseyVector.h"
#include "OdysseyVectorAnimationCell.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorObject.h"
#include "HUD/OdysseyVectorHUD.h"
#include "Undo/OdysseyVectorUndoGroup.h"
#include "Undo/OdysseyVectorUndoUngroup.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"
#include "Undo/OdysseyVectorUndoRemoveObjects.h"
#include "Undo/OdysseyVectorUndoBucketRemove.h"
#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "Undo/OdysseyVectorUndoPathStitch.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "Undo/OdysseyVectorUndoApplyTransformations.h"
#include "Undo/OdysseyVectorUndoVertexAlignment.h"
#include "Undo/OdysseyVectorUndoVertexLock.h"
#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "Undo/OdysseyVectorUndoSelectVertex.h"
#include "Undo/OdysseyVectorUndoTransferObjects.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "Undo/OdysseyVectorUndoTagAdd.h"
#include "Undo/OdysseyVectorUndoTagRemove.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerChartAlter.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerCommit.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerMatching.h"

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/RasterEraserTool/OdysseyPainterEditorRasterEraserTool.h"
#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"
#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketTool.h"
#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformTool.h"

#include "Shortcuts/OdysseyLayerStackGlobalShortcuts.h"
#include "Shortcuts/Global/OdysseyPainterEditorGlobalShortcuts.h"
#include "Mesh/FOdysseyMeshSelector.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// FOdysseyPainterEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyPainterEditor::~FOdysseyPainterEditor()
{
    
}

FOdysseyPainterEditor::FOdysseyPainterEditor(const FName& iId, const FText& iName, UObject* iEditedObject, const FName& iLayoutName)
    : FOdysseyEditor(iId, iName, iEditedObject)
    , mLayoutName(iLayoutName)
    , mSource(nullptr)
    , mMeshSelector(MakeShared<FOdysseyMeshSelector>())
    , mCurrentMainTool(nullptr)
    , mCurrentTemporaryTool(nullptr)
    , mVectorHUDFlags(FOdysseyVectorHUD::HUD_MODE_OBJECT)
    , mVectorDrawingFlags(0)
    , mHUDSystem(new FOdysseyHUDSystem())
	, mBrushContexts()
	, mPaintColor(::ULIS::FColor::Black)
	, mRasterDrawingTool(nullptr)
    , mRasterEraserTool(nullptr)
    , mRasterTransformTool(nullptr)
    , mRasterPrimitiveDrawingTool(nullptr)
	, mVectorPrimitiveDrawingTool(nullptr)
    , mVectorPathDrawingTool(nullptr)
    , mVectorPathEditTool(nullptr)
	, mVectorSelectionTool(nullptr)
	, mVectorScenePanTool(nullptr)
	, mVectorEraserTool(nullptr)
	, mVectorPathPushTool(nullptr)
	, mVectorPathSmoothTool(nullptr)
	, mVectorPathStitchTool(nullptr)
	, mVectorPaintBucketTool(nullptr)
	, mColorPickerTool(nullptr)
	, mVectorGridTool(nullptr)
	, mVectorTransformTool(nullptr)
	, mVectorMatchingTool(nullptr)
	, mVectorChartTool(nullptr)
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyPainterEditor::OnCurrentLayerChanged);
	mBrushContexts.Add(new FOdysseyPainterEditorBrushContext(this));
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyPainterEditor::Initialize()
{

    //Init Tools
	InitTools();
    
    //Init the GUI
    mGUI = MakeShareable(new FOdysseyPainterEditorGUI(this));
    mGUI->Initialize();

    //Init the shortcuts
    TAttribute<UOdysseyLayerStack*> layerStackAttr = TAttribute<UOdysseyLayerStack*>::CreateRaw(this, &FOdysseyPainterEditor::LayerStack);
    GetShortcuts().Add(MakeShared<FOdysseyLayerStackGlobalShortcuts>(layerStackAttr));
    GetShortcuts().Add(MakeShared<FOdysseyPainterEditorGlobalShortcuts>(SharedThis(this)));
    
    //Init the extensions
    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->Initialize();

}

TSharedRef<FTabManager::FLayout>
FOdysseyPainterEditor::CreateLayout()
{
    FOdysseyEditorLayoutBuilder builder(mLayoutName);
    mGUI->BuildLayout(builder);

    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->BuildLayout(builder);

    return builder.GetLayout();
}

void
FOdysseyPainterEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyEditor::BindShortcuts(iToolkit);

    GetGUI()->BindShortcuts(iToolkit);

    mRasterDrawingTool->BindShortcuts(iToolkit);
    mRasterEraserTool->BindShortcuts(iToolkit);
	mRasterPaintBucketTool->BindShortcuts(iToolkit);
    mVectorPathDrawingTool->BindShortcuts(iToolkit);
    mVectorPathEditTool->BindShortcuts(iToolkit);
    mRasterTransformTool->BindShortcuts(iToolkit);
    mRasterPrimitiveDrawingTool->BindShortcuts(iToolkit);
	mVectorPrimitiveDrawingTool->BindShortcuts(iToolkit);
	mVectorSelectionTool->BindShortcuts(iToolkit);
	mVectorScenePanTool->BindShortcuts(iToolkit);
	mVectorEraserTool->BindShortcuts(iToolkit);
	mVectorPathPushTool->BindShortcuts(iToolkit);
	mVectorPathSmoothTool->BindShortcuts(iToolkit);
	mVectorPathStitchTool->BindShortcuts(iToolkit);
	mVectorPaintBucketTool->BindShortcuts(iToolkit);
	mColorPickerTool->BindShortcuts(iToolkit);
	mVectorGridTool->BindShortcuts(iToolkit);
	mVectorTransformTool->BindShortcuts(iToolkit);
	mVectorMatchingTool->BindShortcuts(iToolkit);
	mVectorChartTool->BindShortcuts(iToolkit);
	mVectorTrajectoryTool->BindShortcuts(iToolkit);

	//---

	const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

	#define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateRaw( this, &FOdysseyPainterEditor::__VA_ARGS__ ), FCanExecuteAction() );
	#undef MAP_ACTION

    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->BindShortcuts(iToolkit);
}

void
FOdysseyPainterEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyEditor::ExtendMenu(iOwner, iMenuName);

    GetGUI()->ExtendMenu(iOwner, iMenuName);

    mRasterDrawingTool->ExtendMenu(iOwner, iMenuName);
    mRasterEraserTool->ExtendMenu(iOwner, iMenuName);
    mRasterTransformTool->ExtendMenu(iOwner, iMenuName);
	mRasterPaintBucketTool->ExtendMenu(iOwner, iMenuName);
    mVectorPathDrawingTool->ExtendMenu(iOwner,iMenuName);
    mVectorPathEditTool->ExtendMenu(iOwner,iMenuName);
    mRasterPrimitiveDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPrimitiveDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorSelectionTool->ExtendMenu(iOwner, iMenuName);
	mVectorScenePanTool->ExtendMenu(iOwner, iMenuName);
	mVectorEraserTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathPushTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathSmoothTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathStitchTool->ExtendMenu(iOwner, iMenuName);
	mVectorPaintBucketTool->ExtendMenu(iOwner, iMenuName);
	mColorPickerTool->ExtendMenu(iOwner, iMenuName);
	mVectorGridTool->ExtendMenu(iOwner, iMenuName);
	mVectorTransformTool->ExtendMenu(iOwner, iMenuName);
	mVectorMatchingTool->ExtendMenu(iOwner, iMenuName);
	mVectorChartTool->ExtendMenu(iOwner, iMenuName);
	mVectorTrajectoryTool->ExtendMenu(iOwner, iMenuName);

    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->ExtendMenu(iOwner, iMenuName);
}

void 
FOdysseyPainterEditor::OnClose()
{
    //BE CAREFUL: OnClose can be called twice when quiting Unreal Engine
    // due to a bug in Unreal code

    SetSource(nullptr);

    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->Finalize();

    mGUI->Finalize();

    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
	delete mHUDSystem;
    mHUDSystem = nullptr;

    FOdysseyEditor::OnClose();
}

void
FOdysseyPainterEditor::InitTools()
{
    mRasterDrawingTool = NewObject<UOdysseyPainterEditorRasterDrawingTool>();
    mRasterEraserTool = NewObject<UOdysseyPainterEditorRasterEraserTool>();
    mRasterTransformTool = NewObject<UOdysseyPainterEditorRasterTransformTool>();
	mRasterPaintBucketTool = NewObject<UOdysseyPainterEditorRasterPaintBucketTool>();
    mVectorPathDrawingTool = NewObject<UOdysseyPainterEditorVectorPathDrawingTool>();
    mVectorPathEditTool = NewObject<UOdysseyPainterEditorVectorPathEditTool>();
    mRasterPrimitiveDrawingTool = NewObject<UOdysseyPainterEditorRasterPrimitiveDrawingTool>();
	mVectorPrimitiveDrawingTool = NewObject<UOdysseyPainterEditorVectorPrimitiveDrawingTool>();
	mVectorSelectionTool = NewObject<UOdysseyPainterEditorVectorSelectionTool>();
    mVectorScenePanTool = NewObject<UOdysseyPainterEditorVectorScenePanTool>();
    mVectorEraserTool = NewObject<UOdysseyPainterEditorVectorEraserTool>();
    mVectorPathPushTool = NewObject<UOdysseyPainterEditorVectorPathPushTool>();
    mVectorPathSmoothTool = NewObject<UOdysseyPainterEditorVectorPathSmoothTool>();
    mVectorPathStitchTool = NewObject<UOdysseyPainterEditorVectorPathStitchTool>();
	mVectorPaintBucketTool = NewObject<UOdysseyPainterEditorVectorPaintBucketTool>();
	mColorPickerTool = NewObject<UOdysseyPainterEditorColorPickerTool>();
	mVectorGridTool = NewObject<UOdysseyPainterEditorVectorGridTool>();
	mVectorTransformTool = NewObject<UOdysseyPainterEditorVectorTransformTool>();
	mVectorMatchingTool = NewObject<UOdysseyPainterEditorVectorMatchingTool>();
	mVectorChartTool = NewObject<UOdysseyPainterEditorVectorChartTool>();
	mVectorTrajectoryTool = NewObject<UOdysseyPainterEditorVectorTrajectoryTool>();

	mRasterDrawingTool->SetEditor(this);
    mRasterEraserTool->SetEditor(this);
    mRasterTransformTool->SetEditor(this);
	mRasterPaintBucketTool->SetEditor(this);
    mVectorPathDrawingTool->SetEditor(this);
    mVectorPathEditTool->SetEditor(this);
    mRasterPrimitiveDrawingTool->SetEditor(this);
    mVectorPrimitiveDrawingTool->SetEditor(this);
    mVectorSelectionTool->SetEditor(this);
    mVectorScenePanTool->SetEditor(this);
    mVectorEraserTool->SetEditor(this);
    mVectorPathPushTool->SetEditor(this);
    mVectorPathSmoothTool->SetEditor(this);
    mVectorPathStitchTool->SetEditor(this);
	mVectorPaintBucketTool->SetEditor(this);
	mColorPickerTool->SetEditor(this);
	mVectorGridTool->SetEditor(this);
	mVectorTransformTool->SetEditor(this);
	mVectorMatchingTool->SetEditor(this);
	mVectorChartTool->SetEditor(this);
	mVectorTrajectoryTool->SetEditor(this);
	mRasterDrawingTool->SetBrushContexts(&mBrushContexts);

    //Default Tools a defined by their position in mTools
    //example: mRasterDrawingTool appears before mRasterTransformTool
    //so mRasterDrawingTool will be the default tool

    //Default Raster Tool
	mTools.Add(mRasterDrawingTool);

    //Default Vector Tool
    mTools.Add(mVectorPathDrawingTool);

    //Other Raster Tools
    mTools.Add(mRasterEraserTool);
    mTools.Add(mRasterTransformTool);
    mTools.Add(mRasterPrimitiveDrawingTool);

    //Other Vector Tools
	mTools.Add(mRasterPaintBucketTool);
    mTools.Add(mVectorPrimitiveDrawingTool);
    mTools.Add(mVectorPathEditTool);
    mTools.Add(mVectorSelectionTool);
    mTools.Add(mVectorScenePanTool);
    mTools.Add(mVectorEraserTool);
    mTools.Add(mVectorPathPushTool);
    mTools.Add(mVectorPathSmoothTool);
    mTools.Add(mVectorPathStitchTool);
	mTools.Add(mVectorPaintBucketTool);
	mTools.Add(mVectorGridTool);
	mTools.Add(mVectorTransformTool);
	mTools.Add(mVectorMatchingTool);
	mTools.Add(mVectorChartTool);
	mTools.Add(mVectorTrajectoryTool);
    //Generic Tools
	mTools.Add(mColorPickerTool);
}

FSimpleMulticastDelegate&
FOdysseyPainterEditor::OnCurrentMainToolChanged()
{
    return mOnCurrentMainToolChanged;
}

FSimpleMulticastDelegate&
FOdysseyPainterEditor::OnCurrentTemporaryToolChanged()
{
    return mOnCurrentTemporaryToolChanged;
}

FSimpleMulticastDelegate&
FOdysseyPainterEditor::OnCurrentToolChanged()
{
    return mOnCurrentToolChanged;
}

FSimpleMulticastDelegate&
FOdysseyPainterEditor::OnSourceChanged()
{
    return mOnSourceChanged;
}

TSharedPtr<FOdysseyPainterEditorSource>
FOdysseyPainterEditor::GetSource() const
{
    return mSource;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyPainterEditorRasterDrawingTool*
FOdysseyPainterEditor::GetRasterDrawingTool() const
{
    return mRasterDrawingTool;
}

UOdysseyPainterEditorRasterEraserTool*
FOdysseyPainterEditor::GetRasterEraserTool() const
{
    return mRasterEraserTool;
}

UOdysseyPainterEditorRasterTransformTool* 
FOdysseyPainterEditor::GetRasterTransformTool() const
{
    return mRasterTransformTool;
}

UOdysseyPainterEditorRasterPrimitiveDrawingTool*
FOdysseyPainterEditor::GetRasterPrimitiveDrawingTool() const
{
    return mRasterPrimitiveDrawingTool;
}

UOdysseyPainterEditorRasterPaintBucketTool*
FOdysseyPainterEditor::GetRasterPaintBucketTool() const
{
    return mRasterPaintBucketTool;
}

UOdysseyPainterEditorVectorPrimitiveDrawingTool*
FOdysseyPainterEditor::GetVectorPrimitiveDrawingTool() const
{
    return mVectorPrimitiveDrawingTool;
}

UOdysseyPainterEditorVectorPathDrawingTool*
FOdysseyPainterEditor::GetVectorPathDrawingTool() const
{
    return mVectorPathDrawingTool;
}

UOdysseyPainterEditorVectorPathEditTool*
FOdysseyPainterEditor::GetVectorPathEditTool() const
{
    return mVectorPathEditTool;
}

UOdysseyPainterEditorVectorSelectionTool*
FOdysseyPainterEditor::GetVectorSelectionTool() const
{
    return mVectorSelectionTool;
}

UOdysseyPainterEditorVectorGridTool*
FOdysseyPainterEditor::GetVectorGridTool() const
{
    return mVectorGridTool;
}

UOdysseyPainterEditorVectorTransformTool*
FOdysseyPainterEditor::GetVectorTransformTool() const
{
    return mVectorTransformTool;
}

UOdysseyPainterEditorVectorMatchingTool*
FOdysseyPainterEditor::GetVectorMatchingTool() const
{
    return mVectorMatchingTool;
}

UOdysseyPainterEditorVectorTrajectoryTool*
FOdysseyPainterEditor::GetVectorTrajectoryTool() const
{
    return mVectorTrajectoryTool;
}

UOdysseyPainterEditorVectorChartTool*
FOdysseyPainterEditor::GetVectorChartTool() const
{
    return mVectorChartTool;
}

UOdysseyPainterEditorVectorScenePanTool*
FOdysseyPainterEditor::GetVectorScenePanTool() const
{
    return mVectorScenePanTool;
}

UOdysseyPainterEditorVectorEraserTool*
FOdysseyPainterEditor::GetVectorEraserTool() const
{
    return mVectorEraserTool;
}

UOdysseyPainterEditorVectorPathPushTool*
FOdysseyPainterEditor::GetVectorPathPushTool() const
{
    return mVectorPathPushTool;
}

UOdysseyPainterEditorVectorPathSmoothTool*
FOdysseyPainterEditor::GetVectorPathSmoothTool() const
{
    return mVectorPathSmoothTool;
}

UOdysseyPainterEditorVectorPathStitchTool*
FOdysseyPainterEditor::GetVectorPathStitchTool() const
{
    return mVectorPathStitchTool;
}

UOdysseyPainterEditorVectorPaintBucketTool*
FOdysseyPainterEditor::GetVectorPaintBucketTool() const
{
    return mVectorPaintBucketTool;
}

UOdysseyPainterEditorColorPickerTool*
FOdysseyPainterEditor::GetColorPickerTool() const
{
    return mColorPickerTool;
}

TArray<FOdysseyBrushContext*>&
FOdysseyPainterEditor::GetBrushContexts()
{
    return mBrushContexts;
}

FOdysseyHUDSystem* 
FOdysseyPainterEditor::HUDSystem() const
{
	return mHUDSystem;
}

const FOdysseyBrushColor&
FOdysseyPainterEditor::PaintColor() const
{
	return mPaintColor;
}

UOdysseyPainterEditorTool*
FOdysseyPainterEditor::GetCurrentMainTool() const
{
    return mCurrentMainTool;
}

UOdysseyPainterEditorTool*
FOdysseyPainterEditor::GetCurrentTemporaryTool() const
{
    return mCurrentTemporaryTool;
}

UOdysseyPainterEditorTool*
FOdysseyPainterEditor::GetCurrentTool() const
{
    return mCurrentTemporaryTool ? mCurrentTemporaryTool : mCurrentMainTool;
}

void
FOdysseyPainterEditor::InactivateAllTools()
{
    if (!mCurrentMainTool && !mCurrentTemporaryTool)
        return;

    if (mCurrentTemporaryTool)
    {
        mCurrentTemporaryTool->Inactivate();
        mCurrentTemporaryTool = nullptr;
        mOnCurrentTemporaryToolChanged.Broadcast();
    }

    if (mCurrentMainTool)
    {
        mCurrentMainTool->Inactivate();
        mCurrentMainTool = nullptr;
        mOnCurrentMainToolChanged.Broadcast();
    }
    mOnCurrentToolChanged.Broadcast();
}

void
FOdysseyPainterEditor::InactivateMainTool()
{
    if (mCurrentMainTool)
    {
        mCurrentMainTool->Inactivate();
        mCurrentMainTool = nullptr;
        mOnCurrentMainToolChanged.Broadcast();
    }
    mOnCurrentToolChanged.Broadcast();
}

void
FOdysseyPainterEditor::ActivateMainTool( UOdysseyPainterEditorTool* iTool )
{
    if (mCurrentMainTool && mCurrentMainTool == iTool && mCurrentMainTool->IsActivated() )
        return;

    if (mCurrentTemporaryTool && mCurrentTemporaryTool->IsActivated())
    {
        mCurrentTemporaryTool->Inactivate();
        mCurrentTemporaryTool = nullptr;
        mOnCurrentTemporaryToolChanged.Broadcast();
    }
    
    if (mCurrentMainTool && mCurrentMainTool != iTool && mCurrentMainTool->IsActivated())
    {
        mCurrentMainTool->Inactivate();
        mCurrentMainTool = nullptr;
    }
    
    if (!iTool || !iTool->IsActivable())
    {
        mOnCurrentMainToolChanged.Broadcast();
        mOnCurrentToolChanged.Broadcast();
        return;
    }

    mCurrentMainTool = iTool;
    mCurrentMainTool->Activate();

    if (LayerStack())
    {
        UOdysseyLayer* currentLayer = LayerStack()->CurrentLayer.Get();
        if (currentLayer)
        {
            UClass* layerClass = currentLayer->GetClass();
            if (!mCurrentMainToolPerLayerClass.Contains(layerClass))
                mCurrentMainToolPerLayerClass.Add(layerClass, nullptr);

            mCurrentMainToolPerLayerClass[layerClass] = mCurrentMainTool;
        }
    }

    mOnCurrentMainToolChanged.Broadcast();
    mOnCurrentToolChanged.Broadcast();
}

void
FOdysseyPainterEditor::InactivateTemporaryTool()
{
    if (!mCurrentTemporaryTool)
        return;

    mCurrentTemporaryTool->Inactivate();
    mCurrentTemporaryTool = nullptr;
    mOnCurrentTemporaryToolChanged.Broadcast();

    if (mCurrentMainTool && mCurrentMainTool->IsActivable())
    {
        mCurrentMainTool->Activate();
    }
    mOnCurrentToolChanged.Broadcast();
}

void
FOdysseyPainterEditor::ActivateTemporaryTool( UOdysseyPainterEditorTool* iTool )
{
    if (!iTool || mCurrentTemporaryTool == iTool || !iTool->IsActivable())
        return;

    if (mCurrentTemporaryTool && mCurrentTemporaryTool->IsActivated())
    {
        mCurrentTemporaryTool->Inactivate();
        mCurrentTemporaryTool = nullptr;
    }

    if (mCurrentMainTool && mCurrentMainTool->IsActivated())
    {
        mCurrentMainTool->Inactivate();
    }

    mCurrentTemporaryTool = iTool;
    mCurrentTemporaryTool->Activate();

    mOnCurrentTemporaryToolChanged.Broadcast();
    mOnCurrentToolChanged.Broadcast();
}

void
FOdysseyPainterEditor::SanitizeCurrentTool()
{
    InactivateAllTools();

	UOdysseyLayer* currentLayer = LayerStack()->CurrentLayer.Get();
	if (!currentLayer)
		return;

	UOdysseyPainterEditorTool* tool = nullptr;
	UClass* layerClass = currentLayer->GetClass();
	if (mCurrentMainToolPerLayerClass.Contains(layerClass))
		tool = mCurrentMainToolPerLayerClass[layerClass];
	
	if (!tool || !tool->IsActivable())
		tool = FindDefaultToolForCurrentLayer();

    if (!tool || !tool->IsActivable())
        return;

	ActivateMainTool(tool);
}

FOdysseyMediaProvider
FOdysseyPainterEditor::GetCurrentMediaProvider()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = GetSource(); 
    if (!source)
        return FOdysseyMediaProvider();

    return source->GetCurrentMediaProvider();
}

UOdysseyLayerStack*
FOdysseyPainterEditor::LayerStack() const
{
	TSharedPtr<FOdysseyPainterEditorSource> source = GetSource();
	if (!source)
		return nullptr;

	return source->GetLayerStack();
}

TSharedPtr<FOdysseyMeshSelector>
FOdysseyPainterEditor::GetMeshSelector() const
{
    return mMeshSelector;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyPainterEditor::SetSource(TSharedPtr<FOdysseyPainterEditorSource> iSource)
{
    if (mSource)
    {
        mSource->Inactivate();
        mSource = nullptr;
		
        InactivateAllTools();
    }

    if (iSource)
    {
        mSource = iSource;
        mSource->OnAddEditedObjectDelegate().AddLambda([this](UObject* iObject) { AddEditedObject(iObject);});
        mSource->OnRemoveEditedObjectDelegate().AddLambda([this](UObject* iObject) { RemoveEditedObject(iObject);});
        mSource->Activate();
        
        if ( mCurrentMainTool && mCurrentMainTool->IsActivable() )
        {
            ActivateMainTool(mCurrentMainTool);
        }
        else
        {
            //select the best tool
            SanitizeCurrentTool();
        }
    }

    OnSourceChanged().Broadcast();
}

void
FOdysseyPainterEditor::PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit)
{
	mPaintColor = iColor;

	//PATCH: should be automatic in the new drawing Tool, fix it asap
	if (iIsCommit)
		FOdysseyObjectEditorUtils::SetPropertyValue(GetRasterDrawingTool()->GetBrushOptions(), "Color", iColor);
}

UOdysseyPainterEditorTool*
FOdysseyPainterEditor::FindDefaultToolForCurrentLayer()
{
	for (UOdysseyPainterEditorTool* tool : mTools)
	{
		if ( !tool->IsActivable() )
			continue;

		return tool;
	}
	return nullptr;
}

FOdysseyPainterEditorGUI*
FOdysseyPainterEditor::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShareable(new FOdysseyPainterEditorGUI(this));
	return mGUI.Get();
}

void
FOdysseyPainterEditor::AddExtension(TSharedPtr<FOdysseyPainterEditorExtension> iExtension)
{
    mExtensions.Add(iExtension);
}

void
FOdysseyPainterEditor::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	if ( iLayerStack != LayerStack() )
		return;

	SanitizeCurrentTool(); //Refresh the current tool when we change layer
}

void
FOdysseyPainterEditor::SetVectorHUDFlags( uint64 iVectorHUDFlags )
{
    mVectorHUDFlags = iVectorHUDFlags;
}

uint64
FOdysseyPainterEditor::GetVectorHUDFlags()
{
    return mVectorHUDFlags;
}

void
FOdysseyPainterEditor::SetVectorDrawingFlags( uint64 iVectorDrawingFlags )
{
    mVectorDrawingFlags = iVectorDrawingFlags;
}

uint64
FOdysseyPainterEditor::GetVectorDrawingFlags()
{
    return mVectorDrawingFlags;
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

    UOdysseyPainterEditorTool* tool = GetCurrentTool();
	if (tool)
		tool->Tick(iDeltaTime);
}

//--------------------------------------------------------------------------------------
//------ Generic methods for vector layers. currently placed here, although it's not really needed.
//------ This might be put somewhere else, in a CommonFunctions file or something as static methods.

void
FOdysseyPainterEditor::AddEditMenuEntry( FMenuBuilder& iMenuBuilder )
{
    UOdysseyPainterEditorVectorBaseTool* vectorBaseTool = Cast<UOdysseyPainterEditorVectorBaseTool>(GetCurrentTool());

    if( vectorBaseTool )
    {
	    iMenuBuilder.BeginSection("Tool Options", TAttribute(FText::FromString("Tool/Options")));
	    {
            vectorBaseTool->ExtendContextMenu( iMenuBuilder );
        }
        iMenuBuilder.EndSection();
    }
}

// static
void
FOdysseyPainterEditor::BringForward( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorObject* selectedObject = vectorEngine->GetLastSelectedObject();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( selectedObject )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.bring-forward", "Bring forward"));
        if( GUndo )
        {
           FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTransferObjects( iScene
                                                                           , selectedObject
                                                                           , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        selectedObject->BringForward();

        iScene->Update( 0 );
    }

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::SendBackward( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorObject* selectedObject = vectorEngine->GetLastSelectedObject();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( selectedObject )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.send-backward", "Send backward"));
        if( GUndo )
        {
           FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTransferObjects( iScene
                                                                           , selectedObject
                                                                           , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
            
            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        selectedObject->SendBackward();

        iScene->Update( 0 );
    }

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::ApplyTransformations( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    // concerns only top-most objects of a branch, including the scene
    vectorEngine->GetFocusedAncestorList( objectList );

    // Backup before, for undoing
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.apply-transformations", "Apply Transformations"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoApplyTransformations( iScene
                                                                             , objectList
                                                                             , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    for( FOdysseyVectorObject* transformedObject : objectList )
    {
        transformedObject->ApplyTransformations();
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::MakePaintGroup( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorBucket*> removedBucketArray;
    std::vector<FOdysseyVectorObject*> cubicPathArray;
    std::list<FOdysseyVectorObject*> objectList;
    FOdysseyVectorGroupPaint* paintGroup;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    // concerns all selected objects of a branch but the scene
    vectorEngine->GetFocusedObjectList( objectList );

    paintGroup = FOdysseyVectorEngine::MakePaintGroupFromObjects( iScene
                                                                , objectList
                                                                , cubicPathArray
                                                                , removedBucketArray );

    if( paintGroup )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.make-group-paint", "Group Paint"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoGroup( iScene
                                                                  , paintGroup
                                                                  , cubicPathArray
                                                                  , removedBucketArray
                                                                  , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
            
            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        vectorEngine->ClearObjectSelection();
        vectorEngine->SelectObject( paintGroup );

        iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::Ungroup( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorObject* selectedObject = vectorEngine->GetLastSelectedObject();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( selectedObject )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
        {
            FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>( selectedObject );
            FOdysseyVectorObject* groupParent = group->GetParent();
            // we work on a copy of the list to be able to delete children while iterating
            std::list<FOdysseyVectorObject*> childrenList = group->GetChildrenList();

            // needed for undos
            GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.ungroup", "Ungroup"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoUngroup( iScene
                                                                        , group
                                                                        , notificationFlags );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();

            for( FOdysseyVectorObject* child : childrenList )
            {
                groupParent->TransferChild( child, groupParent->GetLastChild() );
            }

            //groupParent->RemoveChild( group );

            vectorEngine->ClearObjectSelection();

            iScene->UpdateMatrix();
            iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        }
    }

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

void
FOdysseyPainterEditor::GroupAndAddInbetweenerTag( FOdysseyPainterEditor* iEditor
                                                , FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    // this call will also handle the creation of an Undo entry
    FOdysseyVectorGroup* group = _Group( iEditor, iScene );
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( group )
    {
        FOdysseyVectorTagInbetweener *inbetweenerTag = new FOdysseyVectorTagInbetweener( iScene->GetSharedEnv()
                                                                                       , group
                                                                                       , 16
                                                                                       , 16 );

        //inbetweenerTag->SetInterpolationDirection( eInbetweenerInterpolationDirection::Backward );

        group->AddTag( inbetweenerTag );
    }

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::Group( FOdysseyPainterEditor* iEditor
                            , FOdysseyVectorGroupPaint* iScene )
{
    _Group( iEditor, iScene );
}

// static
FOdysseyVectorGroup*
FOdysseyPainterEditor::_Group( FOdysseyPainterEditor* iEditor
                             , FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorObject*> objectOldParentArray;
    std::vector<FOdysseyVectorObject*> objectArray;
    std::list<FOdysseyVectorObject*> objectList;
    FOdysseyVectorGroup* group;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    vectorEngine->GetFocusedAncestorList( objectList );

    group = vectorEngine->GroupObjects( iScene, objectList, objectArray );

    if( group )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.group", "Group"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoGroup( iScene
                                                                  , group
                                                                  , objectArray
                                                                  , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
            
            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

        vectorEngine->ClearObjectSelection();
        vectorEngine->SelectObject( group );

    }

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );

    return group;
}

// static
void
FOdysseyPainterEditor::SelectAllPoints( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    // concerns all selected objects of a branch including implicit selection
    vectorEngine->GetFocusedObjectList( objectList );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.select-all-points","Select All Points"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectVertex( iScene
                                                                     , objectList
                                                                     , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    for( FOdysseyVectorObject* focusedObject : objectList )
    {
        if( focusedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(focusedObject);

            path->SelectAllVertices();
        }

        if( focusedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(focusedObject);

            paintgroup->SelectAllBuckets();
        }
    }

    iScene->Update( 0 ); // updated invalidated objects. No need to update paintgroups

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::SelectAllObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.select-all-objects", "Select All"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( iScene, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    vectorEngine->SelectAllInSelectionSpace();

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::ResetView( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-view", "Reset view"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, iScene, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->ResetTransform();
    iScene->UpdateMatrix();
    iScene->Update( 0 );

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::LockPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot 
    // segment handles coordinates before they'll get aligned.
    vectorEngine->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.unalign-point-selection","Unalign Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexLock( iScene, selectedVertexArray, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
    //---------- end of undo ------------//

    for( FOdysseyVectorVertex* vertex : selectedVertexArray )
    {
        vertex->SetLocked( true );
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // updated invalidated objects

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::UnlockPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot 
    // segment handles coordinates before they'll get aligned.
    vectorEngine->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.unalign-point-selection","Unalign Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexLock( iScene, selectedVertexArray, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
    //---------- end of undo ------------//

    for( FOdysseyVectorVertex* vertex : selectedVertexArray )
    {
        vertex->SetLocked( false );
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // updated invalidated objects

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::UnalignPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot 
    // segment handles coordinates before they'll get aligned.
    vectorEngine->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.unalign-point-selection","Unalign Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene, selectedVertexArray, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );        

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
    //---------- end of undo ------------//

    // the actual unalignment
    for( FOdysseyVectorVertex* vertex : selectedVertexArray )
    {
        vertex->SetHandleAligned( false );
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // updated invalidated objects

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::AlignPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot 
    // segment handles coordinates before they'll get aligned.
    vectorEngine->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.align-point-selection","Align Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene, selectedVertexArray, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction(); 
    //---------- end of undo ------------//

    // the actual alignment ( alignment is based on the first segment met)
    for( FOdysseyVectorVertex* vertex : selectedVertexArray )
    {
        FOdysseyVectorSegment* segment = vertex->GetFirstSegment();
        // we don't check the validity of the pointer to the segment because a vertex is suppose
        // to always belong to at least 1 segment.
        vertex->AlignHandles( segment->GetHandle( vertex ) );
        vertex->SetHandleAligned( true );
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // updated invalidated objects

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::DeletePointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorPath*> removedPathArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorPath*> addedPathArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;
    // concerns all selected objects of a branch including implicit selection
    vectorEngine->GetFocusedObjectList( objectList );

    removedPathArray.reserve( 10 );
    removedVertexArray.reserve( 10 );
    removedSegmentArray.reserve( 10 );
    addedSegmentArray.reserve( 10 );

    for( FOdysseyVectorObject* focusedObject : objectList )
    {
        if( focusedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(focusedObject);
            std::vector<FOdysseyVectorVertex*> selectedVertexArray;

            path->GetSelectedVertices( selectedVertexArray );

            if( selectedVertexArray.size() )
            {
                FOdysseyVectorPath::DeleteVertex( path
                                                , selectedVertexArray
                                                , removedVertexArray
                                                , removedSegmentArray
                                                , removedPathArray
                                                , addedSegmentArray );
            }
        }
    }

    for( int i = 0; i < removedPathArray.size(); i++ )
    {
        FOdysseyVectorPath* path = removedPathArray[i];

        path->GetParent()->RemoveChild( path );
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // updated invalidated objects

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-point-selection","Delete Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                  , removedPathArray
                                                                  , removedVertexArray
                                                                  , removedSegmentArray
                                                                  , addedPathArray
                                                                  , addedVertexArray
                                                                  , addedSegmentArray
                                                                  , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::DeleteObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*>& selectedObjectList = vectorEngine->GetSelectedObjectList();
    std::vector<FOdysseyVectorObject*> removedObjectArray;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    removedObjectArray.reserve( selectedObjectList.size() );

    vectorEngine->RemoveObjects( selectedObjectList, removedObjectArray );

    vectorEngine->ClearObjectSelection();

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-objects", "Delete Objects"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoRemoveObjects( iScene
                                                                      , removedObjectArray
                                                                      , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::RemoveInbetweenerTag( FOdysseyPainterEditor* iEditor
                                           , FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*>& selectedObjectList = vectorEngine->GetSelectedObjectList();
    std::vector<FOdysseyVectorTag*> removedTagArray;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    removedTagArray.reserve( selectedObjectList.size() );

    for( FOdysseyVectorObject* selectedObject : selectedObjectList )
    {
        FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

        if( tag )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            selectedObject->RemoveTag( tag );

            removedTagArray.push_back( tag );

            inbetweenerTag->RedrawAnimationCells();
        }
    }

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-tags", "Delete Tags"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagRemove( iScene
                                                                  , removedTagArray
                                                                  , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::CommitSelectedInbetweenerTag( FOdysseyPainterEditor* iEditor
                                                   , FOdysseyVectorSharedEnv* iSharedEnv )
{
    std::list<FOdysseyVectorGroupPaint*> committedSceneList;
    std::list<FOdysseyVectorObject*> addedObjectList;
    std::list<FOdysseyVectorTag*> removedTagList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;
    std::list<FOdysseyVectorTag*> selectedTagList;

    iSharedEnv->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass(), selectedTagList );


    for( FOdysseyVectorTag* tag : selectedTagList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

        // commit will concern the whole tree, we just commit the top one
        if( inbetweenerTag->IsTopSelectedTag() )
        {
            inbetweenerTag->Commit( removedTagList
                                  , addedObjectList
                                  , committedSceneList );
        }
    }

    if( removedTagList.size() )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.commit-tags", "Commit Tags"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerCommit( iSharedEnv
                                                                                 , removedTagList
                                                                                 , addedObjectList
                                                                                 , committedSceneList
                                                                                 , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        // request redraw
        for( FOdysseyVectorGroupPaint* scene : committedSceneList )
        {
            scene->GetEngine()->Invalidate( 0 );
        }

        // call callbacks if any (for refreshing GUI e.g)
        FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
    }
}

// static
void
FOdysseyPainterEditor::AddInbetweenerTag( FOdysseyPainterEditor* iEditor
                                        , FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*>& selectedObjectList = vectorEngine->GetSelectedObjectList();
    std::vector<FOdysseyVectorTag*> addedTagArray;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    for( FOdysseyVectorObject* selectedObject : selectedObjectList )
    {
        FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

        if( tag == nullptr )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = new FOdysseyVectorTagInbetweener( iScene->GetSharedEnv()
                                                                                           , selectedObject
                                                                                           , 16
                                                                                           , 16 );

            selectedObject->AddTag( inbetweenerTag );

            addedTagArray.push_back( inbetweenerTag );
        }
    }

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.add-tags", "Add Tags"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagAdd( iScene
                                                               , addedTagArray 
                                                               , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::ResetInbetweenerGrid( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;
    std::list<FOdysseyVectorTag*> selectedTagList;

    iScene->GetSharedEnv()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                     , selectedTagList );

    for( FOdysseyVectorTag* tag : selectedTagList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

        selectedInbetweenerTagList.push_back( inbetweenerTag );
    }

    if( selectedInbetweenerTagList.size() )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-chart","Reset Spacing Chart"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerMatching( iScene
                                                                                    , selectedInbetweenerTagList
                                                                                    , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        for( FOdysseyVectorTagInbetweener* inbetweenerTag : selectedInbetweenerTagList )
        {
            inbetweenerTag->ResetGrid();
        }
    }

    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::ResetBreakdownSpacingChart( FOdysseyPainterEditor* iEditor
                                                 , FOdysseyVectorGroupPaint* iScene
                                                 , bool iResetPositionning )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;
    std::list<FOdysseyVectorTag*> selectedTagList;
    uint32 cellIndex = iScene->GetEngine()->GetAnimationCell()->GetIndex();

    iScene->GetSharedEnv()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                     , selectedTagList );

    if( selectedTagList.size() )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-chart","Reset Spacing Chart"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene->GetSharedEnv()
                                                                                     , selectedTagList
                                                                                     , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        for( FOdysseyVectorTag* tag : selectedTagList )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
            FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdownByCellIndex( cellIndex );

            if( breakdown )
            {
                breakdown->GetChart()->Reset( iResetPositionning );
            }
        }
    }

    iScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
}

// static
void
FOdysseyPainterEditor::ResetInbetweenerTagSpacingChart( FOdysseyPainterEditor* iEditor
                                                      , FOdysseyVectorSharedEnv* iSharedEnv )
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;
    std::list<FOdysseyVectorTag*> selectedTagList;

    iSharedEnv->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                         , selectedTagList );

    if( selectedTagList.size() )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-chart","Reset Spacing Chart"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iSharedEnv
                                                                                     , selectedTagList
                                                                                     , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        for( FOdysseyVectorTag* tag : selectedTagList )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
            {
                breakdown->GetChart()->Reset( false );
            }

             // request redraw
            inbetweenerTag->GetOwner()->GetEngine()->Invalidate( 0 );
        }
    }

    iSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, notificationFlags );
}

// static
void
FOdysseyPainterEditor::FlipHorizontal( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    // concerns only the top-most selected objects of a branch, including the scene
    vectorEngine->GetFocusedAncestorList( objectList );
 
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.flip-horizontal", "Flip Horizontal"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, objectList, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    FOdysseyVectorEngine::FlipObjectsHorizontal( objectList );

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::FlipVertical( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    // concerns only the top-most selected objects of a branch, including the scene
    vectorEngine->GetFocusedAncestorList( objectList );

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.flip-vertical", "Flip Vertical"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, objectList, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    FOdysseyVectorEngine::FlipObjectsVertical( objectList );

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::ClearColoring( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorBucket*> bucketArray;
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    // concerns all objects of a branch, including the scene
    vectorEngine->GetFocusedObjectList( objectList );

    bucketArray.reserve( 100 );

    // first step: retrieve all buckets for undoing.
    for( FOdysseyVectorObject* clearedObject : objectList )
    {
        if( clearedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(clearedObject);
            std::list<FOdysseyVectorBucket*>& bucketList = paintgroup->GetBucketList();

            for( FOdysseyVectorBucket* bucket : bucketList )
            {
                bucketArray.push_back( bucket );
            }
        }
    }

    // second step: the actual removal.
    for( FOdysseyVectorObject* clearedObject : objectList )
    {
        if( clearedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(clearedObject);

            paintgroup->RemoveAllBuckets();
        }
    }

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.clear-coloring", "Clear Coloring"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( iScene, bucketArray, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();


    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // re-colorize paint group

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::DeleteBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
    FOdysseyVectorGroupPaint* scene = ownerObject->GetScene();
    FOdysseyVectorEngine* vectorEngine = scene->GetEngine();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

        paintGroup->RemoveBucket( iBucket );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-bucket","Delete Bucket"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( scene, iBucket, notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
            
            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    scene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // re-colorize paint group

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( scene, notificationFlags );
}

void
FOdysseyPainterEditor::AlterContourWidth( FOdysseyVectorGroupPaint* iScene
                                        , double iValue
                                        , bool   iAbsolute )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    //std::list<FOdysseyVectorGroupPaint*> paintgroupList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_HUD;

    vectorEngine->Traverse
    ( iScene
    , 0
    , [ iScene
      , vectorEngine
      //, &paintgroupList
      , iValue
      , iAbsolute ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
              {
                  FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(object);
 
                  //paintgroupList.push_back( paintgroup );

                  paintgroup->AlterContourWidth( iValue, iAbsolute );
              }
              // do not recurse
              return FOdysseyVectorEngine::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );

    //for( )
    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

static void
SetBucketPropagation( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket, bool iPropagate )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
    FOdysseyVectorGroupPaint* scene = ownerObject->GetScene();
    FOdysseyVectorEngine* vectorEngine = scene->GetEngine();
    uint64 notificationFlags = 0;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.propagate-bucket","Propagate Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( scene, iBucket, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iBucket->SetPropagated( iPropagate );

    scene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // request redraw
    vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( scene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::PropagateBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    SetBucketPropagation( iEditor, iBucket, true );
}

// static
void
FOdysseyPainterEditor::UnpropagateBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    SetBucketPropagation( iEditor, iBucket, false );
}

static FInbetweenerChart*
GetCopiedChart()
{
    static FInbetweenerChart copiedChart( nullptr );

    return &copiedChart;
}

// static
void
FOdysseyPainterEditor::PasteSpacingChart( FOdysseyPainterEditor* iEditor
                                        , FOdysseyVectorGroupPaint* iScene )
{
/*-----
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FInbetweenerChart* copiedChart = GetCopiedChart();
    uint64 notificationFlags = 0;

    if( copiedChart->GetInbetweenArray().size() > 0 )
    {
        // we substract 1 because the buffer also holds the final position
        // which is not an inbetween per-se.
        uint32 drawingCount = copiedChart->GetInbetweenArray().size();

        vectorEngine->GetSelectedInbetweenerTagList( selectedInbetweenerTagList );

        if( selectedInbetweenerTagList.size() )
        {
            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.paste-chart","Paste Spacing Chart"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene
                                                                                         , selectedInbetweenerTagList
                                                                                         , notificationFlags );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
                TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();

            for( FOdysseyVectorTagInbetweener* inbetweenerTag : selectedInbetweenerTagList )
            {
                if( inbetweenerTag->GetDrawingCount() == drawingCount )
                {
                    for( uint32 i = 0; i < drawingCount; i++ )
                    {
                        inbetweenerTag->GetChart().GetDrawing( i )->spacing = copiedChart->GetDrawing( i )->spacing;
                    }

                    inbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
                }
            }
        }
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    vectorEngine->ResetHUD();
    vectorEngine->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
*/
}

// static
void
FOdysseyPainterEditor::CopySpacingChart( FOdysseyPainterEditor* iEditor
                                       , FOdysseyVectorGroupPaint* iScene )
{
/*-------
    FOdysseyVectorObject* selectedObject = iScene->GetEngine()->GetLastSelectedObject();

    if( selectedObject )
    {
        FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

        if( tag )
        {
            FOdysseyVectorTagInbetweener* tagInbetweener = static_cast<FOdysseyVectorTagInbetweener*>(tag);
            FInbetweenerChart* copiedChart = GetCopiedChart();

            (*copiedChart) = tagInbetweener->GetChart();
        }
    }
*/
}

static std::list<FOdysseyVectorObject*>&
GetCopiedObjectList()
{
    static std::list<FOdysseyVectorObject*> copiedObjectList;

    return copiedObjectList;
}

static FOdysseyVectorObject*
GetStoreObject()
{
    static FOdysseyVectorObject storeObject("StoreObject");

    return &storeObject;
}

// static
void
FOdysseyPainterEditor::CopyTransformation( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetEngine()->GetLastSelectedObject();

    if( selectedObject )
    {
        FOdysseyVectorObject* storeObject = GetStoreObject();

        selectedObject->CopyTransformation( *storeObject );
    }
}

// static
void
FOdysseyPainterEditor::PasteTransformation( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorObject* selectedObject = iScene->GetEngine()->GetLastSelectedObject();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( selectedObject )
    {
        FOdysseyVectorObject* storeObject = GetStoreObject();

        //----- needed for undos -----//
        GEditor->BeginTransaction(LOCTEXT("PasteTransformation", "Paste Transformation"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, selectedObject, notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
            
            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
        // -------------------------- //

        storeObject->CopyTransformation( *selectedObject );

        selectedObject->UpdateMatrix();
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    vectorEngine->ResetHUD();
    // request redraw
    vectorEngine->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::CopyObjects( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> objectList;

    // concerns all objects of a branch but the scene
    vectorEngine->GetFocusedAncestorList( objectList );

    if( objectList.size() )
    {
        // First step : clear previously copied objects
        GetCopiedObjectList().remove_if( []( FOdysseyVectorObject* iCopiedObject ){ delete iCopiedObject; return true; } );

        // second step : copy selection.
        for( FOdysseyVectorObject* copiedObject : objectList )
        {
            GetCopiedObjectList().push_back( copiedObject->Copy() );
        }
    }
}

// static
void
FOdysseyPainterEditor::PasteObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> pastedObjectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    // TODO: Check why pastedObjectList has to be copied, unclear
    // First copy all objects. This is needed to record their state-before-addition for the UNDO operation.
    for( FOdysseyVectorObject* copiedObject : GetCopiedObjectList() )
    {
        pastedObjectList.push_back( copiedObject->Copy() );
    }

    vectorEngine->ClearObjectSelection();

    // This undo must be set before association with the new parent object
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.paste-objects","Paste"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene
                                                                                                    , pastedObjectList
                                                                                                    , notificationFlags ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        
        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    for( FOdysseyVectorObject* pastedObject : pastedObjectList )
    {
        //BLPoint shifting;

        iScene->AppendChild( pastedObject );

        //shifting = iScene->GetInverseWorldMatrix().mapVector( 10.0f, 10.0f ); // shift object by 10 pixels

        //pastedObject->Invalidate();
        //pastedObject->Translate( newObject->GetTranslationX() + shifting.x, newObject->GetTranslationY() + shifting.y );
        pastedObject->UpdateMatrix();

        vectorEngine->SelectObject( pastedObject );
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    vectorEngine->ResetHUD();
    // request redraw
    vectorEngine->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

// static
void
FOdysseyPainterEditor::MergeScenes( FOdysseyVectorGroupPaint* iDestinationScene
                                  , const TArray<FOdysseyVectorGroupPaint*>& iSourceSceneArray )
{
    FOdysseyVectorEngine* vectorEngine = iDestinationScene->GetEngine();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    for( int i = 0; i < iSourceSceneArray.Num(); i++ )
    {
        for( FOdysseyVectorObject* child : iSourceSceneArray[i]->GetChildrenList() )
        {
            FOdysseyVectorObject* copiedChild = child->Copy();

            iDestinationScene->AppendChild( copiedChild );
        }

        iSourceSceneArray[i]->CopyBuckets( iDestinationScene, false );
    }

    iDestinationScene->UpdateMatrix();
    iDestinationScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    vectorEngine->ResetHUD();
    // request redraw
    vectorEngine->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iDestinationScene, notificationFlags );
}

void
FOdysseyPainterEditor::StitchVertices( FOdysseyPainterEditor* iEditor
                                     , FOdysseyVectorGroupPaint* iScene
                                     , FOdysseyVectorVertex* iVertexA
                                     , FOdysseyVectorVertex* iVertexB )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorVertex* StitchVertex;
    // for undos
    std::vector<FOdysseyVectorPath*> addedPathArray; // stays empty
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FOdysseyVectorPath*> removedPathArray; // receives the merged path if any
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> mergedSegmentArray;
    std::vector<FOdysseyVectorVertex*> mergedVertexArray;
    std::list<FOdysseyVectorObject*>& selectedObjectList = vectorEngine->GetSelectedObjectList();
    FOdysseyVectorPath* mergedPath = nullptr;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( ( iVertexA->GetSegmentCount() == 1 ) && ( iVertexB->GetSegmentCount() == 1 ) )
    {
        if( iVertexA->GetOwnerAsPath() != iVertexB->GetOwnerAsPath() )
        {
            // TODO: remove vertexB->GetPath() from selected objects.
            mergedPath = iVertexB->GetOwnerAsPath();

            iVertexB->GetOwnerAsPath()->GetParent()->RemoveChild( mergedPath );
            iVertexA->GetOwnerAsPath()->Merge( mergedPath, mergedVertexArray, mergedSegmentArray );
            // update the pointer with the newly created vertex's. Note, Merge alters the original vertex's ID.
            iVertexB = mergedVertexArray[iVertexB->GetID()];

            vectorEngine->UnselectObject( mergedPath );

            removedPathArray.push_back( mergedPath );
        }

        StitchVertex = vectorEngine->Stitch( iVertexA, iVertexB, addedSegmentArray, removedSegmentArray, true );

        if( StitchVertex )
        {
            addedVertexArray.push_back( StitchVertex );
            removedVertexArray.push_back( iVertexA );
            removedVertexArray.push_back( iVertexB );

            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.stitch-vertices","Vector Path Stitch Tool"));
            if( GUndo )
            {
                FOdysseyVectorUndo *undo = new FOdysseyVectorUndoPathStitch( iScene
                                                                            , removedPathArray
                                                                            , removedVertexArray
                                                                            , removedSegmentArray
                                                                            , addedPathArray
                                                                            , addedVertexArray
                                                                            , addedSegmentArray
                                                                            , mergedVertexArray
                                                                            , mergedSegmentArray
                                                                            , notificationFlags );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
                TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();
        }
    }

    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    vectorEngine->ResetHUD();
    // request redraw
    vectorEngine->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( iScene, notificationFlags );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyPainterEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FOdysseyEditor::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(mRasterDrawingTool);
    Collector.AddReferencedObject(mRasterEraserTool);
    Collector.AddReferencedObject(mRasterTransformTool);
    Collector.AddReferencedObject(mRasterPrimitiveDrawingTool);
	Collector.AddReferencedObject(mRasterPaintBucketTool);
	Collector.AddReferencedObject(mVectorPrimitiveDrawingTool);
	Collector.AddReferencedObject(mVectorPathDrawingTool);
	Collector.AddReferencedObject(mVectorPathEditTool);
	Collector.AddReferencedObject(mVectorSelectionTool);
    Collector.AddReferencedObject(mVectorScenePanTool);
    Collector.AddReferencedObject(mVectorEraserTool);
    Collector.AddReferencedObject(mVectorPathPushTool);
    Collector.AddReferencedObject(mVectorPathSmoothTool);
    Collector.AddReferencedObject(mVectorPathStitchTool);
	Collector.AddReferencedObject(mVectorPaintBucketTool);
	Collector.AddReferencedObject(mColorPickerTool);
	Collector.AddReferencedObject(mVectorGridTool);
	Collector.AddReferencedObject(mVectorTransformTool);
	Collector.AddReferencedObject(mVectorMatchingTool);
	Collector.AddReferencedObject(mVectorChartTool);
	Collector.AddReferencedObject(mVectorTrajectoryTool);
}

#undef LOCTEXT_NAMESPACE
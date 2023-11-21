// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorCommands.h"
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
#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "Undo/OdysseyVectorUndoSelectVertex.h"
#include "Undo/OdysseyVectorUndoTransferObjects.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"
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

#define LOCTEXT_NAMESPACE "FOdysseyPainterEditor"

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
    , mSelectedTool(nullptr)
    , mVectorHUDFlags(FOdysseyVectorHUD::HUD_MODE_OBJECT)
    , mVectorDrawingFlags(0)
    , mHUDSystem(new FOdysseyHUDSystem())
	, mBrushContexts()
	, mPaintColor(::ULIS::FColor::Black)
	, mRasterDrawingTool(nullptr)
    , mRasterTransformTool(nullptr)
    , mRasterPrimitiveDrawingTool(nullptr)
	, mVectorPrimitiveDrawingTool(nullptr)
    , mVectorPathDrawingTool(nullptr)
    , mVectorPathEditTool(nullptr)
	, mVectorPathCutTool(nullptr)
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
	mRasterPaintBucketTool->BindShortcuts(iToolkit);
    mVectorPathDrawingTool->BindShortcuts(iToolkit);
    mVectorPathEditTool->BindShortcuts(iToolkit);
    mRasterTransformTool->BindShortcuts(iToolkit);
    mRasterPrimitiveDrawingTool->BindShortcuts(iToolkit);
	mVectorPrimitiveDrawingTool->BindShortcuts(iToolkit);
	mVectorPathCutTool->BindShortcuts(iToolkit);
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
    GetGUI()->ExtendMenu(iOwner, iMenuName);

    mRasterDrawingTool->ExtendMenu(iOwner, iMenuName);
    mRasterTransformTool->ExtendMenu(iOwner, iMenuName);
	mRasterPaintBucketTool->ExtendMenu(iOwner, iMenuName);
    mVectorPathDrawingTool->ExtendMenu(iOwner,iMenuName);
    mVectorPathEditTool->ExtendMenu(iOwner,iMenuName);
    mRasterPrimitiveDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPrimitiveDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathCutTool->ExtendMenu(iOwner, iMenuName);
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
    mRasterTransformTool = NewObject<UOdysseyPainterEditorRasterTransformTool>();
	mRasterPaintBucketTool = NewObject<UOdysseyPainterEditorRasterPaintBucketTool>();
    mVectorPathDrawingTool = NewObject<UOdysseyPainterEditorVectorPathDrawingTool>();
    mVectorPathEditTool = NewObject<UOdysseyPainterEditorVectorPathEditTool>();
    mRasterPrimitiveDrawingTool = NewObject<UOdysseyPainterEditorRasterPrimitiveDrawingTool>();
	mVectorPrimitiveDrawingTool = NewObject<UOdysseyPainterEditorVectorPrimitiveDrawingTool>();
	mVectorPathCutTool = NewObject<UOdysseyPainterEditorVectorPathCutTool>();
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

	mRasterDrawingTool->SetEditor(this);
    mRasterTransformTool->SetEditor(this);
	mRasterPaintBucketTool->SetEditor(this);
    mVectorPathDrawingTool->SetEditor(this);
    mVectorPathEditTool->SetEditor(this);
    mRasterPrimitiveDrawingTool->SetEditor(this);
    mVectorPrimitiveDrawingTool->SetEditor(this);
    mVectorPathCutTool->SetEditor(this);
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
	mRasterDrawingTool->SetBrushContexts(mBrushContexts);

    //Default Tools a defined by their position in mTools
    //example: mRasterDrawingTool appears before mRasterTransformTool
    //so mRasterDrawingTool will be the default tool

    //Default Raster Tool
	mTools.Add(mRasterDrawingTool);

    //Default Vector Tool
	mTools.Add(mRasterPaintBucketTool);
    mTools.Add(mVectorPathDrawingTool);

    //Other Raster Tools
    mTools.Add(mRasterTransformTool);
    mTools.Add(mRasterPrimitiveDrawingTool);

    //Other Vector Tools
    mTools.Add(mVectorPrimitiveDrawingTool);
    mTools.Add(mVectorPathEditTool);
    mTools.Add(mVectorPathCutTool);
    mTools.Add(mVectorSelectionTool);
    mTools.Add(mVectorScenePanTool);
    mTools.Add(mVectorEraserTool);
    mTools.Add(mVectorPathPushTool);
    mTools.Add(mVectorPathSmoothTool);
    mTools.Add(mVectorPathStitchTool);
	mTools.Add(mVectorPaintBucketTool);
	mTools.Add(mVectorGridTool);
	mTools.Add(mVectorTransformTool);
    //Generic Tools
	mTools.Add(mColorPickerTool);
}

FSimpleMulticastDelegate&
FOdysseyPainterEditor::OnSelectedToolChanged()
{
    return mOnSelectedToolChanged;
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

UOdysseyPainterEditorVectorPathCutTool*
FOdysseyPainterEditor::GetVectorPathCutTool() const
{
    return mVectorPathCutTool;
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
FOdysseyPainterEditor::GetSelectedTool() const
{
    return mSelectedTool;
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
		
        if (mSelectedTool)
            mSelectedTool->Inactivate();
    }

    if (iSource)
    {
        mSource = iSource;
        mSource->Activate();
        
        if ( mSelectedTool && mSelectedTool->IsActivable() )
        {
            //just reload the tool
            mSelectedTool->Activate();
        }
        else
        {
            //select the best tool
            RefreshCurrentTool();
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

void
FOdysseyPainterEditor::SetSelectedTool(UOdysseyPainterEditorTool* iTool)
{
	if (mSelectedTool)
		mSelectedTool->Inactivate();
		
    mSelectedTool = iTool;

	if (mSelectedTool)
		mSelectedTool->Activate();

    if (LayerStack())
    {
        UOdysseyLayer* currentLayer = LayerStack()->CurrentLayer.Get();
        if (currentLayer)
        {
            UClass* layerClass = currentLayer->GetClass();
            if (!mCurrentToolPerLayerClass.Contains(layerClass))
                mCurrentToolPerLayerClass.Add(layerClass, nullptr);

            mCurrentToolPerLayerClass[layerClass] = mSelectedTool;
        }
    }

    mOnSelectedToolChanged.Broadcast();
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
FOdysseyPainterEditor::RefreshCurrentTool()
{
	UOdysseyLayer* currentLayer = LayerStack()->CurrentLayer.Get();
	if (!currentLayer)
	{
		SetSelectedTool(nullptr);
		return;
	}

	UOdysseyPainterEditorTool* tool = nullptr;

	UClass* layerClass = currentLayer->GetClass();
	if (mCurrentToolPerLayerClass.Contains(layerClass))
		tool = mCurrentToolPerLayerClass[layerClass];
	
	if (!tool || !tool->IsActivable())
		tool = FindDefaultToolForCurrentLayer();

	SetSelectedTool(tool);
}

void
FOdysseyPainterEditor::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	if ( iLayerStack != LayerStack() )
		return;

	RefreshCurrentTool(); //Refresh the current tool when we change layer
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

	if (mSelectedTool)
		mSelectedTool->Tick(iDeltaTime);
}

//--------------------------------------------------------------------------------------
//------ Generic methods for vector layers. currently placed here, although it's not really needed.
//------ This might be put somewhere else, in a CommonFunctions file or something as static methods.

void
FOdysseyPainterEditor::AddEditMenuEntry( FMenuBuilder& iMenuBuilder )
{
    UOdysseyPainterEditorVectorBaseTool* vectorBaseTool = Cast<UOdysseyPainterEditorVectorBaseTool>(mSelectedTool);

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
FOdysseyPainterEditor::BringForward( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorObject* selectedObject = vectorEngine->GetLastSelectedObject();

    if( selectedObject )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("BringForward", "Bring forward"));
        if( GUndo )
        {
           FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTransferObjects( iScene, selectedObject );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        selectedObject->BringForward();

        iScene->Update( 0 );
    }

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY );
}

// static
void
FOdysseyPainterEditor::SendBackward( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorObject* selectedObject = vectorEngine->GetLastSelectedObject();

    if( selectedObject )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("SendBackward", "Send backward"));
        if( GUndo )
        {
           FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTransferObjects( iScene, selectedObject );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        selectedObject->SendBackward();

        iScene->Update( 0 );
    }

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY );
}

// static
void
FOdysseyPainterEditor::ApplyTransformations( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> objectList;

    // concerns only top-most objects of a branch, including the scene
    vectorEngine->GetFocusedAncestorList( objectList );

    // Backup before, for undoing
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("ApplyTransformations", "Apply Transformations"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoApplyTransformations( iScene, objectList );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    for( FOdysseyVectorObject* transformedObject : objectList )
    {
        transformedObject->ApplyTransformations();
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::MakePaintGroup( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorObject*> cubicPathOldParentArray;
    std::vector<FOdysseyVectorBucket*> removedBucketArray;
    std::vector<FOdysseyVectorObject*> cubicPathArray;
    std::list<FOdysseyVectorObject*> objectList;
    FOdysseyVectorGroupPaint* paintGroup;

    // concerns all selected objects of a branch but the scene
    vectorEngine->GetFocusedObjectList( objectList );

    paintGroup = FOdysseyVectorEngine::MakePaintGroupFromObjects( iScene
                                                                , objectList
                                                                , cubicPathArray
                                                                , cubicPathOldParentArray
                                                                , removedBucketArray );

    if( paintGroup )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("GroupPaint", "Group Paint"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoGroup( iScene
                                                                  , paintGroup
                                                                  , cubicPathArray
                                                                  , cubicPathOldParentArray
                                                                  , removedBucketArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        vectorEngine->ClearObjectSelection();
        vectorEngine->SelectObject( paintGroup );

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
    }

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::Ungroup( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorObject* selectedObject = vectorEngine->GetLastSelectedObject();

    if( selectedObject )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
        {
            FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>( selectedObject );
            FOdysseyVectorObject* groupParent = group->GetParent();
            // we work on a copy of the list to be able to delete children while iterating
            std::list<FOdysseyVectorObject*> childrenList = group->GetChildrenList();

            // needed for undos
            GEditor->BeginTransaction(LOCTEXT("Ungroup", "Ungroup"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoUngroup( iScene, group );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
            }
            GEditor->EndTransaction();

            for( FOdysseyVectorObject* child : childrenList )
            {
                groupParent->TransferChild( child, groupParent->GetLastChild() );
            }

            //groupParent->RemoveChild( group );

            vectorEngine->ClearObjectSelection();

            iScene->UpdateMatrix();
            iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
        }
    }

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::Group( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorObject*> objectOldParentArray;
    std::vector<FOdysseyVectorObject*> objectArray;
    std::list<FOdysseyVectorObject*> objectList;
    FOdysseyVectorGroup* group;

    // concerns all selected objects of a branch but the scene
    vectorEngine->GetFocusedObjectList( objectList );

    group = vectorEngine->GroupObjects( iScene, objectList, objectArray, objectOldParentArray );

    if( group )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("Group", "group"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoGroup( iScene
                                                                  , group
                                                                  , objectArray
                                                                  , objectOldParentArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        iScene->Update(0); // no paintgroups to update

        vectorEngine->ClearObjectSelection();
        vectorEngine->SelectObject( group );

    }

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::SelectAllPoints( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> objectList;

    // concerns all selected objects of a branch including implicit selection
    vectorEngine->GetFocusedObjectList( objectList );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("SellectAllPoints","Sellect All Points"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectVertex( iScene, objectList );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
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

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::SelectAllObjects( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("SelectAll", "Select All"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( iScene );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    vectorEngine->SelectAllInSelectionSpace();

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::ResetView( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("ResetView", "Reset view"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, iScene );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iScene->ResetTransform();
    iScene->UpdateMatrix();
    iScene->Update( 0 );

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

// static
void
FOdysseyPainterEditor::UnalignPointSelection( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorVertex*> unalignedVertexArray; // for undoing

    unalignedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot 
    // segment handles coordinates before they'll get aligned.
    vectorEngine->Traverse(
        iScene
      , iScene
      , 0
      , [ iScene
        , vectorEngine
        , &unalignedVertexArray ]( FOdysseyVectorObject* object, uint64 iTraversalFlags ) -> uint64
        {
            if( vectorEngine->ObjectHasFocus( iScene, object, iTraversalFlags ) )
            {
                if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                {
                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                    for( FOdysseyVectorVertex* vertex : path->GetSelectedVertexList() )
                    {
                        if( vertex->IsHandleAligned() == true )
                        {
                            unalignedVertexArray.push_back( vertex );
                        }
                    }
                }

                return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
            }

            return 0;
        } );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("UnalignPointSelection","Unalign Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene, unalignedVertexArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
    //---------- end of undo ------------//

    // the actual unalignment
    for( FOdysseyVectorVertex* vertex : unalignedVertexArray )
    {
        vertex->SetHandleAligned( false );
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // updated invalidated objects

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::AlignPointSelection( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorVertex*> alignedVertexArray; // for undoing

    alignedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot 
    // segment handles coordinates before they'll get aligned.
    vectorEngine->Traverse(
        iScene
      , iScene
      , 0
      , [ iScene
        , vectorEngine
        , &alignedVertexArray ]( FOdysseyVectorObject* object, uint64 iTraversalFlags ) -> uint64
        {
            if( vectorEngine->ObjectHasFocus( iScene, object, iTraversalFlags ) )
            {
                if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                {
                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                    for( FOdysseyVectorVertex* vertex : path->GetSelectedVertexList() )
                    {
                        if( vertex->IsHandleAligned() == false )
                        {
                            alignedVertexArray.push_back( vertex );
                        }
                    }
                }

                return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
            }

            return 0;
        } );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("AlignPointSelection","Align Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene, alignedVertexArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction(); 
    //---------- end of undo ------------//

    // the actual alignment ( alignment is based on the first segment met)
    for( FOdysseyVectorVertex* vertex : alignedVertexArray )
    {
        FOdysseyVectorSegment* segment = vertex->GetFirstSegment();
        // we don't check the validity of the pointer to the segment because a vertex is suppose
        // to always belong to at least 1 segment.
        vertex->AlignHandles( segment->GetHandle( vertex ) );
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // updated invalidated objects

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::DeletePointSelection( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorPath*> removedPathArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorPath*> addedPathArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::list<FOdysseyVectorObject*> objectList;

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

            FOdysseyVectorPath::DeleteVertex( path
                                            , selectedVertexArray
                                            , removedVertexArray
                                            , removedSegmentArray
                                            , removedPathArray
                                            , addedSegmentArray );
        }
    }

    for( int i = 0; i < removedPathArray.size(); i++ )
    {
        FOdysseyVectorPath* path = removedPathArray[i];

        path->GetParent()->RemoveChild( path );
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // updated invalidated objects

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("DeletePointSelection","Delete Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                  , removedPathArray
                                                                  , removedVertexArray
                                                                  , removedSegmentArray
                                                                  , addedPathArray
                                                                  , addedVertexArray
                                                                  , addedSegmentArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::DeleteObjects( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*>& selectedObjectList = vectorEngine->GetSelectedObjectList();
    std::vector<FOdysseyVectorObject*> removedObjectArray;

    removedObjectArray.reserve( selectedObjectList.size() );

    vectorEngine->RemoveObjects( selectedObjectList, removedObjectArray );

    vectorEngine->ClearObjectSelection();

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("DeleteObjects", "Delete Objects"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoRemoveObjects( iScene
                                                                      , removedObjectArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::FlipHorizontal( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> objectList;

    // concerns only the top-most selected objects of a branch, including the scene
    vectorEngine->GetFocusedAncestorList( objectList );
 
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("FlipHorizontal", "Flip Horizontal"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, objectList );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    FOdysseyVectorEngine::FlipObjectsHorizontal( objectList );

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::FlipVertical( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> objectList;

    // concerns only the top-most selected objects of a branch, including the scene
    vectorEngine->GetFocusedAncestorList( objectList );

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("FlipVertical", "Flip Vertical"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, objectList );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    FOdysseyVectorEngine::FlipObjectsVertical( objectList );

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::ClearColoring( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorBucket*> bucketArray;
    std::list<FOdysseyVectorObject*> objectList;

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
    GEditor->BeginTransaction(LOCTEXT("ClearColoring", "Clear Coloring"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( iScene, bucketArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();


    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // re-colorize paint group

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

// static
void
FOdysseyPainterEditor::DeleteBucket( FOdysseyVectorBucket* iBucket )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
    FOdysseyVectorGroupPaint* scene = ownerObject->GetScene();

    if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

        paintGroup->RemoveBucket( iBucket );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("DeleteBucket","Delete Bucket"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( scene, iBucket );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();
    }

    scene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // re-colorize paint group

    scene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}


static void
SetBucketPropagation( FOdysseyVectorBucket* iBucket, bool iPropagate )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
    FOdysseyVectorGroupPaint* scene = ownerObject->GetScene();

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("PropagateBucket","Propagate Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( scene, iBucket );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iBucket->SetPropagated( iPropagate );

    scene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    scene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

// static
void
FOdysseyPainterEditor::PropagateBucket( FOdysseyVectorBucket* iBucket )
{
    SetBucketPropagation( iBucket, true );
}

// static
void
FOdysseyPainterEditor::UnpropagateBucket( FOdysseyVectorBucket* iBucket )
{
    SetBucketPropagation( iBucket, false );
}

static std::list<FOdysseyVectorObject*>&
GetCopiedObjectList()
{
    static std::list<FOdysseyVectorObject*> copiedObjectList;

    return copiedObjectList;
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
FOdysseyPainterEditor::PasteObjects( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*> pastedObjectList;

    // TODO: Check why pastedObjectList has to be copied, unclear
    // First copy all objects. This is needed to record their state-before-addition for the UNDO operation.
    for( FOdysseyVectorObject* copiedObject : GetCopiedObjectList() )
    {
        pastedObjectList.push_back( copiedObject->Copy() );
    }

    vectorEngine->ClearObjectSelection();

    // This undo must be set before association with the new parent object
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("DefaultTool","Paste"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene, pastedObjectList ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    for( FOdysseyVectorObject* pastedObject : pastedObjectList )
    {
        //BLPoint shifting;

        iScene->AppendChild( pastedObject );

        //shifting = iScene->GetInverseWorldMatrix().mapVector( 10.0f, 10.0f ); // shift object by 10 pixels

        pastedObject->Invalidate();
        //pastedObject->Translate( newObject->GetTranslationX() + shifting.x, newObject->GetTranslationY() + shifting.y );
        pastedObject->UpdateMatrix();

        vectorEngine->SelectObject( pastedObject );
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    vectorEngine->ResetHUD();
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

void
FOdysseyPainterEditor::StitchVertices( FOdysseyVectorGroupPaint* iScene
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

    if( ( iVertexA->GetSegmentCount() == 1 ) && ( iVertexB->GetSegmentCount() == 1 ) )
    {
        if( iVertexA->GetPath() != iVertexB->GetPath() )
        {
            // TODO: remove vertexB->GetPath() from selected objects.
            mergedPath = iVertexB->GetPath();

            iVertexB->GetPath()->GetParent()->RemoveChild( mergedPath );
            iVertexA->GetPath()->Merge( mergedPath, mergedVertexArray, mergedSegmentArray );
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
            GEditor->BeginTransaction(LOCTEXT("VectorPathStitchTool","Vector Path Stitch Tool"));
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
                                                                            , mergedSegmentArray );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
            }
            GEditor->EndTransaction();
        }
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyPainterEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FOdysseyEditor::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(mRasterDrawingTool);
    Collector.AddReferencedObject(mRasterTransformTool);
    Collector.AddReferencedObject(mRasterPrimitiveDrawingTool);
	Collector.AddReferencedObject(mRasterPaintBucketTool);
	Collector.AddReferencedObject(mVectorPrimitiveDrawingTool);
	Collector.AddReferencedObject(mVectorPathDrawingTool);
	Collector.AddReferencedObject(mVectorPathEditTool);
	Collector.AddReferencedObject(mVectorPathCutTool);
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
}

#undef LOCTEXT_NAMESPACE
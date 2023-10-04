// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditor.h"
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
#include "Undo/OdysseyVectorUndoSendBackward.h"
#include "Undo/OdysseyVectorUndoBringForward.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"
#include "Undo/OdysseyVectorUndoSceneRemoveSelection.h"
#include "Undo/OdysseyVectorUndoBucketRemove.h"
#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "Undo/OdysseyVectorUndoPathStitch.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "Undo/OdysseyVectorUndoApplyTransformations.h"

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"
#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"
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
    , mVectorEditionMode(eVectorEditionMode::Object)
    , mHUDSystem(new FOdysseyHUDSystem())
	, mBrushContexts()
	, mPaintColor(::ULIS::FColor::Black)
	, mRasterDrawingTool(nullptr)
    , mRasterTransformTool(nullptr)
	, mVectorPrimitiveDrawingTool(nullptr)
    , mVectorPathDrawingTool(nullptr)
    , mVectorPathEditTool(nullptr)
	, mVectorPathCutTool(nullptr)
	, mVectorPickTool(nullptr)
	, mVectorScenePanTool(nullptr)
	, mVectorEraserTool(nullptr)
	, mVectorPathPushTool(nullptr)
	, mVectorPathSmoothTool(nullptr)
	, mVectorPathStitchTool(nullptr)
	, mPaintBucketTool(nullptr)
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
    mVectorPathDrawingTool->BindShortcuts(iToolkit);
    mVectorPathEditTool->BindShortcuts(iToolkit);
    mRasterTransformTool->BindShortcuts(iToolkit);
	mVectorPrimitiveDrawingTool->BindShortcuts(iToolkit);
	mVectorPathCutTool->BindShortcuts(iToolkit);
	mVectorPickTool->BindShortcuts(iToolkit);
	mVectorScenePanTool->BindShortcuts(iToolkit);
	mVectorEraserTool->BindShortcuts(iToolkit);
	mVectorPathPushTool->BindShortcuts(iToolkit);
	mVectorPathSmoothTool->BindShortcuts(iToolkit);
	mVectorPathStitchTool->BindShortcuts(iToolkit);
	mPaintBucketTool->BindShortcuts(iToolkit);
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
    mVectorPathDrawingTool->ExtendMenu(iOwner,iMenuName);
    mVectorPathEditTool->ExtendMenu(iOwner,iMenuName);
	mVectorPrimitiveDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathCutTool->ExtendMenu(iOwner, iMenuName);
	mVectorPickTool->ExtendMenu(iOwner, iMenuName);
	mVectorScenePanTool->ExtendMenu(iOwner, iMenuName);
	mVectorEraserTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathPushTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathSmoothTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathStitchTool->ExtendMenu(iOwner, iMenuName);
	mPaintBucketTool->ExtendMenu(iOwner, iMenuName);
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
    mVectorPathDrawingTool = NewObject<UOdysseyPainterEditorVectorPathDrawingTool>();
    mVectorPathEditTool = NewObject<UOdysseyPainterEditorVectorPathEditTool>();
	mVectorPrimitiveDrawingTool = NewObject<UOdysseyPainterEditorVectorPrimitiveDrawingTool>();
	mVectorPathCutTool = NewObject<UOdysseyPainterEditorVectorPathCutTool>();
	mVectorPickTool = NewObject<UOdysseyPainterEditorVectorPickTool>();
    mVectorScenePanTool = NewObject<UOdysseyPainterEditorVectorScenePanTool>();
    mVectorEraserTool = NewObject<UOdysseyPainterEditorVectorEraserTool>();
    mVectorPathPushTool = NewObject<UOdysseyPainterEditorVectorPathPushTool>();
    mVectorPathSmoothTool = NewObject<UOdysseyPainterEditorVectorPathSmoothTool>();
    mVectorPathStitchTool = NewObject<UOdysseyPainterEditorVectorPathStitchTool>();
	mPaintBucketTool = NewObject<UOdysseyPainterEditorPaintBucketTool>();
	mColorPickerTool = NewObject<UOdysseyPainterEditorColorPickerTool>();
	mVectorGridTool = NewObject<UOdysseyPainterEditorVectorGridTool>();
	mVectorTransformTool = NewObject<UOdysseyPainterEditorVectorTransformTool>();

	mRasterDrawingTool->SetEditor(this);
    mRasterTransformTool->SetEditor(this);
    mVectorPrimitiveDrawingTool->SetEditor(this);
    mVectorPathDrawingTool->SetEditor(this);
    mVectorPathEditTool->SetEditor(this);
    mVectorPrimitiveDrawingTool->SetEditor(this);
    mVectorPathCutTool->SetEditor(this);
    mVectorPickTool->SetEditor(this);
    mVectorScenePanTool->SetEditor(this);
    mVectorEraserTool->SetEditor(this);
    mVectorPathPushTool->SetEditor(this);
    mVectorPathSmoothTool->SetEditor(this);
    mVectorPathStitchTool->SetEditor(this);
	mPaintBucketTool->SetEditor(this);
	mColorPickerTool->SetEditor(this);
	mVectorGridTool->SetEditor(this);
	mVectorTransformTool->SetEditor(this);
	mRasterDrawingTool->SetBrushContexts(mBrushContexts);

	mTools.Add(mRasterDrawingTool);
    mTools.Add(mRasterTransformTool);
    mTools.Add(mVectorPrimitiveDrawingTool);
    mTools.Add(mVectorPathDrawingTool);
    mTools.Add(mVectorPathEditTool);
    mTools.Add(mVectorPrimitiveDrawingTool);
    mTools.Add(mVectorPathCutTool);
    mTools.Add(mVectorPickTool);
    mTools.Add(mVectorScenePanTool);
    mTools.Add(mVectorEraserTool);
    mTools.Add(mVectorPathPushTool);
    mTools.Add(mVectorPathSmoothTool);
    mTools.Add(mVectorPathStitchTool);
	mTools.Add(mPaintBucketTool);
	mTools.Add(mColorPickerTool);
	mTools.Add(mVectorGridTool);
	mTools.Add(mVectorTransformTool);
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

UOdysseyPainterEditorRasterTransformTool* FOdysseyPainterEditor::GetRasterTransformTool() const
{
    return mRasterTransformTool;
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

UOdysseyPainterEditorVectorPickTool*
FOdysseyPainterEditor::GetVectorPickTool() const
{
    return mVectorPickTool;
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

UOdysseyPainterEditorPaintBucketTool*
FOdysseyPainterEditor::GetPaintBucketTool() const
{
    return mPaintBucketTool;
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
FOdysseyPainterEditor::SetVectorEditionMode(eVectorEditionMode iVectorEditionMode)
{
    mVectorEditionMode = iVectorEditionMode;
}

eVectorEditionMode
FOdysseyPainterEditor::GetVectorEditionMode()
{
    return mVectorEditionMode;
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
FOdysseyPainterEditor::BringForward( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

    if( selectedObject )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("BringForward", "Bring forward"));
        if( GUndo )
        {
           FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBringForward( iScene, selectedObject );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        selectedObject->BringForward();

        iScene->Update( 0 );
    }

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY );
}

void
FOdysseyPainterEditor::SendBackward( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

    if( selectedObject )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("SendBackward", "Send backward"));
        if( GUndo )
        {
           FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSendBackward( iScene, selectedObject );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        selectedObject->SendBackward();

        iScene->Update( 0 );
    }

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY );
}


void
FOdysseyPainterEditor::Ungroup( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

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

            for( std::list<FOdysseyVectorObject*>::iterator it = childrenList.begin(); it != childrenList.end(); ++it )
            {
                FOdysseyVectorObject* child = (*it);

                groupParent->TransferChild( child, groupParent->GetLastChild() );
            }

            //groupParent->RemoveChild( group );

            iScene->ClearSelection();
            iScene->UpdateMatrix();
            iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
        }
    }

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

static void
ApplyTransformationsRecursive( FOdysseyVectorObject* iObject )
{
    if( iObject->IsSelected() == true )
    {
        iObject->ApplyTransformations();
    }

    for( FOdysseyVectorObject* child : iObject->GetChildrenList() )
    {
        ApplyTransformationsRecursive( child );
    }
}

void
FOdysseyPainterEditor::ApplyTransformations( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* engine = iScene->GetEngine();
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    std::list<FOdysseyVectorObject*>& focusedObjectList = selectedObjectList.size() ? selectedObjectList :
                                                                                      engine->GetChildrenList();

    // Backup before, for undoing
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("ApplyTransformations", "Apply Transformations"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoApplyTransformations( iScene, focusedObjectList );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();


    if( selectedObjectList.size() == 0 )
    {
        iScene->SetIsSelected( true );
    }

    ApplyTransformationsRecursive( iScene );

    iScene->SetIsSelected( false );

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    engine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    engine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                  | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::GroupPaint( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    std::vector<FOdysseyVectorObject*> cubicPathArray;
    std::vector<FOdysseyVectorObject*> cubicPathOldParentArray;
    std::vector<FOdysseyVectorBucket*> removedBucketArray;
    FOdysseyVectorGroupPaint* paintGroup = iScene->MakePaintGroupFromSelectedObjects( cubicPathArray
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

        iScene->ClearSelection();
        iScene->Select( paintGroup );
        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
    }

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::Group( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{

    std::vector<FOdysseyVectorObject*> objectArray;
    std::vector<FOdysseyVectorObject*> objectOldParentArray;
    FOdysseyVectorGroup* group = iScene->GroupSelectedObjects( objectArray, objectOldParentArray );

    if( group )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("Group", "group"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoGroup( iScene, group, objectArray, objectOldParentArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        iScene->ClearSelection();
        iScene->Select( group );
        iScene->Update( 0 );
    }

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::SelectAll( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("SelectAll", "Select All"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelect( iScene );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iEngine->SelectAllInSelectionSpace();

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::ResetView( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
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

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
FOdysseyPainterEditor::GetVertexSelection( FOdysseyVectorScene* iScene
                                         , std::vector<FOdysseyVectorPoint*>& iSelectedPointArray )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject  = *it;

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

            path->GetSelectedPoints( iSelectedPointArray, ePointSelectionFlags::Vertex );
        }

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            paintGroup->GetSelectedPoints( iSelectedPointArray, ePointSelectionFlags::Vertex );
        }
    }
}

void
FOdysseyPainterEditor::UnalignPointSelection( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorPoint*> selectedPointArray;

    GetVertexSelection( iScene, selectedPointArray );

    for( int i = 0; i < selectedPointArray.size(); i++ )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(selectedPointArray[i]);

        //vertex->UnalignHandles();
    }

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::AlignPointSelection( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    std::vector<FOdysseyVectorPoint*> selectedPointArray;

    GetVertexSelection( iScene, selectedPointArray );

    for( int i = 0; i < selectedPointArray.size(); i++ )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(selectedPointArray[i]);

        //vertex->AlignHandles();
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS ); // updated invalidated objects

    vectorEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                        | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

static void
GroupPaintDeletePoint( FOdysseyVectorGroupPaint* iGroupPaint
                     , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                     , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                     , std::vector<FOdysseyVectorPath*>& iRemovedPathArray
                     , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
{
    std::list<FOdysseyVectorObject*>& childrenList = iGroupPaint->GetChildrenList();
    std::list<FOdysseyVectorObject*>::iterator it;

    for( it = childrenList.begin(); it != childrenList.end(); ++it )
    {
        FOdysseyVectorObject* child = *it;

        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);
            std::vector<FOdysseyVectorPoint*> selectedPointArray;

            path->GetSelectedPoints( selectedPointArray, ePointSelectionFlags::Vertex );

            FOdysseyVectorPath::DeletePoint( path
                                           , selectedPointArray
                                           , iRemovedVertexArray
                                           , iRemovedSegmentArray
                                           , iRemovedPathArray
                                           , iAddedSegmentArray );
        }
    }
}

void
FOdysseyPainterEditor::DeletePointSelection( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    std::vector<FOdysseyVectorPath*> removedPathArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorPath*> addedPathArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;

    removedPathArray.reserve( 10 );
    removedVertexArray.reserve( 10 );
    removedSegmentArray.reserve( 10 );
    addedSegmentArray.reserve( 10 );

    for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject  = *it;

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);
            std::vector<FOdysseyVectorPoint*> selectedPointArray;

            path->GetSelectedPoints( selectedPointArray, ePointSelectionFlags::Vertex );

            FOdysseyVectorPath::DeletePoint( path
                                           , selectedPointArray
                                           , removedVertexArray
                                           , removedSegmentArray
                                           , removedPathArray
                                           , addedSegmentArray );
        }

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            GroupPaintDeletePoint( paintGroup
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

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::DeleteObjectSelection( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("DeleteObjectSelection", "Delete Object Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSceneRemoveSelection( iScene );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iScene->RemoveSelectedObjects();
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::FlipHorizontal( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("FlipHorizontal", "Flip Horizontal"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, iScene->GetSelectedObjectList() );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iScene->FlipSelectionHorizontal( true /*ignored for now*/ );

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::ClearColoring( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    std::list<FOdysseyVectorObject*>& focusedObjectList = selectedObjectList.size() ? selectedObjectList :
                                                                                      iEngine->GetChildrenList();

    std::vector<FOdysseyVectorBucket*> bucketArray;

    bucketArray.reserve( 100 );

    // first step: retrieve all buckets for undoing.
    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        if( focusedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(focusedObject);
            std::list<FOdysseyVectorBucket*>& bucketList = paintgroup->GetBucketList();

            for( FOdysseyVectorBucket* bucket : bucketList )
            {
                bucketArray.push_back( bucket );
            }
        }
    }

    // first step: the actual removal.
    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        if( focusedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(focusedObject);

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

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::FlipVertical( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("FlipVertical", "Flip Vertical"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, iScene->GetSelectedObjectList() );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iScene->FlipSelectionVertical( true /*ignored for now*/ );

    iScene->Update( 0 );

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::DeleteBucket( FOdysseyVectorBucket* iBucket )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
    FOdysseyVectorScene* scene = ownerObject->GetScene();

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
    FOdysseyVectorScene* scene = ownerObject->GetScene();

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

void
FOdysseyPainterEditor::PropagateBucket( FOdysseyVectorBucket* iBucket )
{
    SetBucketPropagation( iBucket, true );
}

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
FOdysseyPainterEditor::CopyObjectSelection( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    if( selectedObjectList.size() )
    {
        // First step : clear previously copied objects
        GetCopiedObjectList().remove_if( []( FOdysseyVectorObject* iCopiedObject ){ delete iCopiedObject; return true; } );

        // second step : copy selection.
        for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* selectedObject = (*it);

            if( selectedObject->HasSelectedAncestor() == false )
            {
                GetCopiedObjectList().push_back( selectedObject->Copy() );
            }
        }
    }
}

// static
void
FOdysseyPainterEditor::PasteObjectSelection( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    std::list<FOdysseyVectorObject*> pastedObjectList;

    // First copy all objects. This is needed to record their state-before-addition for the UNDO operation.
    for( std::list<FOdysseyVectorObject*>::iterator it = GetCopiedObjectList().begin(); it != GetCopiedObjectList().end(); ++it )
    {
        FOdysseyVectorObject* copiedObject = (*it);

        pastedObjectList.push_back( copiedObject->Copy() );
    }

    iScene->ClearSelection();

    // This undo must be set before association with the new parent object
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("DefaultTool","Paste"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene, pastedObjectList ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    for( std::list<FOdysseyVectorObject*>::iterator it = pastedObjectList.begin(); it != pastedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* pastedObject = (*it);
        //BLPoint shifting;

        iScene->AppendChild( pastedObject );

        //shifting = iScene->GetInverseWorldMatrix().mapVector( 10.0f, 10.0f ); // shift object by 10 pixels

        pastedObject->Invalidate();
        //pastedObject->Translate( newObject->GetTranslationX() + shifting.x, newObject->GetTranslationY() + shifting.y );
        pastedObject->UpdateMatrix();

        iScene->Select( pastedObject );
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->ResetHUD();
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

void
FOdysseyPainterEditor::StitchVertices( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    std::vector<FOdysseyVectorVertex*> pickedVertexArray;
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
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    std::list<FOdysseyVectorObject*>::iterator oit;

    pickedVertexArray.reserve( 2 );

    for( oit = selectedObjectList.begin(); oit != selectedObjectList.end(); ++oit )
    {
        FOdysseyVectorObject* selectedObject = (*oit);

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(selectedObject);
            std::list<FOdysseyVectorVertex*>& selectedVertexList = selectedPath->GetSelectedVertexList();
            std::list<FOdysseyVectorVertex*>::iterator vit;

            for( vit = selectedVertexList.begin(); vit != selectedVertexList.end(); ++vit )
            {
                FOdysseyVectorVertex* vertex = (*vit);

                if( vertex->IsSelected() )
                {
                    pickedVertexArray.push_back( vertex );
                }
            }
        }
    }

    if( pickedVertexArray.size() == 2 )
    {
        FOdysseyVectorVertex* vertexA = pickedVertexArray[0];
        FOdysseyVectorVertex* vertexB = pickedVertexArray[1];
        FOdysseyVectorPath* mergedPath = nullptr;

        if( ( vertexA->GetSegmentCount() == 1 ) && ( vertexB->GetSegmentCount() == 1 ) )
        {
            if( vertexA->GetPath() != vertexB->GetPath() )
            {
                // TODO: remove vertexB->GetPath() from selected objects.
                mergedPath = vertexB->GetPath();

                vertexB->GetPath()->GetParent()->RemoveChild( mergedPath );
                vertexA->GetPath()->Merge( mergedPath, mergedVertexArray, mergedSegmentArray );
                // update the pointer with the newly created vertex's. Note, Merge alters the original vertex's ID.
                vertexB = mergedVertexArray[vertexB->GetID()];

                iScene->Unselect( mergedPath );

                removedPathArray.push_back( mergedPath );
            }

            StitchVertex = iEngine->Stitch( vertexA, vertexB, addedSegmentArray, removedSegmentArray, true );

            if( StitchVertex )
            {
                addedVertexArray.push_back( StitchVertex );
                removedVertexArray.push_back( vertexA );
                removedVertexArray.push_back( vertexB );

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
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
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
	Collector.AddReferencedObject(mVectorPrimitiveDrawingTool);
	Collector.AddReferencedObject(mVectorPathDrawingTool);
	Collector.AddReferencedObject(mVectorPathEditTool);
	Collector.AddReferencedObject(mVectorPathCutTool);
	Collector.AddReferencedObject(mVectorPickTool);
    Collector.AddReferencedObject(mVectorScenePanTool);
    Collector.AddReferencedObject(mVectorEraserTool);
    Collector.AddReferencedObject(mVectorPathPushTool);
    Collector.AddReferencedObject(mVectorPathSmoothTool);
    Collector.AddReferencedObject(mVectorPathStitchTool);
	Collector.AddReferencedObject(mPaintBucketTool);
	Collector.AddReferencedObject(mColorPickerTool);
	Collector.AddReferencedObject(mVectorGridTool);
	Collector.AddReferencedObject(mVectorTransformTool);
}

#undef LOCTEXT_NAMESPACE
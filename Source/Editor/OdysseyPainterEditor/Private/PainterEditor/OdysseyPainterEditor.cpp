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
#include "UObject/OdysseyObjectEditorUtils.h"

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

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickTool.h"
#include "Tools/VectorSceneScaleTool/OdysseyPainterEditorVectorSceneScaleTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathWidthTool/OdysseyPainterEditorVectorPathWidthTool.h"
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
    //mToolContext->Unset();
	delete mHUDSystem;
}

FOdysseyPainterEditor::FOdysseyPainterEditor()
    : mSource(nullptr)
    , mSelectedTool(nullptr)
    , mVectorEditionMode(eVectorEditionMode::Object)
    , mHUDSystem(new FOdysseyHUDSystem())
	, mBrushContexts()
	, mPaintColor(::ULIS::FColor::Black)
    //, mToolContext()
	, mRasterDrawingTool(nullptr)
	, mVectorPrimitiveDrawingTool(nullptr)
	, mVectorPathDrawingTool(nullptr)
	, mVectorPathEditTool(nullptr)
	, mVectorPathCutTool(nullptr)
	, mVectorPickTool(nullptr)
	, mVectorSceneScaleTool(nullptr)
	, mVectorScenePanTool(nullptr)
	, mVectorEraserTool(nullptr)
	, mVectorPathPushTool(nullptr)
	, mVectorPathWidthTool(nullptr)
	, mVectorPathSmoothTool(nullptr)
	, mVectorPathStitchTool(nullptr)
	, mPaintBucketTool(nullptr)
	, mColorPickerTool(nullptr)
	, mVectorGridTool(nullptr)
	, mVectorTransformTool(nullptr)
{
    //mToolContext = MakeShared<FOdysseyPainterEditorToolContext>(this);
	mBrushContexts.Add(new FOdysseyPainterEditorBrushContext(this));
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyPainterEditor::InitData(UObject* iEditedObject)
{
	//Init Tools
	InitTools();
}

void
FOdysseyPainterEditor::InitTools()
{
    mRasterDrawingTool = NewObject<UOdysseyPainterEditorRasterDrawingTool>();
	mVectorPrimitiveDrawingTool = NewObject<UOdysseyPainterEditorVectorPrimitiveDrawingTool>();
	mVectorPathDrawingTool = NewObject<UOdysseyPainterEditorVectorPathDrawingTool>();
	mVectorPathEditTool = NewObject<UOdysseyPainterEditorVectorPathEditTool>();
	mVectorPathCutTool = NewObject<UOdysseyPainterEditorVectorPathCutTool>();
	mVectorPickTool = NewObject<UOdysseyPainterEditorVectorPickTool>();
    mVectorSceneScaleTool = NewObject<UOdysseyPainterEditorVectorSceneScaleTool>();
    mVectorScenePanTool = NewObject<UOdysseyPainterEditorVectorScenePanTool>();
    mVectorEraserTool = NewObject<UOdysseyPainterEditorVectorEraserTool>();
    mVectorPathPushTool = NewObject<UOdysseyPainterEditorVectorPathPushTool>();
    mVectorPathWidthTool = NewObject<UOdysseyPainterEditorVectorPathWidthTool>();
    mVectorPathSmoothTool = NewObject<UOdysseyPainterEditorVectorPathSmoothTool>();
    mVectorPathStitchTool = NewObject<UOdysseyPainterEditorVectorPathStitchTool>();
	mPaintBucketTool = NewObject<UOdysseyPainterEditorPaintBucketTool>();
	mColorPickerTool = NewObject<UOdysseyPainterEditorColorPickerTool>();
	mVectorGridTool = NewObject<UOdysseyPainterEditorVectorGridTool>();
	mVectorTransformTool = NewObject<UOdysseyPainterEditorVectorTransformTool>();

	mRasterDrawingTool->SetEditor(this);
    mVectorPrimitiveDrawingTool->SetEditor(this);
    mVectorPathDrawingTool->SetEditor(this);
    mVectorPathEditTool->SetEditor(this);
    mVectorPathCutTool->SetEditor(this);
    mVectorPickTool->SetEditor(this);
    mVectorSceneScaleTool->SetEditor(this);
    mVectorScenePanTool->SetEditor(this);
    mVectorEraserTool->SetEditor(this);
    mVectorPathPushTool->SetEditor(this);
    mVectorPathWidthTool->SetEditor(this);
    mVectorPathSmoothTool->SetEditor(this);
    mVectorPathStitchTool->SetEditor(this);
	mPaintBucketTool->SetEditor(this);
	mColorPickerTool->SetEditor(this);
	mVectorGridTool->SetEditor(this);
	mVectorTransformTool->SetEditor(this);
	mRasterDrawingTool->SetBrushContexts(mBrushContexts);

	mRasterDrawingTool->SetEditor(this);
    mVectorPrimitiveDrawingTool->SetEditor(this);
    mVectorPathDrawingTool->SetEditor(this);
    mVectorPathEditTool->SetEditor(this);
    mVectorPathCutTool->SetEditor(this);
    mVectorPickTool->SetEditor(this);
    mVectorSceneScaleTool->SetEditor(this);
    mVectorScenePanTool->SetEditor(this);
    mVectorEraserTool->SetEditor(this);
    mVectorPathPushTool->SetEditor(this);
    mVectorPathWidthTool->SetEditor(this);
    mVectorPathSmoothTool->SetEditor(this);
    mVectorPathStitchTool->SetEditor(this);
	mPaintBucketTool->SetEditor(this);
	mColorPickerTool->SetEditor(this);
	mVectorGridTool->SetEditor(this);
	mVectorTransformTool->SetEditor(this);
	mRasterDrawingTool->SetEditor(this);

	mTools.Add(mRasterDrawingTool);
    mTools.Add(mVectorPrimitiveDrawingTool);
    mTools.Add(mVectorPathDrawingTool);
    mTools.Add(mVectorPathEditTool);
    mTools.Add(mVectorPathCutTool);
    mTools.Add(mVectorPickTool);
    mTools.Add(mVectorSceneScaleTool);
    mTools.Add(mVectorScenePanTool);
    mTools.Add(mVectorEraserTool);
    mTools.Add(mVectorPathPushTool);
    mTools.Add(mVectorPathWidthTool);
    mTools.Add(mVectorPathSmoothTool);
    mTools.Add(mVectorPathStitchTool);
	mTools.Add(mPaintBucketTool);
	mTools.Add(mColorPickerTool);
	mTools.Add(mVectorGridTool);
	mTools.Add(mVectorTransformTool);
}

void
FOdysseyPainterEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyEditor::BindShortcuts(iToolkit);

	//---

	const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

	#define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateRaw( this, &FOdysseyPainterEditor::__VA_ARGS__ ), FCanExecuteAction() );

	/* MAP_ACTION(painterEditorCommands.Undo, Undo)
	MAP_ACTION(painterEditorCommands.Redo, Redo )
    MAP_ACTION(painterEditorCommands.ClearUndo, ClearUndo ) */

	#undef MAP_ACTION
}

void
FOdysseyPainterEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	FOdysseyEditor::ExtendMenu(iOwner, iMenuName);
}

bool
FOdysseyPainterEditor::OnCloseRequested()
{
    //Cleanup
    if (mSelectedTool)
        mSelectedTool->Inactivate();
    
    if (mSource)
    {
        mSource->Inactivate();
        mSource = nullptr;
    }

    return FOdysseyEditor::OnCloseRequested();
}

void
FOdysseyPainterEditor::OnSelectedToolChanged()
{
}

FSimpleMulticastDelegate&
FOdysseyPainterEditor::OnSelectedToolChangedDelegate()
{
    return mOnSelectedToolChanged;
}

TSharedPtr<FOdysseyPainterEditorSource>
FOdysseyPainterEditor::GetSource() const
{
    return mSource;
}

void
FOdysseyPainterEditor::OnSourceInactivated()
{

}

void
FOdysseyPainterEditor::OnSourceActivated()
{

}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyPainterEditorRasterDrawingTool*
FOdysseyPainterEditor::GetRasterDrawingTool() const
{
    return mRasterDrawingTool;
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

UOdysseyPainterEditorVectorSceneScaleTool*
FOdysseyPainterEditor::GetVectorSceneScaleTool() const
{
    return mVectorSceneScaleTool;
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

UOdysseyPainterEditorVectorPathWidthTool*
FOdysseyPainterEditor::GetVectorPathWidthTool() const
{
    return mVectorPathWidthTool;
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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyPainterEditor::SetSource(TSharedPtr<FOdysseyPainterEditorSource> iSource)
{
    if (mSource)
    {
        mSource->Inactivate();
        mSource = nullptr;
        OnSourceInactivated();
    }

    if (iSource)
    {
        mSource = iSource;
        mSource->Activate();
        OnSourceActivated();
    }
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
    
    OnSelectedToolChanged();
    mOnSelectedToolChanged.Broadcast();
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

        selectedObject->MoveFront();

        iScene->Update( 0 );
    }

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
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

        selectedObject->MoveBack();

        iScene->Update( 0 );
    }

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
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

                group->GetParent()->TransferChild( child );
            }

            group->GetParent()->RemoveChild( group );

            iScene->ClearSelection();
            iScene->UpdateMatrix();
            iScene->Update( 0 );
        }
    }

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::GroupPaint( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    std::vector<FOdysseyVectorObject*> cubicPathArray;
    std::vector<FOdysseyVectorObject*> cubicPathOldParentArray;
    std::vector<FOdysseyVectorObject*> removedPaintGroupArray;
    FOdysseyVectorGroupPaint* paintGroup = iScene->MakePaintGroupFromSelectedObjects( cubicPathArray
                                                                                    , cubicPathOldParentArray
                                                                                    , removedPaintGroupArray );

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
                                                                  , removedPaintGroupArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        iScene->ClearSelection();
        iScene->Select( paintGroup );
        iScene->Update( 0 );
    }

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
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
FOdysseyPainterEditor::DeleteSelection( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("DeleteSelection", "Delete Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSceneRemoveSelection( iScene );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iScene->RemoveSelectedObjects();
    iScene->Update( 0 );

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::FlipHorizontal( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    std::vector<FObjectTransform> objecTransformArray;

    FObjectTransform::MakeArrayFromObjectList( iScene->GetSelectedObjectList(), objecTransformArray );

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("FlipHorizontal", "Flip Horizontal"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, objecTransformArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iScene->FlipSelectionHorizontal( true /*ignored for now*/ );

    iScene->Update( 0 );

    iEngine->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyPainterEditor::FlipVertical( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    std::vector<FObjectTransform> objecTransformArray;

    FObjectTransform::MakeArrayFromObjectList( iScene->GetSelectedObjectList(), objecTransformArray );

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("FlipVertical", "Flip Vertical"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, objecTransformArray );

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

    if( ownerObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

        paintGroup->RemoveBucket( iBucket );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("DeleteBucket","Delete Bucket"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( scene, paintGroup, iBucket );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();
    }

    scene->Update( 0 ); // re-colorize paint group
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

    scene->Update( 0 ); // re-colorize paint group
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

    iScene->Update( 0 );

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
	Collector.AddReferencedObject(mVectorPrimitiveDrawingTool);
	Collector.AddReferencedObject(mVectorPathDrawingTool);
	Collector.AddReferencedObject(mVectorPathEditTool);
	Collector.AddReferencedObject(mVectorPathCutTool);
	Collector.AddReferencedObject(mVectorPickTool);
    Collector.AddReferencedObject(mVectorSceneScaleTool);
    Collector.AddReferencedObject(mVectorScenePanTool);
    Collector.AddReferencedObject(mVectorEraserTool);
    Collector.AddReferencedObject(mVectorPathPushTool);
    Collector.AddReferencedObject(mVectorPathWidthTool);
    Collector.AddReferencedObject(mVectorPathSmoothTool);
    Collector.AddReferencedObject(mVectorPathStitchTool);
	Collector.AddReferencedObject(mPaintBucketTool);
	Collector.AddReferencedObject(mColorPickerTool);
	Collector.AddReferencedObject(mVectorGridTool);
	Collector.AddReferencedObject(mVectorTransformTool);
}

#undef LOCTEXT_NAMESPACE
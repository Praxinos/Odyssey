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

#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoGroup.h"
#include "Undo/OdysseyVectorUndoUngroup.h"
#include "Undo/OdysseyVectorUndoSendBackward.h"
#include "Undo/OdysseyVectorUndoBringForward.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"
#include "Undo/OdysseyVectorUndoSceneRemoveSelection.h"

#define LOCTEXT_NAMESPACE "FOdysseyPainterEditor"

/////////////////////////////////////////////////////
// FOdysseyPainterEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyPainterEditor::~FOdysseyPainterEditor()
{
	delete mHUDSystem;
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
FOdysseyPainterEditor::InitData(UObject* iEditedObject)
{
	//Init Tools
	InitTools();
	SelectDefaultTool();
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

    return FOdysseyEditor::OnCloseRequested();
}

FOdysseyPainterEditor::FOnSelectedToolChange&
FOdysseyPainterEditor::OnSelectedToolChangedDelegate()
{
    return mOnSelectedToolChange;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

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

    mOnSelectedToolChange.Broadcast(iTool);
}

void
FOdysseyPainterEditor::SelectDefaultTool()
{
	if ( mSelectedTool && mSelectedTool->IsActivable() )
		return;

	UOdysseyPainterEditorTool* toolToSelect = nullptr;
    for (UOdysseyPainterEditorTool* tool : mTools)
    {
		if ( !tool->IsActivable() )
			continue;

		toolToSelect = tool;
		break;
    }

	SetSelectedTool(toolToSelect);
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );
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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );
}

#undef LOCTEXT_NAMESPACE
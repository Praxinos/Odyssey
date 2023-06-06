// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectPickTool/OdysseyPainterEditorVectorObjectPickTool.h"
#include <chrono>

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorObjectPickTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectPickTool::~UOdysseyPainterEditorVectorObjectPickTool()
{
    delete mSelectionHUD;
}

UOdysseyPainterEditorVectorObjectPickTool::UOdysseyPainterEditorVectorObjectPickTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Lasso64");

    mSelectionHUD = new FOdysseyVectorHUDSelection( );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectPickTool::ActivateVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , int32 iSizeX
                                                         , int32 iSizeY )
{
    mSelectionHUD->Init( iSizeX, iSizeY );
    mSelectionHUD->UpdateSelectionBox( iScene );

    iEngine->ClearHUD( );
    iEngine->AddHUD( mSelectionHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNALL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FKey& iKey )
{
    // Note: this also calls iScene->Update(0)
    UOdysseyPainterEditorDefaultTool::OnKeyDownVector( iEngine, iScene, iKey );

    return false;
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FKey& iKey )
{
    // Note: this also calls iScene->Update(0)
    UOdysseyPainterEditorDefaultTool::OnKeyUpVector( iEngine, iScene, iKey );

    return false;
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    mPressedMouseCoords.x = iPointInTexture.x;
    mPressedMouseCoords.y = iPointInTexture.y;

    mPointArray.clear();

    mSelectionHUD->SetSelecting( true, &mPointArray );

    mPointArray.push_back( ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y ) );

    iScene->Signal( FOdysseyVectorScene::SIGNALL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorObjectPickTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

    mPointArray.push_back( point );

    iScene->Signal( FOdysseyVectorScene::SIGNALL_SCENE_REDRAW );
}

static void
SetSelectionSpace( FOdysseyVectorEngine* iVectorEngine, FOdysseyVectorObject* iSelectedObject )
{
    if( UOdysseyPainterEditorDefaultTool::DoubleClicked() == true )
    {
        FOdysseyVectorGroup* selectedGroup = nullptr;

        if( iSelectedObject )
        {
            if( iSelectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
            {
                selectedGroup = static_cast<FOdysseyVectorGroup*>(iSelectedObject);
            }
        }

        iVectorEngine->SetSelectionSpace( selectedGroup );
    }
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    std::vector<FOdysseyVectorObject*> pickedObjectArray;
    FOdysseyVectorUndoSelect* undoSelect = nullptr;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorObjectPickTool","Vector Object Pick Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelect( iScene );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    // deselect all if control key is not pressed
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() == false )
    {
        iScene->ClearSelection();
    }

    if ( mPointArray.size() == 1 )
    {
        iEngine->Pick( iScene, mPointArray, pickedObjectArray, FOdysseyVectorObject::PICK_MATH_BASED );
    }

    if ( mPointArray.size() > 1 )
    {
        iEngine->Pick( iScene, mPointArray, pickedObjectArray, FOdysseyVectorObject::PICK_MASK_BASED );
    }

    // if no dragging occured, we only select the object that is the most forward
    if( ( iPointInTexture.x == mPressedMouseCoords.x )
     && ( iPointInTexture.y == mPressedMouseCoords.y ) )
    {
        if( pickedObjectArray.size() )
        {
            iScene->Select( pickedObjectArray.back() );
        }
    }
    // otherwise we select all objects lying in the selection area
    else
    {
        for ( int i = 0; i < pickedObjectArray.size(); i++ )
        {
            iScene->Select( pickedObjectArray[i] );
        }
    }

    SetSelectionSpace( iEngine, iScene->GetLastSelected() );

    mSelectionHUD->SetSelecting( false, nullptr );
    mSelectionHUD->UpdateSelectionBox( iScene );

    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNALL_SCENE_REDRAW | FOdysseyVectorScene::SIGNALL_OBJECT_SELECTED );

    return true;
}

void
UOdysseyPainterEditorVectorObjectPickTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE

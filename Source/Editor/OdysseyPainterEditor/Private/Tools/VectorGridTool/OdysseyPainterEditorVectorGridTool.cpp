// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorGridTool"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorGridTool::~UOdysseyPainterEditorVectorGridTool()
{
}

UOdysseyPainterEditorVectorGridTool::UOdysseyPainterEditorVectorGridTool()
    : mMultipleSelectionMode( false )
    , DivisionsX( 4 )
    , DivisionsY( 4 )
    , PickingRadius( 10.0f )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Grid64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorGridTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( &mGridHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorGridTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->AddHUD( &mGridHUD );

    mGridHUD.MakeGrid( iScene, DivisionsX, DivisionsY );
    mGridHUD.Export( mPointArray );
    // clear selected nodes
    mGridNodeArray.clear();

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorGridTool","Vector Grid Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene, mPointArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    for( int i = 0; i < iPointInTexture.keysDown.Num(); i++ )
    {
        if( ( iPointInTexture.keysDown[i] == EKeys::LeftShift ) || ( iPointInTexture.keysDown[i] == EKeys::RightShift ) )
        {
            mMultipleSelectionMode = true;
        }
    }

    // multiple selection mode
    if ( mMultipleSelectionMode == true )
    {
        mMultipleSelectionMode = true;

        mGridHUD.StartSelectionRectangle( iPointInTexture.x, iPointInTexture.y );
    }
    else
    {
        FGridNode* gridNode = mGridHUD.PickNode( iPointInTexture.x, iPointInTexture.y, PickingRadius );

        mGridNodeArray.clear();

        if( gridNode )
        {
            mGridNodeArray.push_back( gridNode );
        }
    }

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture )
{
    if( mMultipleSelectionMode == true )
    {
        mGridHUD.DragSelectionRectangle( iPointInTexture.x, iPointInTexture.y );
    }
    else
    {
        FSelectionBox& selectionBox = mGridHUD.GetSelectionBox();

        if( selectionBox.space )
        {
            BLPoint spaceDif = selectionBox.space->GetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                                                    , iPointInTexture.deltaPosition.Y );

            for( int i = 0; i < mGridNodeArray.size(); i++ )
            {
                mGridNodeArray[i]->Set( mGridNodeArray[i]->GetX() + spaceDif.x, mGridNodeArray[i]->GetY() + spaceDif.y );
            }

            mGridHUD.Deform();

            // update invalidated objects
            iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );
        }
    }

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    if( mMultipleSelectionMode == true )
    {
        mGridNodeArray.clear();

        mGridHUD.EndSelectionRectangle( mGridNodeArray );
    }

    iScene->Update( 0 );
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    mMultipleSelectionMode = false;

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::Commit()
{

}

void
UOdysseyPainterEditorVectorGridTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FName& iPropertyName )
{
    mGridHUD.MakeGrid( iScene, DivisionsX, DivisionsY );
}

#undef LOCTEXT_NAMESPACE

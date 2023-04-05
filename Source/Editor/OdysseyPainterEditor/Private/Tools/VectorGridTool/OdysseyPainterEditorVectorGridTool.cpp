// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorGridTool::~UOdysseyPainterEditorVectorGridTool()
{
}

UOdysseyPainterEditorVectorGridTool::UOdysseyPainterEditorVectorGridTool()
    : DivisionsX( 4 )
    , DivisionsY( 4 )
    , PickingRadius( 10.0f )
    , mMultipleSelectionMode( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Grid64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorGridTool::Activate( FOdysseyVectorEngine* iEngine
                                             , FOdysseyVectorScene* iScene )
{
    mGridHUD.MakeGrid( iScene, DivisionsX, DivisionsY );
    mGridHUD.Export( mPointArray );

    mGridNodeArray.clear();

    iEngine->ClearHUD( );
    iEngine->AddHUD( &mGridHUD );

    mUndoPointPosition = nullptr;
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                , FOdysseyVectorScene* iScene
                                                , FOdysseyVectorUndo** iUndo
                                                , const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
    if( iUndo && GUndo )
    {
        mUndoPointPosition = new FOdysseyVectorUndoPointPosition( iScene );

        mUndoPointPosition->RecordBefore( mPointArray );

        (*iUndo) = mUndoPointPosition;

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(*iUndo) );
    }

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

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
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

            iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );
        }
    }
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                              , FOdysseyVectorScene* iScene
                                              , const FOdysseyPoint& iPointInTexture
                                              , const FKey& iKey )
{
    if( mUndoPointPosition )
    {
        mUndoPointPosition->RecordAfter( mPointArray );
    }

    if( mMultipleSelectionMode == true )
    {
        mGridNodeArray.clear();

        mGridHUD.EndSelectionRectangle( mGridNodeArray );
    }

    iScene->Update( 0 );

    mMultipleSelectionMode = false;

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::Commit()
{

}

void
UOdysseyPainterEditorVectorGridTool::PropertyChanged( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FName& iPropertyName )
{
    mGridHUD.MakeGrid( iScene, DivisionsX, DivisionsY );
}

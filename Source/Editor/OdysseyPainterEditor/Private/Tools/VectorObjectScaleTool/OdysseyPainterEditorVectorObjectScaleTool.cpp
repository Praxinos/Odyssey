// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectScaleTool/OdysseyPainterEditorVectorObjectScaleTool.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorObjectScaleTool"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectScaleTool::~UOdysseyPainterEditorVectorObjectScaleTool()
{
}

UOdysseyPainterEditorVectorObjectScaleTool::UOdysseyPainterEditorVectorObjectScaleTool()
    : Uniform( true )
    , PickingRadius( 25.0f )
    , mTransformHUD()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectScaleTool64");
}

void
UOdysseyPainterEditorVectorObjectScaleTool::FitHUD( FOdysseyVectorScene* iScene )
{
    mTransformHUD.UpdateSelectionBox( iScene, false );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectScaleTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( &mTransformHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorObjectScaleTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( &mTransformHUD );

    FitHUD( iScene );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorObjectScaleTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    FSelectionBox& selectionBox = mTransformHUD.GetSelectionBox();

    if( selectionBox.space )
    {
        BLPoint localCoords = selectionBox.space->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

        mPickedHandle = mTransformHUD.Pick( iPointInTexture.x, iPointInTexture.y, PickingRadius );

        mOldLocalMouseX = localCoords.x;
        mOldLocalMouseY = localCoords.y;
    }

/*
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorObjectScaleTool","Vector Object Scale Tool"));
    if( GUndo )
    {
        // save selected object translation/rotation/scaling before transform
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, iScene->GetSelectedObjectList() );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
*/
    // remember for undos. we don't set undos in the mouse down event because it could conflict with the undo created by
    // UOdysseyPainterEditorVectorObjectPickTool::OnMouseUpVector() called when no dragging was made.
    mObjectTransformArray.clear();
    FObjectTransform::MakeArrayFromObjectList( iScene->GetSelectedObjectList(), mObjectTransformArray );

    mDragging = false;

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

::ULIS::FRectI
UOdysseyPainterEditorVectorObjectScaleTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD.GetSelectionBox();
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };

    mDragging = true;

    if ( selectionBox.space )
    {
        BLPoint localCoords = selectionBox.space->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
        double difx = localCoords.x - mOldLocalMouseX;
        double dify = localCoords.y - mOldLocalMouseY;
        //::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );
        //::ULIS::FRectD localBBox = selectedObject->GetBBox( false );
        double oldX1 = selectionBox.rect.x
             , oldY1 = selectionBox.rect.y
             , oldX2 = selectionBox.rect.x + selectionBox.rect.w
             , oldY2 = selectionBox.rect.y + selectionBox.rect.h;
        double oldDiagonal = sqrt( ( selectionBox.rect.w * selectionBox.rect.w )
                                 + ( selectionBox.rect.h * selectionBox.rect.h ) );
        double x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
        ::ULIS::FVec2D pivot;

        if ( mPickedHandle == 0 )
        {
            x1 = localCoords.x;
            y1 = localCoords.y;
            x2 = oldX2;
            y2 = oldY2;

            pivot.x = oldX2;
            pivot.y = oldY2;
        }

        if ( mPickedHandle == 1 )
        {
            x1 = oldX1;
            y1 = localCoords.y;
            x2 = localCoords.x;
            y2 = oldY2;

            pivot.x = oldX1;
            pivot.y = oldY2;
        }

        if ( mPickedHandle == 2 )
        {
            x1 = oldX1;
            y1 = oldY1;
            x2 = localCoords.x;
            y2 = localCoords.y;

            pivot.x = oldX1;
            pivot.y = oldY1;
        }

        if ( mPickedHandle == 3 )
        {
            x1 = localCoords.x;
            y1 = oldY1;
            x2 = oldX2;
            y2 = localCoords.y;

            pivot.x = oldX2;
            pivot.y = oldY1;
        }

        if( mPickedHandle != -1 )
        {
            BLMatrix2D spaceMatrix = selectionBox.space->GetWorldMatrix();
            BLMatrix2D invertSpaceMatrix;
            double x2mx1 = ( x2 - x1 );
            double y2my1 = ( y2 - y1 );
            BLMatrix2D scalingMatrix;

            spaceMatrix.translate( pivot.x, pivot.y );

            BLMatrix2D::invert( invertSpaceMatrix, spaceMatrix );

            scalingMatrix.reset();

            if( Uniform )
            {
                double newDiagonal = sqrt( ( x2mx1 * x2mx1 ) + ( y2my1 * y2my1 ) );
                double ratio = newDiagonal / oldDiagonal;

                scalingMatrix.scale( ratio, ratio );
            }
            else
            {
                scalingMatrix.scale( x2mx1 / selectionBox.rect.w, y2my1 / selectionBox.rect.h );
            }

            for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
            {
                FOdysseyVectorObject* object = (*it);

                if ( object->HasSelectedAncestor() == false )
                {
                    double translationX;
                    double translationY;
                    double rotation;
                    double scalingX;
                    double scalingY;
                    BLMatrix2D objectSpaceMatrix;
                    BLMatrix2D objectScaledMatrix;
                    BLMatrix2D objectLocalMatrix;
                    BLMatrix2D objectWorldMatrix = object->GetWorldMatrix();
                    BLMatrix2D parentInverseWorldMatrix = object->GetParent()->GetInverseWorldMatrix();

                    // transfer object in "Scaling Space" coordinates system
                    FOdysseyVector::MatrixMultiply( invertSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

                    // scale the object (local to the "Scaling Space" coordinates system)
                    FOdysseyVector::MatrixMultiply( scalingMatrix, objectSpaceMatrix, objectScaledMatrix );

                    // transfer the object back to world coordinates system
                    FOdysseyVector::MatrixMultiply( spaceMatrix, objectScaledMatrix, objectWorldMatrix );

                    // Convert the object to its parent coordinate system, i.e its local coordinates system.
                    FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, objectWorldMatrix, objectLocalMatrix );

                    // Extract the local transformations
                    FOdysseyVector::ExtractTransformations( objectLocalMatrix
                                                            , &translationX
                                                            , &translationY
                                                            , &rotation // in radians
                                                            , &scalingX
                                                            , &scalingY );

                    // Apply the local transformations
                    object->Translate( translationX, translationY );
                    object->Rotate( rotation / M_PI * 180 );
                    object->Scale( scalingX, scalingY );
                }
            }
        }

        // Update the matrix for all objects
        iScene->UpdateMatrix();

        // update the selection box with the newly modified matrices
        mTransformHUD.UpdateSelectionBox( iScene, false );

        mOldLocalMouseX = localCoords.x;
        mOldLocalMouseY = localCoords.y;

        //RedrawCurrentLayer( { /*beforeBBox | selectedObject->GetBBox( true )*/{ 0, 0, 0, 0 } } );
    }

    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );

    return redrawRegion; // unused for now
}

bool
UOdysseyPainterEditorVectorObjectScaleTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    if( mDragging == true )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("VectorObjectScaleTool","Vector Object Scale Tool"));
        if( GUndo )
        {
            // save selected object translation/rotation/scaling before transform
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, mObjectTransformArray );

            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();
    }
    else
    {
        mPointArray.clear();
        mPointArray.push_back( ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y ) );

        // includes its own undo record
        UOdysseyPainterEditorVectorObjectPickTool::OnMouseUpVector( iEngine, iScene, iPointInTexture, iKey );

        FitHUD( iScene );
    }

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorObjectScaleTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE

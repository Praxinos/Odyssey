// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectRotateTool/OdysseyPainterEditorVectorObjectRotateTool.h"
#include "Tools/VectorObjectRotateTool/OdysseyPainterEditorVectorObjectRotateToolHUD.h"
#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorObjectRotateTool"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectRotateTool::~UOdysseyPainterEditorVectorObjectRotateTool()
{
}

UOdysseyPainterEditorVectorObjectRotateTool::UOdysseyPainterEditorVectorObjectRotateTool()
    : mPickedPivot( nullptr )
    , mDragging( false )
    , mRotating( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectRotateTool64");

    mObjectRotateHUD = new FOdysseyPainterEditorVectorObjectRotateToolHUD( this );
}
/*
void
UOdysseyPainterEditorVectorObjectRotateTool::FitHUD( FOdysseyVectorScene* iScene )
{
    FSelectionBox& selectionBox = mObjectRotateHUD->GetSelectionBox(); // this is a reference, it does not matter it we get it before the call to UpdateSelectionBox 

    mObjectRotateHUD->UpdateSelectionBox( iScene, false );

    mObjectRotateHUD->SetPivot( selectionBox.rect.x + ( selectionBox.rect.w * 0.5f )
                              , selectionBox.rect.y + ( selectionBox.rect.h * 0.5f ) );
}
*/
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectRotateTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mObjectRotateHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorObjectRotateTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mObjectRotateHUD );

    mObjectRotateHUD->Reset( iScene );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture
                                                              , const FKey& iKey )
{
    FSelectionBox& selectionBox = mObjectRotateHUD->GetSelectionBox();

    mPickedPivot = mObjectRotateHUD->PickPivot( iPointInTexture.x, iPointInTexture.y ) ? &mObjectRotateHUD->GetPivot() : nullptr;

    if( selectionBox.rect.Area() )
    {
        if( mPickedPivot == nullptr )
        {
            BLPoint pt = selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
            ::ULIS::FVec2D& pivot = mObjectRotateHUD->GetPivot();

            mStartAtVector.x = pt.x - pivot.x;
            mStartAtVector.y = pt.y - pivot.y;

            if( mStartAtVector.DistanceSquared() )
            {
                mStartAtVector.Normalize();
            }
        }
    }

    // remember for undos. we don't set undos in the mouse down event because it could conflict with the undo created by
    // UOdysseyPainterEditorVectorObjectPickTool::OnMouseUpVector() called when no dragging was made.
    mObjectTransformArray.clear();
    FObjectTransform::MakeArrayFromObjectList( iScene->GetSelectedObjectList(), mObjectTransformArray );

    mDragging = false;
    mRotating = false;
    mCumulAngle = 0.0f;

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

::ULIS::FRectI
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mObjectRotateHUD->GetSelectionBox();
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };

    mDragging = true;

    if( selectionBox.rect.Area() )
    {
        if ( mPickedPivot )
        {
            BLPoint localCoords = selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );

            mObjectRotateHUD->SetPivot( localCoords.x, localCoords.y );
        }
        else
        {
            std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
            //::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );
            //::ULIS::FRectD localBBox = selectedObject->GetBBox( false );
            BLMatrix2D spaceMatrix = selectionBox.worldMatrix;
            BLMatrix2D invertSpaceMatrix;
            ::ULIS::FVec2D& pivot = mObjectRotateHUD->GetPivot();
            BLPoint pt = selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
            ::ULIS::FVec2D endAtVector;
            BLMatrix2D rotateMatrix;
            double rotationAngle = 0.0f;

            mRotating = true;

            endAtVector.x = pt.x - pivot.x;
            endAtVector.y = pt.y - pivot.y;

            if( endAtVector.DistanceSquared() )
            {
                endAtVector.Normalize();
            }

            rotationAngle = fabs( acos( endAtVector.DotProduct( mStartAtVector ) ) );

            mCumulAngle += rotationAngle;

            rotateMatrix.resetToRotation( FOdysseyVector::Cross2D( endAtVector, mStartAtVector ) < 0.0f ? rotationAngle
                                                                                                      : - rotationAngle ); // Radians

            spaceMatrix.translate( pivot.x, pivot.y );

            BLMatrix2D::invert( invertSpaceMatrix, spaceMatrix );

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

                    // transfer object in "Rotation Space" coordinates system
                    FOdysseyVector::MatrixMultiply( invertSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

                    // rotate the object (local to the "Rotation Space" coordinates system)
                    FOdysseyVector::MatrixMultiply( rotateMatrix, objectSpaceMatrix, objectScaledMatrix );

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
                    object->Rotate( rotation / M_PI * 180 ); // in degrees
                    object->Scale( scalingX, scalingY );
                }
            }

            // Update the matrix for all objects
            iScene->UpdateMatrix();

            // update the selection box with the newly modified matrices
            mObjectRotateHUD->UpdateSelectionBox( iScene, false );

            // update vector with new matrix
            pt = selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );

            mStartAtVector.x = pt.x - pivot.x;
            mStartAtVector.y = pt.y - pivot.y;

            if( mStartAtVector.DistanceSquared() )
            {
                mStartAtVector.Normalize();
            }
        }
    }

    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );

    return redrawRegion; // unused for now
}

bool
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    if( mDragging == true )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("VectorObjectRotateTool","Vector Object Rotate Tool"));
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

        mObjectRotateHUD->Reset( iScene );
    }

    mDragging = false;
    mRotating = false;

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorObjectRotateTool::Commit()
{

}

bool
UOdysseyPainterEditorVectorObjectRotateTool::IsDragging()
{
    return mDragging;
}

bool
UOdysseyPainterEditorVectorObjectRotateTool::IsRotating()
{
    return mRotating;
}

double
UOdysseyPainterEditorVectorObjectRotateTool::GetCumulAngle()
{
    return mCumulAngle;
}

#undef LOCTEXT_NAMESPACE

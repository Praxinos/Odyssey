// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformTool.h"
#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformToolHUD.h"
#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorTransformTool"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorTransformTool::~UOdysseyPainterEditorVectorTransformTool()
{
}

UOdysseyPainterEditorVectorTransformTool::UOdysseyPainterEditorVectorTransformTool()
    : PickingRadius( 10.0f )
    , mPickedPivot( nullptr )
    , Uniform( true )
    , mDragging( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.TransformTool32");

    mTransformHUD = new FOdysseyPainterEditorVectorTransformToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorTransformTool::Load()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorTransformTool::LoadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyPainterEditorVectorTransformTool::Unload()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorTransformTool::UnloadVector( vectorEngine, vectorScene );
    }
}

bool
UOdysseyPainterEditorVectorTransformTool::IsActivable() const
{
    return !!mToolContext->GetVectorEngine();
}

void
UOdysseyPainterEditorVectorTransformTool::UnloadVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mTransformHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorTransformTool::LoadVector( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene )
{
    TSharedPtr< SViewport > viewportWidget; // to force keyboard focus on mouse hover.
                                            // Prevents the user from having to click at least once in the viewport.
    // we need the focus on the viewport for keyboard 
    viewportWidget = mToolContext->GetEditor()->GetGUI()->GetViewportTab()->GetViewport()->GetViewportWidget();

    // we need the focus on the viewport for keyboard 
    FSlateApplication::Get().SetKeyboardFocus( viewportWidget );

    iEngine->ClearHUD();
    iEngine->AddHUD( mTransformHUD );

    mTransformHUD->Reset( iScene );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorTransformTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , const FKey& iKey )
{
    UniformAtKeyDown = Uniform;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Uniform = true;
    }

    UOdysseyPainterEditorDefaultTool::OnKeyDownVector( iEngine, iScene, iKey );
    //iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorTransformTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                                       , FOdysseyVectorScene* iScene
                                                       , const FKey& iKey )
{
    Uniform = UniformAtKeyDown;

    UOdysseyPainterEditorDefaultTool::OnKeyUpVector( iEngine, iScene, iKey );
    //iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return false;
}

void
UOdysseyPainterEditorVectorTransformTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ::ULIS::FRectI redrawRect = UOdysseyPainterEditorVectorTransformTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );

        if( redrawRect.Area() )
        {
            vectorEngine->GetInvalidTileMap().Invalidate(redrawRect);
            vectorScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
        }
    }
}

::ULIS::FRectI
UOdysseyPainterEditorVectorTransformTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    ::ULIS::FRectI imageRegion;

    iEngine->GetColorImageSize( imageRegion );

    if( mTransformHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y ) )
    {
        redrawRegion = imageRegion; // redraw the whole image when the gizmo must be redrawn
    }

    return redrawRegion;
}

bool
UOdysseyPainterEditorVectorTransformTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorVectorTransformTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
    }
    return ret;
}

bool
UOdysseyPainterEditorVectorTransformTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    uint32 hudFlags = mTransformHUD->GetFlags();

    mPickedPivot = hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ZAXIS ? &mTransformHUD->GetGizmo() : nullptr;

    // remember for undos. we don't set undos in the mouse down event because it could conflict with the undo created by
    // UOdysseyPainterEditorVectorObjectPickTool::OnMouseUpVector() called when no dragging was made.
    mObjectTransformArray.clear();
    FObjectTransform::MakeArrayFromObjectList( iScene->GetSelectedObjectList(), mObjectTransformArray );

    mDragging = false;

    if( selectionBox.rect.Area() )
    {
        if( mPickedPivot )
        {

        }
/*
        if( mPickedPivot == nullptr )
        {
            BLPoint pt = selectionBox.space->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            ::ULIS::FVec2D& pivot = mObjectRotateHUD->GetPivot();

            mStartAtVector.x = pt.x - pivot.x;
            mStartAtVector.y = pt.y - pivot.y;

            if( mStartAtVector.DistanceSquared() )
            {
                mStartAtVector.Normalize();
            }
        }
*/
    }

    if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ROTATE )
    {
        mTransformHUD->ShowSelectionBox( false );
    }
 
    return true;
}

void
UOdysseyPainterEditorVectorTransformTool::TranslateObjectSelection( FOdysseyVectorEngine* iEngine
                                                                  , FOdysseyVectorScene* iScene
                                                                  , const FOdysseyPoint& iPointInTexture )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    uint32 hudFlags = mTransformHUD->GetFlags();
    ::ULIS::FVec2D& gizmo = mTransformHUD->GetGizmo();
    BLMatrix2D spaceMatrix = selectionBox.worldMatrix;
    BLMatrix2D invertSpaceMatrix;
    BLMatrix2D translateMatrix;
    BLPoint translateBy;

    BLMatrix2D::invert( invertSpaceMatrix, spaceMatrix );

    translateBy = invertSpaceMatrix.mapVector( iPointInTexture.deltaPosition.X
                                                , iPointInTexture.deltaPosition.Y );

    translateMatrix.reset();

    if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_XAXIS     )
        || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_TRANSLATE ) )
    {
        translateMatrix.translate( translateBy.x, 0 );
    }

    if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_YAXIS     )
        || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_TRANSLATE ) )
    {
        translateMatrix.translate( 0, translateBy.y );
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
            BLMatrix2D objectTranslateMatrix;
            BLMatrix2D objectLocalMatrix;
            BLMatrix2D objectWorldMatrix = object->GetWorldMatrix();
            BLMatrix2D parentInverseWorldMatrix = object->GetParent()->GetInverseWorldMatrix();

            // transfer object in "Selection Space" coordinates system
            FOdysseyVector::MatrixMultiply( invertSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

            // translate the object (local to the "Selection Space" coordinates system)
            FOdysseyVector::MatrixMultiply( translateMatrix, objectSpaceMatrix, objectTranslateMatrix );

            // transfer the object back to world coordinates system
            FOdysseyVector::MatrixMultiply( spaceMatrix, objectTranslateMatrix, objectWorldMatrix );

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
    mTransformHUD->UpdateSelectionBox( iScene, World );
}

double
UOdysseyPainterEditorVectorTransformTool::GetRotationAngle( const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    BLPoint pt[2] = { selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x - iPointInTexture.deltaPosition.X
                                                              , iPointInTexture.y - iPointInTexture.deltaPosition.Y )
                    , selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x
                                                              , iPointInTexture.y ) };
    ::ULIS::FVec2D& pivot = mTransformHUD->GetGizmo();
    ::ULIS::FVec2D vector[2];
    double angle = 0.0f;

    vector[0].x = pt[0].x - pivot.x;
    vector[0].y = pt[0].y - pivot.y;

    vector[1].x = pt[1].x - pivot.x;
    vector[1].y = pt[1].y - pivot.y;

    if( vector[0].DistanceSquared() )
    {
        vector[0].Normalize();
    }

    if( vector[1].DistanceSquared() )
    {
        vector[1].Normalize();
    }

    angle = fabs( acos( vector[0].DotProduct( vector[1] ) ) );

    return FOdysseyVector::Cross2D( vector[0], vector[1] ) > 0.0f ? angle : - angle;
}

void
UOdysseyPainterEditorVectorTransformTool::RotateObjectSelection( FOdysseyVectorEngine* iEngine
                                                               , FOdysseyVectorScene* iScene
                                                               , const FOdysseyPoint& iPointInTexture )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    BLMatrix2D spaceMatrix = selectionBox.worldMatrix;
    BLMatrix2D invertSpaceMatrix;
    ::ULIS::FVec2D& pivot = mTransformHUD->GetGizmo();
    BLMatrix2D rotateMatrix;
    double rotationAngle = GetRotationAngle( iPointInTexture );
    BLPoint worldPivot = selectionBox.worldMatrix.mapPoint( pivot.x, pivot.y );

    rotateMatrix.resetToRotation( rotationAngle ); // Radians

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
    mTransformHUD->UpdateSelectionBox( iScene, World );

    // replace pivot correctly.
    BLPoint spacePivot = selectionBox.inverseWorldMatrix.mapPoint( worldPivot.x, worldPivot.y );
    pivot.x = spacePivot.x;
    pivot.y = spacePivot.y;
}

void
UOdysseyPainterEditorVectorTransformTool::ScaleObjectSelection( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorScene* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    //::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    uint32 hudFlags = mTransformHUD->GetFlags();
    BLPoint oldLocalCoords = selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x - iPointInTexture.deltaPosition.X
                                                                     , iPointInTexture.y - iPointInTexture.deltaPosition.Y );
    BLPoint localCoords = selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
    double difx = localCoords.x - oldLocalCoords.x;
    double dify = localCoords.y - oldLocalCoords.y;
    double oldX1 = selectionBox.rect.x
            , oldY1 = selectionBox.rect.y
            , oldX2 = selectionBox.rect.x + selectionBox.rect.w
            , oldY2 = selectionBox.rect.y + selectionBox.rect.h;
    double oldDiagonal = sqrt( ( selectionBox.rect.w * selectionBox.rect.w )
                                + ( selectionBox.rect.h * selectionBox.rect.h ) );
    double x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
    ::ULIS::FVec2D pivot;

    if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPLEFT )
    {
        x1 = localCoords.x;
        y1 = localCoords.y;
        x2 = oldX2;
        y2 = oldY2;

        pivot.x = oldX2;
        pivot.y = oldY2;
    }

    if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPRIGHT )
    {
        x1 = oldX1;
        y1 = localCoords.y;
        x2 = localCoords.x;
        y2 = oldY2;

        pivot.x = oldX1;
        pivot.y = oldY2;
    }

    if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMRIGHT )
    {
        x1 = oldX1;
        y1 = oldY1;
        x2 = localCoords.x;
        y2 = localCoords.y;

        pivot.x = oldX1;
        pivot.y = oldY1;
    }

    if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMLEFT )
    {
        x1 = localCoords.x;
        y1 = oldY1;
        x2 = oldX2;
        y2 = localCoords.y;

        pivot.x = oldX2;
        pivot.y = oldY1;
    }

    //if( ( hudFlags & FOdysseyPainterEditorVectorObjectScaleToolHUD::HANDLE_MASK ) != 0 )
    {
        BLMatrix2D spaceMatrix = selectionBox.worldMatrix;
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
    mTransformHUD->UpdateSelectionBox( iScene, World );
}

void
UOdysseyPainterEditorVectorTransformTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorTransformTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

void
UOdysseyPainterEditorVectorTransformTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    uint32 hudFlags = mTransformHUD->GetFlags();

    // What do we consider dragging ? We have to move at least a few pixels, otherwise we wouldn't
    // be able to differentiate an actual dragging from a simple down-up click, especially
    // when using the stylus, which is too sensitive to allow a Down and a UP at the very same
    // position, unlike the mouse. And we use the simple down-up click as a selection tool.
    // See OnMouseUpVector() for details.
    if( ( mDragging == true ) // if we are already dragging, don't bother checking other conditions
     || ( ::ULIS::FVec2D( iPointInTexture.deltaPosition.X
                        , iPointInTexture.deltaPosition.Y ).Distance() > 3.0f ) )
    {
        mDragging = true;

        if( selectionBox.rect.Area() )
        {
            if ( mPickedPivot )
            {
                BLMatrix2D& inverseWorldMatrix = selectionBox.inverseWorldMatrix;
                BLPoint localCoords = inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );

                mTransformHUD->SetGizmo( localCoords.x, localCoords.y );
            }
            else
            {
                if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_XAXIS     )
                 || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_YAXIS     )
                 || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_TRANSLATE ) )
                {
                    TranslateObjectSelection( iEngine, iScene, iPointInTexture );
                }

                if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ROTATE )
                {
                    RotateObjectSelection( iEngine, iScene, iPointInTexture );
                }

                if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPLEFT     )
                 || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPRIGHT    )
                 || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMRIGHT )
                 || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMLEFT  ) )
                {
                    ScaleObjectSelection( iEngine, iScene, iPointInTexture );
                }
            }
        }

        iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
    }
}

bool
UOdysseyPainterEditorVectorTransformTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorVectorTransformTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

bool
UOdysseyPainterEditorVectorTransformTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey )
{
    FOdysseyPainterEditor* painterEditor = mToolContext->GetEditor();
    bool stillClick = mDragging ? false : true;

    mPickedPivot = nullptr;
    mDragging = false;
    mTransformHUD->ShowSelectionBox( true );

    if( stillClick )
    {
        // use the pick tool if the Down and Up events were at the same position (no dragging )
        painterEditor->GetVectorPickTool()->OnMouseDown(iPointInTexture, iKey);
        painterEditor->GetVectorPickTool()->OnMouseUp(iPointInTexture, iKey);
    }
    else
    {
        if( mObjectTransformArray.size() )
        {
            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("VectorTransformTool","Vector Transform Tool"));
            if( GUndo )
            {
                // save selected object translation/rotation/scaling before transform
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, mObjectTransformArray );

                GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
            }
            GEditor->EndTransaction();
        }

        iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
    }

    return true;
}

void
UOdysseyPainterEditorVectorTransformTool::Commit()
{

}

void
UOdysseyPainterEditorVectorTransformTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;
    
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        PropertyChangedVector( vectorEngine, vectorScene, PropertyChangedEvent.GetPropertyName() );
    }
}

void
UOdysseyPainterEditorVectorTransformTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                               , FOdysseyVectorScene* iScene
                                                               , const FName& iPropertyName )
{
    //mTransformHUD->MakeTransform( iScene, DivisionsX, DivisionsY );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

#undef LOCTEXT_NAMESPACE

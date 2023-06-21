// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectMoveTool/OdysseyPainterEditorVectorObjectMoveTool.h"
#include "Tools/VectorObjectMoveTool/OdysseyPainterEditorVectorObjectMoveToolHUD.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorObjectMoveTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectMoveTool::~UOdysseyPainterEditorVectorObjectMoveTool()
{
    delete mObjectMoveHUD;
}

UOdysseyPainterEditorVectorObjectMoveTool::UOdysseyPainterEditorVectorObjectMoveTool()
    : Radius(10.0f)
    , World( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectMoveTool64");

    mObjectMoveHUD = new FOdysseyPainterEditorVectorObjectMoveToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectMoveTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mObjectMoveHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorObjectMoveTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mObjectMoveHUD );

    mObjectMoveHUD->Reset( iScene ); // update selection box

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
/*
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorObjectMoveTool","Vector Object Move Tool"));
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
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    ::ULIS::FRectI imageRegion;

    iEngine->GetColorImageSize( imageRegion );

    if( mObjectMoveHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y ) )
    {
        redrawRegion = imageRegion;
    }

    return redrawRegion;
}

::ULIS::FRectI
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    //::ULIS::FRectD beforeBBox = FOdysseyVectorObject::GetBoundingBoxFromList( selectedObjectList );
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    uint32 gizmoFlags = mObjectMoveHUD->GetGizmoFlags();
    FSelectionBox& selectionBox = mObjectMoveHUD->GetSelectionBox();

    mDragging = true;

    if( selectionBox.space )
    {
        BLMatrix2D spaceMatrix = World ? iScene->GetWorldMatrix()
                                       : selectionBox.space->GetWorldMatrix();
        BLMatrix2D invertSpaceMatrix;
        BLMatrix2D translateMatrix;
        BLPoint translateBy;

        BLMatrix2D::invert( invertSpaceMatrix, spaceMatrix );

        translateBy = invertSpaceMatrix.mapVector( iPointInTexture.deltaPosition.X
                                                 , iPointInTexture.deltaPosition.Y );

        translateMatrix.reset();

        if ( ( gizmoFlags & FOdysseyPainterEditorVectorObjectMoveToolHUD::PICK_XAXIS )
          || ( gizmoFlags == 0 ) )
        {
            translateMatrix.translate( translateBy.x, 0 );
        }

        if ( ( gizmoFlags & FOdysseyPainterEditorVectorObjectMoveToolHUD::PICK_YAXIS )
          || ( gizmoFlags == 0 ) )
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
        mObjectMoveHUD->Reset( iScene ); // Updates selection box and gizmo position
    }

    //::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );
    //::ULIS::FRectD localBBox = selectedObject->GetBBox( false );

/*
    for( std::list<FOdysseyVectorObject*>::iterator it = selectObjectList.begin(); it != selectObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        if ( selectedObject->HasSelectedAncestor() == false )
        {
            FOdysseyVectorObject* parentObject = selectedObject->GetParent();
            BLPoint localDif = parentObject->GetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                                              , iPointInTexture.deltaPosition.Y );

            if( gizmoFlags )
            {
                if( ( gizmoFlags & FOdysseyPainterEditorVectorObjectMoveToolHUD::PICK_XAXIS ) == 0 )
                {
                    localDif.x = 0;
                }

                if( ( gizmoFlags & FOdysseyPainterEditorVectorObjectMoveToolHUD::PICK_YAXIS ) == 0 )
                {
                    localDif.y = 0;
                }
            }

            selectedObject->Translate( selectedObject->GetTranslationX() + localDif.x
                                     , selectedObject->GetTranslationY() + localDif.y );

            selectedObject->UpdateMatrix();
        }
    }

    mObjectMoveHUD->Reset( iScene ); // Updates selection box and gizmo position
*/
    // update invalidated objects
    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );

    return redrawRegion; // unused
} 

bool
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    if( mDragging == true )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("VectorObjectMoveTool","Vector Object Move Tool"));
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
    }

    mObjectMoveHUD->Reset( iScene ); // updates selection box and gizmo

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorObjectMoveTool::Commit()
{

}

void
UOdysseyPainterEditorVectorObjectMoveTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                                , FOdysseyVectorScene* iScene
                                                                , const FName& iPropertyName )
{
    if( iPropertyName == "World" )
    {
        mObjectMoveHUD->Reset( iScene ); // updates selection box and gizmo
    }

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

#undef LOCTEXT_NAMESPACE

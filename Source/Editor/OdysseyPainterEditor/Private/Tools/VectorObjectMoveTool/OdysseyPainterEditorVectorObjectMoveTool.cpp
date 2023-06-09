// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectMoveTool/OdysseyPainterEditorVectorObjectMoveTool.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorObjectMoveTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectMoveTool::~UOdysseyPainterEditorVectorObjectMoveTool()
{
}

UOdysseyPainterEditorVectorObjectMoveTool::UOdysseyPainterEditorVectorObjectMoveTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectMoveTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectMoveTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->RemoveHUD( &mDummyHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorObjectMoveTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    // iEngine->ClearHUD();
    //iEngine->AddHUD( &mDummyHUD );

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

void
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    std::list<FOdysseyVectorObject*>& selectObjectList = iScene->GetSelectedObjectList();
    ::ULIS::FRectD beforeBBox = FOdysseyVectorObject::GetBoundingBoxFromList( selectObjectList );

    mDragging = true;

    for( std::list<FOdysseyVectorObject*>::iterator it = selectObjectList.begin(); it != selectObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        if ( selectedObject->HasSelectedAncestor() == false )
        {
            FOdysseyVectorObject* parentObject = selectedObject->GetParent();
            BLPoint localDif = parentObject->GetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                                              , iPointInTexture.deltaPosition.Y );

            selectedObject->Translate( selectedObject->GetTranslationX() + localDif.x
                                     , selectedObject->GetTranslationY() + localDif.y );

            selectedObject->UpdateMatrix();
        }
    }

    // update invalidated objects
    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );
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

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorObjectMoveTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE

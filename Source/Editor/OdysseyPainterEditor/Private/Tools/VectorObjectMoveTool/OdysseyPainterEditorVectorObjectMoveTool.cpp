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
    //iEngine->AddHUD( &mDummyHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorObjectMoveTool","Vector Object Move Tool"));
    if( GUndo )
    {
        // save selected object translation/rotation/scaling before transform
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, iScene->GetSelectedObjectList() );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorObjectMoveTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE

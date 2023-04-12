// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectMoveTool/OdysseyPainterEditorVectorObjectMoveTool.h"

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
UOdysseyPainterEditorVectorObjectMoveTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
}

bool
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , FOdysseyVectorUndo** iUndo
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
    if( iUndo && GUndo )
    {
        // save selected object translation/rotation/scaling before transform
        (*iUndo) = new FOdysseyVectorUndoObjectTransform( iScene, iScene->GetSelectedObjectList() );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(*iUndo) );
    }

    return true;
}

void
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
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

    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );
}

bool
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    iScene->Update( 0 );

    return true;
}

void
UOdysseyPainterEditorVectorObjectMoveTool::Commit()
{

}

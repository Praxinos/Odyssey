// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathCutTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathCutTool::~UOdysseyPainterEditorVectorPathCutTool()
{
}

UOdysseyPainterEditorVectorPathCutTool::UOdysseyPainterEditorVectorPathCutTool()
    : mCubicPathHUD( FOdysseyVectorHUDPathCubic::VIEW_PATH | FOdysseyVectorHUDPathCubic::VIEW_POINT )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathCutTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathCutTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( &mCubicPathHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathCutTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( &mCubicPathHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

    mLineHUD.SetP0( iPointInTexture.x, iPointInTexture.y );
    mLineHUD.SetP1( iPointInTexture.x, iPointInTexture.y );

    iEngine->AddHUD( &mLineHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorPathCutTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FVec2D& p0 = mLineHUD.GetP0();
    ::ULIS::FVec2D& p1 = mLineHUD.GetP1();
    ::ULIS::FRectI rect = ::ULIS::FRectI::FromMinMax( ::ULIS::FMath::Min(p0.x, p1.x)
                                                    , ::ULIS::FMath::Min(p0.y, p1.y)
                                                    , ::ULIS::FMath::Max(p0.x, p1.x)
                                                    , ::ULIS::FMath::Max(p0.y, p1.y) );

    mLineHUD.SetP1( iPointInTexture.x, iPointInTexture.y );

    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES ); // refresh vector scene and GUI widgets via delegates.
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                       , FOdysseyVectorScene* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray;

    // crashes if I don't reserve. Why that ?
    removedSegmentArray.reserve(50);
    addedSegmentArray.reserve(50);
    addedVertexArray.reserve(50);

    iEngine->RemoveHUD( &mLineHUD );

    for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);
        BLMatrix2D& inverseWorldMatrix = selectedObject->GetInverseWorldMatrix();
        ::ULIS::FVec2D& p0 = mLineHUD.GetP0();
        ::ULIS::FVec2D& p1 = mLineHUD.GetP1();
        BLPoint localP0 = inverseWorldMatrix.mapPoint( p0.x, p0.y );
        BLPoint localP1 = inverseWorldMatrix.mapPoint( p1.x, p1.y );

        if( selectedObject->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
        {
            FOdysseyVectorPathCubic *cubicPath = static_cast<FOdysseyVectorPathCubic*>(selectedObject);

            cubicPath->Cut( ::ULIS::FVec2D( localP0.x, localP0.y )
                          , ::ULIS::FVec2D( localP1.x, localP1.y )
                          , addedVertexArray
                          , addedSegmentArray
                          , removedSegmentArray );
            cubicPath->Invalidate();
        }
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathCutTool","Vector Path Cut Tool"));
    if( GUndo )
    {
        std::vector<FOdysseyVectorPath*> removedPathArray; // empty on purpose.
        std::vector<FOdysseyVectorVertex*> removedVertexArray; // empty on purpose.
        std::vector<FOdysseyVectorPath*> addedPathArray; // empty on purpose.

        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                  , removedPathArray
                                                                  , removedVertexArray
                                                                  , removedSegmentArray
                                                                  , addedPathArray
                                                                  , addedVertexArray
                                                                  , addedSegmentArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED );

    return true;
}

void
UOdysseyPainterEditorVectorPathCutTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE

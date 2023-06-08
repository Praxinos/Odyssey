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

    if ( selectedObject )
    {
        BLPoint localCoords = selectedObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

        mStartCutAt.x = localCoords.x;
        mStartCutAt.y = localCoords.y;
    }

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
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    ::ULIS::FVec2D endCutAt;

    iEngine->RemoveHUD( &mLineHUD );

    if ( selectedObject )
    {
        BLPoint localCoords = selectedObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

        endCutAt.x = localCoords.x;
        endCutAt.y = localCoords.y;

        if( selectedObject->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
        {
            FOdysseyVectorPathCubic *cubicPath = static_cast<FOdysseyVectorPathCubic*>(selectedObject);

            // crashes if I don't reserve. Why that ?
            addedVertexArray.reserve(50);
            addedSegmentArray.reserve(50);
            removedSegmentArray.reserve(50);

            cubicPath->Cut( mStartCutAt, endCutAt, addedVertexArray, addedSegmentArray, removedSegmentArray );
            cubicPath->Invalidate();
        }
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathCutTool","Vector Path Cut Tool"));
    if( GUndo )
    {
        std::vector<FOdysseyVectorVertex*> removedVertexArray; // empty on purpose.

        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                  , removedVertexArray
                                                                  , removedSegmentArray
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

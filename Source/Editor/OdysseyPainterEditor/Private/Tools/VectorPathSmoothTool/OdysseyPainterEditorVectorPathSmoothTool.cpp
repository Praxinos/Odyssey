// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Undo/OdysseyVectorUndoSegmentReshape.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathSmoothTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathSmoothTool::~UOdysseyPainterEditorVectorPathSmoothTool()
{
}

UOdysseyPainterEditorVectorPathSmoothTool::UOdysseyPainterEditorVectorPathSmoothTool()
    : mPickingHUD()
    , Radius(20.0f)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathSmoothTool64");

    mPickedPointArray.reserve( 50 );

    mPickingHUD.SetRadius( Radius );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( &mPickingHUD );
    iEngine->RemoveHUD( &mPathPushHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->AddHUD( &mPickingHUD );
    iEngine->AddHUD( &mPathPushHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    mPickedPointArray.clear();

    iEngine->PickPoints( iScene
                       , iPointInTexture.x
                       , iPointInTexture.y
                       , Radius
                       , mPickedPointArray
                       , FOdysseyVectorPath::PICK_POINT );

    return true;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    double diameter = Radius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                          , (int)iPointInTexture.y - (int)Radius
                          , (int)diameter
                          , (int)diameter };

    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    std::vector<FOdysseyVectorVertex*> vertexArray;
    std::vector<FOdysseyVectorSegment*> segmentArray;

    FOdysseyVectorPoint::ArrayToVertexArray( mPickedPointArray, vertexArray );
    FOdysseyVectorVertex::ArrayToSegmentArray( vertexArray, segmentArray );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathSmoothTool","Vector Path Smooth Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSegmentReshape( iScene, segmentArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    for( int i = 0; i < mPickedPointArray.size(); i++ )
    {
        FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>(mPickedPointArray[i]);

        if( cubicVertex->GetSegmentCount() == 2 )
        {
            FOdysseyVectorPathCubic::SmoothSegments( cubicVertex, true, true );
        }
    }

    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return false;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathSmoothTool::PropertyChanged( const FName& iPropertyName )
{
    mPickingHUD.SetRadius( Radius );
}

#undef LOCTEXT_NAMESPACE

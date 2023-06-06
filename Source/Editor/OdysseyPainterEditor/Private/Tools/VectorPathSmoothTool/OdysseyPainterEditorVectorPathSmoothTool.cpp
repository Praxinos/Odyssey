// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"

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
UOdysseyPainterEditorVectorPathSmoothTool::ActivateVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD(&mPickingHUD);

    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );

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
/*
    if( rect.x < 0 ) rect.x = 0;
    if( rect.y < 0 ) rect.y = 0;

    rect = rect & layerStack->GetSurface()->Block()->Rect();

    if( rect.Area() )
    {*/
    /*}*/
    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    mPickedPointArray.clear();

    iEngine->PickPoints( iScene
                       , iPointInTexture.x
                       , iPointInTexture.y
                       , Radius
                       , mPickedPointArray
                       , FOdysseyVectorPath::PICK_POINT );

    for( int i = 0; i < mPickedPointArray.size(); i++ )
    {
        FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>(mPickedPointArray[i]);

        if( cubicVertex->GetSegmentCount() == 2 )
        {
            FOdysseyVectorPathCubic::SmoothSegments( cubicVertex, true, true );
        }
    }

    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SCENE_REDRAW | FOdysseyVectorScene::OBJECT_MODIFIED );

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

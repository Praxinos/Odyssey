// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathWidthTool/OdysseyPainterEditorVectorPathWidthTool.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathWidthTool::~UOdysseyPainterEditorVectorPathWidthTool()
{
}

UOdysseyPainterEditorVectorPathWidthTool::UOdysseyPainterEditorVectorPathWidthTool()
    : mPickingHUD()
    , Radius( 20.0f )
    , Strength( 0.01f ) // 1 percent
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathWidthTool64");

    mPickingHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathWidthTool::Load()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorPathWidthTool::LoadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyPainterEditorVectorPathWidthTool::Unload()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorPathWidthTool::UnloadVector( vectorEngine, vectorScene );
    }
}

bool
UOdysseyPainterEditorVectorPathWidthTool::IsActivable() const
{
    return !!mToolContext->GetVectorEngine();
}

void
UOdysseyPainterEditorVectorPathWidthTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( &mPickingHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathWidthTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( &mPickingHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathWidthTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorVectorPathWidthTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
    }
    return ret;
}

bool
UOdysseyPainterEditorVectorPathWidthTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey)
{
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorPathWidthTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorPathWidthTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

void
UOdysseyPainterEditorVectorPathWidthTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathWidthTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorPathWidthTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

void
UOdysseyPainterEditorVectorPathWidthTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    std::vector<FOdysseyVectorSegment*> segmentArray;

    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    // this callback crashes if I dont reserve memory. I have no idea why. To troubleshoot later.
    segmentArray.reserve( 500 );

    iEngine->PickSegments( iScene
                         , false
                         , iPointInTexture.x
                         , iPointInTexture.y
                         , Radius
                         , segmentArray
                         , nullptr );

    for( int i = 0; i < segmentArray.size(); i++ )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>( segmentArray[i] );
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(cubicSegment->GetPath());
        BLPoint localPoint = path->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        FOdysseyVectorVertex* vertex0 = static_cast<FOdysseyVectorVertex*>(cubicSegment->GetVertex(0));
        FOdysseyVectorVertex* vertex1 = static_cast<FOdysseyVectorVertex*>(cubicSegment->GetVertex(1));
        ::ULIS::FVec2D& point0 = vertex0->GetCoords();
        ::ULIS::FVec2D& point1 = vertex1->GetCoords();
        ::ULIS::FVec2D p0Vec = { localPoint.x - point0.x, localPoint.y - point0.y };
        ::ULIS::FVec2D p1Vec = { localPoint.x - point1.x, localPoint.y - point1.y };
        double p0VecDistance = p0Vec.Distance();
        double p1VecDistance = p1Vec.Distance();
        double totaldistance = p0VecDistance + p1VecDistance;

        vertex0->SetRadius( vertex0->GetRadius() * ( 1.0f + Strength * ( p0VecDistance / totaldistance ) ) );
        vertex1->SetRadius( vertex1->GetRadius() * ( 1.0f + Strength * ( p1VecDistance / totaldistance ) ) );

        cubicSegment->Invalidate();
    }

    // update vector scene and GUI widgets via delegates.
    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathWidthTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorVectorPathWidthTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

bool
UOdysseyPainterEditorVectorPathWidthTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey )
{
    iScene->Update( 0 ); // update vector scene and GUI widgets via delegates.

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return false;
}

void
UOdysseyPainterEditorVectorPathWidthTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathWidthTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;
    
    PropertyChanged( PropertyChangedEvent.GetPropertyName() );

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathWidthTool::PropertyChanged( const FName& iPropertyName )
{
    mPickingHUD.SetRadius( Radius );
}

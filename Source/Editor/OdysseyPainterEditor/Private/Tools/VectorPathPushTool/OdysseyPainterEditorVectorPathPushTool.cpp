// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathPushTool::~UOdysseyPainterEditorVectorPathPushTool()
{
}

UOdysseyPainterEditorVectorPathPushTool::UOdysseyPainterEditorVectorPathPushTool()
    : Radius(20.0f)
    , PreserveSmoothness(true)
    , mPushHUD()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathPushTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathPushTool::Activate()
{
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if(currentVectorLayer)
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();
        vectorEngine->AddHUD(&mPushHUD);

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorPathPushTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorPathPushTool::HasVertex( FOdysseyVectorPoint* iPoint )
{
    for( int i = 0; i < mPushedPointArray.size(); i++ )
    {
        if( mPushedPointArray[i].point == iPoint )
        {
             return true;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPathPushTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        std::vector<double> pickedSegmentDistanceArray;

        // this callback crashes if I dont reserve memory. I have no idea why. To troubleshoot later.
        mSegmentArray.reserve( 500 );
        pickedSegmentDistanceArray.reserve( 500 ); // unused for now

        mSegmentArray.clear();
        mPushedPointArray.clear();

        vectorEngine->PickSegments( currentVectorLayer->GetScene()
                                  , iPointInTexture.x
                                  , iPointInTexture.y
                                  , Radius
                                  , mSegmentArray
                                  , pickedSegmentDistanceArray );

        for( int i = 0; i < mSegmentArray.size(); i++ )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>( mSegmentArray[i] );

            if( cubicSegment )
            {
                FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(cubicSegment->GetPath());
                BLPoint localPoint = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
                ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
                ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();
                ::ULIS::FVec2D& point0 = cubicSegment->GetVertex(0)->GetCoords( cubicSegment );
                ::ULIS::FVec2D& point1 = cubicSegment->GetVertex(1)->GetCoords( cubicSegment );
                ::ULIS::FVec2D cp0Vec = { localPoint.x - ctrlPoint0.x, localPoint.y - ctrlPoint0.y };
                ::ULIS::FVec2D cp1Vec = { localPoint.x - ctrlPoint1.x, localPoint.y - ctrlPoint1.y };

                mPushedPointArray.push_back( FPushedPoint( cubicSegment->GetHandle(0), Radius / cp0Vec.Distance() ) );
                mPushedPointArray.push_back( FPushedPoint( cubicSegment->GetHandle(1), Radius / cp1Vec.Distance() ) );

                if( HasVertex( cubicSegment->GetPoint(0) ) == false )
                {
                    ::ULIS::FVec2D p0Vec = { localPoint.x - point0.x, localPoint.y - point0.y };

                    mPushedPointArray.push_back( FPushedPoint( cubicSegment->GetPoint(0), Radius / p0Vec.Distance() ) );
                }

                if( HasVertex( cubicSegment->GetPoint(1) ) == false )
                {
                    ::ULIS::FVec2D p1Vec = { localPoint.x - point1.x, localPoint.y - point1.y };

                    mPushedPointArray.push_back( FPushedPoint( cubicSegment->GetPoint(1), Radius / p1Vec.Distance() ) );
                }
            }
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathPushTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        double diameter = Radius * 2.0f;
        ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                              , (int)iPointInTexture.y - (int)Radius
                              , (int)diameter
                              , (int)diameter };

        mPushHUD.SetRadius( Radius );
        mPushHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
/*
        if( rect.x < 0 ) rect.x = 0;
        if( rect.y < 0 ) rect.y = 0;

        rect = rect & layerStack->GetSurface()->Block()->Rect();

        if( rect.Area() )
        {*/
            currentVectorLayer->RenderImageChanged( /*{ rect },*/ true );
        /*}*/
    }
}

void
UOdysseyPainterEditorVectorPathPushTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        mPushHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

        for( int i = 0; i < mPushedPointArray.size(); i++ )
        {
            FOdysseyVectorPoint* point = mPushedPointArray[i].point;
            FOdysseyVectorPath* path;

            if( point->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
            {
                FOdysseyVectorHandleSegment* handleSegment = static_cast<FOdysseyVectorHandleSegment*>(point);

                path = handleSegment->GetParent()->GetPath();
            }
            else
            {
                FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(point);

                path = vertex->GetPath();
            }

            BLPoint delta = path->GetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                                   , iPointInTexture.deltaPosition.Y );

            point->SetX( point->GetX() + ( delta.x * mPushedPointArray[i].ratio ) );
            point->SetY( point->GetY() + ( delta.y * mPushedPointArray[i].ratio ) );

            /*if( vertex )
            {
                if( vertex->IsSmooth() && PreserveSmoothness )
                {
                    FOdysseyVectorVertexCubic* cubicVertex = static_cast<FOdysseyVectorVertexCubic*>(vertex);

                    if( cubicVertex )
                    {
                        cubicVertex->SmoothSegments( false );
                    }
                }
            }*/
        }

        for( int i = 0; i < mSegmentArray.size(); i++ )
        {
            mSegmentArray[i]->Invalidate();
        }

        currentVectorLayer->GetScene()->Update( FOdysseyVectorObject::FREQUENTUPDATES
                                              | FOdysseyVectorObject::KEEPINVALIDATED );

        currentVectorLayer->RenderImageChanged(/* { rect }, */true);
    }
}

bool
UOdysseyPainterEditorVectorPathPushTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        currentVectorLayer->GetScene()->Update( 0 );

        // redraw the whole layer
        currentVectorLayer->RenderImageChanged(false);
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathPushTool::Commit()
{

}

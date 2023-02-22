// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathPushTool::~UOdysseyPainterEditorVectorPathPushTool()
{
}

UOdysseyPainterEditorVectorPathPushTool::UOdysseyPainterEditorVectorPathPushTool()
    : Radius(20.0f)
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

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorPathPushTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorPathPushTool::HasVertex( UOdysseyVectorPoint* iPoint )
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
                                  , 20.0f
                                  , mSegmentArray
                                  , pickedSegmentDistanceArray );

        for( int i = 0; i < mSegmentArray.size(); i++ )
        {
            UOdysseyVectorSegmentCubic* cubicSegment = Cast<UOdysseyVectorSegmentCubic>( mSegmentArray[i] );

            if( cubicSegment )
            {
                UOdysseyVectorPathCubic* cubicPath = Cast<UOdysseyVectorPathCubic>(cubicSegment->GetPath());
                BLPoint localPoint = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
                ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetControlPoint(0)->GetCoords();
                ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetControlPoint(1)->GetCoords();
                ::ULIS::FVec2D& point0 = cubicSegment->GetPoint(0)->GetCoords();
                ::ULIS::FVec2D& point1 = cubicSegment->GetPoint(1)->GetCoords();
                ::ULIS::FVec2D cp0Vec = { localPoint.x - ctrlPoint0.x, localPoint.y - ctrlPoint0.y };
                ::ULIS::FVec2D cp1Vec = { localPoint.x - ctrlPoint1.x, localPoint.y - ctrlPoint1.y };

                mPushedPointArray.push_back( FPushedPoint( cubicSegment->GetControlPoint(0), Radius / cp0Vec.Distance() ) );
                mPushedPointArray.push_back( FPushedPoint( cubicSegment->GetControlPoint(1), Radius / cp1Vec.Distance() ) );

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

//        UE_LOG(LogTemp, Warning, TEXT("%d %d"), pickedSegmentArray.size(), pickedSegmentDistanceArray.size() ); 
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathPushTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        for( int i = 0; i < mPushedPointArray.size(); i++ )
        {
            UOdysseyVectorPoint* point = mPushedPointArray[i].point;

            point->SetX( point->GetX() + ( iPointInTexture.deltaPosition.X * mPushedPointArray[i].ratio ) );
            point->SetY( point->GetY() + ( iPointInTexture.deltaPosition.Y * mPushedPointArray[i].ratio ) );
        }

        for( int i = 0; i < mSegmentArray.size(); i++ )
        {
            mSegmentArray[i]->Invalidate();
        }

        currentVectorLayer->GetScene()->Update();

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

        // redraw the whole layer
        currentVectorLayer->RenderImageChanged(false);
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathPushTool::Commit()
{

}

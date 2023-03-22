// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathWidthTool/OdysseyPainterEditorVectorPathWidthTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathWidthTool::~UOdysseyPainterEditorVectorPathWidthTool()
{
}

UOdysseyPainterEditorVectorPathWidthTool::UOdysseyPainterEditorVectorPathWidthTool()
    : Radius(20.0f)
    , Strength(0.01f) // 1 percent
    , mPickingHUD()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathWidthTool64");

    mPickingHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathWidthTool::Activate()
{
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if(currentVectorLayer)
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();
        vectorEngine->AddHUD(&mPickingHUD);

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorPathWidthTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorPathWidthTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return true;
}

void
UOdysseyPainterEditorVectorPathWidthTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        currentVectorLayer->RenderImageChanged(true);
    }
}

void
UOdysseyPainterEditorVectorPathWidthTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        std::vector<FOdysseyVectorSegment*> segmentArray;

        // this callback crashes if I dont reserve memory. I have no idea why. To troubleshoot later.
        segmentArray.reserve( 500 );

        vectorEngine->PickSegments( currentVectorLayer->GetScene()
                                  , iPointInTexture.x
                                  , iPointInTexture.y
                                  , Radius
                                  , segmentArray
                                  , nullptr );

        for( int i = 0; i < segmentArray.size(); i++ )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>( segmentArray[i] );
            FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(cubicSegment->GetPath());
            BLPoint localPoint = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            FOdysseyVectorVertexCubic* vertex0 = static_cast<FOdysseyVectorVertexCubic*>(cubicSegment->GetVertex(0));
            FOdysseyVectorVertexCubic* vertex1 = static_cast<FOdysseyVectorVertexCubic*>(cubicSegment->GetVertex(1));
            ::ULIS::FVec2D& point0 = vertex0->GetCoords( cubicSegment );
            ::ULIS::FVec2D& point1 = vertex1->GetCoords( cubicSegment );
            ::ULIS::FVec2D p0Vec = { localPoint.x - point0.x, localPoint.y - point0.y };
            ::ULIS::FVec2D p1Vec = { localPoint.x - point1.x, localPoint.y - point1.y };
            double p0VecDistance = p0Vec.Distance();
            double p1VecDistance = p1Vec.Distance();
            double totaldistance = p0VecDistance + p1VecDistance;

            vertex0->SetRadius( vertex0->GetRadius() * ( 1.0f + Strength * ( p0VecDistance / totaldistance ) ) );
            vertex1->SetRadius( vertex1->GetRadius() * ( 1.0f + Strength * ( p1VecDistance / totaldistance ) ) );

            cubicSegment->Invalidate();
        }

        currentVectorLayer->GetScene()->Update( FOdysseyVectorObject::FREQUENTUPDATES
                                              | FOdysseyVectorObject::KEEPINVALIDATED );

        currentVectorLayer->RenderImageChanged(true);
    }
}

bool
UOdysseyPainterEditorVectorPathWidthTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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
UOdysseyPainterEditorVectorPathWidthTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathWidthTool::PropertyChanged( const FName& iPropertyName )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        mPickingHUD.SetRadius( Radius );

        currentVectorLayer->RenderImageChanged(false);
    }
}

void
UOdysseyPainterEditorVectorPathWidthTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

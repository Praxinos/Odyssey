// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathDrawingTool::~UOdysseyPainterEditorVectorPathDrawingTool()
{
}

UOdysseyPainterEditorVectorPathDrawingTool::UOdysseyPainterEditorVectorPathDrawingTool()
    : Size(10.0f)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.DrawingTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathDrawingTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();
    FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();

    vectorEngine->SetDrawingFlags( 0 );

    RedrawCurrentLayer( { { 0, 0, 0, 0 } } );
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();
        UOdysseyVectorPathCubic* cubicPath = NewObject<UOdysseyVectorPathCubic>();
        UOdysseyVectorPathBuilder* currentPathBuilder = NewObject<UOdysseyVectorPathBuilder>();
        BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        float radius =  iPointInTexture.pressure * ( Size * 0.5f );
        float roundedUpRadius = ceil (radius);
        float roundedUpDiameter = roundedUpRadius * 2.0f;
        ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
        ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
        uint32 strokeColor = *((uint32*)rgba8.Bits());

        cubicPath->SetStrokeColor( strokeColor );
        currentPathBuilder->SetStrokeColor( strokeColor );

        currentPathBuilder->Attach( cubicPath );

        vectorEngine->GetScene()->AppendChild( cubicPath );
        vectorEngine->GetScene()->AppendChild( currentPathBuilder );

        currentPathBuilder->UpdateMatrix();

        currentPathBuilder->AppendPoint( localCoords.x, localCoords.y, roundedUpRadius );

        vectorEngine->GetScene()->ClearSelection();
        vectorEngine->GetScene()->Select(*currentPathBuilder);

        RedrawCurrentLayer( { layerStack->GetSurface()->Block()->Rect() } );

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();
        UOdysseyVectorPathBuilder* currentPathBuilder = static_cast<UOdysseyVectorPathBuilder*>( vectorEngine->GetScene()->GetLastSelected() );
        UOdysseyVectorPathCubic* cubicPath = currentPathBuilder->GetCubicPath();
        BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        float radius =  iPointInTexture.pressure * ( Size * 0.5f );
        float roundedUpRadius = ceil (radius);
        float roundedUpDiameter = roundedUpRadius * 2.0f;
        ::ULIS::FRectI invalidateRegion = { (int) iPointInTexture.x - (int) roundedUpRadius
                                            , (int) iPointInTexture.y - (int) roundedUpRadius
                                            , (int) roundedUpDiameter
                                            , (int) roundedUpDiameter };
        ::ULIS::FRectI finalRegion = invalidateRegion;

        currentPathBuilder->AppendPoint( localCoords.x, localCoords.y, roundedUpRadius );

        UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(cubicPath->GetLastSegment());

        if ( cubicSegment )
        {
            ::ULIS::FRectD& bbox = cubicSegment->GetBoundingBox();
            BLPoint localBBoxPoint = currentPathBuilder->GetWorldMatrix().mapPoint( bbox.x, bbox.y );
            BLPoint localBBoxSize  = currentPathBuilder->GetWorldMatrix().mapVector( bbox.w, bbox.h );
            ::ULIS::FRectI worldBBox = { ( int ) localBBoxPoint.x
                                        , ( int ) localBBoxPoint.y
                                        , ( int ) localBBoxSize.x
                                        , ( int ) localBBoxSize.y };

            /*worldBBox.Sanitize();*/

            invalidateRegion = finalRegion | worldBBox;
        } 

        invalidateRegion.Sanitize();

        finalRegion = invalidateRegion & layerStack->GetSurface()->Block()->Rect();

        vectorEngine->GetScene()->Update();

        RedrawCurrentLayer( { finalRegion } );
    }
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();
        UOdysseyVectorPathBuilder* currentPathBuilder = static_cast<UOdysseyVectorPathBuilder*>( vectorEngine->GetScene()->GetLastSelected() );
        UOdysseyVectorPathCubic* cubicPath = currentPathBuilder->GetCubicPath();
        BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        float radius =  iPointInTexture.pressure * ( Size * 0.5f );
        float roundedUpRadius = ceil (radius);
        float roundedUpDiameter = roundedUpRadius * 2.0f;

        if( ( iKey == EKeys::LeftControl ) || ( iKey == EKeys::RightControl ) )
        {
            currentPathBuilder->End( localCoords.x, localCoords.y, roundedUpRadius, true );
        }
        else
        {
            currentPathBuilder->End( localCoords.x, localCoords.y, roundedUpRadius, false );
        }

        vectorEngine->GetScene()->Unselect( *currentPathBuilder );
        vectorEngine->GetScene()->RemoveChild( currentPathBuilder );
        vectorEngine->GetScene()->Select( *currentPathBuilder->GetCubicPath() );

        // Update objects marked as invalidated
        vectorEngine->GetScene()->Update();

        RedrawCurrentLayer( { layerStack->GetSurface()->Block()->Rect() } );

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::Commit()
{

}

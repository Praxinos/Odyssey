// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathDrawingTool::~UOdysseyPainterEditorVectorPathDrawingTool()
{
}

UOdysseyPainterEditorVectorPathDrawingTool::UOdysseyPainterEditorVectorPathDrawingTool()
    : Size(10.0f)
    , Absolute(true)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoPen64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathDrawingTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if(currentVectorLayer)
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();

        currentVectorLayer->RenderImageChanged(false);
    }
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
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        UOdysseyVectorScene* scene = currentVectorLayer->GetScene();
        UOdysseyVectorObject* selectedObject = scene->GetLastSelected();
        UOdysseyVectorPathCubic* cubicPath = NewObject<UOdysseyVectorPathCubic>();
        UOdysseyVectorPathBuilder* currentPathBuilder = NewObject<UOdysseyVectorPathBuilder>();
        float radius =  iPointInTexture.pressure * ( Size * 0.5f );
        float roundedUpRadius = ceil (radius);
        float roundedUpDiameter = roundedUpRadius * 2.0f;
        ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
        ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );

        cubicPath->SetForegroundColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );
        currentPathBuilder->SetForegroundColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );

        currentPathBuilder->Attach( cubicPath );

        scene->AppendChild( cubicPath );
        scene->AppendChild( currentPathBuilder );

        cubicPath->UpdateMatrix();
        currentPathBuilder->UpdateMatrix();

        BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

        currentPathBuilder->AppendPoint( localCoords.x, localCoords.y, roundedUpRadius );

        scene->ClearSelection();
        scene->Select( currentPathBuilder );

        mSelectionChanged.Broadcast();

        currentVectorLayer->RenderImageChanged(false);

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        UOdysseyVectorPathBuilder* currentPathBuilder = static_cast<UOdysseyVectorPathBuilder*>( currentVectorLayer->GetScene()->GetLastSelected() );
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

        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(cubicPath->GetLastSegment());

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

        currentVectorLayer->GetScene()->Update( UOdysseyVectorObject::FREQUENTUPDATES
                                              | UOdysseyVectorObject::KEEPINVALIDATED );

        currentVectorLayer->RenderImageChanged({ finalRegion }, true);
    }
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        UOdysseyVectorPathBuilder* currentPathBuilder = static_cast<UOdysseyVectorPathBuilder*>( currentVectorLayer->GetScene()->GetLastSelected() );

        if( currentPathBuilder )
        {
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

            currentVectorLayer->GetScene()->Unselect( currentPathBuilder );
            currentVectorLayer->GetScene()->RemoveChild( currentPathBuilder );
            currentVectorLayer->GetScene()->Select( currentPathBuilder->GetCubicPath() );

            // Update objects marked as invalidated
            currentVectorLayer->GetScene()->Update( 0 );

            currentVectorLayer->RenderImageChanged(false);
        }

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::Commit()
{

}

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
    : Radius( 5.0f )
    , Stitch( true )
    , StitchingRadius( 10 )
    , Absolute( true )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoPen64");

    mPathDrawingHUD.SetRadius( Radius );
    mPathDrawingHUD.SetStitchingRadius( StitchingRadius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathDrawingTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();
        vectorEngine->AddHUD( &mPathDrawingHUD );

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::CanDraw()
{
    return IsActivable();
}

// create path or get the existing one under mouse click.
UOdysseyVectorPathBuilder*
UOdysseyPainterEditorVectorPathDrawingTool::MakePathBuilder( FOdysseyVectorEngine* iVectorEngine
                                                           , UOdysseyVectorScene* iScene
                                                           , double iWorldX
                                                           , double iWorldY )
{
    UOdysseyVectorPathBuilder* pathBuilder = NewObject<UOdysseyVectorPathBuilder>();
    ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
    ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
    FOdysseyVectorVertexCubic* stitchCubicVertex = nullptr;
    UOdysseyVectorPathCubic* cubicPath = nullptr;

    if( Stitch )
    {
        std::vector<FOdysseyVectorPoint*> pickedPointArray;

        pickedPointArray.reserve( 50 );

        iVectorEngine->PickPoints( iScene
                                 , iWorldX
                                 , iWorldY
                                 , StitchingRadius
                                 , pickedPointArray
                                 , UOdysseyVectorPath::PICK_POINT );

        if( pickedPointArray.size() )
        {
            stitchCubicVertex = static_cast<FOdysseyVectorVertexCubic*>( pickedPointArray[0] );

            if( stitchCubicVertex->GetSegmentCount() == 1 )
            {
                cubicPath = Cast<UOdysseyVectorPathCubic>( stitchCubicVertex->GetPath() );
            }
        }
    }

    if ( cubicPath == nullptr )
    {
        cubicPath = NewObject<UOdysseyVectorPathCubic>();

        iScene->AppendChild( cubicPath );

        cubicPath->UpdateMatrix();
        cubicPath->SetForegroundColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );
    }

    iScene->AppendChild( pathBuilder );

    pathBuilder->Attach( cubicPath, stitchCubicVertex );
    pathBuilder->SetForegroundColor( cubicPath->GetForegroundColor() );
    pathBuilder->UpdateMatrix();

    return pathBuilder;
}

bool
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        UOdysseyVectorScene* scene = currentVectorLayer->GetScene();
        UOdysseyVectorObject* selectedObject = scene->GetLastSelected();
        UOdysseyVectorPathBuilder* pathBuilder = MakePathBuilder ( vectorEngine, scene, iPointInTexture.x, iPointInTexture.y );
        UOdysseyVectorPathCubic* cubicPath = pathBuilder->GetCubicPath();

        // take the upper value to prevent stroke with width 0.0
        float radius = iPointInTexture.pressure * Radius;

        BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

        pathBuilder->AppendPoint( localCoords.x, localCoords.y, radius );

        scene->ClearSelection();
        scene->Select( pathBuilder );

        mSelectionChanged.Broadcast();

        currentVectorLayer->RenderImageChanged(false);

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    mPathDrawingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    if( currentVectorLayer )
    {
        currentVectorLayer->RenderImageChanged(false);
    }
}

void
UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    mPathDrawingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        UOdysseyVectorPathBuilder* currentPathBuilder = static_cast<UOdysseyVectorPathBuilder*>( currentVectorLayer->GetScene()->GetLastSelected() );
        UOdysseyVectorPathCubic* cubicPath = currentPathBuilder->GetCubicPath();
        BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        float radius =  iPointInTexture.pressure * Radius;
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
            float radius =  iPointInTexture.pressure * Radius;
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

void
UOdysseyPainterEditorVectorPathDrawingTool::PropertyChanged( const FName& iPropertyName )
{
    if ( iPropertyName == "Radius" )
        mPathDrawingHUD.SetRadius( Radius );

    if ( ( iPropertyName == "StitchingRadius" ) || ( iPropertyName == "Stitch" ) )
    {
        if( Stitch == true )
            mPathDrawingHUD.SetStitchingRadius( StitchingRadius );
        else
            mPathDrawingHUD.SetStitchingRadius( 0.0f );
    }
}

void
UOdysseyPainterEditorVectorPathDrawingTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

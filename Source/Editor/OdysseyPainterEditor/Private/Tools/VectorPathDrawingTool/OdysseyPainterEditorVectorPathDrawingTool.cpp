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
    , mPreviousVertex ( nullptr )
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

static ::ULIS::FRectI
GetInvalidationAreaFromPointer( int iX, int iY, int iRadius ) 
{
    int diameter = iRadius * 2;

    return ::ULIS::FRectI( iX - iRadius, iY - iRadius, diameter, diameter );
}

static ::ULIS::FRectI
RectangleDtoI( ::ULIS::FRectD iRect )
{
    return ::ULIS::FRectI( (int) iRect.x, (int) iRect.y, (int) iRect.w, (int) iRect.h );
}

FOdysseyVectorVertexCubic*
UOdysseyPainterEditorVectorPathDrawingTool::PickVertex( FOdysseyVectorEngine* iVectorEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iPickingRadius )
{
    if( Stitch )
    {
        std::vector<FOdysseyVectorPoint*> pickedPointArray;
        FOdysseyVectorVertexCubic* stitchCubicVertex = nullptr;

        pickedPointArray.reserve( 50 );

        iVectorEngine->PickPoints( iScene
                                 , iWorldX
                                 , iWorldY
                                 , iPickingRadius
                                 , pickedPointArray
                                 , FOdysseyVectorPath::PICK_POINT );

        if( pickedPointArray.size() )
        {
            stitchCubicVertex = static_cast<FOdysseyVectorVertexCubic*>( pickedPointArray[0] );

            if( stitchCubicVertex->GetSegmentCount() == 1 )
            {
                return stitchCubicVertex;
            }
        }
    }

    return nullptr;
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
        FOdysseyVectorScene* scene = currentVectorLayer->GetScene();
        FOdysseyVectorPathBuilder* pathBuilder = new FOdysseyVectorPathBuilder();
        ::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
        ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
        FOdysseyVectorVertexCubic* cubicVertex = PickVertex( vectorEngine, scene, iPointInTexture.x, iPointInTexture.y, StitchingRadius );
        // take the upper value to prevent stroke with width 0.0
        float radius = iPointInTexture.pressure * Radius;
        FOdysseyVectorPathCubic* cubicPath = nullptr;
        BLPoint localCoords;

        if( cubicVertex )
        {
            cubicPath = static_cast<FOdysseyVectorPathCubic*>( cubicVertex->GetPath() );
        }
        else
        {
            cubicVertex = FOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 0.0f );
        }

        mPreviousVertex = cubicVertex;

        if ( cubicPath == nullptr )
        {
            cubicPath = new FOdysseyVectorPathCubic();

            scene->AppendChild( cubicPath );

            cubicPath->AddVertex( cubicVertex );
            cubicPath->UpdateMatrix();
            cubicPath->SetForegroundColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );
        }

        localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

        cubicVertex->Set( localCoords.x, localCoords.y );
        cubicVertex->SetRadius( radius );

        scene->AppendChild( pathBuilder );

        pathBuilder->Attach( cubicPath );
        pathBuilder->SetForegroundColor( cubicPath->GetForegroundColor() );
        pathBuilder->UpdateMatrix();

        pathBuilder->RecordStart( cubicVertex );

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
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    mPathDrawingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorPathBuilder* currentPathBuilder = static_cast<FOdysseyVectorPathBuilder*>( currentVectorLayer->GetScene()->GetLastSelected() );
        FOdysseyVectorPathCubic* cubicPath = currentPathBuilder->GetCubicPath();
        BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        float radius =  iPointInTexture.pressure * Radius;
        float roundedUpRadius = ceil (radius);
        FOdysseyVectorVertexCubic* nextVertex;

        nextVertex = currentPathBuilder->RecordIntermediate( localCoords.x, localCoords.y, radius );

        currentVectorLayer->GetScene()->Update( 0 );

        ::ULIS::FRectI redrawRegion = GetInvalidationAreaFromPointer( iPointInTexture.x
                                                                    , iPointInTexture.y
                                                                    , Radius )
                                    | RectangleDtoI( mPreviousVertex->GetBoundingBox( true ) );

        mPreviousVertex = nextVertex;

        redrawRegion.Sanitize();
        redrawRegion = redrawRegion & layerStack->GetSurface()->Block()->Rect();

        currentVectorLayer->RenderImageChanged( { redrawRegion }, true);
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
        FOdysseyVectorScene* scene = currentVectorLayer->GetScene();
        FOdysseyVectorPathBuilder* currentPathBuilder = static_cast<FOdysseyVectorPathBuilder*>( currentVectorLayer->GetScene()->GetLastSelected() );

        if( currentPathBuilder )
        {
            FOdysseyVectorVertexCubic* cubicVertex = PickVertex( vectorEngine, scene, iPointInTexture.x, iPointInTexture.y, StitchingRadius );
            FOdysseyVectorPathCubic* cubicPath = currentPathBuilder->GetCubicPath();
            BLPoint localCoords = cubicPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            float radius =  iPointInTexture.pressure * Radius;
            float roundedUpRadius = /*ceil (radius)*/mPreviousVertex->GetRadius();

            // the picked cubic vertex must belong to the path we are working with
            if( cubicVertex )
            {
                if ( cubicVertex->GetPath() != cubicPath )
                {
                    cubicVertex = nullptr;
                }
            }

            if( cubicVertex == nullptr )
            {
                cubicVertex = FOdysseyVectorVertexCubic::New( localCoords.x, localCoords.y, roundedUpRadius );

                cubicPath->AddVertex( cubicVertex );
            }

            currentPathBuilder->RecordEnd( cubicVertex );

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

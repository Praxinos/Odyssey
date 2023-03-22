// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathKnotTool/OdysseyPainterEditorVectorPathKnotTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathKnotTool::~UOdysseyPainterEditorVectorPathKnotTool()
{
}

UOdysseyPainterEditorVectorPathKnotTool::UOdysseyPainterEditorVectorPathKnotTool()
    : Radius(20.0f)
    , mPickingHUD()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathKnotTool64");

    mPickingHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathKnotTool::Activate()
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
UOdysseyPainterEditorVectorPathKnotTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorPathKnotTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        ::ULIS::FRectD roi = { iPointInTexture.x - Radius, iPointInTexture.y - Radius, Radius * 2, Radius * 2 };
        std::vector<FOdysseyVectorPoint*> pickedPointArray;
        FOdysseyVectorSegment* createdSegment = nullptr;
        FOdysseyVectorSegment* removedSegment = nullptr;

        pickedPointArray.reserve(500); // crashes if I don't reserve. I don't know why.
/*
        vectorEngine->UseMaskImage();
        vectorEngine->GetBLContext()->setFillAlpha( 0.0f );
        vectorEngine->GetBLContext()->clearAll();
        vectorEngine->GetBLContext()->setFillAlpha( 1.0f );
        vectorEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
        vectorEngine->UseColorImage();
*/
        vectorEngine->PickPoints( currentVectorLayer->GetScene()
                                , iPointInTexture.x
                                , iPointInTexture.y
                                , Radius
                                , pickedPointArray
                                , FOdysseyVectorPath::PICK_POINT );

        if( pickedPointArray.size() >= 2 )
        {
            FOdysseyVectorVertex* vertexA = static_cast<FOdysseyVectorVertex*>( pickedPointArray[0] );
            FOdysseyVectorVertex* vertexB = static_cast<FOdysseyVectorVertex*>( pickedPointArray[1] );

            if( ( vertexA->GetSegmentCount() == 1 ) && ( vertexB->GetSegmentCount() == 1 ) )
            {
                if( vertexA->GetPath() != vertexB->GetPath() )
                {
/*
                    FOdysseyVectorPath* newPath = static_cast<FOdysseyVectorPath*>( vertexB->GetPath()->Copy() );
*/

                    // TODO: remove vertexB->GetPath() from selected objects.
                    vertexB->GetPath()->GetParent()->RemoveChild( vertexB->GetPath() );

                    vertexA->GetPath()->Merge( vertexB->GetPath() );
                }

                vectorEngine->Knot( vertexA, vertexB, &createdSegment, &removedSegment, true );

                currentVectorLayer->GetScene()->Update(0);
            }
        }

        currentVectorLayer->RenderImageChanged(false);
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathKnotTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
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

        mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
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
UOdysseyPainterEditorVectorPathKnotTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        ::ULIS::FRectD roi = { iPointInTexture.x - Radius, iPointInTexture.y - Radius, Radius * 2, Radius * 2 };

/*
        vectorEngine->UseMaskImage();
        vectorEngine->GetBLContext()->setFillAlpha( 0.0f );
        vectorEngine->GetBLContext()->clearAll();
        vectorEngine->GetBLContext()->setFillAlpha( 1.0f );
        vectorEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
        vectorEngine->UseColorImage();
*/
        /*vectorEngine->Knot( roi );*/

        currentVectorLayer->RenderImageChanged(/* { rect }, */true);
    }
}

bool
UOdysseyPainterEditorVectorPathKnotTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        // redraw the whole layer
        currentVectorLayer->RenderImageChanged(false);
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathKnotTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathKnotTool::PropertyChanged( const FName& iPropertyName )
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
UOdysseyPainterEditorVectorPathKnotTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

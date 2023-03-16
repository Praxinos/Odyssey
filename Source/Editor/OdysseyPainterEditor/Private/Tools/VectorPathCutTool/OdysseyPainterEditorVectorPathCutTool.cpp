// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathCutTool::~UOdysseyPainterEditorVectorPathCutTool()
{
}

UOdysseyPainterEditorVectorPathCutTool::UOdysseyPainterEditorVectorPathCutTool()
    : Size(1.0f)
    , mCubicPathHUD( FOdysseyVectorHUDPathCubic::VIEW_PATH | FOdysseyVectorHUDPathCubic::VIEW_POINT )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathCutTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathCutTool::Activate()
{
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if(currentVectorLayer)
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();
        vectorEngine->AddHUD(&mCubicPathHUD);

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorPathCutTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    mLineHUD.SetP0( iPointInTexture.x, iPointInTexture.y );

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorObject* selectedObject = currentVectorLayer->GetScene()->GetLastSelected();

        vectorEngine->AddHUD( &mLineHUD );

        if ( selectedObject )
        {
            BLPoint localCoords = selectedObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

            mStartCutAt.x = localCoords.x;
            mStartCutAt.y = localCoords.y;
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathCutTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();


    if ( currentVectorLayer )
    {
        ::ULIS::FVec2D& p0 = mLineHUD.GetP0();
        ::ULIS::FVec2D& p1 = mLineHUD.GetP1();
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromMinMax(::ULIS::FMath::Min(p0.x, p1.x)
                                                        , ::ULIS::FMath::Min(p0.y, p1.y)
                                                        , ::ULIS::FMath::Max(p0.x, p1.x)
                                                        , ::ULIS::FMath::Max(p0.y, p1.y));

        mLineHUD.SetP1( iPointInTexture.x, iPointInTexture.y );

        currentVectorLayer->RenderImageChanged(/* { rect }, */true);
    }
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();
    ::ULIS::FVec2D endCutAt;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorObject* selectedObject = currentVectorLayer->GetScene()->GetLastSelected();

        vectorEngine->RemoveHUD( &mLineHUD );
        vectorEngine->RemoveHUD( &mLineHUD );

        if ( selectedObject )
        {
            BLPoint localCoords = selectedObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

            endCutAt.x = localCoords.x;
            endCutAt.y = localCoords.y;

            if( selectedObject->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
            {
                FOdysseyVectorPathCubic *cubicPath = static_cast<FOdysseyVectorPathCubic*>(selectedObject);
                std::vector<FOdysseyVectorVertexCubic*> oNewVertexArray;
                std::vector<FOdysseyVectorSegmentCubic*> oNewSegmentArray;
                std::vector<FOdysseyVectorSegmentCubic*> oOldSegmentArray;

                // crashes if I don't reserve. Why that ?
                oNewVertexArray.reserve(50);
                oNewSegmentArray.reserve(50);
                oOldSegmentArray.reserve(50);

                cubicPath->Cut( mStartCutAt, endCutAt, oNewVertexArray, oNewSegmentArray, oOldSegmentArray );
                cubicPath->Invalidate();
            }

            currentVectorLayer->GetScene()->Update( 0 );

            // redraw the whole layer
            currentVectorLayer->RenderImageChanged(false);

            return true;
        }

        // redraw the whole layer
        currentVectorLayer->RenderImageChanged(false);
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathCutTool::Commit()
{

}

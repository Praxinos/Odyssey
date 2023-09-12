// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Layers/LayerRoot/OdysseyAnimationLayerRoot.h"
#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolder.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/OdysseyAnimationLayerStackImageRenderer.h"
#include "OdysseyRectUtils.h"

//===============================================

UOdysseyAnimationLayerStack::UOdysseyAnimationLayerStack()
{
    CompatibleLayers.Add(UOdysseyAnimationLayerFolder::StaticClass());
    CompatibleLayers.Add(UOdysseyAnimationLayerImageRaster::StaticClass());
    CompatibleLayers.Add(UOdysseyAnimationLayerImageVector::StaticClass());

    LayerRootClass = UOdysseyAnimationLayerRoot::StaticClass();
}

UOdysseyAnimation*
UOdysseyAnimationLayerStack::GetAnimation() const
{
    UObject* outer = GetOuter();
    while(outer)
    {
        if (outer->GetClass() == UOdysseyAnimation::StaticClass())
            return Cast<UOdysseyAnimation>(outer);

		outer = outer->GetOuter();
    }

    return nullptr;    
}

FInt32Range
UOdysseyAnimationLayerStack::GetFrameRange() const
{
    return Cast<UOdysseyAnimationLayerRoot>(LayerRoot)->GetFrameRange();
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationLayerStack::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    return MakeShared<FOdysseyAnimationLayerStackImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects());
}

TArray<FGuid>
UOdysseyAnimationLayerStack::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    UOdysseyAnimationLayer* layerRoot = Cast<UOdysseyAnimationLayer>(LayerRoot);
    if ( !layerRoot )
        return idComposition;
    
    idComposition.Append(layerRoot->GetImageRenderingComposition(iRenderType, iFrameIndex));
    return idComposition;
}

TArray<::ULIS::FRectI>
UOdysseyAnimationLayerStack::GetImageRenderingRects() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return {};

    return animation->GetImageRenderingRects();
}

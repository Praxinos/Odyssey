// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#include "OdysseyLayerStackRenderContext.h"

FOdysseyLayerStackRenderContext::FOdysseyLayerStackRenderContext() = default;
FOdysseyLayerStackRenderContext::~FOdysseyLayerStackRenderContext() = default;

FName FOdysseyLayerStackRenderContext::TypeName()
{
    return TEXT("OdysseyLayerStack");
}

TUniquePtr<IOdysseyRenderContextExtension> FOdysseyLayerStackRenderContext::Clone() const
{
    TUniquePtr<FOdysseyLayerStackRenderContext> Copy = MakeUnique<FOdysseyLayerStackRenderContext>();
    Copy->Overrides = Overrides;
    Copy->bLighttablePass = bLighttablePass;
    Copy->bUseDisplayOnlyCurrentLayer = bUseDisplayOnlyCurrentLayer;
    return Copy;
}

const FOdysseyLayerPropertiesOverride* FOdysseyLayerStackRenderContext::Find(const FGuid& Id) const
{
    const FOdysseyLayerPropertiesOverride* LayerOverride = Overrides.Find(Id);
    return LayerOverride && (!bLighttablePass || LayerOverride->bApplyToLighttable) ? LayerOverride : nullptr;
}

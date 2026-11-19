// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "OdysseyRenderContext.h"
#include "OdysseyBlendMode.h"

struct FOdysseyLighttable;

/** Per-node layer settings. Unset values preserve the document settings. */
struct ODYSSEYLAYERSTACK_API FOdysseyLayerStackRenderOverride
{
    TOptional<bool> bVisible;
    TOptional<float> Opacity;
    TOptional<EOdysseyBlendMode> BlendMode;
    TOptional<bool> bInheritsAlpha;
    TSharedPtr<const FOdysseyLighttable> Lighttable;
    bool bApplyToLighttable = false;
};

/** Settings interpreted by OdysseyLayerStack while rendering a request. */
struct ODYSSEYLAYERSTACK_API FOdysseyLayerStackRenderContext : IOdysseyRenderContextExtension
{
    FOdysseyLayerStackRenderContext();
    virtual ~FOdysseyLayerStackRenderContext() override;

    static FName TypeName();

    virtual TUniquePtr<IOdysseyRenderContextExtension> Clone() const override;
    const FOdysseyLayerStackRenderOverride* Find(const FGuid& Id) const;

    TMap<FGuid, FOdysseyLayerStackRenderOverride> Overrides;
    bool bLighttablePass = false;
    bool bUseDisplayOnlyCurrentLayer = true;
};

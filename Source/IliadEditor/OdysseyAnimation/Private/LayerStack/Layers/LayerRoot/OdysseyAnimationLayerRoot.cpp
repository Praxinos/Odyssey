// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerRoot/OdysseyAnimationLayerRoot.h"
#include "OdysseyAnimationProxy.h"

UOdysseyAnimationLayerRoot::UOdysseyAnimationLayerRoot()
{
    CanHaveChildren = true;
    HasLighttable = false;
}

void
UOdysseyAnimationLayerRoot::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    mProxy = MakeShared<FOdysseyAnimationProxy>(this);
}

void
UOdysseyAnimationLayerRoot::PostLoad()
{
    Super::PostLoad();
    mProxy->PostLoad();
}

TSharedPtr<FOdysseyAnimationProxy>
UOdysseyAnimationLayerRoot::GetProxy() const
{
    return mProxy;
}

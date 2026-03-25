// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayer.h"
#include "ArianeLayerFolder.h"

UArianeLayer::~UArianeLayer()
{
}

UArianeLayer::UArianeLayer()
    : bVisible ( true )
{
/*
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.SetTickFunctionEnable(true);

    bAutoRegister = true;
    bWantsInitializeComponent = true;
    bAutoActivate = true;
    bTickInEditor = true;
*/
}

UArianeLayerFolder*
UArianeLayer::GetParent()
{
    return Cast<UArianeLayerFolder>(GetOuter());
}

void
UArianeLayer::SetVisible( bool bInVisible )
{
    bVisible = bInVisible;
}

bool
UArianeLayer::IsVisible( bool bHierarchical )
{
    if( bHierarchical )
    {
        UArianeLayerFolder* ParentLayer = GetParent();

        return ParentLayer ? ( ParentLayer->IsVisible( bHierarchical ) && bVisible ) : bVisible;
    }

    return bVisible;
}

void
UArianeLayer::SetLocked( bool bInLocked )
{
    bLocked = bInLocked;
}

bool
UArianeLayer::IsLocked( bool bHierarchical )
{
    if( bHierarchical )
    {
        UArianeLayerFolder* ParentLayer = GetParent();

        return ParentLayer ? ( ParentLayer->IsLocked( bHierarchical ) && bLocked ) : bLocked;
    }

    return bLocked;
}

// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IOdysseyLayer.h"
#include "IOdysseyLayerImageRenderingCapability.h"
#include <ULIS3>


/**
 * Implements a layer which is a Root
 */
class ODYSSEYLAYER_API FOdysseyRootLayer :
    public IOdysseyLayer,
    public IOdysseyLayerImageRenderingCapability
{
public:
    // Construction / Destruction
    virtual ~FOdysseyRootLayer();
    FOdysseyRootLayer( const FOdysseyRootLayer& iLayer);
    FOdysseyRootLayer();

    virtual FOdysseyRootLayer* Clone() const override;
    virtual void Serialize(FArchive &Ar) override;
    virtual void RenderImage(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos) override;
    virtual bool ImplementsCapability(FGuid iGuid) const override;
    virtual void* GetCapabilityPtrFromGuid(FGuid iGuid) override;

    virtual void AddNode( TSharedPtr<IOdysseyLayer> iNode, int iIndexEmplace = -1 ) override;
    virtual void DeleteNode( int iIndex ) override;

public:
    // Public API
    void OnChildImageResultChanged(TSharedPtr<IOdysseyLayer> iLayer);
};
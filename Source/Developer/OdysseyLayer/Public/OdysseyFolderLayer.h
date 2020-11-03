// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IOdysseyLayer.h"
#include "IOdysseyLayerImageBlendingCapability.h"
#include <ULIS3>


/**
 * Implements a layer which is a folder
 */
class ODYSSEYLAYER_API FOdysseyFolderLayer :
    public IOdysseyLayer,
    public IOdysseyLayerImageBlendingCapability
{
public:
    // Layer Is Open State Changed Event
    // Bool is for the previous value
    DECLARE_MULTICAST_DELEGATE_OneParam(FOdysseyLayerIsOpenChanged, bool);

public:
    // Construction / Destruction
    virtual ~FOdysseyFolderLayer();
    FOdysseyFolderLayer( const FOdysseyFolderLayer& iLayer);
    FOdysseyFolderLayer( const FName& iName );

    virtual FOdysseyFolderLayer* Clone() const override;
    virtual void Serialize(FArchive &Ar);
    virtual void Blend(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos) override;
    virtual void RenderImage(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos) override;
    virtual bool ImplementsCapability(FGuid iGuid) const override;
    virtual void* GetCapabilityPtrFromGuid(FGuid iGuid) override;
    
    virtual void AddNode( TSharedPtr<IOdysseyLayer> iNode, int iIndexEmplace = -1 ) override;
    virtual void DeleteNode( int iIndex ) override;
public:
    // Public API
    bool IsOpen() const;
    void SetIsOpen( bool iIsOpen );

    void OnChildImageResultChanged(TSharedPtr<IOdysseyLayer> iLayer);

    virtual void SetIsVisible(bool iIsVisible) override;

public:
    FOdysseyLayerIsOpenChanged& IsOpenChangedDelegate();

private:
    bool                    mIsOpen; // Todo: This should not be here in the model, but it should be part of the view.
    
    FOdysseyLayerIsOpenChanged mIsOpenChangedDelegate;
};
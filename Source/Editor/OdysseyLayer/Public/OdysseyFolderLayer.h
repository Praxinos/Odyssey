// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IOdysseyLayer.h"
#include "IOdysseyLayerImageBlendingCapability.h"
#include <ULIS>


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
    DECLARE_MULTICAST_DELEGATE_OneParam( FOdysseyLayerIsOpenChanged, bool );

public:
    // Construction / Destruction
    virtual ~FOdysseyFolderLayer();
    FOdysseyFolderLayer( const FOdysseyFolderLayer& iLayer);
    FOdysseyFolderLayer( const FName& iName );

    virtual FOdysseyFolderLayer* Clone() const override;
    virtual void Serialize(FArchive &Ar);
    virtual TArray<::ULIS::FEvent> Blend( ::ULIS::FBlock** ioBlocks, const ::ULIS::FRectI* iRects, const ::ULIS::FVec2I* iPositions, const uint32 iNum, const ::ULIS::FEvent* iEvents ) override;
    virtual TArray<::ULIS::FEvent> RenderImage( ::ULIS::FBlock** ioBlocks, const ::ULIS::FRectI* iRects, const ::ULIS::FVec2I* iPositions, const uint32 iNum ) override;
    virtual bool ImplementsCapability(FGuid iGuid) const override;
    virtual void* GetCapabilityPtrFromGuid(FGuid iGuid) override;
    
    virtual void AddNode( TSharedPtr<IOdysseyLayer> iNode, int iIndexEmplace = -1 ) override;
    virtual void DeleteNode( int iIndex ) override;
public:
    // Public API
    bool IsOpen() const;
    void SetIsOpen( bool iIsOpen );

    void OnChildImageResultChanged( const ::ULIS::FRectI* iRects, const uint32 iNumRects, TSharedPtr< IOdysseyLayer > iLayer );

    virtual void SetIsVisible(bool iIsVisible) override;

public:
    FOdysseyLayerIsOpenChanged& IsOpenChangedDelegate();

private:
    bool                    mIsOpen; // Todo: This should not be here in the model, but it should be part of the view.
    
    FOdysseyLayerIsOpenChanged mIsOpenChangedDelegate;
};
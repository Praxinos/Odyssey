// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyClipboard.h"

class UOdysseyLayer;
class UOdysseyLayerStack;

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyLayerStackClipboardData
    : public IOdysseyClipboardData
    , public FGCObject
{
public:
    virtual ~FOdysseyLayerStackClipboardData() {};
    FOdysseyLayerStackClipboardData();
    FOdysseyLayerStackClipboardData(const TArray<UOdysseyLayer*>& iLayers);

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

public:
    static const FGuid& StaticId();

public:
    bool CanPaste(UOdysseyLayerStack* iLayerStack) const;
    TArray<UOdysseyLayer*> Paste(UOdysseyLayerStack* iLayerStack) const;

private:
    TArray<TObjectPtr<UOdysseyLayer>> mLayers;
};

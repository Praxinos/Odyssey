// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyClipboard.h"

#include <ULIS>

class UOdysseyLayer;

class FOdysseyBlockClipboardData
    : public IOdysseyClipboardData
    , public FGCObject //Allows us to register External UObject in Garbage Collector
{
public:
    virtual ~FOdysseyBlockClipboardData() {};
    FOdysseyBlockClipboardData();
    FOdysseyBlockClipboardData( TSharedPtr<::ULIS::FBlock> iBlock );

 public:
     static const FGuid& StaticId();

public:
    //Getter
    TSharedPtr<::ULIS::FBlock> GetBlock() const;

public:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

private:
    void Copy( TSharedPtr<::ULIS::FBlock> iBlock );

private:
    TSharedPtr<::ULIS::FBlock> mBlockCopy;

};

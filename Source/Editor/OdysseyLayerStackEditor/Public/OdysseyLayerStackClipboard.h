// IDDN FR.001.250001.005.S.P.2019.000.00000
// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyLayerStackClipboard
    : public FGCObject
{
public:
    static FOdysseyLayerStackClipboard* Get();

private:
    FOdysseyLayerStackClipboard();

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

public:
    void Copy(const TArray<UOdysseyLayer*>& iLayers);
    const TArray<UOdysseyLayer*>& GetLayers() const;

private:
    TArray<UOdysseyLayer*> mLayers;
};

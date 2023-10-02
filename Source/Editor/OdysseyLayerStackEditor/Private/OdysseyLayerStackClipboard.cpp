// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerStackClipboard.h"

FOdysseyLayerStackClipboard*
FOdysseyLayerStackClipboard::Get()
{
    static FOdysseyLayerStackClipboard clipboard;
    return &clipboard;
}

FOdysseyLayerStackClipboard::FOdysseyLayerStackClipboard()
{
}

void
FOdysseyLayerStackClipboard::AddReferencedObjects(FReferenceCollector& iCollector)
{
    iCollector.AddReferencedObjects( mLayers );
}

FString
FOdysseyLayerStackClipboard::GetReferencerName() const
{
    return "FOdysseyLayerStackClipboard";
}

void
FOdysseyLayerStackClipboard::Copy(const TArray<UOdysseyLayer*>& iLayers)
{
    mLayers = iLayers;
}

const TArray<UOdysseyLayer*>&
FOdysseyLayerStackClipboard::GetLayers() const
{
    return mLayers;
}

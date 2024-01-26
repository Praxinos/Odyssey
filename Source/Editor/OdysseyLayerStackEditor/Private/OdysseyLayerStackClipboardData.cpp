// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerStackClipboardData.h"
#include "UObject/OdysseyObjectEditorUtils.h"

FOdysseyLayerStackClipboardData::FOdysseyLayerStackClipboardData()
    : IOdysseyClipboardData(StaticId())
    , mLayers()
{
}

FOdysseyLayerStackClipboardData::FOdysseyLayerStackClipboardData(const TArray<UOdysseyLayer*>& iLayers)
    : IOdysseyClipboardData(StaticId())
    , mLayers(iLayers)
{
}

const FGuid&
FOdysseyLayerStackClipboardData::StaticId()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

void
FOdysseyLayerStackClipboardData::AddReferencedObjects(FReferenceCollector& iCollector)
{
    iCollector.AddReferencedObjects( mLayers );
}

FString
FOdysseyLayerStackClipboardData::GetReferencerName() const
{
    return "FOdysseyLayerStackClipboardData";
}

bool
FOdysseyLayerStackClipboardData::CanPaste(UOdysseyLayerStack* iLayerStack) const
{
    bool notSupported = mLayers.ContainsByPredicate(
        [iLayerStack](UOdysseyLayer* iLayer)
        {
            return !iLayerStack->SupportsLayerClass(iLayer->GetClass());
        }
    );

    return !notSupported;
}

TArray<UOdysseyLayer*>
FOdysseyLayerStackClipboardData::Paste(UOdysseyLayerStack* iLayerStack) const
{
    UOdysseyLayer* parent = iLayerStack->CurrentLayer.Get()->GetParent();
    int indexInParent = iLayerStack->CurrentLayer.Get()->GetIndexInParent();
    TArray<UOdysseyLayer*> pastedLayers = iLayerStack->CopyLayers(mLayers, parent, indexInParent);
    FOdysseyObjectEditorUtils::SetPropertyValue(iLayerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(pastedLayers[0]));
    return pastedLayers;
}

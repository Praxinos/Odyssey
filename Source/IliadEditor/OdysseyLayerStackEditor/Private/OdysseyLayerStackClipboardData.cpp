// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLayerStackClipboardData.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayer.h"

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
    TArray<UOdysseyLayer*> pastedLayers;
    UOdysseyLayer* currentLayer = iLayerStack->GetCurrentLayer();
    UOdysseyLayer* parent = currentLayer->GetParent();
    int index = currentLayer->GetIndexInParent();

    if (currentLayer->CanHaveChildren() && currentLayer->ShouldDisplayChildren())
        pastedLayers = iLayerStack->CopyLayers(mLayers, currentLayer);
    else
        pastedLayers = iLayerStack->CopyLayers(mLayers, parent, index);

    iLayerStack->SetCurrentLayer(pastedLayers[0]);
    return pastedLayers;
}

// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationLayerStack.h"
#include "LayerRoot/OdysseyAnimationLayerRoot.h"
#include "LayerFolder/OdysseyAnimationLayerFolder.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyRectUtils.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellSelection.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/OdysseyUndoDelegates.h"
#include "UObject/OdysseyObjectEditorUtils.h"

//===============================================

UOdysseyAnimationLayerStack::UOdysseyAnimationLayerStack()
    : mCellSelection(MakeShared<FOdysseyAnimationCellSelection>(this))
{
    CompatibleLayers.Add(UOdysseyAnimationLayerFolder::StaticClass());
    CompatibleLayers.Add(UOdysseyAnimationLayerImageRaster::StaticClass());
    CompatibleLayers.Add(UOdysseyAnimationLayerImageVector::StaticClass());

    LayerRootClass = UOdysseyAnimationLayerRoot::StaticClass();
}

void
UOdysseyAnimationLayerStack::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationLayerStack::OnRenderingChanged);
}

int
UOdysseyAnimationLayerStack::GetWidth() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return Super::GetWidth();

    return animation->GetWidth();
}

int
UOdysseyAnimationLayerStack::GetHeight() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return Super::GetHeight();

    return animation->GetHeight();
}

::ULIS::eFormat
UOdysseyAnimationLayerStack::GetFormat() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return Super::GetFormat();

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

    return format;
}

UOdysseyAnimation*
UOdysseyAnimationLayerStack::GetAnimation() const
{
    UObject* outer = GetOuter();
    while(outer)
    {
        if (outer->GetClass() == UOdysseyAnimation::StaticClass())
            return Cast<UOdysseyAnimation>(outer);

        outer = outer->GetOuter();
    }

    return nullptr;
}

FInt32Range
UOdysseyAnimationLayerStack::GetFrameRange() const
{
    return Cast<UOdysseyAnimationLayerRoot>(LayerRoot)->GetFrameRange();
}

TArray<FIntRect>
UOdysseyAnimationLayerStack::GetRenderingRects() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return {};

    return { FIntRect(0, 0, animation->GetWidth(), animation->GetHeight()) };
}

TSharedRef<FOdysseyAnimationCellSelection>
UOdysseyAnimationLayerStack::GetCellSelection() const
{
    return mCellSelection;
}

void
UOdysseyAnimationLayerStack::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationLayerStack::OnRenderingChanged);
    if (!iEvent.IsInteractive())
    {
        const FGuid& eventId = iEvent.GetId();
        FInt32Range frameRange = GetFrameRange();
        int startFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
        int endFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetUpperBoundValue() : frameRange.GetUpperBoundValue() - 1;
        for (int i = startFrame; i <= endFrame; i++)
        {
            TArray<FGuid> composition = GetRenderingComposition(EOdysseyRenderingType::Editor, i);
            if (composition.Contains(eventId))
            {
                MarkPackageDirty();
                break;
            }
        }
    }
}

// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDElement.h"
#include "CanvasTypes.h"

IMPLEMENT_HIT_PROXY(HOdysseyHUDElementHitProxy, HHitProxy)

FOdysseyHUDElement::~FOdysseyHUDElement()
{
    EmptyElements();
}

FOdysseyHUDElement::FOdysseyHUDElement()
    : mIsCaptured( false )
    , mIsVisible(true)
{
}

void
FOdysseyHUDElement::Draw(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    if (!IsVisible())
        return;

    // Parent customization always has priority over the customization in each children
    // This way, we only have to customize the HUD once (for the parent) and all children will share it
    FDrawHUDParams childParams = iParams;

    if( mCustomization.mIsActive )
        childParams.mCustomization = &mCustomization;

    DrawHUD(iParams);

    for (TSharedPtr<FOdysseyHUDElement> element : mElements)
        element->Draw(childParams);
}

void
FOdysseyHUDElement::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
}

void FOdysseyHUDElement::DrawCustomizedLine(FCanvas* iCanvas, const FVector2D& iStart, const FVector2D& iEnd, float iTimeOffset, float iPatternLength, float& ioCumulLength, int& ioColorIndex, const TArray<FLinearColor>& iColors, const FHUDCustomization& iCustomization, FBatchedElements* iBatchedElements) const
{
    FVector2D dir = iEnd - iStart;
    float segmentLength = dir.Size();
    if (segmentLength <= 0.f)
        return;

    dir.Normalize();
    float current = 0.f;

    while (current < segmentLength)
    {
        // Absolute position along polygon perimeter, shifted by time offset
        float globalPos = ioCumulLength + current + iTimeOffset;
        float cycleOffset = FMath::Fmod(globalPos, iPatternLength);
        float segmentLeft = iCustomization.mSegmentLength - cycleOffset;

        if (segmentLeft <= 0.f)
        {
            // We are inside the gap -> skip ahead
            float skip = -segmentLeft + iCustomization.mGapLength;
            current += skip;
            continue;
        }

        // Draw the remaining length of this segment
        float available = segmentLength - current;
        float drawLen = FMath::Min(segmentLeft, available);

        FVector2D segmentStart = iStart + dir * current;
        FVector2D segmentEnd = iStart + dir * (current + drawLen);

        iBatchedElements->AddTranslucentLine(
            FVector(segmentStart, 0.f),
            FVector(segmentEnd, 0.f),
            iColors[ioColorIndex],
            iCanvas->GetHitProxyId(),
            1.f,   // thickness
            0.f,   // depth bias
            true   // antialiasing
        );

        // move forward past dash + gap
        current += drawLen + iCustomization.mGapLength;
        ioColorIndex = (ioColorIndex + 1) % iColors.Num();
    }

    ioCumulLength += segmentLength;
}

void FOdysseyHUDElement::AddElement(TSharedPtr<FOdysseyHUDElement> iElementToAdd)
{
    if( iElementToAdd != nullptr )
        mElements.Emplace(iElementToAdd );
}

void FOdysseyHUDElement::RemoveElement(TSharedPtr<FOdysseyHUDElement> iElementToRemove)
{
    if( iElementToRemove != nullptr )
        mElements.Remove(iElementToRemove);
}

void
FOdysseyHUDElement::EmptyElements()
{
    mElements.Empty();
}

void
FOdysseyHUDElement::SetCustomization(const FHUDCustomization& iCustomization)
{
    mCustomization = iCustomization;
}

const FOdysseyHUDElement::FHUDCustomization&
FOdysseyHUDElement::GetCustomization() const
{
    return mCustomization;
}

void FOdysseyHUDElement::ActivateCustomization()
{
    mCustomization.mIsActive = true;
}

void FOdysseyHUDElement::InactivateCustomization()
{
    mCustomization.mIsActive = false;
}

bool FOdysseyHUDElement::IsCaptured() const
{
    bool isCaptured = mIsCaptured;
    return isCaptured;
}

void FOdysseyHUDElement::Capture(bool iCapture)
{
    mIsCaptured = iCapture;
}

bool
FOdysseyHUDElement::IsVisible() const
{
    return mIsVisible.Get();
}

void
FOdysseyHUDElement::SetIsVisible(TAttribute<bool> iIsVisible)
{
    mIsVisible = iIsVisible;
}

bool
FOdysseyHUDElement::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool
FOdysseyHUDElement::OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool
FOdysseyHUDElement::OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool
FOdysseyHUDElement::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

void
FOdysseyHUDElement::OnMouseEnter()
{

}

void
FOdysseyHUDElement::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void
FOdysseyHUDElement::OnMouseLeave()
{

}

void
FOdysseyHUDElement::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}

void
FOdysseyHUDElement::AddReferencedObjects(FReferenceCollector& Collector)
{

}

FString
FOdysseyHUDElement::GetReferencerName() const
{
    return "FOdysseyHUDElement";
}

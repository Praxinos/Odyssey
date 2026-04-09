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
    mCachedTextureToHUD = iParams.mTextureToHUD;
    mCachedHUDToTexture = iParams.mHUDToTexture;
}

FVector2D
FOdysseyHUDElement::TextureToHUD( double iX, double iY ) const
{
    return TextureToHUD( FVector2D( iX, iY ) );
}

FVector2D
FOdysseyHUDElement::TextureToHUD( const FVector2D& iPosition ) const
{
    return mCachedTextureToHUD.Execute( iPosition );
}

FVector2D
FOdysseyHUDElement::HUDToTexture( double iX, double iY ) const
{
    return HUDToTexture( FVector2D( iX, iY ) );
}

FVector2D
FOdysseyHUDElement::HUDToTexture( const FVector2D& iPosition ) const
{
    return mCachedHUDToTexture.Execute( iPosition );
}

void
FOdysseyHUDElement::SetReference(EOdysseyHUDReference iReference)
{
    mReference = iReference;
}

EOdysseyHUDReference
FOdysseyHUDElement::GetReference() const
{
    return mReference;
}

void
FOdysseyHUDElement::InitDrawCustomizedLine(FCanvas* iCanvas, const FHUDCustomization& iCustomization, const FLinearColor& iDefaultColor)
{
    mCustomizedLinesParams.mCanvas = iCanvas;
    mCustomizedLinesParams.mBatchedElements = iCanvas->GetBatchedElements(FCanvas::ET_Line);
    mCustomizedLinesParams.mSegmentLength = iCustomization.mSegmentLength;
    mCustomizedLinesParams.mGapLength = iCustomization.mGapLength;

    mCustomizedLinesParams.mColors = iCustomization.mColors;
    if (mCustomizedLinesParams.mColors.Num() == 0)
        mCustomizedLinesParams.mColors.Add(iDefaultColor);

    mCustomizedLinesParams.mColorIndex = 0;

    // Total pattern length (Segment + Gap)
    float patternLength = mCustomizedLinesParams.mSegmentLength * mCustomizedLinesParams.mColors.Num() + mCustomizedLinesParams.mGapLength;
    double time = FPlatformTime::Seconds();
    float timeOffset = FMath::Fmod(time * iCustomization.mSpeed, patternLength);
    while(timeOffset > 0.f)
    {
        mCustomizedLinesParams.mColorIndex--;
        if (mCustomizedLinesParams.mColorIndex < 0)
            mCustomizedLinesParams.mColorIndex = mCustomizedLinesParams.mColors.Num(); //takes the gap into account (so no -1 here)

        timeOffset -= mCustomizedLinesParams.mColorIndex < mCustomizedLinesParams.mColors.Num() ? mCustomizedLinesParams.mSegmentLength : mCustomizedLinesParams.mGapLength;
    }
    mCustomizedLinesParams.mStartOffset = -timeOffset;
}

void
//FOdysseyHUDElement::DrawCustomizedLine(FCanvas* iCanvas, const FVector2D& iStart, const FVector2D& iEnd, float& ioStartOffset, int& ioColorIndex, const TArray<FLinearColor>& iColors, const FHUDCustomization& iCustomization, FBatchedElements* iBatchedElements) const
FOdysseyHUDElement::DrawCustomizedLine(const FVector2D& iStart, const FVector2D& iEnd)
{
    FVector2D dir = iEnd - iStart;
    float segmentLength = dir.Size();
    if (segmentLength <= 0.f)
        return;

    dir.Normalize();
    float current = 0.f;

    float lineMaxLength = mCustomizedLinesParams.mColorIndex < mCustomizedLinesParams.mColors.Num() ? mCustomizedLinesParams.mSegmentLength : mCustomizedLinesParams.mGapLength;
    float lineSize = FMath::Min(lineMaxLength - mCustomizedLinesParams.mStartOffset, segmentLength);

    while(true)
    {
        //Draw only if we're not in a gap
        if (lineSize > 0.f && mCustomizedLinesParams.mColorIndex < mCustomizedLinesParams.mColors.Num())
        {
            FVector2D segmentStart = iStart + dir * current;
            FVector2D segmentEnd = iStart + dir * (current + lineSize);

            //UE_LOG(LogTemp, Warning, TEXT("current = %f, mCustomizedLinesParams.mColorIndex = %d"), current, mCustomizedLinesParams.mColorIndex);


            mCustomizedLinesParams.mBatchedElements->AddTranslucentLine(
                FVector(segmentStart, 0.f),
                FVector(segmentEnd, 0.f),
                mCustomizedLinesParams.mColors[mCustomizedLinesParams.mColorIndex],
                mCustomizedLinesParams.mCanvas->GetHitProxyId(),
                1.f,   // thickness
                0.f,   // depth bias
                true   // antialiasing
            );
        }

        current += lineSize;
        if (current >= segmentLength)
        {
            mCustomizedLinesParams.mStartOffset = mCustomizedLinesParams.mStartOffset + lineSize;
            break;
        }

        mCustomizedLinesParams.mColorIndex = (mCustomizedLinesParams.mColorIndex + 1) % (mCustomizedLinesParams.mColors.Num() + 1);
        mCustomizedLinesParams.mStartOffset = 0;
        lineMaxLength = mCustomizedLinesParams.mColorIndex < mCustomizedLinesParams.mColors.Num() ? mCustomizedLinesParams.mSegmentLength : mCustomizedLinesParams.mGapLength;
        lineSize = FMath::Min(lineMaxLength, segmentLength - current);
    }
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

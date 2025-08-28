// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDElement.h"

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

// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDElement.h"

IMPLEMENT_HIT_PROXY(HOdysseyHUDElementHitProxy, HHitProxy)

FOdysseyHUDElement::~FOdysseyHUDElement()
{
    EmptyElements();
}

FOdysseyHUDElement::FOdysseyHUDElement()
    : mIsCaptured( false )
{
}

void
FOdysseyHUDElement::DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams)
{
    for (TSharedPtr<FOdysseyHUDElement> element : mElements)
        element->DrawHUD(iParams);
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
FOdysseyHUDElement::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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

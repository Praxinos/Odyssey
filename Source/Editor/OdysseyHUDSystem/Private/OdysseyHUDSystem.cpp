// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDSystem.h"

#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyHUDElement.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyHUDSystem::~FOdysseyHUDSystem()
{
}

FOdysseyHUDSystem::FOdysseyHUDSystem()
{
}

void
FOdysseyHUDSystem::AddElement(TSharedPtr<FOdysseyHUDElement> iElement)
{
    mElements.Add(iElement);
}

void
FOdysseyHUDSystem::RemoveElement(TSharedPtr<FOdysseyHUDElement> iElement)
{
    mElements.Remove(iElement);
}

void
FOdysseyHUDSystem::DrawHUD( const FDrawHUDParams& iParams )
{
    for (TSharedPtr<FOdysseyHUDElement> element : mElements)
    {
        element->DrawHUD(iParams);
    }
}
// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUD.h"

#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyHUDElement.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyHUD::~FOdysseyHUD()
{
}

FOdysseyHUD::FOdysseyHUD()
{
}

void
FOdysseyHUD::AddElement(TSharedPtr<FOdysseyHUDElement> iElement)
{
    mElements.Add(iElement);
}

void
FOdysseyHUD::RemoveElement(TSharedPtr<FOdysseyHUDElement> iElement)
{
    mElements.Remove(iElement);
}

void
FOdysseyHUD::DrawHUD( const FDrawHUDParams& iParams )
{
    for (TSharedPtr<FOdysseyHUDElement> element : mElements)
    {
        element->Draw(iParams);
    }
}

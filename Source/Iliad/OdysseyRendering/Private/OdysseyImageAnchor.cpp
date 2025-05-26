// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyImageAnchor.h"

FVector2D
FOdysseyImageAnchor::GetPosition(FVector2D iDestination, FVector2D iSourceSize) const
{
    FVector2D halfSourceSize = iSourceSize / 2.f;

    switch (Position)
    {
    case EOdysseyImageAnchor::TopLeft:
    {
        return iDestination;
    }
    break;
    case EOdysseyImageAnchor::TopCenter:
    {
        return iDestination - FVector2D(halfSourceSize.X, 0.f);
    }
    break;
    case EOdysseyImageAnchor::TopRight:
    {
        return iDestination - FVector2D(iSourceSize.X, 0.f);
    }
    break;
    case EOdysseyImageAnchor::CenterLeft:
    {
        return iDestination - FVector2D(0.f, halfSourceSize.Y);
    }
    break;
    case EOdysseyImageAnchor::Center:
    {
        return iDestination - FVector2D(halfSourceSize.X, halfSourceSize.Y);
    }
    break;
    case EOdysseyImageAnchor::CenterRight:
    {
        return iDestination - FVector2D(iSourceSize.X, halfSourceSize.Y);
    }
    break;
    case EOdysseyImageAnchor::BottomLeft:
    {
        return iDestination - FVector2D(0.f, iSourceSize.Y);
    }
    break;
    case EOdysseyImageAnchor::BottomCenter:
    {
        return iDestination - FVector2D(halfSourceSize.X, iSourceSize.Y);
    }
    break;
    case EOdysseyImageAnchor::BottomRight:
    {
        return iDestination - FVector2D(iSourceSize.X, iSourceSize.Y);
    }
    break;
    case EOdysseyImageAnchor::Custom:
    {
        return iDestination - CustomPosition;
    }
    break;
    }

    return iDestination;
}

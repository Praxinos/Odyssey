// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorToolConfiguration.h"

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateStyleRegistry.h"

static const FVector2D kIconSize = FVector2D(24.f, 24.f);

const FSlateBrush* FIconToolConfiguration::MakeIconBrush() const
{
    if (mIconSource == EToolIconSource::Style)
    {
        return FOdysseyStyle::GetBrush(mIconStyleSet);
    }
    else if (mIconSource == EToolIconSource::Texture && mIconTexture)
    {
        FSlateBrush* brush = new FSlateBrush();
        brush->SetResourceObject(mIconTexture);
        brush->ImageSize = kIconSize;
        return brush;
    }

    return FStyleDefaults::GetNoBrush();
}

// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Proxies/OdysseyBrushPivot.h"
#include "OdysseyBlock.h"
#include "OdysseySurface.h"

/////////////////////////////////////////////////////
// Utilities
FRectF
//::ul3::FRect
ComputeRectWithPivot(TSharedPtr<FOdysseyBlock> iBlock, const FOdysseyPivot& iPivot, float iX, float iY )
{
    int width = iBlock->Width();
    int height = iBlock->Height();
    float width2 = width / 2.f;
    float height2 = height / 2.f;

    FRectF result;
    //::ul3::FRect result;
    result.x = iX;
    result.y = iY;
    result.w = width;
    result.h = height;

    switch( iPivot.Reference ) {
        case EPivotReference::kTopLeft      : { result.x = iX;          result.y = iY;              break; }
        case EPivotReference::kTopMiddle    : { result.x = iX - width2; result.y = iY;              break; }
        case EPivotReference::kTopRight     : { result.x = iX - width;  result.y = iY;              break; }
        case EPivotReference::kMiddleLeft   : { result.x = iX;          result.y = iY - height2;    break; }
        case EPivotReference::kCenter       : { result.x = iX - width2; result.y = iY - height2;    break; }
        case EPivotReference::kMiddleRight  : { result.x = iX - width;  result.y = iY - height2;    break; }
        case EPivotReference::kBotLeft      : { result.x = iX;          result.y = iY - height;     break; }
        case EPivotReference::kBotMiddle    : { result.x = iX - width2; result.y = iY - height;     break; }
        case EPivotReference::kBotRight     : { result.x = iX - width;  result.y = iY - height;     break; }
    }

    FVector2D computedOffset = iPivot.OffsetMode == EPivotOffsetMode::kAbsolute ? iPivot.Offset : iPivot.Offset * FVector2D( width, height );
    result.x += computedOffset.X;
    result.y += computedOffset.Y;

    return  result;
}


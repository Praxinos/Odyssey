// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushPivot.h"
#include "OdysseySurface.h"

/////////////////////////////////////////////////////
// Utilities
::ul3::FRect
ComputeRectWithPivot( FOdysseyBlock* iBlock, const FOdysseyPivot& iPivot, float iX, float iY )
{
    ::ul3::FRect result;
    int width = iBlock->Width();
    int height = iBlock->Height();
    int width2 = width / 2;
    int height2 = height / 2;

    FVector2D computedOffset = iPivot.OffsetMode == EPivotOffsetMode::kAbsolute ? iPivot.Offset : iPivot.Offset * FVector2D( width, height );
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

    result.x += computedOffset.X;
    result.y += computedOffset.Y;
    return  result;
}


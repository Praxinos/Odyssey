// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTileUtils.h"

namespace Odyssey::TileUtils {

static int32 FloorDiv(int32 Value, int32 Divisor)
{
    return Value >= 0
        ? Value / Divisor
        : (Value + 1) / Divisor - 1;
}

TArray<FIntPoint>
GetTilePositionsFromRect(int32 InTileSize, const FIntRect& InRect)
{
    if (InRect.Min.X >= InRect.Max.X || InRect.Min.Y >= InRect.Max.Y)
        return {};

    const int32 X1 = FloorDiv(InRect.Min.X, InTileSize);
    const int32 Y1 = FloorDiv(InRect.Min.Y, InTileSize);
    const int32 X2 = FloorDiv(InRect.Max.X - 1, InTileSize);
    const int32 Y2 = FloorDiv(InRect.Max.Y - 1, InTileSize);

    int W = X2 - X1 + 1;
    int H = Y2 - Y1 + 1;

    if (W <= 0 || H <= 0)
        return {};

    TArray<FIntPoint> TilePositions;
    TilePositions.Reserve(W * H);
    for( int Y = Y1; Y <= Y2; ++Y )
    {
        for( int X = X1; X <= X2; ++X )
        {
            TilePositions.Add({X, Y});
        }
    }

    return TilePositions;
}

}

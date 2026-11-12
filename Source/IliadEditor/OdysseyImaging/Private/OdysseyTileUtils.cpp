// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTileUtils.h"

namespace Odyssey::TileUtils {

TArray<FIntPoint>
GetTilePositionsFromRect(uint32 InTileSize, const FIntRect& InRect)
{
    if (InRect.Min.X >= InRect.Max.X || InRect.Min.Y >= InRect.Max.Y)
        return {};

    int X1 = InRect.Min.X / InTileSize;
    int Y1 = InRect.Min.Y / InTileSize;
    int X2 = InRect.Max.X / InTileSize;
    int Y2 = InRect.Max.Y / InTileSize;

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

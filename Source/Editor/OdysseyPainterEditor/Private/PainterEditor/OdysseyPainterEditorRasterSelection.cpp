// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorRasterSelection.h"

#include "ULISLoaderModule.h"
#include "OdysseyHUDLine.h"

FOdysseyPainterEditorRasterSelection::~FOdysseyPainterEditorRasterSelection()
{
}

FOdysseyPainterEditorRasterSelection::FOdysseyPainterEditorRasterSelection()
    : mHUD(MakeShared<FOdysseyHUDElement>())
{
}

FSimpleMulticastDelegate& 
FOdysseyPainterEditorRasterSelection::OnChanged()
{
    return mOnChanged;
}

void
FOdysseyPainterEditorRasterSelection::Init(int iWidth, int iHeight)
{
    mBlock = MakeShared<::ULIS::FBlock>(iWidth, iHeight, ::ULIS::Format_GF);
    Clear();
    RefreshHUD();

    mOnChanged.Broadcast();
}

void
FOdysseyPainterEditorRasterSelection::Reset()
{
    mBlock = nullptr;
    RefreshHUD();

    mOnChanged.Broadcast();
}

void
FOdysseyPainterEditorRasterSelection::Clear()
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_GF);
    ctx.Clear(*mBlock);
    ctx.Finish();

    RefreshHUD();

    mOnChanged.Broadcast();
}

bool 
FOdysseyPainterEditorRasterSelection::IsEmpty() const
{
    return mBoundingRect.Area() <= 0;
}

void
FOdysseyPainterEditorRasterSelection::Add(const TArray<FVector2D>& iPolygon)
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_GF);
    std::vector<::ULIS::FVec2I> points;
    for (const FVector2D& point : iPolygon)
    {
        points.push_back(::ULIS::FVec2I(point.X, point.Y));
    }

    ctx.DrawPolygon(*mBlock, points, ::ULIS::FColor::FromGrey8(255), true);
    ctx.Finish();

    RefreshHUD();

    mOnChanged.Broadcast();
}

void
FOdysseyPainterEditorRasterSelection::Substract(const TArray<FVector2D>& iPolygon)
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_GF);
    std::vector<::ULIS::FVec2I> points;
    for ( const FVector2D& point : iPolygon )
    {
        points.push_back(::ULIS::FVec2I(point.X, point.Y));
    }

    ctx.DrawPolygon(*mBlock, points, ::ULIS::FColor::FromGrey8(0), true);
    ctx.Finish();

    RefreshHUD();

    mOnChanged.Broadcast();
}

::ULIS::FRectI
FOdysseyPainterEditorRasterSelection::ComputeBoundingRect(const TArray<FVector2D>& iPoints ) const
{
    if ( iPoints.IsEmpty() )
        return ::ULIS::FRectI::FromXYWH(0, 0, 0, 0);

    int minX = FMath::FloorToFloat(iPoints[0].X) + 0.5f;
    int maxX = FMath::CeilToFloat(iPoints[0].X) + 0.5f;
    int minY = FMath::FloorToFloat(iPoints[0].Y) + 0.5f;
    int maxY = FMath::CeilToFloat(iPoints[0].Y) + 0.5f;

    for ( int j = 1; j < iPoints.Num(); j++ )
    {
        minX = FMath::Min(minX, FMath::FloorToFloat(iPoints[j].X) + 0.5f);
        maxX = FMath::Max(maxX, FMath::CeilToFloat(iPoints[j].X) + 0.5f);
        minY = FMath::Min(minY, FMath::FloorToFloat(iPoints[j].Y) + 0.5f);
        maxY = FMath::Max(maxY, FMath::CeilToFloat(iPoints[j].Y) + 0.5f);
    }

    return ::ULIS::FRectI::FromMinMax(minX, minY, maxX, maxY);
}

::ULIS::FRectI
FOdysseyPainterEditorRasterSelection::GetMaskBoundingRect() const
{
    return mBoundingRect;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyPainterEditorRasterSelection::GetBlock()
{
    return mBlock;
}

TSharedPtr<FOdysseyHUDElement>
FOdysseyPainterEditorRasterSelection::GetHUD()
{
    return mHUD;
}

void
FOdysseyPainterEditorRasterSelection::RefreshHUD()
{
    mPoints.Empty();
    mHUD->EmptyElements();
    mBoundingRect = ::ULIS::FRectI::FromXYWH(0, 0, 0, 0);
    
    if (!mBlock)
        return;

    for (int y = 0; y <= mBlock->Height(); y++)
    {
        float* topLine = y > 0 ? (float*)mBlock->ScanlineBits(y - 1) : nullptr;
        float* line = y < mBlock->Height() ? (float*)mBlock->ScanlineBits(y) : nullptr;
        for (int x = 0; x <= mBlock->Width(); x++)
        {
            float leftValue = line && x > 0 ? line[x - 1] : 0.f;
            float topValue = topLine && x < mBlock->Width() ? topLine[x] : 0.f;
            float value = line && x < mBlock->Width() ? line[x] : 0.f;

            if (leftValue == 0.f && value != 0.f || leftValue != 0.f && value == 0.f)
            {
                mPoints.Add(FVector2D(x, y));
                mPoints.Add(FVector2D(x, y+1));

                //left vertical Line
                TSharedPtr<FOdysseyHUDLine> lineHUD = MakeShared<FOdysseyHUDLine>(FVector2D(x, y), FVector2D(x, y+1));
                mHUD->AddElement(lineHUD);
            }

            if (topValue == 0.f && value != 0.f || topValue != 0.f && value == 0.f)
            {
                mPoints.Add(FVector2D(x, y));
                mPoints.Add(FVector2D(x + 1, y));
                //top horizontal Line
                TSharedPtr<FOdysseyHUDLine> lineHUD = MakeShared<FOdysseyHUDLine>(FVector2D(x, y), FVector2D(x+1, y));
                mHUD->AddElement(lineHUD);
            }
        }
    }

    mBoundingRect = ComputeBoundingRect(mPoints);
}

TArray<::ULIS::FRectI> FOdysseyPainterEditorRasterSelection::GetSelectionAreaAsScanlines()
{
    TArray<::ULIS::FRectI> rectangles;
    ::ULIS::FRectI boundingBox = GetMaskBoundingRect();
    int maxX = boundingBox.x + boundingBox.w;
    int maxY = boundingBox.y + boundingBox.h;
    int minX = boundingBox.x;
    int minY = boundingBox.y;

    for (int y = minY; y <= maxY; y++)
    {
        std::vector< int > nodesX;

        for (int i = 0; i < mPoints.Num(); i++)
        {
            if( mPoints[i].Y == y )
                nodesX.push_back(mPoints[i].X);
        }

        int i = 0;
        int size = int(nodesX.size() - 1);
        while (i < size)
        {
            if (nodesX[i] > nodesX[i + 1])
            {
                int temp = nodesX[i];
                nodesX[i] = nodesX[i + 1];
                nodesX[i + 1] = temp;
                if (i > 0)
                    i--;
            }
            else
            {
                i++;
            }
        }

        for (i = 0; i < nodesX.size() - 1; i++)
        {
            if( nodesX[i] == nodesX[i+1] ) continue;
            if (nodesX[i] > maxX) break;
            if (nodesX[i + 1] > minX)
            {
                if (nodesX[i] < minX)
                    nodesX[i] = minX;
                if (nodesX[i + 1] > maxX)
                    nodesX[i + 1] = maxX;

                rectangles.Add(::ULIS::FRectI::FromXYWH(nodesX[i], y, nodesX[i + 1] - nodesX[i], 1));
            }
        }
    }

    return rectangles;
}

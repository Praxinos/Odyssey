// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyMask.h"

#include "ULISLoaderModule.h"
#include "OdysseyHUDLine.h"

FOdysseyMask::~FOdysseyMask()
{
}

FOdysseyMask::FOdysseyMask()
    : mHUD(MakeShared<FOdysseyHUDElement>())
{
}

void
FOdysseyMask::Init(int iWidth, int iHeight)
{
    mBlock = MakeShared<::ULIS::FBlock>(iWidth, iHeight, ::ULIS::Format_GF);
    Clear();
    RefreshHUD();
}

void
FOdysseyMask::Reset()
{
    mBlock = nullptr;
    RefreshHUD();
}

void
FOdysseyMask::Clear()
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_GF);
    ctx.Clear(*mBlock);
    ctx.Finish();

    RefreshHUD();
}

void
FOdysseyMask::Add(const TArray<FVector2D>& iPolygon)
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
}

void
FOdysseyMask::Substract(const TArray<FVector2D>& iPolygon)
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
}
/*
void
FOdysseyMask::ClearMaskData()
{
    for( int i = 0; i < mMaskZones.Num(); i++ )
    {
        mMaskZones[i].mBlock.Reset();
    }
    mMaskZones.Empty();
}

void
FOdysseyMask::ClearMaskHUD()
{
    for (int i = 0; i < mMaskHUD.Num(); i++)
    {
        mMaskHUD[i].Reset();
    }
    mMaskHUD.Empty();
}*/

::ULIS::FRectI
FOdysseyMask::ComputeBoundingRect(const TArray<FVector2D>& iPoints ) const
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
FOdysseyMask::GetMaskBoundingRect() const
{
    return mBoundingRect;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyMask::GetBlock()
{
    return mBlock;
    /* if( mMaskZones.Num() == 0 ) 
        return nullptr;

    ::ULIS::FRectI boundingBox = GetMaskBoundingRect();

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> maskBlock = MakeShared<::ULIS::FBlock>(boundingBox.w, boundingBox.h, mMaskZones[0].mBlock->Format() );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mMaskZones[0].mBlock->Format());
    ::ULIS::FEvent clearEvent;

    ctx.Clear(
        *maskBlock,
        maskBlock->Rect(),
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
        0,
        nullptr,
        &clearEvent);

    for( int i = 0; i < mMaskZones.Num(); i++ )
    {
        ctx.Blend(
            *mMaskZones[i].mBlock,
            *maskBlock,
            mMaskZones[i].mBlock->Rect(),
            ::ULIS::FVec2I(0,0),
            ::ULIS::Blend_Normal,
            ::ULIS::Alpha_Normal,
            1.f,
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            0,
            &clearEvent,
            nullptr
        );
    }

    ctx.Finish();

    return maskBlock;*/
}

TSharedPtr<FOdysseyHUDElement>
FOdysseyMask::GetHUD()
{
    return mHUD;
}

void
FOdysseyMask::RefreshHUD()
{
    mHUD->EmptyElements();
    mBoundingRect = ::ULIS::FRectI::FromXYWH(0, 0, 0, 0);
    
    if (!mBlock)
        return;

    TArray<FVector2D> points;

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
                points.Add(FVector2D(x, y));
                points.Add(FVector2D(x, y+1));

                //left vertical Line
                TSharedPtr<FOdysseyHUDLine> lineHUD = MakeShared<FOdysseyHUDLine>(FVector2D(x, y), FVector2D(x, y+1));
                mHUD->AddElement(lineHUD);
            }

            if (topValue == 0.f && value != 0.f || topValue != 0.f && value == 0.f)
            {
                points.Add(FVector2D(x, y));
                points.Add(FVector2D(x + 1, y));
                //top horizontal Line
                TSharedPtr<FOdysseyHUDLine> lineHUD = MakeShared<FOdysseyHUDLine>(FVector2D(x, y), FVector2D(x+1, y));
                mHUD->AddElement(lineHUD);
            }
        }
    }

    mBoundingRect = ComputeBoundingRect(points);
}

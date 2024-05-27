// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyMask.h"
#include "ULISLoaderModule.h"

FOdysseyMask::~FOdysseyMask()
{
}

FOdysseyMask::FOdysseyMask()
{
}

void FOdysseyMask::AddFromPointsAndBlock(TArray<FVector2D> iPoints, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    mMaskZones.Add( FOdysseyCanvasZone( iPoints, iBlock ) );
}

void FOdysseyMask::ClearMask()
{
    for( int i = mMaskZones.Num(); i > 0; i-- )
    {
        mMaskZones[i].mBlock.Reset();
    }
    mMaskZones.Empty();
}

::ULIS::FRectI FOdysseyMask::GetMaskBoundingRect()
{
    if( mMaskZones.Num() == 0 )
    {
        return ::ULIS::FRectI::FromXYWH(0, 0, 0, 0);
    }

    int minX = mMaskZones[0].mPolygonPoints[0].X;
    int maxX = mMaskZones[0].mPolygonPoints[0].X;
    int minY = mMaskZones[0].mPolygonPoints[0].Y;
    int maxY = mMaskZones[0].mPolygonPoints[0].Y;

    for( int i = 0; i < mMaskZones.Num(); i++ )
    {
        for (int j = 1; j < mMaskZones[0].mPolygonPoints.Num(); j++)
        {
            minX = FMath::Min(minX, mMaskZones[i].mPolygonPoints[j].X);
            maxX = FMath::Max(maxX, mMaskZones[i].mPolygonPoints[j].X);
            minY = FMath::Min(minY, mMaskZones[i].mPolygonPoints[j].Y);
            maxY = FMath::Max(maxY, mMaskZones[i].mPolygonPoints[j].Y);
        }
    }

    return ::ULIS::FRectI::FromMinMax(minX, minY, maxX, maxY);
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> FOdysseyMask::GetMaskBlock()
{
    if( mMaskZones.Num() == 0 ) 
        return nullptr;

    ::ULIS::FRectI boundingBox = GetMaskBoundingRect();

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> maskBlock = MakeShared<::ULIS::FBlock>(boundingBox.w, boundingBox.h, mMaskZones[0].mBlock->Format() );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mMaskZones[0].mBlock->Format());
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
            nullptr,
            nullptr
        );
    }

    return maskBlock;
}

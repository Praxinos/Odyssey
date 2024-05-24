// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyMask.h"

#include "ULISLoaderModule.h"
#include "GeomTools.h"

FOdysseyMask::~FOdysseyMask()
{
    ClearMaskData();
    ClearMaskHUD();
}

FOdysseyMask::FOdysseyMask()
{
}

void FOdysseyMask::AddFromPointsAndBlock(TArray<FVector2D> iPoints, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    mMaskZones.Add( FOdysseyCanvasZone( iPoints, iBlock ) );
}

void FOdysseyMask::ClearMaskData()
{
    for( int i = 0; i < mMaskZones.Num(); i++ )
    {
        mMaskZones[i].mBlock.Reset();
    }
    mMaskZones.Empty();
}

void FOdysseyMask::ClearMaskHUD()
{
    for (int i = 0; i < mMaskHUD.Num(); i++)
    {
        mMaskHUD[i].Reset();
    }
    mMaskHUD.Empty();
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
        for (int j = 1; j < mMaskZones[i].mPolygonPoints.Num(); j++)
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

    return maskBlock;
}

TArray<TSharedPtr<FOdysseyHUDPolygon>>& FOdysseyMask::GetMaskHUD()
{
    return mMaskHUD;
}

void FOdysseyMask::RefreshMaskHUD()
{
    for( int i = 0; i < mMaskHUD.Num(); i++ )
    {
        mMaskHUD[i].Reset();
    }
    mMaskHUD.Empty();

    TArray<TArray<FVector2D>> polygons;
    TArray<bool> windings;
    for( int i = 0; i < mMaskZones.Num(); i++ )
    {
        polygons.Add( mMaskZones[i].mPolygonPoints );
        windings.Add( false );
    }

    //Doesn't do what I want, I want to reduce multiple polygons to one, if possible
    polygons = FGeomTools2D::ReducePolygons( polygons, windings );

    for( int i = 0; i < polygons.Num(); i++ )
    {
        mMaskHUD.Add(MakeShared<FOdysseyHUDPolygon>());
        TArray<FVector2D>& points = mMaskHUD[i]->GetPoints();
        for( int j = 0; j < polygons[i].Num(); j++ )
        {
            points.Add( polygons[i][j] );
        }
    }
}

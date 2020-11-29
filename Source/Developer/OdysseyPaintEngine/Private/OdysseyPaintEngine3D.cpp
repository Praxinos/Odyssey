// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPaintEngine3D.h"
#include "OdysseyMathUtils.h"
#include "OdysseyInterpolationTypes.h"
#include "OdysseyBrushAssetBase.h"
#include <ULIS3>
#include "ULISLoaderModule.h"
#include <chrono>

#define TILE_SIZE 64

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPaintEngine3D::~FOdysseyPaintEngine3D()
{
}

FOdysseyPaintEngine3D::FOdysseyPaintEngine3D( FOdysseyUndoHistory* iUndoHistoryPtr )
    : mStrokeBlock3D(NULL)
{
}


void
FOdysseyPaintEngine3D::Tick()
{
    if(!mBrushInstance || !mStrokeBlock || !mPreviewBlock || mIsLocked)
        return;

    ExecuteDelayQueue();

    //Execute Tick and Update invalid maps in case of drawing in the tick event
    mBrushInstance->ExecuteTick();
    UpdateInvalidMaps();
    mBrushInstance->ClearInvalidRects();

    //Refresh the tiles
    UpdatePreviewBlockTiles();

    //End the stroke if we need to
    if(mIsPendingEndStroke && mDelayQueue.empty())
    {
        EndStrokeTick();
    }
}

void
FOdysseyPaintEngine3D::Block(FOdysseyBlock* iBlock)
{
    if(mEditedBlock == iBlock)
        return;

    Flush();

    mEditedBlock = iBlock;
    if(!mEditedBlock) {
        delete mStrokeBlock;
        delete mStrokeBlock3D;
        delete mPreviewBlock;
        mStrokeBlock = nullptr;
        mStrokeBlock3D = nullptr;
        mPreviewBlock = nullptr;
        UpdateBrushInstance();
        return;
    }

    if(!mStrokeBlock
         || !mPreviewBlock
         || mStrokeBlock->Size() != mEditedBlock->Size()
         || mStrokeBlock3D->Size() != mEditedBlock->Size()
         || mPreviewBlock->Size() != mEditedBlock->Size()
         || mStrokeBlock->Format() != mEditedBlock->Format()
         || mPreviewBlock->Format() != mEditedBlock->Format())
    {
        delete mStrokeBlock;
        delete mStrokeBlock3D;
        delete mPreviewBlock;
        mStrokeBlock = new FOdysseyBlock(mEditedBlock->Width(),mEditedBlock->Height(),mEditedBlock->Format());
        mStrokeBlock3D = new FOdysseyBlock(mEditedBlock->Width(),mEditedBlock->Height(),mEditedBlock->Format());
        mPreviewBlock = new FOdysseyBlock(mEditedBlock->Width(),mEditedBlock->Height(),mEditedBlock->Format());

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

        ::ul3::FVec2F pos(0,0);
        ::ul3::Clear(hULIS.ThreadPool(),ULIS3_BLOCKING,perfIntent,hULIS.HostDeviceInfo(),ULIS3_NOCB,mStrokeBlock->GetBlock(),mStrokeBlock->GetBlock()->Rect());
        ::ul3::Clear(hULIS.ThreadPool(),ULIS3_BLOCKING,perfIntent,hULIS.HostDeviceInfo(),ULIS3_NOCB,mStrokeBlock3D->GetBlock(),mStrokeBlock3D->GetBlock()->Rect());
        ::ul3::Copy(hULIS.ThreadPool(),ULIS3_BLOCKING,perfIntent,hULIS.HostDeviceInfo(),ULIS3_NOCB,mEditedBlock->GetBlock(),mPreviewBlock->GetBlock(),mPreviewBlock->GetBlock()->Rect(),pos);

        ReallocInvalidMaps();
    }
    UpdateBrushInstance();
}

void
FOdysseyPaintEngine3D::BlendStrokeBlockInPreviewBlock(TArray<::ul3::FRect>& iRects)
{
    if(!mStrokeBlock || !mPreviewBlock)
        return;

    if(iRects.Num() <= 0)
        return;

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent =  ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    for(int i = 0; i < iRects.Num(); i++)
    {
        ::ul3::FVec2F pos(iRects[i].x,iRects[i].y);
        ::ul3::Blend(hULIS.ThreadPool()
                    ,ULIS3_BLOCKING
                    ,perfIntent
                    ,hULIS.HostDeviceInfo()
                    ,ULIS3_NOCB
                    ,mStrokeBlock->GetBlock()
                    ,mPreviewBlock->GetBlock()
                    ,iRects[i]
                    ,pos
                    ,ULIS3_NOAA
                    ,mBlendingModeModifier
                    ,mAlphaModeModifier
                    ,mOpacityModifier
        );
    }
}

FOdysseyBlock* 
FOdysseyPaintEngine3D::StrokeBlock3D()
{
    return mStrokeBlock3D;
}


void 
FOdysseyPaintEngine3D::UpdateTmpTileMapFromTriangles(TArray<FTexturePaintTriangleInfo>& iTriangles)
{
    // TODO: This is a basic algorithm, not a full fledge optimized tile to triangle update
    if( iTriangles.Num() == 0 )
        return;

    for( int i = 0; i < iTriangles.Num(); i++ )
    {
        int xMin = FMath::Min3( iTriangles[i].TrianglePoints[0].X, iTriangles[i].TrianglePoints[1].X, iTriangles[i].TrianglePoints[2].X ) / TILE_SIZE;
        int yMin = FMath::Min3( iTriangles[i].TrianglePoints[0].Y, iTriangles[i].TrianglePoints[1].Y, iTriangles[i].TrianglePoints[2].Y ) / TILE_SIZE;
        int xMax = FMath::Max3( iTriangles[i].TrianglePoints[0].X, iTriangles[i].TrianglePoints[1].X, iTriangles[i].TrianglePoints[2].X ) / TILE_SIZE;
        int yMax = FMath::Max3( iTriangles[i].TrianglePoints[0].Y, iTriangles[i].TrianglePoints[1].Y, iTriangles[i].TrianglePoints[2].Y ) / TILE_SIZE;

        //min and max can be outside the tiles bounds of the texture, so we have to check if they are in
        xMin = FMath::Min( xMin, mCountTileX - 1 ); xMin = FMath::Max( xMin, 0 );
        yMin = FMath::Min( yMin, mCountTileY - 1 ); yMin = FMath::Max( yMin, 0 );
        xMax = FMath::Min( xMax, mCountTileX - 1 ); xMax = FMath::Max( xMax, 0 );
        yMax = FMath::Min( yMax, mCountTileY - 1 ); yMax = FMath::Max( yMax, 0 );

        SetMapWithRect( mTmpInvalidTileMap, ::ul3::FRect( xMin, yMin, xMax - xMin + 1, yMax - yMin + 1 ), true );
    }
}

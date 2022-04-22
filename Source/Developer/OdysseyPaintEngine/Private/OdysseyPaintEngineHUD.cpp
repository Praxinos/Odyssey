// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPaintEngineHUD.h"
#include "OdysseyMathUtils.h"
#include "OdysseyInterpolationTypes.h"
#include "OdysseyBrushAssetBase.h"
#include <ULIS>
#include "ULISLoaderModule.h"
#include <chrono>

#define TILE_SIZE 64

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPaintEngineHUD::~FOdysseyPaintEngineHUD()
{
    delete mHUDBlock;
    DeallocInvalidMap( mHUDBlockInvalidMap );
}

FOdysseyPaintEngineHUD::FOdysseyPaintEngineHUD( FOdysseyUndoHistory* iUndoHistoryPtr )
    : mHUDBlock( NULL )
    , mCountTileX( 0 )
    , mCountTileY( 0 )
    , mHUDBlockInvalidMap( NULL )
    , mColor( ::ULIS::FColor::RGBA8( 0, 0, 0, 255 ) )
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyPaintEngineHUD::SetHUDBlock(FOdysseyBlock* iBlock)
{
    if (mHUDBlock == iBlock)
        return;

    mHUDBlock = iBlock;
    if (!mHUDBlock) 
        return;

    ReallocInvalidMaps();
}

void
FOdysseyPaintEngineHUD::SetColor( const ::ULIS::FColor& iColor )
{
    // TODO: Convert to TAttribute
    mColor = iColor; //No need for any conversion here
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyBlock*
FOdysseyPaintEngineHUD::GetHUDBlock()
{
    return mHUDBlock;
}

const ::ULIS::FColor&
FOdysseyPaintEngineHUD::GetColor() const
{
    return mColor;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- PaintEnginge Ticks

void
FOdysseyPaintEngineHUD::Tick()
{
    if( !mHUDBlock )
        return;

    UE_LOG(LogTemp, Display, TEXT("TIck HUD"));
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Tiles Management

void
FOdysseyPaintEngineHUD::ReallocInvalidMaps()
{
    DeallocInvalidMap( mHUDBlockInvalidMap );

    mCountTileX = mHUDBlock ? ceil( (float)mHUDBlock->Width() / TILE_SIZE ) : 0;
    mCountTileY = mHUDBlock ? ceil( (float)mHUDBlock->Height() / TILE_SIZE ) : 0;

    AllocInvalidMap( mHUDBlockInvalidMap );
}

void
FOdysseyPaintEngineHUD::UpdateInvalidMaps()
{
    /*auto invalid_rects = mBrushInstance->GetInvalidRects();
    for( int j = 0; j < invalid_rects.Num(); ++j )
    {
        UpdateInvalidMaps(invalid_rects[j]);
    }
    mBrushInstance->ClearInvalidRects();*/
}

void
FOdysseyPaintEngineHUD::UpdateInvalidMaps(::ULIS::FRectI iRect)
{
    float xf = float( iRect.x ) / TILE_SIZE;
    float yf = float( iRect.y ) / TILE_SIZE;
    float wf = float( iRect.w ) / TILE_SIZE;
    float hf = float( iRect.h ) / TILE_SIZE;
    if( xf < 0 ) {
        wf += xf;
        xf = 0;
    }
    if( yf < 0 ) {
        hf += yf;
        yf = 0;
    }
    if( wf < 0 )
        wf = 0;
    if( hf < 0 )
        hf = 0;
    int x = xf;
    int y = yf;
    int w = FMath::Min( mCountTileX, int( ceil( xf + wf ) ) ) - x;
    int h = FMath::Min( mCountTileY, int( ceil( yf + hf ) ) ) - y;
    ::ULIS::FRectI tileRect = { x, y, w, h };
    SetMapWithRect(mHUDBlockInvalidMap, tileRect, true);
}

TArray<::ULIS::FRectI>
FOdysseyPaintEngineHUD::GetHUDBlockInvalidTiles()
{
    TArray<::ULIS::FRectI> tiles;
    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            if( mHUDBlockInvalidMap[k][l] )
            {
                ::ULIS::FRectI rect = MakeTileRect(l, k);
                tiles.Add(rect);
            }
        }
    }
    return tiles;
}

void
FOdysseyPaintEngineHUD::SetMapWithRect( InvalidTileMap ioMap, const ::ULIS::FRectI& iRect, bool iValue )
{
    for( int k = 0; k < iRect.h; ++k )
    {
        for( int l = 0; l < iRect.w; ++l )
        {
            ioMap[k + iRect.y][l + iRect.x] = iValue;
        }
    }
}

void
FOdysseyPaintEngineHUD::DeallocInvalidMap( InvalidTileMap& ioMap )
{
    if( !ioMap )
        return;

    for( int i = 0; i < mCountTileY; ++i )
        delete[] ioMap[i];
    delete[] ioMap;
    ioMap = 0;
}

void
FOdysseyPaintEngineHUD::AllocInvalidMap( InvalidTileMap& ioMap )
{
    if( ioMap )
        return;

    ioMap = new bool*[mCountTileY];
    for( int i = 0; i < mCountTileY; ++i )
    {
        ioMap[i] = new bool[mCountTileX];
        for( int j = 0; j < mCountTileX; ++j )
        {
            ioMap[i][j] = false;
        }
    }
}

void
FOdysseyPaintEngineHUD::ClearInvalidMap( InvalidTileMap ioMap )
{
    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            ioMap[k][l] = false;
        }
    }
}

void
FOdysseyPaintEngineHUD::CopyInvalidMap( InvalidTileMap iSrcMap, InvalidTileMap ioDstMap )
{
    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            ioDstMap[k][l] = iSrcMap[k][l];
        }
    }
}

::ULIS::FRectI
FOdysseyPaintEngineHUD::MakeTileRect( int iTileX, int iTileY )
{
    return { iTileX * TILE_SIZE,
             iTileY * TILE_SIZE,
             mHUDBlock ? FMath::Min( iTileX * TILE_SIZE + TILE_SIZE, mHUDBlock->Width() ) - iTileX * TILE_SIZE : 0,
             mHUDBlock ? FMath::Min( iTileY * TILE_SIZE + TILE_SIZE, mHUDBlock->Height() ) - iTileY * TILE_SIZE : 0 } ;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ FGCObject interface

void
FOdysseyPaintEngineHUD::AddReferencedObjects(FReferenceCollector& Collector)
{
}

FString
FOdysseyPaintEngineHUD::GetReferencerName() const
{
	return TEXT("FOdysseyPaintEngineHUD");
}

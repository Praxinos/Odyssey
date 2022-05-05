// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#include "OdysseySurfaceTexture2DEditable.h"
#include "TextureDerivedData.h"
#include "ULISLoaderModule.h"
#include "ImageCore.h"
#include "TextureCompressorModule.h"
#include "Engine/TextureLODSettings.h"
#include "Engine/TextureCube.h"
#include "Engine/Texture2DArray.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/VolumeTexture.h"
#include "VT/VirtualTextureBuildSettings.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Interfaces/ITargetPlatform.h"
#include "Interfaces/ITextureFormat.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "RHI.h"
#include "Async/ParallelFor.h"
#include <ULIS>

#ifdef UE_BUILD_DEBUG
#include <chrono>
#endif

/////////////////////////////////////////////////////
// Utlity
void
CopyUTextureSourceDataIntoBlock(::ULIS::FBlock* iBlock,UTexture* iTexture)
{
    checkf(iBlock->Width() == iTexture->Source.GetSizeX() &&
           iBlock->Height() == iTexture->Source.GetSizeY()
           ,TEXT("Sizes do not match"));

    TArray64<uint8> src;
    src.SetNumUninitialized(iTexture->Source.CalcMipSize(0));
    iTexture->Source.GetMipData(src,0);
    if (TextureSourceFormatNeedsConversionToULISFormat(iTexture->Source.GetFormat()))
    {
        ConvertTextureSourceFormatToULISFormat(src.GetData(), iBlock->Bits(), iBlock->Width(), iBlock->Height(), iTexture->Source.GetFormat());
    }
    else
    {
        FMemory::Memcpy(iBlock->Bits(), src.GetData(), src.Num());
    }
}

void
CopyUTexturePixelDataIntoBlock(::ULIS::FBlock* iBlock, UTexture* iTexture)
{
    FTexturePlatformData* PlatformData = *iTexture->GetRunningPlatformData();
    checkf(
           iBlock->Width() == PlatformData->SizeX
        && iBlock->Height() == PlatformData->SizeY
        , TEXT( "Sizes do not match" )
    );

    ENQUEUE_RENDER_COMMAND( GetTextureData )(
        [ iTexture, iBlock ]( FRHICommandListImmediate& RHICmdList )
        {
            FTexture2DRHIRef texture2DRHI = iTexture->Resource->GetTexture2DRHI();
            uint32 bps = iBlock->BytesPerScanLine();

            uint32 stride;
            // stride value is changed by RHILockTexture2D
            const uint8* src = reinterpret_cast< const uint8* >(
                RHILockTexture2D( texture2DRHI, 0, EResourceLockMode::RLM_ReadOnly, stride, true, true )
            );

            if( src )
            {
                if ( stride == bps )
                {
                    // Copy all raw
                    FMemory::Memcpy( iBlock->Bits(), src, iBlock->BytesTotal() );
                }
                else
                {
                    // Copy line by line
                    uint8* dest = iBlock->Bits();
                    for ( uint32 i = 0; i < texture2DRHI->GetSizeY(); ++i )
                    {
                        FMemory::Memcpy( dest, src, bps );
                        src += stride;
                        dest += bps;
                    }
                }
            }
            RHIUnlockTexture2D( texture2DRHI, 0, true, true );
        }
    );

    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();
}

void
CopyURenderTargetPixelDataIntoBlock(::ULIS::FBlock* iBlock, UTextureRenderTarget2D* iRenderTarget)
{
    checkf(iBlock->Width() == iRenderTarget->GetSurfaceWidth()  &&
           iBlock->Height() == iRenderTarget->GetSurfaceHeight()
           ,TEXT("Sizes do not match"));

    FTextureRenderTargetResource* renderTargetResource = iRenderTarget->GameThread_GetRenderTargetResource();
    checkf( false, TEXT( "Unused" ) );
}

void
InitTextureWithBlockData(const ::ULIS::FBlock* iBlock, UTexture2D* iTexture, ETextureSourceFormat iFormat)
{
    ::ULIS::eFormat targetFormat = ULISFormatForTextureSourceFormat(iFormat);
    const ::ULIS::FBlock* block = iBlock;
    if (iBlock->Format() != targetFormat)
    {
        ::ULIS::FBlock* convblock = new ::ULIS::FBlock(iBlock->Width(), iBlock->Height(), targetFormat);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());
        ctx.ConvertFormat( *iBlock, *convblock );
        ctx.Finish();

        block = convblock;
    }

    if (TextureSourceFormatNeedsConversionToULISFormat(iFormat))
    {
        TArray64<uint8> dst;
        dst.SetNumUninitialized(iBlock->Width() * iBlock->Height() * TextureSourceFormatBytesPerPixel(iFormat));
        ConvertULISFormatToTextureSourceFormat(block->Bits(), dst.GetData(), iBlock->Width(), iBlock->Height(), iFormat);
        iTexture->Source.Init(iBlock->Width(), iBlock->Height(), 1, 1, iFormat, dst.GetData());
    }
    else
    {
        iTexture->Source.Init(block->Width(), block->Height(), 1, 1, iFormat, block->Bits());
    }

    if (block != iBlock)
    {
        delete block;
    }
}

void
CopyBlockDataIntoUTexture( const ::ULIS::FBlock* iBlock, UTexture2D* iTexture )
{
    checkf(  iBlock->Width() == iTexture->Source.GetSizeX() &&
             iBlock->Height() == iTexture->Source.GetSizeY()
           , TEXT( "Sizes do not match" ) );
           
    InitTextureWithBlockData( iBlock, iTexture, iTexture->Source.GetFormat() );
}

::ULIS::FBlock*
NewBlockFromUTextureData(UTexture2D* iTexture, ::ULIS::eFormat iFormat)
{
    ::ULIS::FBlock* block = new ::ULIS::FBlock( iTexture->Source.GetSizeX(),iTexture->Source.GetSizeY(), iFormat );
    FillOdysseyBlockFromUTextureData( block, iTexture, iFormat );
    return block;
}

void
FillOdysseyBlockFromUTextureData( ::ULIS::FBlock* ioBlock, UTexture2D* iTexture, ::ULIS::eFormat iFormat )
{
    if (!ioBlock)
        return;

    if (ioBlock->Width() != iTexture->Source.GetSizeX() || ioBlock->Height() != iTexture->Source.GetSizeY() || ioBlock->Format() != iFormat)
    {
        ioBlock->ReallocInternalData(iTexture->Source.GetSizeX(), iTexture->Source.GetSizeY(), iFormat);
    }

    ::ULIS::eFormat sourceFormat = ULISFormatForTextureSourceFormat(iTexture->Source.GetFormat());
    if (iFormat == sourceFormat)
    {
        CopyUTextureSourceDataIntoBlock(ioBlock, iTexture);
    }
    else
    {
        ::ULIS::FBlock* block = new ::ULIS::FBlock(iTexture->Source.GetSizeX(),iTexture->Source.GetSizeY(),sourceFormat);
        CopyUTextureSourceDataIntoBlock(block,iTexture);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(sourceFormat);
        ctx.ConvertFormat(*block, *ioBlock);
        ctx.Finish();

        delete block;
    }
}

ERawImageFormat::Type
GetRawImageFormatFromTextureSourceFormat(ETextureSourceFormat iFormat)
{
    switch (iFormat)
    {
        case TSF_G8:        return ERawImageFormat::G8;
        case TSF_G16:       return ERawImageFormat::G16;
        case TSF_BGRA8:     return ERawImageFormat::BGRA8;
        case TSF_BGRE8:     return ERawImageFormat::BGRE8;
        case TSF_RGBA16:    return ERawImageFormat::RGBA16;
        case TSF_RGBA16F:   return ERawImageFormat::RGBA16F;
        default:
            //UE_LOG(LogTexture, Fatal, TEXT("Texture %s invalid format."), *InTexture.GetName());
            return ERawImageFormat::BGRA8;
    }
    return ERawImageFormat::BGRA8;
}

void
SortRects( const ::ULIS::FRectI* iRects, const uint32 iNumRects, TArray< TArray< ::ULIS::FRectI > >& oSortedRects ) {
    oSortedRects.Empty();
    if( iNumRects == 0 )
        return;

    // Take the first input rect as the first size category for sorted rects
    oSortedRects.Emplace( TArray< ::ULIS::FRectI >( { iRects[0] } ) );
    for( uint32 i = 1; i < iNumRects; ++i ) {
        bool bFound = false;

        // Iterate over sizes categories
        for( int32 j = 0; j < oSortedRects.Num(); ++j ) {
            checkf( oSortedRects[j].Num() > 0, TEXT( "Bad size in sorted rects sub array, cannot assume element 0 exists in next statement" ) );
            // If a size category is found that matches ...
            if( iRects[i].Size() == oSortedRects[j][0].Size() ) {
                bFound = true;
                // ... Add the input rect to the category ...
                oSortedRects[j].Emplace( iRects[i] );
                break;
            }
        }

        // ... If we couldn't find an existing category for this size, add this category.
        if( !bFound )
            oSortedRects.Emplace( TArray< ::ULIS::FRectI >( { iRects[i] } ) );
    }
}

void
InvalidateTextureFromSourceDataUsingSortedRects( const ::ULIS::FBlock* iData, UTexture2D* iTexture, TArray< TArray< ::ULIS::FRectI > >& ioSrcRects )
{
    /*
     * Okay here is the idea
     * We need to send our data the best way possible to the graphics card and send it according to all the texture parameters
     * For that we use the Compressor->BuildTexture() Method to apply the texture parameters to our input data
     *
     * The problem is, Compressor->BuildTexture() is made assuming we are giving it the whole texture image.
     * But we want to send it only a small part of that image and BuildTexture can lead to image resizing according to the MaxTextureResolution of the BuildSettings parameter.
     *
     * What we do here is the following :
     * - If MaxTextureResolution is 0 or equals the Texture Full Size, then we don't need to worry about the part we are giving to the BuildTexture
     * - If MaxTextureResolution > 0 and < Texture Full Size, then we need to ajust MaxTextureResolution to make it resize our image part as intended
     * - If the resulting MaxTextureResolution < 1, then we need to take a bigger part of the image than expected in order to have a MaxTextureResolution == 1
     *
     * Also this is possible because MaxTextureResolution is always a power of two and is only applied if iTexture also has PowerOfTwo sizes
     *
     * There will be adjustments to make to iRect x y w h values
     * This is clearly a big Patch and should be provided by Epic Games in the first place.
     */


    ////////////////////////////
    // Do nothing if there is no input rects
    if( ioSrcRects.Num() == 0 )
        return;


    ////////////////////////////
#ifdef UE_BUILD_DEBUG
    { // Debug only assert consistency of sorted data, input rects should be sorted based on size to allow batch processing.
        for( int32 i = 0; i < ioSrcRects.Num(); ++i ) {
            checkf( ioSrcRects[i].Num(), TEXT( "Bad empty category" ) );
            const ::ULIS::FVec2I refSize = ioSrcRects[i][0].Size();
            for( int32 j = 1; j < ioSrcRects[i].Num(); ++j )
                checkf( ioSrcRects[i][j].Size() == refSize, TEXT( "Bad input rect array, not sorted based on size could lead to bugs in the rest of the Invalidate pipeline" ) );
        }
    }
#endif


    ////////////////////////////
    // Get Texture BuildSettings, one array of FTextureBuildSettings per rect size category
    TArray< TArray< FTextureBuildSettings > > buildSettings;
    buildSettings.Reserve( ioSrcRects.Num() );
    {
        TArray< FTextureBuildSettings > refBuildSettings;
        GetBuildSettingsForRunningPlatform( *iTexture, refBuildSettings );
        for( int32 i = 0; i < ioSrcRects.Num(); ++i )
            buildSettings.Add( refBuildSettings );
    }


    ////////////////////////////
    // Get Compressor Module
    static ITextureCompressorModule* Compressor = &FModuleManager::LoadModuleChecked<ITextureCompressorModule>( "TextureCompressor" );
    if( !Compressor )
        return;


    ////////////////////////////
    // Compute Texture Full Size
    uint32 textureFullWidth = iTexture->Source.GetSizeX();
    uint32 textureFullHeight = iTexture->Source.GetSizeY();
    switch( static_cast< ETexturePowerOfTwoSetting::Type >( buildSettings[0][0].PowerOfTwoMode ) ) {
        case ETexturePowerOfTwoSetting::None:
            break;

        case ETexturePowerOfTwoSetting::PadToPowerOfTwo:
            textureFullWidth = FMath::RoundUpToPowerOfTwo( textureFullWidth );
            textureFullHeight = FMath::RoundUpToPowerOfTwo( textureFullHeight );
            break;

        case ETexturePowerOfTwoSetting::PadToSquarePowerOfTwo:
            textureFullWidth = textureFullHeight = FMath::Max( FMath::RoundUpToPowerOfTwo( textureFullWidth ), FMath::RoundUpToPowerOfTwo( textureFullHeight ) );
            break;

        default:
            checkf( false, TEXT( "Unknown entry in ETexturePowerOfTwoSetting::Type" ) );
            break;
    }


    ////////////////////////////
    // Dst Rects will be computed right afterwards
    TArray< TArray< ::ULIS::FRectI > > dstRects;
    dstRects.Reserve( ioSrcRects.Num() );
    //Adjust the build settings if needed
    {
        const bool bPredA = buildSettings[0][0].MaxTextureResolution > 0;
        const bool bPredB = buildSettings[0][0].MaxTextureResolution < textureFullWidth;
        const bool bPredC = buildSettings[0][0].MaxTextureResolution < textureFullHeight;
        if( bPredA && ( bPredB || bPredC ) ) {
            uint32 ratio = FMath::Max( textureFullWidth, textureFullHeight ) / buildSettings[0][0].MaxTextureResolution;
            for( int32 i = 0; i < ioSrcRects.Num(); ++i ) {
                dstRects.Emplace( TArray< ::ULIS::FRectI >() );
                dstRects[i].Reserve( ioSrcRects[i].Num() );
                for( int32 j = 0; j < ioSrcRects[i].Num(); ++j ) {
                    ::ULIS::FRectI srcRect = ioSrcRects[i][j];

                    // Be sure to pad our Rect to a power of two, as we know that we are in a power of two context
                    // We assume that iRect x and y are already rounded to a power of two as usually tiles are 64 pixels
                    // wide, so every tile position is in power of two
                    srcRect.x = ( srcRect.x / ratio ) * ratio; // floor to a multiple of ratio
                    srcRect.y = ( srcRect.y / ratio ) * ratio; // floor to a multiple of ratio
                    srcRect.w = FMath::Max( ratio, FMath::RoundUpToPowerOfTwo( ioSrcRects[i][j].x + srcRect.w - srcRect.x ) );
                    srcRect.h = FMath::Max( ratio, FMath::RoundUpToPowerOfTwo( ioSrcRects[i][j].y + srcRect.h - srcRect.y ) );

                    if( uint32( srcRect.x ) + srcRect.w > textureFullWidth )    srcRect.x = textureFullWidth - srcRect.w; // clamp x
                    if( uint32( srcRect.y ) + srcRect.h > textureFullHeight )   srcRect.y = textureFullHeight - srcRect.w; // clamp y

                    // set the MaxTextureResolution for the build setting corresponding to the size category
                    buildSettings[i][0].MaxTextureResolution = FMath::Max( srcRect.w, srcRect.h ) / ratio;

                    // compute dstRect;
                    ::ULIS::FRectI dstRect;
                    dstRect.x = srcRect.x / ratio;
                    dstRect.y = srcRect.y / ratio;
                    dstRect.w = srcRect.w / ratio;
                    dstRect.h = srcRect.h / ratio;

                    if( srcRect.x + srcRect.w > iTexture->Source.GetSizeX() )   srcRect.w = iTexture->Source.GetSizeX() - srcRect.x; // clamp w
                    if( srcRect.y + srcRect.h > iTexture->Source.GetSizeY() )   srcRect.h = iTexture->Source.GetSizeY() - srcRect.y; // clamp h

                    // report computed rects in ioSrcRects & DstRects
                    ioSrcRects[i][j] = srcRect;
                    dstRects[i].Emplace( dstRect );
                }
            }
        } else {
            dstRects = ioSrcRects;
        }
    }


    ////////////////////////////
    //Create the source tile Images that will containing the Texture Source Data we need in order to refresh
    // Then fill the source Image with our Data and convert it from our format to the Texture Source format if needed
    TArray< TArray< TArray< FImage > > > tileImages;
    tileImages.Reserve( ioSrcRects.Num() );
    {
        TArray< TArray< ::ULIS::FBlock > > tileBlocks;
        tileBlocks.Reserve( ioSrcRects.Num() );
        const bool bPred = TextureSourceFormatNeedsConversionToULISFormat( iTexture->Source.GetFormat() );
        const int w = iTexture->Source.IsValid() ? iTexture->Source.GetSizeX() : iTexture->GetSizeX();
        const int h = iTexture->Source.IsValid() ? iTexture->Source.GetSizeY() : iTexture->GetSizeY();
        const ERawImageFormat::Type fmt = GetRawImageFormatFromTextureSourceFormat( iTexture->Source.GetFormat() );
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( iData->Format() );
        for( int32 i = 0; i < ioSrcRects.Num(); ++i ) {
            tileImages.Emplace( TArray< TArray< FImage > >() );
            tileImages[i].Reserve( ioSrcRects[i].Num() );
            tileBlocks.Emplace( TArray< ::ULIS::FBlock >() );
            tileBlocks[i].Reserve( ioSrcRects[i].Num() );
            for( int32 j = 0; j < ioSrcRects[i].Num(); ++j ) {
                const int len = ioSrcRects[i][j].h;
                tileImages[i].Emplace(
                    TArray< FImage >(
                        {
                            FImage(
                                  ioSrcRects[i][j].w
                                , ioSrcRects[i][j].h
                                , 1
                                , fmt
                                , buildSettings[i][0].GetGammaSpace()
                            )
                        }
                    )
                );

                int rowSize = bPred ? ( iTexture->Source.CalcMipSize( 0 ) * ioSrcRects[i][j].w ) / ( w * h ) : rowSize = iData->BytesPerPixel() * ioSrcRects[i][j].w;
                tileImages[i][j][0].RawData.SetNumUninitialized( rowSize * len );
                tileBlocks[i].Emplace( tileImages[i][j][0].RawData.GetData(), ioSrcRects[i][j].w, ioSrcRects[i][j].h, iData->Format() );
                if( bPred ) {
                    // TODO: This part can be optimized and multithreaded
                    for( int k = 0; k < len; k++ ) {
                        //checkf( false, TEXT( "Not implemented properly yet" ) );
                        ConvertULISFormatToTextureSourceFormat(
                              iData->PixelBits( ioSrcRects[i][j].x, ioSrcRects[i][j].y + k )
                            , tileImages[i][j][0].RawData.GetData() + rowSize * k
                            , ioSrcRects[i][j].w
                            , 1
                            , iTexture->Source.GetFormat()
                        );
                    }
                } else {
                    ctx.Copy( *iData, tileBlocks[i][j], ioSrcRects[i][j], ::ULIS::FVec2I( 0 ), ::ULIS::FSchedulePolicy::AsyncCacheEfficient );
                }
            }
        }
        ctx.Finish();
    }

    ////////////////////////////
    // Apply the texture parameters to retrieve the data to send to the graphics card
    TArray< TArray< TArray< FCompressedImage2D > > > compressedMips;
    compressedMips.Reserve( ioSrcRects.Num() );
    {
        uint32 unusedNumMipsInTail, unusedExtData;
        for( int32 i = 0; i < ioSrcRects.Num(); ++i ) {
            compressedMips.Emplace( TArray< TArray< FCompressedImage2D > >() );
            compressedMips[i].Reserve( ioSrcRects[i].Num() );
            for( int32 j = 0; j < ioSrcRects[i].Num(); ++j ) {
                compressedMips[i].Emplace( TArray< FCompressedImage2D >() );
                // Non MT mode
                if( !ensure( Compressor->BuildTexture( tileImages[i][j], TArray< FImage >(), buildSettings[i][0], compressedMips[i][j], unusedNumMipsInTail, unusedExtData ) ) )
                    return;
            }
        }

        // MT mode
        //int32 totalJobs = 0;
        //for( int i = 0; i < ioSrcRects.Num(); ++i )
        //    totalJobs += ioSrcRects[i].Num();
        //
        //ParallelFor( totalJobs, [&]( int32 iIndex ) {
        //    int counter = 0;
        //    int arrayIndex = 0;
        //    int subArrayIndex = 0;
        //    for( int i = 0; i < ioSrcRects.Num(); ++i ) {
        //        counter += ioSrcRects[i].Num();
        //        if( counter > iIndex ) {
        //            arrayIndex = i;
        //            subArrayIndex = iIndex - ( counter - ioSrcRects[i].Num() );
        //            break;
        //        }
        //    }
        //    if( !ensure( Compressor->BuildTexture( tileImages[arrayIndex][subArrayIndex], TArray< FImage >(), buildSettings[arrayIndex][0], compressedMips[arrayIndex][subArrayIndex], unusedNumMipsInTail, unusedExtData ) ) )
        //        return;
        //
        //} );
    }


    ////////////////////////////
    // Verify that we are not streaming to avoid regions to be wrong.
    // This can do an extra call to UpdateResources which is heavy, but
    // called only if TemporarilyDisableStreaming() hasn't been called
    // before on this texture or if someone from outside disabled it.
    iTexture->TemporarilyDisableStreaming();


    ////////////////////////////
    // Send the image data to the graphics card and wait for it to finish
    for( int32 i = 0; i < ioSrcRects.Num(); ++i ) {
        for( int32 j = 0; j < ioSrcRects[i].Num(); ++j ) {
            FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D( dstRects[i][j].x, dstRects[i][j].y, 0, 0, dstRects[i][j].w, dstRects[i][j].h );
            int blockBytes = GPixelFormats[ compressedMips[i][j][0].PixelFormat ].BlockBytes;
            iTexture->UpdateTextureRegions(
                  0
                , 1
                , region
                , compressedMips[i][j][0].SizeX * blockBytes
                , blockBytes
                , compressedMips[i][j][0].RawData.GetData()
                , []( uint8*, const FUpdateTextureRegion2D* iRegions ) {
                    delete  iRegions;
                }
            );
        }
    }


    ////////////////////////////
    // Fence now to ensure the update is processed on the GPU by the end of this function
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();
}

void
InvalidateTextureFromSourceData( const ::ULIS::FBlock* iData, UTexture2D* iTexture, const ::ULIS::FRectI* iRects, const uint32 iNumRects )
{
    TArray< TArray< ::ULIS::FRectI > > sortedRects;
    SortRects( iRects, iNumRects, sortedRects );
    InvalidateTextureFromSourceDataUsingSortedRects( iData, iTexture, sortedRects );
}

void
InvalidateTextureFromData( const ::ULIS::FBlock* iData, UTexture2D* iTexture, const ::ULIS::FRectI* iRects, const uint32 iNumRects )
{
#ifdef UE_BUILD_DEBUG
    //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

    checkf( iData, TEXT( "Error" ) );
    checkf( iTexture, TEXT( "Error") );
    // Considering only one region is an assumption that works but you have to be more carefull with several regions.
    InvalidateTextureFromSourceData( iData, iTexture, iRects, iNumRects );
#ifdef UE_BUILD_DEBUG
    //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    //UE_LOG( LogTemp, Warning, TEXT( "Time difference 0 = %ld �s" ), std::chrono::duration_cast<std::chrono::microseconds>( end - begin ).count() );
#endif
    return;
}

void
InvalidateSurfaceFromData( const ::ULIS::FBlock* iData, FOdysseySurfaceTexture2DEditable* iSurface, const ::ULIS::FRectI* iRects, const uint32 iNumRects )
{
    InvalidateTextureFromData( iData, iSurface->Texture(), iRects, iNumRects );
}

void
InvalidateSurfaceCallback( const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo )
{
    FOdysseySurfaceTexture2DEditable* surface = static_cast< FOdysseySurfaceTexture2DEditable* >( iInfo );
    InvalidateSurfaceFromData( iBlock, surface, iRects, iNumRects );
}

/////////////////////////////////////////////////////
// FOdysseySurfaceTexture2DEditable
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySurfaceTexture2DEditable::~FOdysseySurfaceTexture2DEditable()
{
    mTexture->RemoveFromRoot();
    if(!mIsBorrowedTexture) // If not borrowed, that means transient hence we are responsible for dealloc
    {
        checkf(mTexture,TEXT("Error: texture should be a valid pointer"));
        //texture->RemoveFromRoot(); // RM Prevent GC
        //delete texture;
        if(mTexture->IsValidLowLevel())
            mTexture->ConditionalBeginDestroy();
        mTexture = nullptr;
    }

    if(!mIsBorrowedBlock)
    {
        if(mBlock)
        {
            delete mBlock;
            mBlock = nullptr;
        }
    }
}

FOdysseySurfaceTexture2DEditable::FOdysseySurfaceTexture2DEditable(int iWidth,int iHeight, ::ULIS::eFormat iFormat)
    : mIsBorrowedTexture(false)
    , mIsBorrowedBlock(false)
{
    mTexture = UTexture2D::CreateTransient(iWidth, iHeight, PixelFormatForULISFormat(iFormat));
    #if WITH_EDITORONLY_DATA
    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    #endif
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    mTexture->SRGB = 1;
    //texture->AddToRoot(); // Prevent GC
    mTexture->Filter = TextureFilter::TF_Nearest;
    mTexture->UpdateResource();
    mTexture->FinishCachePlatformData(); //Wait UpdateResource Finished
    mTexture->AddToRoot();

    // Warning: the texture data source / bulk is allocated, then the block is allocated, then we copy the block content into bulk.
    mBlock = new ::ULIS::FBlock(iWidth,iHeight, iFormat, nullptr, ::ULIS::FOnInvalidBlock( &InvalidateSurfaceCallback, static_cast<void*>(this) ));
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ctx.Clear(*mBlock);
    ctx.Finish();
    Invalidate();
    // load texture data from block
    //CopyBlockDataIntoUTexture(mBlock,mTexture);
}

FOdysseySurfaceTexture2DEditable::FOdysseySurfaceTexture2DEditable(UTexture2D* iTexture, ::ULIS::FBlock* iBlock)
    : mIsBorrowedTexture(true)
    , mIsBorrowedBlock(true)
{
    checkf(iTexture,TEXT("iTexture == NULL"));
    checkf(iBlock,TEXT("iBlock == NULL"));
    ::ULIS::eFormat sourceFormat = ULISFormatForTextureSourceFormat(iTexture->Source.GetFormat());
    checkf( sourceFormat == iBlock->Format(),TEXT("iBlock format does not correspond to the expected format"));

    mTexture = iTexture;
    mTexture->AddToRoot();

    mBlock = iBlock;

    mBlock->OnInvalid(::ULIS::FOnInvalidBlock(&InvalidateSurfaceCallback, static_cast<void*>(this)));
}

FOdysseySurfaceTexture2DEditable::FOdysseySurfaceTexture2DEditable(UTexture2D* iTexture)
    : mIsBorrowedTexture(true)
    ,mIsBorrowedBlock(false)
{
    checkf(iTexture,TEXT("Cannot Initialize with Null borrowed texture"));
    mTexture = iTexture;
    mTexture->AddToRoot();

    // Warning: the block is allocated, then the texture data is copied into it.
    ::ULIS::eFormat sourceFormat = ULISFormatForTextureSourceFormat(iTexture->Source.GetFormat());
    mBlock = new ::ULIS::FBlock(
          mTexture->Source.GetSizeX()
        , mTexture->Source.GetSizeY()
        , sourceFormat
        , nullptr
        , ::ULIS::FOnInvalidBlock( &InvalidateSurfaceCallback, static_cast< void* >( this ) )
    );

    CopyUTextureSourceDataIntoBlock( mBlock, mTexture );
}

FOdysseySurfaceTexture2DEditable::FOdysseySurfaceTexture2DEditable(::ULIS::FBlock* iBlock)
    : mIsBorrowedTexture(false)
    ,mIsBorrowedBlock(true)
{
    checkf(iBlock,TEXT("Cannot Initialize with Null borrowed block"));
    mBlock = iBlock;

    mTexture = UTexture2D::CreateTransient(mBlock->Width(),mBlock->Height(), PixelFormatForULISFormat(mBlock->Format()));
    #if WITH_EDITORONLY_DATA
    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    #endif
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    mTexture->SRGB = 1;
    mTexture->Filter = TextureFilter::TF_Nearest;
    mTexture->UpdateResource();
    mTexture->FinishCachePlatformData(); //Wait UpdateResource Finished
    mTexture->AddToRoot();

    mBlock->OnInvalid( ::ULIS::FOnInvalidBlock( &InvalidateSurfaceCallback, static_cast< void* >( this ) ) );

    // load texture data from block
    //CopyBlockDataIntoUTexture(mBlock,mTexture);
    Invalidate();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

::ULIS::FBlock*
FOdysseySurfaceTexture2DEditable::Block()
{
    return mBlock;
}

const ::ULIS::FBlock*
FOdysseySurfaceTexture2DEditable::Block() const
{
    return mBlock;
}

UTexture2D*
FOdysseySurfaceTexture2DEditable::Texture()
{
    return mTexture;
}

const UTexture2D*
FOdysseySurfaceTexture2DEditable::Texture() const
{
    return mTexture;
}

bool
FOdysseySurfaceTexture2DEditable::IsBorrowedTexture() const
{
    return mIsBorrowedTexture;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- Public Tampon Methods
int
FOdysseySurfaceTexture2DEditable::Width()
{
    return mBlock->Width();
}

int
FOdysseySurfaceTexture2DEditable::Height()
{
    return mBlock->Height();
}

void
FOdysseySurfaceTexture2DEditable::Invalidate()
{
    mBlock->Dirty();
}

/*
void
FOdysseySurfaceTexture2DEditable::Invalidate(int iX1,int iY1,int iX2,int iY2)
{
    mBlock->Dirty(::ULIS::FRectI::FromMinMax(iX1,iY1,iX2,iY2));
}

void
FOdysseySurfaceTexture2DEditable::Invalidate(const ::ULIS::FRectI& iRect)
{
    mBlock->Dirty(iRect);
}
*/

void
FOdysseySurfaceTexture2DEditable::Invalidate( const ::ULIS::FRectI* iRects, const uint32 iNumRects )
{
    mBlock->Dirty( iRects, iNumRects );
}


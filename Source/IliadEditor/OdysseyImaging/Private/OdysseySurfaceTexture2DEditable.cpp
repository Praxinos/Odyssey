// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
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
#include "RHITypes.h"
#include "Async/ParallelFor.h"
#include "TextureCompiler.h"
#include "OdysseyPixelFormat.h"
#include "ColorManagement/TransferFunctions.h"
#include <ULIS>
#include "ULISUtils.h"

#ifdef UE_BUILD_DEBUG
#include <chrono>
#endif

/////////////////////////////////////////////////////
// Utlity

void
CopyImageToBlock(const FImage& iImage, ::ULIS::FBlock* iBlock)
{
    checkf(iBlock->Width() == iImage.GetWidth() &&
           iBlock->Height() == iImage.GetHeight()
           ,TEXT("Sizes do not match"));

    void* ptr = iImage.GetPixelPointer(0, 0);
    if (RawImageFormatNeedsConversionToULISFormat(iImage.Format))
    {
        ConvertRawImageFormatToULISFormat((uint8*)ptr, iBlock->Bits(), iBlock->Width(), iBlock->Height(), iImage.Format);
    }
    else
    {
        FMemory::Memcpy(iBlock->Bits(), (uint8*)ptr, iBlock->BytesTotal());
    }
}

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
CopyURenderTargetPixelDataIntoBlock(::ULIS::FBlock* iBlock, UTextureRenderTarget2D* iRenderTarget)
{
    checkf(iBlock->Width() == iRenderTarget->GetSurfaceWidth()  &&
           iBlock->Height() == iRenderTarget->GetSurfaceHeight()
           ,TEXT("Sizes do not match"));

    FTextureRenderTargetResource* renderTargetResource = iRenderTarget->GameThread_GetRenderTargetResource();
    checkf( false, TEXT( "Unused" ) );
}

void
CopyBlockDataToTextureSource(const ::ULIS::FBlock* iBlock, UTexture2D* iTexture, const TArray<FIntRect>& SrcRects)
{
    uint8* dst = iTexture->Source.LockMip(0);

    for (const FIntRect& SrcRect : SrcRects)
    {
        ETextureSourceFormat textureSourceFormat = iTexture->Source.GetFormat();
        ::ULIS::eFormat targetFormat = ULISFormatForTextureSourceFormat(textureSourceFormat);
        ::ULIS::FBlock block(SrcRect.Width(), SrcRect.Height(), targetFormat);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());
        ctx.ConvertFormat( *iBlock, block, ULISUtils::ToULISRectI(SrcRect));
        ctx.Finish();

        if (TextureSourceFormatNeedsConversionToULISFormat(textureSourceFormat))
        {
            TArray64<uint8> converted;
            converted.SetNumUninitialized(SrcRect.Width() * SrcRect.Height() * TextureSourceFormatBytesPerPixel(textureSourceFormat));
            ConvertULISFormatToTextureSourceFormat(block.Bits(), converted.GetData(), SrcRect.Width(), SrcRect.Height(), textureSourceFormat);

            int32 bpp = GTextureSourceFormats[textureSourceFormat].BytesPerPixel;
            for (int y = 0; y < SrcRect.Height(); y++)
            {
                uint8* srcLine = converted.GetData() + y * SrcRect.Width() * bpp;
                uint8* dstLine = dst + ((SrcRect.Min.Y + y) * iTexture->Source.GetSizeX() + SrcRect.Min.X) * bpp;
                FMemory::Memcpy(dstLine, srcLine, SrcRect.Width() * bpp);
            }
        }
        else
        {
            int32 bpp = GTextureSourceFormats[textureSourceFormat].BytesPerPixel;
            for (int y = 0; y < SrcRect.Height(); y++)
            {
                uint8* srcLine = block.Bits() + y * block.Width() * bpp;
                uint8* dstLine = dst + ((SrcRect.Min.Y + y) * iTexture->Source.GetSizeX() + SrcRect.Min.X) * bpp;
                FMemory::Memcpy(dstLine, srcLine, SrcRect.Width() * bpp);
            }
        }
    }

    iTexture->Source.UnlockMip(0);
}

void
InitTextureWithBlockData(const ::ULIS::FBlock* iBlock, UTexture2D* iTexture, ETextureSourceFormat iFormat)
{
    ::ULIS::eFormat targetFormat = ULISFormatForTextureSourceFormat(iFormat);
    ::ULIS::FBlock block(iBlock->Width(), iBlock->Height(), targetFormat);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block.Format());
    ctx.ConvertFormat( *iBlock, block );
    ctx.Finish();

    if (TextureSourceFormatNeedsConversionToULISFormat(iFormat))
    {
        TArray64<uint8> converted;
        converted.SetNumUninitialized(block.Width() * block.Height() * TextureSourceFormatBytesPerPixel(iFormat));
        ConvertULISFormatToTextureSourceFormat(block.Bits(), converted.GetData(), block.Width(), block.Height(), iFormat);

        iTexture->Source.Init(block.Width(), block.Height(), 1, 1, iFormat, converted.GetData());
    }
    else
    {
        iTexture->Source.Init(block.Width(), block.Height(), 1, 1, iFormat, block.Bits());
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
    FTextureCompilingManager::Get().FinishCompilation({ iTexture });

    ::ULIS::FBlock* block = new ::ULIS::FBlock( iTexture->Source.GetSizeX(),iTexture->Source.GetSizeY(), iFormat );
    FillOdysseyBlockFromUTextureData( block, iTexture, iFormat );
    return block;
}

void
FillOdysseyBlockFromUTextureData( ::ULIS::FBlock* ioBlock, UTexture2D* iTexture, ::ULIS::eFormat iFormat )
{
    FTextureCompilingManager::Get().FinishCompilation({ iTexture });

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

UTexture2D*
NewRGBAFTextureFromBlockData(::ULIS::FBlock* iBlock)
{
    UTexture2D* newTexture = NewObject<UTexture2D>();
    newTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;

    ETextureSourceFormat srcFormat = TextureSourceFormatForULISFormat(::ULIS::eFormat::Format_RGBAF);

    InitTextureWithBlockData( iBlock, newTexture, srcFormat);

    newTexture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ newTexture });


    return newTexture;

    /*
    check(iBlock);

    // We are using the source art so grab the original width/height
    const int32 Width = iBlock->Width();
    const int32 Height = iBlock->Height();

    check(Width > 0 && Height > 0);

    EPixelFormat pixelFormat = PixelFormatForULISFormat(iBlock->Format());
    if( pixelFormat == PF_Unknown )
        pixelFormat = PF_B8G8R8A8;

    // Allocate the new texture
    UTexture2D* NewTexture2D = UTexture2D::CreateTransient(Width, Height, pixelFormat);

    // Fill in the base mip for the texture we created
    void* MipData = (void*)NewTexture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
    //uint16* MipData = (uint16*)NewTexture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

    FMemory::Memcpy(MipData, iBlock->Bits(), iBlock->BytesTotal());
    NewTexture2D->PlatformData->Mips[0].BulkData.Unlock();

    UE_LOG(LogTemp, Display, TEXT("Num bytes: %ld"), iBlock->BytesTotal());
    // Update the remote texture data
    NewTexture2D->UpdateResource();
    return NewTexture2D;
    */
}

/* void
ImageLinearToSRGB(const FImageView& iImage)
{
    FImage rgba32FImage(iImage.SizeX, iImage.SizeY, ERawImageFormat::RGBA32F, EGammaSpace::Linear);
    FImageCore::CopyImage(iImage, rgba32FImage);

    TArrayView64<FLinearColor> colors = rgba32FImage.AsRGBA32F();
    for (FLinearColor& color : colors)
    {
        color.R = UE::Color::EncodeSRGB(color.R);
        color.G = UE::Color::EncodeSRGB(color.G);
        color.B = UE::Color::EncodeSRGB(color.B);
    }

    FImageCore::CopyImage(rgba32FImage, iImage);
}

void
ImageSRGBToLinear(const FImageView& iImage)
{
    FImage rgba32FImage(iImage.SizeX, iImage.SizeY, ERawImageFormat::RGBA32F, EGammaSpace::Linear);
    FImageCore::CopyImage(iImage, rgba32FImage);

    TArrayView64<FLinearColor> colors = rgba32FImage.AsRGBA32F();
    for (FLinearColor& color : colors)
    {
        color.R = UE::Color::DecodeSRGB(color.R);
        color.G = UE::Color::DecodeSRGB(color.G);
        color.B = UE::Color::DecodeSRGB(color.B);
    }

    FImageCore::CopyImage(rgba32FImage, iImage);
} */

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
        case TSF_RGBA32F:   return ERawImageFormat::RGBA32F;
        default: break;
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
        GetBuildSettingsForRunningPlatform( *iTexture, ETextureEncodeSpeed::Fast, refBuildSettings, nullptr );
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

                EGammaSpace gammaSpace = ((fmt != ERawImageFormat::G8 && fmt != ERawImageFormat::BGRA8) || !iTexture->SRGB) ? EGammaSpace::Linear : EGammaSpace::sRGB;
                //EGammaSpace gammaSpace = !iTexture->SRGB ? EGammaSpace::Linear : EGammaSpace::sRGB;

                const int len = ioSrcRects[i][j].h;
                tileImages[i].Emplace(
                    TArray< FImage >(
                        {
                            FImage(
                                  ioSrcRects[i][j].w
                                , ioSrcRects[i][j].h
                                , 1
                                , fmt
                                , gammaSpace
                            )
                        }
                    )
                );

                int rowSize = bPred ? ( iTexture->Source.CalcMipSize( 0 ) * ioSrcRects[i][j].w ) / ( w * h ) : rowSize = iData->BytesPerPixel() * ioSrcRects[i][j].w;
                tileImages[i][j][0].RawData.SetNumUninitialized( rowSize * len );
                tileBlocks[i].Emplace( tileImages[i][j][0].RawData.GetData(), ioSrcRects[i][j].w, ioSrcRects[i][j].h, iData->Format() );
                if( bPred ) {
                    // TODO: This part can be  optimizedand multithreaded
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
                    ctx.Finish();
                }
            }
        }
        ctx.Finish();
    }

    ////////////////////////////
    // Apply the texture parameters to retrieve the data to send to the graphics card
    TArray< TArray< TArray< FCompressedImage2D > > > compressedMips;
    TArray< FImage > images;
    compressedMips.Reserve( ioSrcRects.Num() );
    {
        uint32 unusedNumMipsInTail, unusedExtData;
        for( int32 i = 0; i < ioSrcRects.Num(); ++i ) {
            compressedMips.Emplace( TArray< TArray< FCompressedImage2D > >() );
            compressedMips[i].Reserve( ioSrcRects[i].Num() );
            for( int32 j = 0; j < ioSrcRects[i].Num(); ++j ) {
                compressedMips[i].Emplace( TArray< FCompressedImage2D >() );
                // Non MT mode
                if( !ensure( Compressor->BuildTexture( tileImages[i][j], images, buildSettings[i][0], FStringView(), compressedMips[i][j], unusedNumMipsInTail, unusedExtData, nullptr )))
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

    //TemporarilyDisableStreaming() can call UpdateResource()
    //And UpdateTextureRegions needs any compilation of the texture to be finished
    FTextureCompilingManager::Get().FinishCompilation({ iTexture });

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
    TArray< ::ULIS::FRectI > rects(iRects, iNumRects);
    rects = rects.FilterByPredicate( [iTexture]( const ::ULIS::FRectI& iRect ) {
        return iRect.w > 0 && iRect.h > 0 && iRect.x >= 0 && iRect.y >= 0 && iRect.x + iRect.w <= iTexture->GetSurfaceWidth() && iRect.y + iRect.h <= iTexture->GetSurfaceHeight();
    } );

    TArray< TArray< ::ULIS::FRectI > > sortedRects;
    SortRects( rects.GetData(), rects.Num(), sortedRects );
    InvalidateTextureFromSourceDataUsingSortedRects( iData, iTexture, sortedRects );


    /*TArray< TArray< ::ULIS::FRectI > > sortedRects;
    SortRects( iRects, iNumRects, sortedRects );
    InvalidateTextureFromSourceDataUsingSortedRects( iData, iTexture, sortedRects );*/
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
    if(!mIsBorrowedTexture) // If not borrowed, that means transient hence we are responsible for dealloc
    {
        checkf(mTexture,TEXT("Error: texture should be a valid pointer"));
        //texture->RemoveFromRoot(); // RM Prevent GC
        //delete texture;
        if(mTexture->IsValidLowLevel())
            mTexture->ConditionalBeginDestroy();
        mTexture = nullptr;
    }
}

FOdysseySurfaceTexture2DEditable::FOdysseySurfaceTexture2DEditable(int iWidth,int iHeight, ::ULIS::eFormat iFormat, bool iSRGB)
    : mIsBorrowedTexture(false)
{
    EPixelFormat pixelFormat = PixelFormatForULISFormat(iFormat);
    mTexture = TStrongObjectPtr<UTexture2D>(UTexture2D::CreateTransient(iWidth, iHeight, pixelFormat));
    #if WITH_EDITORONLY_DATA
    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    #endif
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;

    //IsImageInfoValid() in ImageCore.h allows use of sRGB only on G8 and BGRA8 textures
    if (iSRGB)
        mTexture->SRGB = pixelFormat == EPixelFormat::PF_G8 || pixelFormat == EPixelFormat::PF_B8G8R8A8;
    else
        mTexture->SRGB = false;

    mTexture->Filter = TextureFilter::TF_Nearest;
    mTexture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({mTexture.Get()});

    // Warning: the texture data source / bulk is allocated, then the block is allocated, then we copy the block content into bulk.
    mBlock = MakeShared<::ULIS::FBlock>(iWidth,iHeight, iFormat, nullptr, ::ULIS::FOnInvalidBlock( &InvalidateSurfaceCallback, static_cast<void*>(this) ));
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ctx.Clear(*mBlock);
    ctx.Finish();
    Invalidate();
    // load texture data from block
    //CopyBlockDataIntoUTexture(mBlock,mTexture);
}

FOdysseySurfaceTexture2DEditable::FOdysseySurfaceTexture2DEditable(UTexture2D* iTexture, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
    : mIsBorrowedTexture(true)
{
    checkf(iTexture,TEXT("iTexture == NULL"));
    checkf(iBlock,TEXT("iBlock == NULL"));
    ::ULIS::eFormat sourceFormat = ULISFormatForTextureSourceFormat(iTexture->Source.GetFormat());
    checkf( sourceFormat == iBlock->Format(),TEXT("iBlock format does not correspond to the expected format"));

    mTexture = TStrongObjectPtr<UTexture2D>(iTexture);

    mBlock = iBlock;

    mBlock->OnInvalid(::ULIS::FOnInvalidBlock(&InvalidateSurfaceCallback, static_cast<void*>(this)));
}

FOdysseySurfaceTexture2DEditable::FOdysseySurfaceTexture2DEditable(UTexture2D* iTexture)
    : mIsBorrowedTexture(true)
{
    checkf(iTexture,TEXT("Cannot Initialize with Null borrowed texture"));
    mTexture = TStrongObjectPtr<UTexture2D>(iTexture);

    // Warning: the block is allocated, then the texture data is copied into it.
    ::ULIS::eFormat sourceFormat = ULISFormatForTextureSourceFormat(iTexture->Source.GetFormat());
    mBlock = MakeShared<::ULIS::FBlock>(
          mTexture->Source.GetSizeX()
        , mTexture->Source.GetSizeY()
        , sourceFormat
        , nullptr
        , ::ULIS::FOnInvalidBlock( &InvalidateSurfaceCallback, static_cast< void* >( this ) )
    );

    CopyUTextureSourceDataIntoBlock( mBlock.Get(), mTexture.Get());
}

FOdysseySurfaceTexture2DEditable::FOdysseySurfaceTexture2DEditable(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, bool iSRGB)
    : mIsBorrowedTexture(false)
{
    checkf(iBlock,TEXT("Cannot Initialize with Null borrowed block"));
    mBlock = iBlock;

    EPixelFormat pixelFormat = PixelFormatForULISFormat(mBlock->Format());

    mTexture = TStrongObjectPtr<UTexture2D>(UTexture2D::CreateTransient(mBlock->Width(),mBlock->Height(), pixelFormat));
    #if WITH_EDITORONLY_DATA
    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    #endif
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;

    //IsImageInfoValid() in ImageCore.h allows use of sRGB only on G8 and BGRA8 textures
    if (iSRGB)
        mTexture->SRGB = pixelFormat == EPixelFormat::PF_G8 || pixelFormat == EPixelFormat::PF_B8G8R8A8;
    else
        mTexture->SRGB = false;
    mTexture->Filter = TextureFilter::TF_Nearest;
    mTexture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ mTexture.Get() });

    mBlock->OnInvalid( ::ULIS::FOnInvalidBlock( &InvalidateSurfaceCallback, static_cast< void* >( this ) ) );

    // load texture data from block
    //CopyBlockDataIntoUTexture(mBlock,mTexture);
    Invalidate();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseySurfaceTexture2DEditable::Block() const
{
    return mBlock;
}

UTexture2D*
FOdysseySurfaceTexture2DEditable::Texture()
{
    return mTexture.Get();
}

const UTexture2D*
FOdysseySurfaceTexture2DEditable::Texture() const
{
    return mTexture.Get();
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
FOdysseySurfaceTexture2DEditable::Invalidate(const TArray<::ULIS::FRectI>& iRects)
{
    mBlock->Dirty( iRects.GetData(), iRects.Num());
}

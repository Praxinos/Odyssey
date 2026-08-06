// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTexture2DUtils.h"

#include "Engine/Texture2D.h"

namespace Odyssey {

void
ResizeTexture2D( UTexture2D* InOutTexture, uint32 InNewWidth, uint32 InNewHeight )
{
    FTextureSource& Source = InOutTexture->Source;

    // Read the existing image
    FImage OldImage;
    verify(Source.GetMipImage(OldImage, 0));

    // Create a new image with the same format
    FImage NewImage(
        InNewWidth,
        InNewHeight,
        OldImage.Format,
        OldImage.GammaSpace);

    // Fill with zeros (transparent/black, depending on the format)
    FMemory::Memzero(NewImage.RawData.GetData(), NewImage.RawData.Num());

    const int32 BytesPerPixel = OldImage.GetBytesPerPixel();

    const int32 CopyWidth  = FMath::Min(OldImage.SizeX, NewImage.SizeX);
    const int32 CopyHeight = FMath::Min(OldImage.SizeY, NewImage.SizeY);

    const int32 SrcPitch = OldImage.SizeX * BytesPerPixel;
    const int32 DstPitch = NewImage.SizeX * BytesPerPixel;
    const int32 CopyBytesPerRow = CopyWidth * BytesPerPixel;

    for (int32 Y = 0; Y < CopyHeight; ++Y)
    {
        FMemory::Memcpy(
            NewImage.RawData.GetData() + Y * DstPitch,
            OldImage.RawData.GetData() + Y * SrcPitch,
            CopyBytesPerRow);
    }

    // Replace the texture source
    Source.Init(NewImage);
    InOutTexture->PostEditChange();
}

}

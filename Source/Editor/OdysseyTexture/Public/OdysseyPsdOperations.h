// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Serialization/BufferArchive.h"

#include <ULIS>

class UTexture2D;
class UOdysseyTextureLayerStack;

struct FPsdLayerInfo
{
    uint32_t mLeft = 0, mTop = 0, mBottom = 0, mRight = 0;
    int16_t mID[32];
    uint16_t mNumChannels;
    int32_t mChannelSize[32];
    uint32_t mStartChannelPos[32];
    char mBlendModeKey[5];
    uint8_t mOpacity;
    uint8_t mFlags;
    uint32_t mExtraSize;
    uint32_t mExtraPosition;
    uint32_t mExtraRead;

    uint32_t mLayerMaskSize;
    uint32_t mYMask;
    uint32_t mXMask;
    uint32_t mWMask;
    uint32_t mHMask;
    uint8_t  mColorMask;
    uint8_t  mFlagsMask;

    uint32_t mLayerBlendingSize;

    uint8_t mNameSize;
    char mName[257];

    uint32_t mSizeLayerImage = 0;
    uint8_t* mLayerImageDst = nullptr;
    uint16_t* mLayerImageDst16 = nullptr;
    uint32_t* mLayerImageDst32 = nullptr;

    uint32_t mDividerType;
};

class FOdysseyPsdOperations
{
public:
    // Construction / Destruction
    ~FOdysseyPsdOperations();
    FOdysseyPsdOperations(const TCHAR* iFilename, UTexture2D* ioTexture);

private:
    /** The header of the file, with basic info about the image and verification that it's indeed a psd file */
    bool ReadFileHeader();

    /** Used for indexed colored images (look up tables), mostly useless for our purposes for now */
    bool ReadFileColorMode();

    /** Image resources, stores paths of pen strokes and such, mostly useless for our purposes for now */
    bool ReadImageResources();

    bool ReadLayerAndMaskInfo();

    bool ReadLayerInfo();

    bool ReadMaskInfo();

    bool ReadLayers();

    bool ReadAdditionalLayerInfoSignature();

    bool ReadAdditionalLayerInfo( uint32_t sectionEnd );

    //bool ReadImageData();

    bool ReadLayerStackData();
    bool ReadLayerStackData16();
    bool ReadLayerStackData32();

    void GenerateLayerStackFromLayerStackData();

    void CopyUncompressed(uint32_t* dst,uint32_t length);
    void CopyUncompressed(uint16_t* dst,uint32_t length);
    void CopyUncompressed(uint8_t* dst,uint32_t length);

    void DecodeAndCopyRLE(uint32_t* dst,uint32_t length);
    void DecodeAndCopyRLE(uint16_t* dst,uint32_t length);
    void DecodeAndCopyRLE(uint8_t* dst,uint32_t length);

    void UnpredictZip16(uint8* dst,uint32_t length, uint32_t numColumns, uint32_t rowSize);
    void UnpredictZip32(uint8* src, uint8* dst, uint32_t length,uint32_t numColumns, uint32_t numRows, uint32_t rowSize);

    //Size of dst should be 8(bits) * 3(channels) = 24 times size of src
    void PlanarByteConvertBitMapToBGRA8(uint8_t* src,uint8_t* dst,uint32_t length);

    void PlanarByteConvertOrdered(uint8_t* src,uint8_t* dst,uint32_t length, uint8_t numChannels, uint8_t channelsOrder[]);
    void PlanarByteConvertOrdered(uint16_t* src,uint16_t* dst,uint32_t length,uint8_t numChannels,uint8_t channelsOrder[]);

    void PlanarByteConvert( uint8_t* src, uint8_t* dst, uint32_t length, uint8_t numChannels );
    void PlanarByteConvert(uint16_t* src,uint16_t* dst,uint32_t length,uint8_t numChannels);
    void PlanarByteConvert(uint32_t* src,uint32_t* dst,uint32_t length,uint8_t numChannels);

    void NegateImage( uint8_t* ioSrc, uint32_t length );
    void NegateImage( uint16_t* ioSrc, uint32_t length );
    void NegateImage( uint32_t* ioSrc,uint32_t length );

    void lerp24BitsInto32Bits( uint32_t* ioSrc,uint32_t length );

    ::ULIS::eBlendMode GetBlendingModeFromPSD( char iBlendModeKey[5] );
    void CreateLayerStack();

public:

    //-- Getters / Setters
    uint16_t GetChannelsNumber();
    uint32_t GetImageHeight();
    uint32_t GetImageWidth();
    uint16_t GetBitDepth();
    uint16_t GetColorMode();

    uint8_t* GetImageDst();
    uint16_t* GetImageDst16();

    //-- Import / Export
    /** Tries to import the file passed in the constructor, return true if succeeded */
    bool Import();

private:
    IFileHandle* mFileHandle;

    uint16_t mChannelsNumber;
    uint32_t mImageHeight;
    uint32_t mImageWidth;
    uint16_t mBitDepth;
    uint16_t mColorMode;

    uint32_t mImageStart;

    uint8_t* mImageDst;
    uint16_t* mImageDst16;
    uint32_t* mImageDst32;

    UTexture2D* mTexture;
    UOdysseyTextureLayerStack* mLayerStack;

    TArray<FPsdLayerInfo> mLayersInfo;
};


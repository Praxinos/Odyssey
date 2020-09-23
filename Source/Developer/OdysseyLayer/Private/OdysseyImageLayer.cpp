// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyImageLayer.h"
#include "OdysseyBlock.h"
#include "OdysseyLayerStack.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyImageLayer"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyImageLayer::~FOdysseyImageLayer()
{
    delete mBlock;
}

FOdysseyImageLayer::FOdysseyImageLayer( const FName& iName, FVector2D iSize, ::ul3::tFormat iFormat)
    : IOdysseyLayer( iName, IOdysseyLayer::eType::kImage )
    , mBlock( nullptr )
    , mBlendingMode( ::ul3::BM_NORMAL )
    , mOpacity( 1.0f )
    , mIsAlphaLocked( false )
{
    check( iSize.X >= 0 && iSize.Y >= 0 );

    mBlock = new FOdysseyBlock( iSize.X, iSize.Y, iFormat);
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool()
                , ULIS3_BLOCKING
                , perfIntent
                , hULIS.HostDeviceInfo()
                , ULIS3_NOCB
                , mBlock->GetBlock()
                , mBlock->GetBlock()->Rect() );
}

FOdysseyImageLayer::FOdysseyImageLayer( const FName& iName, FOdysseyBlock* iBlock )
    : IOdysseyLayer( iName, IOdysseyLayer::eType::kImage )
    , mBlock( iBlock )
    , mBlendingMode( ::ul3::BM_NORMAL )
    , mOpacity( 1.0f )
    , mIsAlphaLocked( false )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

FOdysseyBlock*
FOdysseyImageLayer::GetBlock() const
{
    return mBlock;
}

::ul3::eBlendingMode
FOdysseyImageLayer::GetBlendingMode() const
{
    return mBlendingMode;
}

void
FOdysseyImageLayer::SetBlendingMode( ::ul3::eBlendingMode iBlendingMode )
{
    mBlendingMode = iBlendingMode;
}

void
FOdysseyImageLayer::SetBlendingMode( FText iBlendingMode )
{
    const int max = static_cast< int >( ::ul3::NUM_BLENDING_MODES );
    for( uint8 i = 0; i < max; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[i] ) );
        if( iBlendingMode.EqualTo( entry ) )
        {
            SetBlendingMode( static_cast<::ul3::eBlendingMode>( i ) );
            return;
        }
    }
}

FText
FOdysseyImageLayer::GetBlendingModeAsText() const
{
    return FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[static_cast<int>( mBlendingMode )] ) );
}

float
FOdysseyImageLayer::GetOpacity() const
{
    return mOpacity;
}

void
FOdysseyImageLayer::SetOpacity( float iOpacity )
{
    if( iOpacity < 0.f || iOpacity > 1.f )
        return;

    mOpacity = iOpacity;
}

bool
FOdysseyImageLayer::IsAlphaLocked() const
{
    return mIsAlphaLocked;
}

void
FOdysseyImageLayer::SetIsAlphaLocked( bool iIsAlphaLocked )
{

	mIsAlphaLocked = iIsAlphaLocked;
}


void
FOdysseyImageLayer::CopyPropertiesFrom( const FOdysseyImageLayer &iCopy )
{
    mOpacity = iCopy.GetOpacity();
    mBlendingMode = iCopy.GetBlendingMode();
    mIsLocked = iCopy.IsLocked();
    mIsVisible = iCopy.IsVisible();
    mIsAlphaLocked = iCopy.IsAlphaLocked();
}

// Custom serialization version for FOdysseyLayerStack
struct FOdysseyImageLayerObjectVersion
{
	enum Type
	{
		// Before any version changes were made
		SavePixelFormat,
		
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

private:
	FOdysseyImageLayerObjectVersion() {}
};

const FGuid FOdysseyImageLayerObjectVersion::GUID(0xE2CA928C, 0x4FCB03A0, 0xE22252AA, 0xA88FC0B5);
FCustomVersionRegistration FOdysseyImageLayerObjectVersionRegistration(FOdysseyImageLayerObjectVersion::GUID, FOdysseyImageLayerObjectVersion::LatestVersion, TEXT("FOdysseyImageLayerObjectVersion::SavePixelFormat"));

FArchive& 
operator<<(FArchive &Ar,FOdysseyImageLayer* ioSaveImageLayer)
{
    if(!ioSaveImageLayer)
        return Ar;

	//Set the Object Version
	Ar.UsingCustomVersion(FOdysseyImageLayerObjectVersion::GUID);

    /*IOdysseySerializable* serializable = (FOdysseyImageLayer*)(ioSaveImageLayer);
    Ar << serializable;*/
    Ar << ioSaveImageLayer->mName;
    Ar << ioSaveImageLayer->mIsLocked;
    Ar << ioSaveImageLayer->mIsVisible;
    Ar << ioSaveImageLayer->mIsAlphaLocked;
    int bm = static_cast< int >( ioSaveImageLayer->mBlendingMode );
    Ar << bm;
    Ar << ioSaveImageLayer->mOpacity;

    if(Ar.IsSaving()) {
        int width = ioSaveImageLayer->mBlock->Width();
        int height = ioSaveImageLayer->mBlock->Height();
		::ul3::tFormat format = ioSaveImageLayer->mBlock->Format();

        Ar << width;
        Ar << height;
		if (Ar.CustomVer(FOdysseyImageLayerObjectVersion::GUID) >= FOdysseyImageLayerObjectVersion::SavePixelFormat)
		{
			
			Ar << format;
		}

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::FBlock* blockLayerData = ::ul3::XCopy( hULIS.ThreadPool()
                                                    , ULIS3_BLOCKING
                                                    , perfIntent
                                                    , hULIS.HostDeviceInfo()
                                                    , ULIS3_NOCB
                                                    , ioSaveImageLayer->mBlock->GetBlock()
                                                    , ::ul3::FRect( 0, 0, ioSaveImageLayer->mBlock->Width(), ioSaveImageLayer->mBlock->Height() ) );

        TArray<uint8> layerData = TArray<uint8>();
        layerData.AddUninitialized(blockLayerData->BytesTotal());
        FMemory::Memcpy(layerData.GetData(),blockLayerData->DataPtr(),blockLayerData->BytesTotal());
        delete blockLayerData;

        Ar << layerData;
    } else if(Ar.IsLoading())
    {
        int width;
        int height;

        Ar << width;
        Ar << height;

		::ul3::tFormat format = ULISFormatForUE4TextureSourceFormat(ETextureSourceFormat::TSF_BGRA8);
		if (Ar.CustomVer(FOdysseyImageLayerObjectVersion::GUID) >= FOdysseyImageLayerObjectVersion::SavePixelFormat)
		{
			Ar << format;
		}

        check(!ioSaveImageLayer->mBlock);
        ioSaveImageLayer->mBlock = new FOdysseyBlock(width,height, format);

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Clear( hULIS.ThreadPool()
                    , ULIS3_BLOCKING
                    , perfIntent
                    , hULIS.HostDeviceInfo()
                    , ULIS3_NOCB
                    , ioSaveImageLayer->mBlock->GetBlock()
                    , ioSaveImageLayer->mBlock->GetBlock()->Rect() );

        TArray< uint8 > layerData = TArray< uint8 >();
        layerData.AddUninitialized(ioSaveImageLayer->mBlock->GetBlock()->BytesTotal());

        Ar << layerData;

        // What is this ? A copy ? But layerData is uninitialized data ?
        for(int j = 0; j < layerData.Num(); j++) {
            *(ioSaveImageLayer->mBlock->GetBlock()->DataPtr() + j) = layerData[j];
        }
    }
    return Ar;
}

//---

#undef LOCTEXT_NAMESPACE

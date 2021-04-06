// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureImportFactory.h"

#include "Editor.h"
#include "EditorStyleSet.h"
#include "Engine/Texture2D.h"

#include "ULISLoaderModule.h"
#include "OdysseyBlock.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTexture.h"
#include "OdysseyPsdOperations.h"

#include <ULIS3>

/////////////////////////////////////////////////////
// UOdysseyTextureFactory
UOdysseyTextureImportFactory::UOdysseyTextureImportFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    // From UFactory
    bEditorImport = true;
    ImportPriority = 101; //Default other factories importing textures are at 100
    SupportedClass = UOdysseyTexture::StaticClass();

    Formats.Add(TEXT("psd;Texture"));
}

bool UOdysseyTextureImportFactory::ConfigureProperties()
{
    return true;
}

UObject* UOdysseyTextureImportFactory::FactoryCreateBinary(UClass* Class,UObject* InParent,FName Name,EObjectFlags Flags,UObject* Context,const TCHAR* Type,const uint8*& Buffer,const uint8* BufferEnd,FFeedbackContext* Warn)
{
    //Load PSD as texture
    FOdysseyPsdOperations psdReader = FOdysseyPsdOperations(*CurrentFilename);

    if( !psdReader.Import() )
        return nullptr;
    
    UTexture2D* object = NewObject<UTexture2D>(InParent,Name,Flags | RF_Transactional);
    
    if(psdReader.GetLayerStack())
    {
        FOdysseyBlock* srcblock = new FOdysseyBlock(psdReader.GetLayerStack()->Width(),psdReader.GetLayerStack()->Height(),psdReader.GetLayerStack()->Format());
        psdReader.GetLayerStack()->ComputeResultInBlock(srcblock->GetBlock(),::ul3::FRect(0,0,psdReader.GetImageWidth(),psdReader.GetImageHeight()));
        InitTextureWithBlockData(srcblock,object,UE4TextureSourceFormatForULISFormat(srcblock->Format()));

        UOdysseyTextureAssetUserData* userData = NewObject< UOdysseyTextureAssetUserData >(object,NAME_None,RF_Public);
        userData->SetLayerStack(psdReader.GetLayerStack());
        object->AddAssetUserData(userData);
    }


    /*
    if( psdReader.GetBitDepth() > 8 )
    {
        if( psdReader.GetChannelsNumber() == 4 )
            srcblock = new ::ul3::FBlock((::ul3::tByte*)psdReader.GetImageDst16(),psdReader.GetImageWidth(),psdReader.GetImageHeight(),ULIS3_FORMAT_RGBA16);
        else
            srcblock = new ::ul3::FBlock((::ul3::tByte*)psdReader.GetImageDst16(),psdReader.GetImageWidth(),psdReader.GetImageHeight(),ULIS3_FORMAT_RGB16);

        FOdysseyBlock* myBlock = new FOdysseyBlock(psdReader.GetImageWidth(),psdReader.GetImageHeight(),ULIS3_FORMAT_RGBA16);

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 MT_bit = ULIS3_PERF_MT;
        ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

        ::ul3::Conv(hULIS.ThreadPool(),ULIS3_BLOCKING,perfIntent,hULIS.HostDeviceInfo(),ULIS3_NOCB,srcblock,myBlock->GetBlock());

        //UE_LOG(LogTemp,Display,TEXT("RGBBlock: %d, RGBABlock: %d"),srcblock->BytesTotal(),myBlock->GetArray().Num());

        InitTextureWithBlockData(myBlock,object,ETextureSourceFormat::TSF_RGBA16);
    }
    else
    {
        if(psdReader.GetChannelsNumber() == 4)
            srcblock = new ::ul3::FBlock((::ul3::tByte*)psdReader.GetImageDst(),psdReader.GetImageWidth(),psdReader.GetImageHeight(),ULIS3_FORMAT_ARGB8);
        else
            srcblock = new ::ul3::FBlock((::ul3::tByte*)psdReader.GetImageDst(),psdReader.GetImageWidth(),psdReader.GetImageHeight(),ULIS3_FORMAT_RGB8);


        FOdysseyBlock* myBlock = new FOdysseyBlock(psdReader.GetImageWidth(),psdReader.GetImageHeight(),ULIS3_FORMAT_BGRA8);

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 MT_bit = ULIS3_PERF_MT;
        ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

        ::ul3::Conv(hULIS.ThreadPool(),ULIS3_BLOCKING,perfIntent,hULIS.HostDeviceInfo(),ULIS3_NOCB,srcblock,myBlock->GetBlock());

        //UE_LOG(LogTemp,Display,TEXT("RGBBlock: %d, RGBABlock: %d"),srcblock->BytesTotal(),myBlock->GetArray().Num());

        InitTextureWithBlockData(myBlock,object,ETextureSourceFormat::TSF_BGRA8);
    }
    */


    object->PostEditChange();
        
    return object;
}

bool UOdysseyTextureImportFactory::FactoryCanImport(const FString& Filename)
{
    //If it's a psd, we take care of it. It not, other factories will take care of it
    FString Extension = FPaths::GetExtension(Filename);

    return (Formats.ContainsByPredicate(
        [&Extension](const FString& Format)
    {
        return Format.StartsWith(Extension);
    }));
}

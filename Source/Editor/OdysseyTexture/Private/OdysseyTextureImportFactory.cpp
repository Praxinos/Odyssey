// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureImportFactory.h"

#include "Editor.h"
#include "EditorStyleSet.h"
#include "Engine/Texture2D.h"

#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyPsdOperations.h"

#include "ULISLoaderModule.h"
#include <ULIS>

/////////////////////////////////////////////////////
// UOdysseyTextureFactory
UOdysseyTextureImportFactory::UOdysseyTextureImportFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    // From UFactory
    bEditorImport = true;
    ImportPriority = 101; //Default other factories importing textures are at 100
    SupportedClass = UTexture2D::StaticClass();

    Formats.Add(TEXT("psdfix;Texture"));
}

bool UOdysseyTextureImportFactory::ConfigureProperties()
{
    return true;
}

UObject* UOdysseyTextureImportFactory::FactoryCreateBinary(UClass* Class,UObject* InParent,FName Name,EObjectFlags Flags,UObject* Context,const TCHAR* Type,const uint8*& Buffer,const uint8* BufferEnd,FFeedbackContext* Warn)
{
    //Load PSD as texture
    UTexture2D* texture = NewObject<UTexture2D>(InParent,Name,Flags | RF_Transactional);
    FOdysseyPsdOperations psdReader = FOdysseyPsdOperations(*CurrentFilename, texture);

    if( !psdReader.Import() )
        return nullptr;
    
    /* UTexture2D* object = NewObject<UTexture2D>(InParent,Name,Flags | RF_Transactional);
    
    if(psdReader.GetLayerStack())
    {
        ::ULIS::FBlock* srcblock = new ::ULIS::FBlock( psdReader.GetLayerStack()->Width(), psdReader.GetLayerStack()->Height(), psdReader.GetLayerStack()->Format() );
        ::ULIS::FRectI rect( 0, 0, psdReader.GetImageWidth() , psdReader.GetImageHeight() );
        psdReader.GetLayerStack()->ComputeResultInBlock( srcblock, &rect, 1 );
        InitTextureWithBlockData(srcblock,object,TextureSourceFormatForULISFormat(srcblock->Format()));

        UOdysseyTextureLayerStackUserData* userData = NewObject< UOdysseyTextureLayerStackUserData >( object, NAME_None, RF_Public );
        userData->SetOldLayerStack( psdReader.GetLayerStack() );
        object->AddAssetUserData( userData );
    } */


    /*
    if( psdReader.GetBitDepth() > 8 )
    {
        if( psdReader.GetChannelsNumber() == 4 )
            srcblock = new ::ULIS::FBlock((::ULIS::tByte*)psdReader.GetImageDst16(),psdReader.GetImageWidth(),psdReader.GetImageHeight(),::ULIS::Format_RGBA16);
        else
            srcblock = new ::ULIS::FBlock((::ULIS::tByte*)psdReader.GetImageDst16(),psdReader.GetImageWidth(),psdReader.GetImageHeight(),::ULIS::Format_RGB16);

        ::ULIS::FBlock* myBlock = new ::ULIS::FBlock(psdReader.GetImageWidth(),psdReader.GetImageHeight(),::ULIS::Format_RGBA16);

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ULIS::uint32 MT_bit = ULIS_PERF_MT;
        ::ULIS::uint32 perfIntent = MT_bit | ULIS_PERF_SSE42;

        ::ULIS::Conv(hULIS.ThreadPool(),ULIS_BLOCKING,perfIntent,hULIS.HostDeviceInfo(),ULIS_NOCB,srcblock,myBlock);

        //UE_LOG(LogTemp,Display,TEXT("RGBBlock: %d, RGBABlock: %d"),srcblock->BytesTotal(),myBlock->BytesTotal());

        InitTextureWithBlockData(myBlock,object,ETextureSourceFormat::TSF_RGBA16);
    }
    else
    {
        if(psdReader.GetChannelsNumber() == 4)
            srcblock = new ::ULIS::FBlock((::ULIS::tByte*)psdReader.GetImageDst(),psdReader.GetImageWidth(),psdReader.GetImageHeight(),::ULIS::Format_ARGB8);
        else
            srcblock = new ::ULIS::FBlock((::ULIS::tByte*)psdReader.GetImageDst(),psdReader.GetImageWidth(),psdReader.GetImageHeight(),::ULIS::Format_RGB8);


        ::ULIS::FBlock* myBlock = new ::ULIS::FBlock(psdReader.GetImageWidth(),psdReader.GetImageHeight(),::ULIS::Format_BGRA8);

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ULIS::uint32 MT_bit = ULIS_PERF_MT;
        ::ULIS::uint32 perfIntent = MT_bit | ULIS_PERF_SSE42;

        ::ULIS::Conv(hULIS.ThreadPool(),ULIS_BLOCKING,perfIntent,hULIS.HostDeviceInfo(),ULIS_NOCB,srcblock,myBlock);

        //UE_LOG(LogTemp,Display,TEXT("RGBBlock: %d, RGBABlock: %d"),srcblock->BytesTotal(),myBlock->BytesTotal());

        InitTextureWithBlockData(myBlock,object,ETextureSourceFormat::TSF_BGRA8);
    }
    */

    texture->PostEditChange();
        
    return texture;
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

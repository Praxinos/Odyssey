// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureFactory.h"

 #include "Editor.h"
#include "EditorStyleSet.h"
#include "Engine/Texture2D.h"

#include "ULISLoaderModule.h"
#include "OdysseyBlock.h"
#include "OdysseySurfaceEditable.h"
#include "OdysseyTexture.h"
#include "SOdysseyTextureConfigureWindow.h"

#include <ULIS3>

/////////////////////////////////////////////////////
// UOdysseyTextureFactory
UOdysseyTextureFactory::UOdysseyTextureFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    // From UFactory
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UOdysseyTexture::StaticClass();
}

bool UOdysseyTextureFactory::ConfigureProperties()
{
    //We go in here before creating the texture: Meaning we can have any modal window here.
    //If return false, we don't create the object, if true, we create it
    TSharedPtr<SOdysseyTextureConfigureWindow> textureConfigurationWindow = SNew( SOdysseyTextureConfigureWindow );

    GEditor->EditorAddModalWindow( textureConfigurationWindow.ToSharedRef() );
    mTextureWidth = textureConfigurationWindow->GetWidth();
    mTextureHeight = textureConfigurationWindow->GetHeight();
    mTextureFormat = textureConfigurationWindow->GetFormat();
    mDefaultName = textureConfigurationWindow->GetDefaultName().ToString();
    mBackgroundColor = textureConfigurationWindow->GetBackgroundColor();

    return textureConfigurationWindow->GetWindowAnswer();
}

FString
UOdysseyTextureFactory::GetDefaultNewAssetName() const
{
    return !mDefaultName.IsEmpty() ? mDefaultName : Super::GetDefaultNewAssetName();
}

UObject*
UOdysseyTextureFactory::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    check( iClass->IsChildOf( UOdysseyTexture::StaticClass() ) );

    // Init internal data
    FOdysseyBlock block( mTextureWidth, mTextureHeight, ULISFormatForUE4TextureSourceFormat(mTextureFormat), nullptr, nullptr, true );

    ::ul3::FPixelValue color( ::ul3::FPixelValue::FromRGBAF( mBackgroundColor.R, mBackgroundColor.G, mBackgroundColor.B, mBackgroundColor.A ) );

    //TODO: should fill the default native texture for the thumbnail

    ::ul3::FRect canvasRect = ::ul3::FRect( 0, 0, mTextureWidth, mTextureHeight );
    uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::Fill( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block.GetBlock(), color, canvasRect );
    
    UTexture2D* texture = NewObject<UTexture2D>( iParent, iName, iFlags | RF_Transactional );
    InitTextureWithBlockData(&block, texture, mTextureFormat);
    UOdysseyTextureAssetUserData* userData = NewObject< UOdysseyTextureAssetUserData >(texture, NAME_None, RF_Public);
	::ul3::tFormat format = ULISFormatForUE4TextureSourceFormat(texture->Source.GetFormat());
    userData->GetLayerStack()->Init(mTextureWidth, mTextureHeight, format);

	FName layerName = userData->GetLayerStack()->GetLayerRoot()->GetNextLayerName();
	TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, FVector2D(mTextureWidth, mTextureHeight), userData->GetLayerStack()->Format()));
	userData->GetLayerStack()->AddLayer(imageLayer);
    userData->GetLayerStack()->FillCurrentLayerWithColor( color );
    texture->AddAssetUserData( userData );
    texture->PostEditChange();

    return texture;
}

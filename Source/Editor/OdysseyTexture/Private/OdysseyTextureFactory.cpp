// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureFactory.h"

#include "Editor.h"
#include "EditorStyleSet.h"
#include "Engine/Texture2D.h"

#include "ULISLoaderModule.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "SOdysseyTextureConfigureWindow.h"

#include <ULIS>

/////////////////////////////////////////////////////
// UOdysseyTextureFactory
UOdysseyTextureFactory::UOdysseyTextureFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    // From UFactory
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UTexture2D::StaticClass();
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
    check(iClass->IsChildOf(UTexture2D::StaticClass()));

    // Init internal data
    ::ULIS::FBlock block( mTextureWidth, mTextureHeight, ULISFormatForTextureSourceFormat(mTextureFormat), nullptr, ::ULIS::FOnInvalidBlock(), false );

    ::ULIS::FColor color( ::ULIS::FColor::RGBAF( mBackgroundColor.R, mBackgroundColor.G, mBackgroundColor.B, mBackgroundColor.A ) );

    //TODO: should fill the default native texture for the thumbnail

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block.Format());
    ctx.Fill(block, color );
    ctx.Finish();
    
    UTexture2D* texture = NewObject<UTexture2D>( iParent, iName, iFlags | RF_Transactional );
    InitTextureWithBlockData(&block, texture, mTextureFormat);
    UOdysseyTextureAssetUserData* userData = NewObject< UOdysseyTextureAssetUserData >(texture, NAME_None, RF_Public);
	::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    userData->GetLayerStack()->Init(mTextureWidth, mTextureHeight, format);

	FName layerName = userData->GetLayerStack()->GetLayerRoot()->GetNextLayerName();
	TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, FVector2D(mTextureWidth, mTextureHeight), userData->GetLayerStack()->Format()));
	userData->GetLayerStack()->AddLayer(imageLayer);
    userData->GetLayerStack()->FillCurrentLayerWithColor( color );
    texture->AddAssetUserData( userData );
    texture->PostEditChange();

    return texture;
}

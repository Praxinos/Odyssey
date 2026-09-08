// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureFactory.h"

#include "Editor.h"
#include "Engine/Texture2D.h"

#include "ULISLoaderModule.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTelemetry.h"
#include "Texture/SOdysseyTextureConfigureWindow.h"

#include <ULIS>

UOdysseyTextureFactory::UOdysseyTextureFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    // From UFactory
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UTexture2D::StaticClass();
}

void
UOdysseyTextureFactory::SetConfiguration(const FOdysseyTextureConfiguration& iConfiguration)
{
    mTextureConfiguration = iConfiguration;
}

bool UOdysseyTextureFactory::ConfigureProperties()
{
    //We go in here before creating the texture: Meaning we can have any modal window here.
    //If return false, we don't create the object, if true, we create it
    TSharedPtr<SOdysseyTextureConfigureWindow> textureConfigurationWindow = SNew( SOdysseyTextureConfigureWindow );

    GEditor->EditorAddModalWindow( textureConfigurationWindow.ToSharedRef() );

    mTextureConfiguration = textureConfigurationWindow->GetConfiguration();

    return textureConfigurationWindow->GetWindowAnswer();
}

FString
UOdysseyTextureFactory::GetDefaultNewAssetName() const
{
    return !mTextureConfiguration.Name.ToString().IsEmpty() ? mTextureConfiguration.Name.ToString() : Super::GetDefaultNewAssetName();
}

FName
UOdysseyTextureFactory::GetNewAssetThumbnailOverride() const
{
    return TEXT("ClassThumbnail.OdysseyTexture");
}

UObject*
UOdysseyTextureFactory::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    check(iClass->IsChildOf(UTexture2D::StaticClass()));

    {
        using FAssetAddedFields = FAssetAdded_TelemetryFields;

        TArray<FAnalyticsEventAttribute> Attributes;
        Attributes.Emplace( FAssetAddedFields::AssetClassPath_KeyName_AsString, iClass->GetPathName() );

        FOdysseyTelemetry::Get().RecordEvent( FAssetAddedFields::KeyName, Attributes );
    }

    return mTextureConfiguration.CreateTexture(iParent, iName, iFlags | RF_Transactional);
}

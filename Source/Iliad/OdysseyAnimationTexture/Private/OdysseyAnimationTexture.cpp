// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationTexture.h"
#include "OdysseyAnimationTextureResource.h"
#include "Engine/Texture2D.h"
#include "Misc/TransactionObjectEvent.h"
#include "RHI.h"
#include "TextureCompiler.h"
#include "TextureResource.h"

#include "UObject/OdysseyObjectEditorUtils.h"

void
UOdysseyAnimationTexture::SetPlayer(UOdysseyAnimationPlayer* iPlayer)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationTexture, Player), iPlayer);
}

UOdysseyAnimationPlayer*
UOdysseyAnimationTexture::GetPlayer() const
{
    return Player;
}

FTextureResource*
UOdysseyAnimationTexture::CreateResource()
{
    FOdysseyAnimationTextureResource* resource = new FOdysseyAnimationTextureResource(*this, mDimensions);
    FRHITexture2D* rhi = nullptr;
    if (Player)
    {
        UTexture2D* playerTexture = Player->GetTexture();
        if (playerTexture)
        {
            FTextureCompilingManager::Get().FinishCompilation({playerTexture});
            FTextureResource* playerResource = playerTexture->GetResource();
            if ( playerResource )
                rhi = playerResource->TextureRHI;
        }
    }
    resource->UpdateTextureReference(rhi);
    return resource;
}

EMaterialValueType
UOdysseyAnimationTexture::GetMaterialType() const
{
    return MCT_Texture2D;
}

float
UOdysseyAnimationTexture::GetSurfaceWidth() const
{
    return mDimensions.X;
}

float
UOdysseyAnimationTexture::GetSurfaceHeight() const
{
    return mDimensions.Y;
}

void
UOdysseyAnimationTexture::PlayerChanged()
{
    //TODO: Connect to Player's TextureChanged Event and call UpdateResource() when it happens

    //When the player changes we have to update our resource accordingly
    UpdateResource();
}

void
UOdysseyAnimationTexture::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationTexture, Player) )
        PlayerChanged();
}

void
UOdysseyAnimationTexture::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyAnimationTexture::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
    }
}

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationTexture.h"
/*
#include "OdysseyAnimationTextureResource.h"
#include "Misc/TransactionObjectEvent.h"
#include "TextureCompiler.h"
#include "ImageUtils.h"

#include "UObject/OdysseyObjectEditorUtils.h" */

/**
 * Get the optimal placeholder to use during texture compilation
 */ 
/* static UTexture2D* GetDefaultTexture2D()
{
	static TStrongObjectPtr<UTexture2D> CheckerboardTexture;

	if (!CheckerboardTexture.IsValid())
		CheckerboardTexture.Reset(FImageUtils::CreateCheckerboardTexture(FColor(200, 200, 200, 128), FColor(128, 128, 128, 128)));

	return CheckerboardTexture.Get();
}

FTextureResource*
UOdysseyAnimationTexture::CreateResource()
{
	FOdysseyAnimationTextureResource* resource = new FOdysseyAnimationTextureResource(*this, mDimensions);
	if (!Player)
		return new FTexture2DResource(this, GetDefaultTexture2D()->GetResource()->GetTexture2DResource());

	UTexture2D* playerTexture = Player->GetTexture();
	if (!playerTexture)
		return new FTexture2DResource(this, GetDefaultTexture2D()->GetResource()->GetTexture2DResource());

	FTextureCompilingManager::Get().FinishCompilation({playerTexture});
	FTextureResource* playerResource = playerTexture->GetResource();
	if ( !playerResource )
		return new FTexture2DResource(this, GetDefaultTexture2D()->GetResource()->GetTexture2DResource());
	
	FRHITexture2D* rhi = playerResource->TextureRHI;
	resource->UpdateTextureReference(rhi);
	return resource;
}

void
UOdysseyAnimationTexture::PostLoad()
{
	Super::PostLoad();

	if (Player)
	{
		Player->OnTextureChanged().AddUObject(this, &UOdysseyAnimationTexture::OnPlayerTextureChanged);
		PlayerHandle = Player;
	}
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
	if (PlayerHandle)
	{
		PlayerHandle->OnTextureChanged().RemoveAll(this);
		PlayerHandle = Player;
	}

	if (Player)
		Player->OnTextureChanged().AddUObject(this, &UOdysseyAnimationTexture::OnPlayerTextureChanged);

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

void
UOdysseyAnimationTexture::OnPlayerTextureChanged()
{
	UpdateResource();
} */

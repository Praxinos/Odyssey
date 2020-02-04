// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "EngineUtils.h"

#include "OdysseyTexture.h"


UOdysseyTexture::UOdysseyTexture(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    mLayerStack = new FOdysseyLayerStack();
}


//UObject Interface--------
//-------------------------

void UOdysseyTexture::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
        
    Ar << mLayerStack;
    Ar << mResultTexture;
}

//End UObject Interface----
//-------------------------

//UTexture Interface-------
//-------------------------
float UOdysseyTexture::GetSurfaceWidth() const
{
    return 0.f;
}

float UOdysseyTexture::GetSurfaceHeight() const
{
    return 0.f;
}

FTextureResource* UOdysseyTexture::CreateResource()
{
    return NULL;
}

EMaterialValueType UOdysseyTexture::GetMaterialType() const
{
    return EMaterialValueType::MCT_Texture2D;
}

//End UTexture Interface---
//-------------------------

UTexture2D* UOdysseyTexture::GetResultTexture2D()
{
    return mResultTexture;
}

void UOdysseyTexture::SetResultTexture2D( UTexture2D* iTexture )
{
    mResultTexture = iTexture;
}

FOdysseyLayerStack* UOdysseyTexture::GetLayerStack()
{
    return mLayerStack;
}

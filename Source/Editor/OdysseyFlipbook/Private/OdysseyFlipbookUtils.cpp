// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFlipbookUtils.h"

#include "AssetRegistryModule.h"

#include "PaperFlipbook.h"
#include "PaperImporterSettings.h"
#include "PaperSprite.h"

#include "SOdysseyTextureConfigureWindow.h"
#include "OdysseyTextureAssetUserData.h"
#include "OdysseySurfaceEditable.h"

FOdysseyFlipbookUtils::FOdysseyFlipbookUtils(UPaperFlipbook* iFlipbook)
    : mFlipbook(iFlipbook)
{
}

bool
FOdysseyFlipbookUtils::AddKeyFrame(UTexture2D** oTexture, UPaperSprite** oSprite)
{
    // Displays a modal window asking for Width and Height of the new texture to draw in
	// If validated, it creates a new sprite and a new texture using the same name and path as the flipbook but adding some suffixes
    TSharedPtr<SOdysseyTextureConfigureWindow> textureConfigurationWindow = SNew( SOdysseyTextureConfigureWindow );
    GEditor->EditorAddModalWindow( textureConfigurationWindow.ToSharedRef() );

    //If cancel is clicked, we do nothing
    if(!textureConfigurationWindow->GetWindowAnswer())
        return false;

    int32 width = textureConfigurationWindow->GetWidth();
    int32 height = textureConfigurationWindow->GetHeight();

    UTexture2D* texture = *oTexture = CreateTexture(width, height);
    if (!texture)
        return false;

    UPaperSprite* sprite = *oSprite = CreateSprite();
    if (!sprite)
        return false;

    SetSpriteTexture(sprite, texture);

    FPaperFlipbookKeyFrame keyframe;
	keyframe.Sprite = sprite;
	keyframe.FrameRun = 1;
	
	{
		FScopedFlipbookMutator mutator(mFlipbook);
		mutator.KeyFrames.Add(keyframe);
	}

	return true;
}

bool
FOdysseyFlipbookUtils::FixKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite)
{
    if (iIndex < 0 || iIndex >= mFlipbook->GetNumKeyFrames())
        return false;

	UTexture2D* texture = GetKeyframeTexture(iIndex);
	if (texture)
		return false;

    // Displays a modal window asking for Width and Height of the new texture to draw in
	// If validated, it creates a new sprite and a new texture using the same name and path as the flipbook but adding some suffixes
    TSharedPtr<SOdysseyTextureConfigureWindow> textureConfigurationWindow = SNew( SOdysseyTextureConfigureWindow );
    GEditor->EditorAddModalWindow( textureConfigurationWindow.ToSharedRef() );

	//If cancel is clicked, we do nothing
    if(!textureConfigurationWindow->GetWindowAnswer())
        return false;

    int32 width = textureConfigurationWindow->GetWidth();
    int32 height = textureConfigurationWindow->GetHeight();

	*oTexture = texture = CreateTexture(width, height);
	if (!texture)
		return false;

    UPaperSprite* sprite = GetKeyframeSprite(iIndex);
	if (!sprite)
	{
        *oSprite = sprite = CreateSprite();
		if (!sprite)
			return false;
	}
    
    SetSpriteTexture(sprite, texture);
    
	{
		FScopedFlipbookMutator mutator(mFlipbook);
		mutator.KeyFrames[iIndex].Sprite = sprite;
	}
	return true;
}

void
FOdysseyFlipbookUtils::MoveKeyFrames(TArray<int32> iSrcIndexes, int32 iDstIndex)
{
    FScopedFlipbookMutator mutator(mFlipbook);

    //store keyframes in the given order
    int32 fixedDestIndex = iDstIndex;
    TArray<FPaperFlipbookKeyFrame> keyframes;
    for( int i = 0; i < iSrcIndexes.Num(); i++)
    {
        keyframes.Add(mutator.KeyFrames[iSrcIndexes[i]]);
        if (iSrcIndexes[i] < iDstIndex)
        {
            fixedDestIndex--;
        }
    }

    //remove given keyframes
    iSrcIndexes.Sort();
    for( int i = iSrcIndexes.Num() - 1; i >= 0; i--)
    {
        mutator.KeyFrames.RemoveAt(iSrcIndexes[i]);
    }
        
    //insert keyframes at their new place
    mutator.KeyFrames.Insert(keyframes, fixedDestIndex);
}

//Duplication
bool
FOdysseyFlipbookUtils::DuplicateKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite)
{
    UTexture2D* texture = GetKeyframeTexture(iIndex);
    if(!texture)
        return false;

    int32 width = texture->GetSizeX();
    int32 height = texture->GetSizeY();

    UOdysseyTextureAssetUserData* textureUserData = Cast<UOdysseyTextureAssetUserData>(texture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
    FOdysseyBlock* block = NULL;
    if( textureUserData )
    {
        textureUserData->GetLayerStack()->ComputeResultBlock();
        block = textureUserData->GetLayerStack()->GetResultBlock();
        *oTexture = CreateTexture(width, height, block);
    }
    else
    {
        block = NewOdysseyBlockFromUTextureData(texture);
        *oTexture = CreateTexture(width, height, block);    
        delete block;
    }
    
    if (!*oTexture)
        return false;

    *oSprite = CreateSprite();
    if (!*oSprite)
        return false;

    SetSpriteTexture(*oSprite, *oTexture);

    FPaperFlipbookKeyFrame keyframe;
	keyframe.Sprite = *oSprite;
	keyframe.FrameRun = 1;
	
	{
		FScopedFlipbookMutator mutator(mFlipbook);
		mutator.KeyFrames.Insert(keyframe, iIndex + 1);
	}

	return true;
}

//Deletion
void
FOdysseyFlipbookUtils::RemoveKeyFrame(int32 iIndex)
{
    FPaperFlipbookKeyFrame keyframe = mFlipbook->GetKeyFrameChecked(iIndex);

	//Remove from Data
	{
		FScopedFlipbookMutator mutator(mFlipbook);
		mutator.KeyFrames.RemoveAt(iIndex);
	}
}

UTexture2D*
FOdysseyFlipbookUtils::CreateTexture(int32 iWidth, int32 iHeight, FOdysseyBlock* iBlock)
{
    FOdysseyBlock* blockPtr = iBlock;
    if (!iBlock)
        blockPtr = new FOdysseyBlock( iWidth, iHeight, ETextureSourceFormat::TSF_BGRA8, nullptr, nullptr, true );

    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    FString PackageName = FPaths::GetPath( mFlipbook->GetPathName() ) + "/";
    FString AssetName = mFlipbook->GetName() + "_Texture";
    AssetTools.CreateUniqueAssetName(PackageName,AssetName,PackageName,AssetName);

    UPackage* package = CreatePackage( nullptr, *PackageName );
    
    //Create texture
    UTexture2D* texture = NewObject<UTexture2D>(package, FName(AssetName), RF_Public | RF_Standalone | RF_Transactional );
    
    //Set texture format
    texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    texture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;

    // Init Texture and its layerstack with iBlock
    InitTextureWithBlockData(blockPtr, texture);
    UOdysseyTextureAssetUserData* userData = NewObject< UOdysseyTextureAssetUserData >(texture, NAME_None, RF_Public);
    userData->GetLayerStack()->InitFromData(blockPtr);
    texture->AddAssetUserData( userData );

    //Init is done
    texture->PostEditChange();
	texture->UpdateResource();

	FAssetRegistryModule::AssetCreated(texture);
	UPackage::SavePackage(package, texture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *AssetName);

	package->MarkAsFullyLoaded();
	texture->MarkPackageDirty();

    if (!iBlock)
        delete blockPtr;
        
    return texture;
}

UPaperSprite*
FOdysseyFlipbookUtils::CreateSprite()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    FString PackageName = FPaths::GetPath( mFlipbook->GetPathName() ) + "/";
    FString AssetName = mFlipbook->GetName() + "_Sprite";
    AssetTools.CreateUniqueAssetName(PackageName,AssetName,PackageName,AssetName);

    UPackage* package = CreatePackage( nullptr, *PackageName );
    
    UPaperSprite* sprite = NewObject<UPaperSprite>(package, FName(AssetName), RF_Public | RF_Standalone | RF_Transactional );

    //Set the correct Render Geometry Type
    UClass* spriteClass = sprite->StaticClass();

	FStructProperty* renderGeometryProperty = FindFProperty<FStructProperty>(spriteClass,"RenderGeometry");
    if (!renderGeometryProperty)
        return NULL;

	FByteProperty* geometryType = FindFProperty<FByteProperty>(renderGeometryProperty->Struct, "GeometryType");
    if (!geometryType)
        return NULL;

	geometryType->SetPropertyValue_InContainer(renderGeometryProperty->ContainerPtrToValuePtr<FSpriteGeometryCollection>(sprite), ESpritePolygonMode::SourceBoundingBox);

    //Init sprite
	FSpriteAssetInitParameters spriteInitParams;

	const UPaperImporterSettings* importerSettings = GetDefault<UPaperImporterSettings>();
	importerSettings->ApplySettingsForSpriteInit(spriteInitParams, ESpriteInitMaterialLightingMode::Automatic);
	sprite->InitializeSprite(spriteInitParams);

    //Finalize asset creation
	FAssetRegistryModule::AssetCreated(sprite);
	UPackage::SavePackage(package, sprite, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *AssetName);

	package->MarkAsFullyLoaded();
	sprite->MarkPackageDirty();

    return sprite;
}

void
FOdysseyFlipbookUtils::SetKeyframeTexture(int32 iIndex, UTexture2D* iTexture)
{
    UPaperSprite* sprite = GetKeyframeSprite(iIndex);
    if (!sprite)
        return;

    SetSpriteTexture(sprite, iTexture);

    {
		FScopedFlipbookMutator mutator(mFlipbook);
		mutator.KeyFrames[iIndex].Sprite = sprite;
	}
}

void
FOdysseyFlipbookUtils::SetKeyframeSprite(int32 iIndex, UPaperSprite* iSprite)
{
    FScopedFlipbookMutator mutator(mFlipbook);
    mutator.KeyFrames[iIndex].Sprite = iSprite;
}

void
FOdysseyFlipbookUtils::SetSpriteTexture(UPaperSprite* iSprite, UTexture2D* iTexture)
{
    //Set the texture in the existing sprite
	UClass* spriteClass = iSprite->StaticClass();
    FSoftObjectProperty* sourceTextureProperty = FindFProperty<FSoftObjectProperty>(spriteClass, "SourceTexture");
    sourceTextureProperty->SetObjectPropertyValue(sourceTextureProperty->ContainerPtrToValuePtr<UPaperSprite>(iSprite), iTexture);
}

UTexture2D*
FOdysseyFlipbookUtils::GetKeyframeTexture(int32 iIndex)
{
	const UPaperSprite* sprite = GetKeyframeSprite(iIndex);
	if (!sprite)
		return NULL;

	return sprite->GetSourceTexture();
}

UPaperSprite*
FOdysseyFlipbookUtils::GetKeyframeSprite(int32 iIndex)
{
    if (iIndex < 0 || iIndex >= mFlipbook->GetNumKeyFrames())
		return NULL;

	const FPaperFlipbookKeyFrame& keyFrame = mFlipbook->GetKeyFrameChecked(iIndex);
	return keyFrame.Sprite;
}

int32
FOdysseyFlipbookUtils::GetKeyframeIndexAtPosition(float iFramePosition)
{
    if (iFramePosition < 0 || iFramePosition >= mFlipbook->GetNumFrames())
    {
        return -1;
    }

    int32 position = 0;

    for (int32 i = 0; i < mFlipbook->GetNumKeyFrames(); i++)
    {
        position += mFlipbook->GetKeyFrameChecked(i).FrameRun;
        
        if (position > iFramePosition)
            return i;
    }

    return -1;
}

float
FOdysseyFlipbookUtils::GetKeyframeStartPosition(int32 iIndex)
{
    if (iIndex < 0 || iIndex >= mFlipbook->GetNumKeyFrames())
        return -1;

    int32 position = 0; //int32 to avoid float imprecision in the for loop

    for (int32 i = 0; i < iIndex; i++)
    {
        position += mFlipbook->GetKeyFrameChecked(i).FrameRun;
    }

    return position;
}
// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookWrapper.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "PaperFlipbook.h"
#include "PaperImporterSettings.h"
#include "PaperSprite.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyPixelFormat.h"
#include "ULISLoaderModule.h"
#include "TextureCompiler.h"
#include "UObject/SavePackage.h"
#include "UObject/OdysseyObjectEditorUtils.h"


FOdysseyFlipbookWrapper::~FOdysseyFlipbookWrapper()
{
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.Remove(mOnPrePropertyChangedDelegateHandle);
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(mOnPropertyChangedDelegateHandle);
}

FOdysseyFlipbookWrapper::FOdysseyFlipbookWrapper()
    : mFlipbook(nullptr)
    , mSpritePreviousTexture(NULL)
{
    mOnPrePropertyChangedDelegateHandle = FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw(this, &FOdysseyFlipbookWrapper::OnPreGlobalObjectPropertyChanged);
    mOnPropertyChangedDelegateHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FOdysseyFlipbookWrapper::OnGlobalObjectPropertyChanged);
}

void
FOdysseyFlipbookWrapper::CreateEmptyKeyFrame(int32 iIndex)
{
    FPaperFlipbookKeyFrame keyframe;
	FScopedFlipbookMutator mutator(mFlipbook);
	mutator.KeyFrames.Insert(keyframe, iIndex);
    
	mFlipbook->MarkPackageDirty();

    //TODO: Do it by changing UProperties
}

UPaperFlipbook*
FOdysseyFlipbookWrapper::GetFlipbook() const
{
    return mFlipbook;
}

void
FOdysseyFlipbookWrapper::SetFlipbook(UPaperFlipbook* iFlipbook)
{
    mFlipbook = iFlipbook;
    mOnFlipbookChanged.Broadcast();
}

bool
FOdysseyFlipbookWrapper::CreateKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite)
{
    // Displays a modal window asking for Width and Height of the new texture to draw in
	// If validated, it creates a new sprite and a new texture using the same name and path as the flipbook but adding some suffixes
    TSharedPtr<SOdysseyTextureConfigureWindow> textureConfigurationWindow = SNew( SOdysseyTextureConfigureWindow, mTextureConfiguration);
    GEditor->EditorAddModalWindow( textureConfigurationWindow.ToSharedRef() );

    //If cancel is clicked, we do nothing
    if(!textureConfigurationWindow->GetWindowAnswer())
        return false;

    mTextureConfiguration = textureConfigurationWindow->GetConfiguration();
    
    FOdysseyTextureConfiguration textureConfiguration = mTextureConfiguration;
    FString textureName = textureConfiguration.Name.ToString() + TEXT("_Texture");
    FString spriteName = textureConfiguration.Name.ToString() + TEXT("_Sprite");
    textureConfiguration.Name = FName(*textureName);
    
    //Create the keyframe
    CreateEmptyKeyFrame(iIndex);

    //Create the sprite and add it to the keyframe
    UPaperSprite* sprite = *oSprite = CreateSprite(spriteName);
    if (!sprite)
        return false;

    //Create the texture and add it to the keyframe
    UTexture2D* texture = *oTexture = CreateTexture(textureConfiguration);
    if (!texture)
        return false;

    SetSpriteTexture(sprite, texture); //Finishes the sprite initialization before giving it to the flipbook, otherwise it calls some unwanted callbacks in the GUI
    SetKeyframeSprite(iIndex, sprite);
	return true;
}

//Duplication
bool
FOdysseyFlipbookWrapper::DuplicateKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite)
{
    if (iIndex < 0 || iIndex >= mFlipbook->GetNumKeyFrames())
        return false;

    const FPaperFlipbookKeyFrame& keyframe = mFlipbook->GetKeyFrameChecked(iIndex);

    //Create the keyframe
    CreateEmptyKeyFrame(iIndex + 1);
    SetKeyFrameLength(iIndex + 1, keyframe.FrameRun);

    //Create the sprite and add it to the keyframe
    UPaperSprite* sprite = GetKeyframeSprite(iIndex);
    if (sprite)
    {
        sprite = *oSprite = CreateSprite(mFlipbook->GetName() + "_Sprite");
        if (!sprite)
            return false;

        //Create the texture and add it to the keyframe
        UTexture2D* srcTexture = GetKeyframeTexture(iIndex);
        if (sprite && srcTexture)
        {
            FOdysseyTextureConfiguration textureConfiguration;
            textureConfiguration.Width = srcTexture->Source.GetSizeX();
            textureConfiguration.Height = srcTexture->Source.GetSizeX();
            textureConfiguration.Format = EOdysseyTextureSourceFormat::kCustom;
            textureConfiguration.CustomFormat = srcTexture->Source.GetFormat();
            FString textureName = mFlipbook->GetName() + TEXT("_Texture");
            textureConfiguration.Name = FName(*textureName);

            UTexture2D* texture = *oTexture = CreateTexture(textureConfiguration);
            if (!texture)
                return false;

            UOdysseyTextureLayerStackUserData* userData = NewObject<UOdysseyTextureLayerStackUserData>(texture, NAME_None, RF_Public);

            UOdysseyTextureLayerStackUserData* srcTextureUserData = Cast<UOdysseyTextureLayerStackUserData>(srcTexture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
            if ( srcTextureUserData )
            {
                userData->InitWithDuplicateLayerStack(srcTextureUserData->GetLayerStack());
            }
            else
            {
                userData->InitWithDefaultLayerStack();
            }
            
            //CopyTextureContent(srcTexture, texture);

            SetSpriteTexture(sprite, texture);  //Finishes the sprite initialization before giving it to the flipbook, otherwise it calls some unwanted callbacks in the GUI
        }

        SetKeyframeSprite(iIndex + 1, sprite);
    }

	return true;
}

bool
FOdysseyFlipbookWrapper::FixKeyFrame(int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite)
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

    FOdysseyTextureConfiguration textureConfiguration = textureConfigurationWindow->GetConfiguration();
    FString textureName = textureConfiguration.Name.ToString() + TEXT("_Texture");
    FString spriteName = textureConfiguration.Name.ToString() + TEXT("_Sprite");
    textureConfiguration.Name = FName(*textureName);

    UPaperSprite* sprite = GetKeyframeSprite(iIndex);
	if (!sprite)
	{
        sprite = *oSprite = CreateSprite(spriteName);
		if (!sprite)
			return false;        
	}

	texture = *oTexture = CreateTexture(textureConfiguration);
	if (!texture)
		return false;

    SetSpriteTexture(sprite, texture); //Finishes the sprite initialization before giving it to the flipbook, otherwise it calls some unwanted callbacks in the GUI
    SetKeyframeSprite(iIndex, sprite);

	return true;
}

void
FOdysseyFlipbookWrapper::MoveKeyFrames(TArray<int32> iSrcIndexes, int32 iDstIndex)
{
    //TODO: Do it by changing UProperties
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
    
	mFlipbook->MarkPackageDirty();
}

//Deletion
void
FOdysseyFlipbookWrapper::RemoveKeyFrame(int32 iIndex)
{
    //TODO: Do it with UProperties
    FPaperFlipbookKeyFrame keyframe = mFlipbook->GetKeyFrameChecked(iIndex);

	//Remove from Data
	{
		FScopedFlipbookMutator mutator(mFlipbook);
		mutator.KeyFrames.RemoveAt(iIndex);
	}
    
	mFlipbook->MarkPackageDirty();
}

void
FOdysseyFlipbookWrapper::SetKeyFrameLength(int32 iIndex, int32 iLength)
{
    FScopedFlipbookMutator mutator(mFlipbook);
    mutator.KeyFrames[iIndex].FrameRun = iLength;
    
	mFlipbook->MarkPackageDirty();
}

UTexture2D*
FOdysseyFlipbookWrapper::CreateTexture(FOdysseyTextureConfiguration iTextureConfiguration)
{
	//Create Asset for Texture
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    FString PackageName = FPaths::GetPath( mFlipbook->GetPathName() ) + "/";
    FString AssetName = iTextureConfiguration.Name.ToString();
    AssetTools.CreateUniqueAssetName(PackageName,AssetName,PackageName,AssetName);
    UPackage* package = CreatePackage( *PackageName );

    UTexture2D* texture2D = iTextureConfiguration.CreateTexture(package, FName(AssetName), RF_Public | RF_Standalone | RF_Transactional);

	FAssetRegistryModule::AssetCreated(texture2D);
    
    FSavePackageArgs packageArgs;
    packageArgs.SaveFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
    UPackage::SavePackage( package, texture2D, *AssetName, packageArgs );

	package->MarkAsFullyLoaded();
	texture2D->MarkPackageDirty();

    return texture2D;
}

UPaperSprite*
FOdysseyFlipbookWrapper::CreateSprite(FString iName)
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    FString PackageName = FPaths::GetPath( mFlipbook->GetPathName() ) + "/";
    FString AssetName = iName;
    AssetTools.CreateUniqueAssetName(PackageName,AssetName,PackageName,AssetName);

    UPackage* package = CreatePackage( *PackageName );
    
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
    
    FSavePackageArgs packageArgs;
    packageArgs.SaveFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
    UPackage::SavePackage( package, sprite, *AssetName, packageArgs );

	package->MarkAsFullyLoaded();
	sprite->MarkPackageDirty();

    return sprite;
}

void
FOdysseyFlipbookWrapper::SetKeyframeSprite(int32 iIndex, UPaperSprite* iSprite)
{
    //TODO: Do It With UProperties
    FScopedFlipbookMutator mutator(mFlipbook);
    mutator.KeyFrames[iIndex].Sprite = iSprite;
    
	mFlipbook->MarkPackageDirty();
}

void
FOdysseyFlipbookWrapper::SetKeyframeTexture(int32 iIndex, UTexture2D* iTexture)
{
    UPaperSprite* sprite = GetKeyframeSprite(iIndex);
    if (!sprite)
        return;

    SetSpriteTexture(sprite, iTexture);
}

void
FOdysseyFlipbookWrapper::SetSpriteTexture(UPaperSprite* iSprite, UTexture2D* iTexture)
{
    //Set the texture in the existing sprite
    UClass* spriteClass = iSprite->StaticClass();
    FSoftObjectProperty* sourceTextureProperty = FindFProperty<FSoftObjectProperty>(spriteClass, "SourceTexture");
    sourceTextureProperty->SetObjectPropertyValue(sourceTextureProperty->ContainerPtrToValuePtr<UPaperSprite>(iSprite), iTexture);

    FPropertyChangedEvent event(sourceTextureProperty, EPropertyChangeType::ValueSet);
    iSprite->PostEditChangeProperty(event);
    iSprite->MarkPackageDirty();
}

UTexture2D*
FOdysseyFlipbookWrapper::GetKeyframeTexture(int32 iIndex)
{
	const UPaperSprite* sprite = GetKeyframeSprite(iIndex);
	if (!sprite)
		return NULL;

	return sprite->GetSourceTexture();
}

UPaperSprite*
FOdysseyFlipbookWrapper::GetKeyframeSprite(int32 iIndex)
{
    if (iIndex < 0 || iIndex >= mFlipbook->GetNumKeyFrames())
		return NULL;

	const FPaperFlipbookKeyFrame& keyFrame = mFlipbook->GetKeyFrameChecked(iIndex);
	return keyFrame.Sprite;
}

int32
FOdysseyFlipbookWrapper::GetKeyframeIndexAtPosition(float iFramePosition)
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
FOdysseyFlipbookWrapper::GetKeyframeStartPosition(int32 iIndex)
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


void
FOdysseyFlipbookWrapper::ShowKeyFrameSpriteInContentBrowser(int32 iIndex)
{
    UPaperSprite* sprite = GetKeyframeSprite(iIndex);
    if (!sprite)
        return;

    TArray<UObject*> ObjectsToSync;
    ObjectsToSync.Add(sprite);
    GEditor->SyncBrowserToObjects(ObjectsToSync);
}

void
FOdysseyFlipbookWrapper::OpenKeyFrameSpriteEditor(int32 iIndex)
{
    UPaperSprite* sprite = GetKeyframeSprite(iIndex);
    if (!sprite)
        return;

	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(sprite);
}

FOnSpriteTextureChanged&
FOdysseyFlipbookWrapper::OnSpriteTextureChanged()
{
    return mOnSpriteTextureChanged;
}

FSimpleMulticastDelegate&
FOdysseyFlipbookWrapper::OnFlipbookChanged()
{
    return mOnFlipbookChanged;
}

void
FOdysseyFlipbookWrapper::OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain)
{
	if (UPaperSprite* sprite = Cast<UPaperSprite>(iObject))
    {
        OnPreSpriteTextureChanged(sprite, iEditPropertyChain);
    }
}

void
FOdysseyFlipbookWrapper::OnGlobalObjectPropertyChanged(UObject* iObject, FPropertyChangedEvent& iPropertyChangedEvent)
{
    if (UPaperSprite* sprite = Cast<UPaperSprite>(iObject))
    {
        OnSpriteTextureChanged(sprite, iPropertyChangedEvent);
    }
}
/*
void
FOdysseyFlipbookWrapper::OnPreFlipbookPropertyChanged(UPaperFlipbook* iFlipbook, const FEditPropertyChain& iEditPropertyChain)
{
    if (iFlipbook != mFlipbook)
        return;

    FName propertyName = iEditPropertyChain.GetActiveNode().GetValue();
    if (propertyName == "KeyFrames")
    {
        switch(iEditPropertyChain.ChangeType)
        {
            case EPropertyChangeType::Unspecified:
                //TODO: try to see if there was a change
                break;

            case EPropertyChangeType::ArrayAdd:
                break;

            case EPropertyChangeType::ArrayRemove:
                int32 index = iEditPropertyChain.GetArrayIndex(propertyName.ToString());
                mOnKeyFrameRemoved.Broadcast(index, mFlipbook->GetKeyFrameChecked(index));
                break;

            case EPropertyChangeType::ArrayClear:
                break;
                
            case EPropertyChangeType::ValueSet:
                break;

            case EPropertyChangeType::Duplicate:
                break;

            case EPropertyChangeType::Interactive:
                break;
            
            case EPropertyChangeType::Redirected:
                break;
        }
    }
} */

void
FOdysseyFlipbookWrapper::OnPreSpriteTextureChanged(UPaperSprite* iSprite, const FEditPropertyChain& iEditPropertyChain)
{
    if (!mFlipbook->ContainsSprite(iSprite))
        return;

    FProperty* property = iEditPropertyChain.GetActiveNode()->GetValue();
    if (property->GetFName() == "SourceTexture")
    {
		FSoftObjectProperty* textureProperty = CastField<FSoftObjectProperty>(property);
        mSpritePreviousTexture = Cast<UTexture2D>(textureProperty->GetObjectPropertyValue(textureProperty->ContainerPtrToValuePtr<UPaperSprite>(iSprite)));
    }
}

/*
void
FOdysseyFlipbookWrapper::OnFlipbookPropertyChanged(UPaperFlipbook* iFlipbook, FPropertyChangedEvent& iPropertyChangedEvent)
{
    if (iFlipbook != mFlipbook)
        return;

    FName propertyName = iPropertyChangedEvent.GetPropertyName();
    if (propertyName == "KeyFrames")
    {
        switch(iPropertyChangedEvent.ChangeType)
        {
            case EPropertyChangeType::Unspecified:
                //TODO: try to see if there was a change
                break;

            case EPropertyChangeType::ArrayAdd:
            {
                int32 index = iPropertyChangedEvent.GetArrayIndex(propertyName.ToString());
                mOnKeyFrameAdded.Broadcast(index);
            }
            break;

            case EPropertyChangeType::ArrayRemove:
                break;

            case EPropertyChangeType::ArrayClear:
                break;
                
            case EPropertyChangeType::ValueSet:
                break;

            case EPropertyChangeType::Duplicate:
                break;

            case EPropertyChangeType::Interactive:
                break;
            
            case EPropertyChangeType::Redirected:
                break;
        }
    }
} */

void
FOdysseyFlipbookWrapper::OnSpriteTextureChanged(UPaperSprite* iSprite, FPropertyChangedEvent& iPropertyChangedEvent)
{
    if (!mFlipbook->ContainsSprite(iSprite))
        return;

    FName propertyName = iPropertyChangedEvent.GetPropertyName();
    if (propertyName == "SourceTexture")
    {
        mOnSpriteTextureChanged.Broadcast(iSprite, mSpritePreviousTexture);
    }
}

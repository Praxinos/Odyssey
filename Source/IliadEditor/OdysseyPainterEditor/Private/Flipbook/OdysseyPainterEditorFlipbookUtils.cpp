// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorFlipbookUtils.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "PaperFlipbook.h"
#include "PaperImporterSettings.h"
#include "PaperSprite.h"
#include "UObject/SavePackage.h"

#include "OdysseyTextureLayerStackUserData.h"

namespace OdysseyPainterEditorFlipbookUtils
{

void
CreateEmptyKeyFrame(UPaperFlipbook* iFlipbook, int32 iIndex)
{
    FPaperFlipbookKeyFrame keyframe;
    FScopedFlipbookMutator mutator(iFlipbook);
    mutator.KeyFrames.Insert(keyframe, iIndex);

    iFlipbook->MarkPackageDirty();
}

bool
CreateKeyFrame(UPaperFlipbook* iFlipbook, int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite, FOdysseyTextureConfiguration& ioTextureConfiguration)
{
    // Displays a modal window asking for Width and Height of the new texture to draw in
    // If validated, it creates a new sprite and a new texture using the same name and path as the flipbook but adding some suffixes
    TSharedPtr<SOdysseyTextureConfigureWindow> textureConfigurationWindow = SNew( SOdysseyTextureConfigureWindow, ioTextureConfiguration);
    GEditor->EditorAddModalWindow( textureConfigurationWindow.ToSharedRef() );

    //If cancel is clicked, we do nothing
    if(!textureConfigurationWindow->GetWindowAnswer())
        return false;

    // save all settings for future KeyFrame creations
    ioTextureConfiguration = textureConfigurationWindow->GetConfiguration();

    FOdysseyTextureConfiguration textureConfiguration = ioTextureConfiguration;
    FString textureName = textureConfiguration.Name.ToString() + TEXT("_Texture");
    FString spriteName = textureConfiguration.Name.ToString() + TEXT("_Sprite");
    textureConfiguration.Name = FName(*textureName);

    //Create the keyframe
    CreateEmptyKeyFrame(iFlipbook, iIndex);

    //Create the sprite and add it to the keyframe
    UPaperSprite* sprite = *oSprite = CreateSprite(iFlipbook, spriteName);
    if (!sprite)
        return false;

    //Create the texture and add it to the keyframe
    UTexture2D* texture = *oTexture = CreateTexture(iFlipbook, textureConfiguration);
    if (!texture)
        return false;

    SetSpriteTexture(sprite, texture); //Finishes the sprite initialization before giving it to the flipbook, otherwise it calls some unwanted callbacks in the GUI
    SetKeyframeSprite(iFlipbook, iIndex, sprite);
    return true;
}

//Duplication
bool
DuplicateKeyFrame(UPaperFlipbook* iFlipbook, int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite)
{
    if (iIndex < 0 || iIndex >= iFlipbook->GetNumKeyFrames())
        return false;

    const FPaperFlipbookKeyFrame& keyframe = iFlipbook->GetKeyFrameChecked(iIndex);

    //Create the keyframe
    CreateEmptyKeyFrame(iFlipbook, iIndex + 1);
    SetKeyFrameLength(iFlipbook, iIndex + 1, keyframe.FrameRun);

    //Create the sprite and add it to the keyframe
    UPaperSprite* sprite = GetKeyframeSprite(iFlipbook, iIndex);
    if (sprite)
    {
        sprite = *oSprite = CreateSprite(iFlipbook, iFlipbook->GetName() + "_Sprite");
        if (!sprite)
            return false;

        //Create the texture and add it to the keyframe
        UTexture2D* srcTexture = GetKeyframeTexture(iFlipbook, iIndex);
        if (sprite && srcTexture)
        {
            FOdysseyTextureConfiguration textureConfiguration;
            textureConfiguration.Width = srcTexture->Source.GetSizeX();
            textureConfiguration.Height = srcTexture->Source.GetSizeX();
            textureConfiguration.Format = EOdysseyTextureSourceFormat::kCustom;
            textureConfiguration.CustomFormat = srcTexture->Source.GetFormat();
            FString textureName = iFlipbook->GetName() + TEXT("_Texture");
            textureConfiguration.Name = FName(*textureName);

            UTexture2D* texture = *oTexture = CreateTexture(iFlipbook, textureConfiguration);
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

        SetKeyframeSprite(iFlipbook, iIndex + 1, sprite);
    }

    return true;
}

bool
FixKeyFrame(UPaperFlipbook* iFlipbook, int32 iIndex, UTexture2D** oTexture, UPaperSprite** oSprite, FOdysseyTextureConfiguration& ioTextureConfiguration)
{
    if (iIndex < 0 || iIndex >= iFlipbook->GetNumKeyFrames())
        return false;

    UTexture2D* texture = GetKeyframeTexture(iFlipbook, iIndex);
    if (texture)
        return false;

    // Displays a modal window asking for Width and Height of the new texture to draw in
    // If validated, it creates a new sprite and a new texture using the same name and path as the flipbook but adding some suffixes
    TSharedPtr<SOdysseyTextureConfigureWindow> textureConfigurationWindow = SNew( SOdysseyTextureConfigureWindow, ioTextureConfiguration );
    GEditor->EditorAddModalWindow( textureConfigurationWindow.ToSharedRef() );

    //If cancel is clicked, we do nothing
    if(!textureConfigurationWindow->GetWindowAnswer())
        return false;

    FOdysseyTextureConfiguration textureConfiguration = textureConfigurationWindow->GetConfiguration();
    FString textureName = textureConfiguration.Name.ToString() + TEXT("_Texture");
    FString spriteName = textureConfiguration.Name.ToString() + TEXT("_Sprite");
    textureConfiguration.Name = FName(*textureName);

    UPaperSprite* sprite = GetKeyframeSprite(iFlipbook, iIndex);
    if (!sprite)
    {
        sprite = *oSprite = CreateSprite(iFlipbook, spriteName);
        if (!sprite)
            return false;
    }

    texture = *oTexture = CreateTexture(iFlipbook, textureConfiguration);
    if (!texture)
        return false;

    SetSpriteTexture(sprite, texture); //Finishes the sprite initialization before giving it to the flipbook, otherwise it calls some unwanted callbacks in the GUI
    SetKeyframeSprite(iFlipbook, iIndex, sprite);

    return true;
}

void
MoveKeyFrames(UPaperFlipbook* iFlipbook, TArray<int32> iSrcIndexes, int32 iDstIndex)
{
    FScopedFlipbookMutator mutator(iFlipbook);

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

    iFlipbook->MarkPackageDirty();
}

//Deletion
void
RemoveKeyFrame(UPaperFlipbook* iFlipbook, int32 iIndex)
{
    FPaperFlipbookKeyFrame keyframe = iFlipbook->GetKeyFrameChecked(iIndex);

    //Remove from Data
    {
        FScopedFlipbookMutator mutator(iFlipbook);
        mutator.KeyFrames.RemoveAt(iIndex);
    }

    iFlipbook->MarkPackageDirty();
}

void
SetKeyFrameLength(UPaperFlipbook* iFlipbook, int32 iIndex, int32 iLength)
{
    FScopedFlipbookMutator mutator(iFlipbook);
    mutator.KeyFrames[iIndex].FrameRun = iLength;

    iFlipbook->MarkPackageDirty();
}

UTexture2D*
CreateTexture(UPaperFlipbook* iFlipbook, FOdysseyTextureConfiguration iTextureConfiguration)
{
    //Create Asset for Texture
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    FString PackageName = FPaths::GetPath( iFlipbook->GetPathName() ) + "/";
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
CreateSprite(UPaperFlipbook* iFlipbook, FString iName)
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    FString PackageName = FPaths::GetPath( iFlipbook->GetPathName() ) + "/";
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
SetKeyframeSprite(UPaperFlipbook* iFlipbook, int32 iIndex, UPaperSprite* iSprite)
{
    FScopedFlipbookMutator mutator(iFlipbook);
    mutator.KeyFrames[iIndex].Sprite = iSprite;

    iFlipbook->MarkPackageDirty();
}

void
SetKeyframeTexture(UPaperFlipbook* iFlipbook, int32 iIndex, UTexture2D* iTexture)
{
    UPaperSprite* sprite = GetKeyframeSprite(iFlipbook, iIndex);
    if (!sprite)
        return;

    SetSpriteTexture(sprite, iTexture);
}

void
SetSpriteTexture(UPaperSprite* iSprite, UTexture2D* iTexture)
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
GetKeyframeTexture(UPaperFlipbook* iFlipbook, int32 iIndex)
{
    const UPaperSprite* sprite = GetKeyframeSprite(iFlipbook, iIndex);
    if (!sprite)
        return NULL;

    return sprite->GetSourceTexture();
}

UPaperSprite*
GetKeyframeSprite(UPaperFlipbook* iFlipbook, int32 iIndex)
{
    if (iIndex < 0 || iIndex >= iFlipbook->GetNumKeyFrames())
        return NULL;

    const FPaperFlipbookKeyFrame& keyFrame = iFlipbook->GetKeyFrameChecked(iIndex);
    return keyFrame.Sprite;
}

int32
GetKeyframeIndexAtPosition(UPaperFlipbook* iFlipbook, float iFramePosition)
{
    if (iFramePosition < 0 || iFramePosition >= iFlipbook->GetNumFrames())
    {
        return -1;
    }

    int32 position = 0;

    for (int32 i = 0; i < iFlipbook->GetNumKeyFrames(); i++)
    {
        position += iFlipbook->GetKeyFrameChecked(i).FrameRun;

        if (position > iFramePosition)
            return i;
    }

    return -1;
}

float
GetKeyframeStartPosition(UPaperFlipbook* iFlipbook, int32 iIndex)
{
    if (iIndex < 0 || iIndex >= iFlipbook->GetNumKeyFrames())
        return -1;

    int32 position = 0; //int32 to avoid float imprecision in the for loop

    for (int32 i = 0; i < iIndex; i++)
    {
        position += iFlipbook->GetKeyFrameChecked(i).FrameRun;
    }

    return position;
}


void
ShowKeyFrameSpriteInContentBrowser(UPaperFlipbook* iFlipbook, int32 iIndex)
{
    UPaperSprite* sprite = GetKeyframeSprite(iFlipbook, iIndex);
    if (!sprite)
        return;

    TArray<UObject*> ObjectsToSync;
    ObjectsToSync.Add(sprite);
    GEditor->SyncBrowserToObjects(ObjectsToSync);
}

void
OpenKeyFrameSpriteEditor(UPaperFlipbook* iFlipbook, int32 iIndex)
{
    UPaperSprite* sprite = GetKeyframeSprite(iFlipbook, iIndex);
    if (!sprite)
        return;

    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(sprite);
}

}

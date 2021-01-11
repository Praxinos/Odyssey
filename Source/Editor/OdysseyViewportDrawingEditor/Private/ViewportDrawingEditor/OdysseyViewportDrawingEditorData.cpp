// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorData.h"

#include "ULISLoaderModule.h"
#include "OdysseyTextureAssetUserData.h"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorData
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorData::~FOdysseyViewportDrawingEditorData()
{
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.Remove(mOnPrePropertyChangedDelegateHandle);
    UPackage::PreSavePackageEvent.Remove(mOnPackagePreSaveHandle);
    UPackage::PackageSavedEvent.Remove(mOnPackageSavedHandle);

    if(mBrushInstance)
    {
        mBrushInstance->RemoveFromRoot();
        mBrushInstance = NULL;
    }

    if(mDisplaySurface)
    {
        delete mDisplaySurface;
    }

    delete mPaintEngine;
}

FOdysseyViewportDrawingEditorData::FOdysseyViewportDrawingEditorData()
    : mTexture( NULL )
    , mLayerStack( NULL )
    , mDisplaySurface( NULL )
    , mPaintEngine(new FOdysseyPaintEngine3D())
    , mBrush(NULL)
    , mBrushInstance(NULL)
    , mPaintColor(::ul3::FPixelValue::FromRGBA8(0,0,0))
{
    mOnPrePropertyChangedDelegateHandle = FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw(this,&FOdysseyViewportDrawingEditorData::OnPreGlobalObjectPropertyChanged);

    /* FDelegateHandle */ mOnPackagePreSaveHandle = UPackage::PreSavePackageEvent.AddRaw(this,&FOdysseyViewportDrawingEditorData::OnPackagePreSave);
    /* FDelegateHandle */ mOnPackageSavedHandle = UPackage::PackageSavedEvent.AddRaw(this,&FOdysseyViewportDrawingEditorData::OnPackageSaved);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyViewportDrawingEditorData::Init(UTexture2D* iTexture)
{
    if (iTexture == mTexture)
        return;

    //Data edited texture changed, so we reset unload the texture and reset its properties
    if( mTexture )
    {
        mPaintEngine->Flush();
        SyncTextureAndInvalidate();
        ApplyPropertiesBackup();

        // Setup Surface
        if (mDisplaySurface)
            delete mDisplaySurface;
    }

    mTexture = iTexture;

    if (!iTexture)
    {
        mLayerStack = nullptr;
        return;
    }

    PrepareTextureProperties();

    UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
    if(!userData)
    {
        ::ul3::tFormat format = ULISFormatForUE4TextureSourceFormat(mTexture->Source.GetFormat());
        userData = NewObject< UOdysseyTextureAssetUserData >(mTexture,NAME_None,RF_Public);
        userData->GetLayerStack()->Init(mTexture->Source.GetSizeX(),mTexture->Source.GetSizeY(),format);
        mTexture->AddAssetUserData(userData);
        FOdysseyBlock* textureData = NewOdysseyBlockFromUTextureData(mTexture,userData->GetLayerStack()->Format());
        TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(userData->GetLayerStack()->GetLayerRoot()->GetNextLayerName(),textureData));
        userData->GetLayerStack()->AddLayer(imageLayer);
        mTexture->PostEditChange();
    }

    // Get Texture LayerStack
    mLayerStack = userData->GetLayerStack();

    mDisplaySurface = new FOdysseySurfaceEditable(mTexture);
    mDisplaySurface->Invalidate();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getters Setters

UTexture2D*
FOdysseyViewportDrawingEditorData::Texture()
{
    return mTexture;
}

FOdysseyLayerStack*					
FOdysseyViewportDrawingEditorData::LayerStack()
{
    return mLayerStack;
}

FOdysseySurfaceEditable*            
FOdysseyViewportDrawingEditorData::DisplaySurface()
{
    return mDisplaySurface;
}

FOdysseyPaintEngine3D*		        
FOdysseyViewportDrawingEditorData::PaintEngine()
{
    return mPaintEngine;
}

UOdysseyBrush*                      
FOdysseyViewportDrawingEditorData::Brush()
{
    return mBrush;
}

UOdysseyBrushAssetBase*             
FOdysseyViewportDrawingEditorData::BrushInstance()
{
    return mBrushInstance;
}

::ul3::FPixelValue
FOdysseyViewportDrawingEditorData::PaintColor() const
{
    return mPaintColor;
}


void						        
FOdysseyViewportDrawingEditorData::Brush(UOdysseyBrush* iBrush)
{
    mBrush = iBrush;
}

void						        
FOdysseyViewportDrawingEditorData::BrushInstance(UOdysseyBrushAssetBase* iBrushInstance)
{
    mBrushInstance = iBrushInstance;
}

void
FOdysseyViewportDrawingEditorData::PaintColor(::ul3::FPixelValue iColor)
{
    mPaintColor = iColor;
}


void
FOdysseyViewportDrawingEditorData::SyncTextureAndInvalidate()
{
    if (!mDisplaySurface || !mTexture)
        return;

    CopyBlockDataIntoUTexture(mDisplaySurface->Block(), mTexture);
    mTexture->UpdateResource();
}

void
FOdysseyViewportDrawingEditorData::PrepareTextureProperties()
{
    if (!mTexture)
        return;

    FTextureFormatSettings textureFormatSettings;
    mTexture->GetLayerFormatSettings(0,textureFormatSettings);

    // Create new Texture Properties Backup
    mPropertiesBackup = { textureFormatSettings.CompressionNone };

    // Overwrite Texture properties
    textureFormatSettings.CompressionNone = 1;

    mTexture->SetLayerFormatSettings(0,textureFormatSettings);
    mTexture->UpdateResource();
    mTexture->TemporarilyDisableStreaming(); //needed to be able to draw on previously streamed textures, avoids using NoMipMaps
}

void
FOdysseyViewportDrawingEditorData::ApplyPropertiesBackup()
{
    if (!mTexture)
        return;

    FTextureFormatSettings textureFormatSettings;
    mTexture->GetLayerFormatSettings(0,textureFormatSettings);
    textureFormatSettings.CompressionNone = mPropertiesBackup.mTextureCompressionNone;
    mTexture->SetLayerFormatSettings(0, textureFormatSettings);
    
    mTexture->UpdateResource();
}

void
FOdysseyViewportDrawingEditorData::OnPreGlobalObjectPropertyChanged(UObject* iObject,const FEditPropertyChain& iEditPropertyChain)
{
    if( !mTexture )
        return;

    if(mTexture == Cast<UTexture2D>(iObject))
    {
        //Texture properties will change, we need to SyncTextureWithBlock
        CopyBlockDataIntoUTexture(mDisplaySurface->Block(),mTexture);
    }
}

void
FOdysseyViewportDrawingEditorData::OnPackagePreSave(UPackage* iPackage)
{
    if(!mTexture)
        return;

    UPackage* package = CastChecked<UPackage>(mTexture->GetOuter());
    if(package != iPackage)
        return;

    PaintEngine()->Flush();
    SyncTextureAndInvalidate();
    ApplyPropertiesBackup();
}

void
FOdysseyViewportDrawingEditorData::OnPackageSaved(const FString& iPackageFilename,UObject* iOuter)
{
    if(!mTexture)
        return;

    if(mTexture->GetOuter() != iOuter)
        return;

    PrepareTextureProperties();
}
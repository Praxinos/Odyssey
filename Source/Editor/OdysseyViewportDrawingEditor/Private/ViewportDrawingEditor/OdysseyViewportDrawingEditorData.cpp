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
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyViewportDrawingEditorData::Init(UTexture2D* iTexture)
{
    if( iTexture )
    {
        //Data edited texture changed, so we reset unload the texture and reset its properties
        if( mTexture )
        {
            mPaintEngine->Flush();
            SyncTextureAndInvalidate();
            ApplyPropertiesBackup();
        }

        mTexture = iTexture;

        PrepareTextureProperties();

        UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
        if(!userData)
        {
            ::ul3::tFormat format = ULISFormatForUE4TextureSourceFormat(mTexture->Source.GetFormat());
            userData = NewObject< UOdysseyTextureAssetUserData >(mTexture,NAME_None,RF_Public);
            userData->GetLayerStack()->Init(mTexture->GetSizeX(),mTexture->GetSizeY(),format);
            mTexture->AddAssetUserData(userData);
            FOdysseyBlock* textureData = NewOdysseyBlockFromUTextureData(mTexture,userData->GetLayerStack()->Format());
            TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(userData->GetLayerStack()->GetLayerRoot()->GetNextLayerName(),textureData));
            userData->GetLayerStack()->AddLayer(imageLayer);
            mTexture->PostEditChange();
        }

        // Get Texture LayerStack
        mLayerStack = userData->GetLayerStack();

        // Setup Surface
        if( mDisplaySurface )
            delete mDisplaySurface;

        mDisplaySurface = new FOdysseySurfaceEditable(mTexture);
        mDisplaySurface->Invalidate();
    }
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
    CopyBlockDataIntoUTexture(mDisplaySurface->Block(),mTexture);
    InvalidateTextureFromData(mDisplaySurface->Block(),mTexture);
}

void
FOdysseyViewportDrawingEditorData::PrepareTextureProperties()
{
    FTextureFormatSettings textureFormatSettings;
    mTexture->GetLayerFormatSettings(0,textureFormatSettings);

    // Create new Texture Properties Backup
    mPropertiesBackup ={mTexture->MipGenSettings,mTexture->CompressionSettings,mTexture->LODGroup,textureFormatSettings};

    // Overwrite Texture properties
    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps; //Mandatory or can lead to display not refreshing because it displays a mipmap instead of the texture itself (I guess)

    textureFormatSettings.CompressionNone = 1;
    mTexture->SetLayerFormatSettings(0,textureFormatSettings);

    // mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    // mTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
    mTexture->UpdateResource();
}

void
FOdysseyViewportDrawingEditorData::ApplyPropertiesBackup()
{
    mTexture->MipGenSettings = mPropertiesBackup.mTextureMipGenBackup;
    mTexture->SetLayerFormatSettings(0,mPropertiesBackup.mTextureFormatSettings);
    // mTexture->CompressionSettings = mPropertiesBackup.mTextureCompressionBackup;
    // mTexture->LODGroup = mPropertiesBackup.mTextureGroupBackup;
    mTexture->UpdateResource();
}
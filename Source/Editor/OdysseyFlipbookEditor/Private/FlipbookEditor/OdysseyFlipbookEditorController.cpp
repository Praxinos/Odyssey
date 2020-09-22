// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFlipbookEditorController.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "Models/OdysseyFlipbookEditorCommands.h"
#include "SOdysseyAboutScreen.h"
#include "OdysseyBrushAssetBase.h"

#include "OdysseyFlipbookEditorData.h"
#include "OdysseyFlipbookEditorGUI.h"
#include "OdysseyFlipbookWrapper.h"

#include "SOdysseySurfaceViewport.h"

#include "PaperFlipbook.h"
#include "PaperSprite.h"



#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorController
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorController::~FOdysseyFlipbookEditorController()
{
	mData->FlipbookWrapper()->OnSpriteTextureChanged().Remove(mOnSpriteTextureChangedHandle);
	if (mData->LayerStack())
	{
		mData->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);
	}
}

FOdysseyFlipbookEditorController::FOdysseyFlipbookEditorController(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorGUI>& iGUI)
	: mData(iData)
	, mGUI(iGUI)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyFlipbookEditorController::Init(const TSharedRef<FUICommandList>& iToolkitCommands)
{
	mOnSpriteTextureChangedHandle = mData->FlipbookWrapper()->OnSpriteTextureChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnSpriteTextureChanged);

	// Add Menu Extender
    GetMenuExtenders().Add(CreateMenuExtenders(iToolkitCommands));

	// Register our commands. This will only register them if not previously registered
	FOdysseyFlipbookEditorCommands::Register();

    // Init Painter Editor
    FOdysseyPainterEditorController::InitOdysseyPainterEditorController(iToolkitCommands);

    // Bind each command to its function
    BindCommands(iToolkitCommands);

	InitLayerStack();
}

void
FOdysseyFlipbookEditorController::InitLayerStack()
{
	//This function is also called when the layerstack changed, which is similar to changing a layer (and a bit more), so we call this
	if (!mData->LayerStack())
	{
		mData->PaintEngine()->Block(NULL);
		return;
	}

	OnLayerStackCurrentLayerChanged(mData->LayerStack()->GetCurrentLayer());
	if (!(mData->LayerStack()->OnCurrentLayerChanged().IsBound()))
		mData->LayerStack()->OnCurrentLayerChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerStackCurrentLayerChanged);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyFlipbookEditorController::BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands)
{
	mGUI->GetTimelineTab()->BindCommands(iToolkitCommands);
	FOdysseyPainterEditorController::BindCommands(iToolkitCommands);
}

TSharedPtr<FExtender>
FOdysseyFlipbookEditorController::CreateMenuExtenders(const TSharedRef<FUICommandList>& iToolkitCommands)
{
    FExtender* extender = new FExtender();

    return MakeShareable(extender);
}

void
FOdysseyFlipbookEditorController::OnPaintEngineStrokeChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(mData->LayerStack());
	FOdysseyPainterEditorController::OnPaintEngineStrokeChanged(iChangedTiles);
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		mData->LayerStack()->ComputeResultBlockWithTempBuffer(iChangedTiles[i], mData->PaintEngine()->TempBuffer(), mData->PaintEngine()->GetOpacity(), mData->PaintEngine()->GetBlendingMode(), mData->PaintEngine()->GetAlphaMode());
	}
}

void
FOdysseyFlipbookEditorController::OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(mData->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineStrokeWillEnd(iChangedTiles);
	mData->LayerStack()->mDrawingUndo->StartRecord();
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		mData->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
		mData->LayerStack()->BlendTempBufferOnCurrentBlock(iChangedTiles[i], mData->PaintEngine()->TempBuffer(), mData->PaintEngine()->GetOpacity(), mData->PaintEngine()->GetBlendingMode(), mData->PaintEngine()->GetAlphaMode());
	}
	mData->LayerStack()->mDrawingUndo->EndRecord();
}

void
FOdysseyFlipbookEditorController::OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(mData->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineStrokeEnd(iChangedTiles);
	mData->LayerStack()->ComputeResultBlock();
	mData->Texture()->MarkPackageDirty(); 
}

void
FOdysseyFlipbookEditorController::OnPaintEngineStrokeAbort()
{
    check(mData->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineStrokeAbort();
	mData->LayerStack()->ComputeResultBlock();
	//TODO: Check how to abort undo recording
}

void
FOdysseyFlipbookEditorController::OnTimelineCurrentKeyframeChanged(int32 iKeyframe)
{
	SetTextureAtKeyframeIndex(mGUI->GetTimelineTab()->GetCurrentKeyframeIndex());
}

void
FOdysseyFlipbookEditorController::OnTimelineScrubStarted()
{
}

void
FOdysseyFlipbookEditorController::OnTimelineScrubStopped()
{
	//TODO: unlock paintengine
	SetTextureAtKeyframeIndex(mGUI->GetTimelineTab()->GetCurrentKeyframeIndex());

	//Force display Surface
	mGUI->GetViewportTab()->SetSurface(mData->DisplaySurface());

	//Cleanup Preview Surface
	mData->PreviewSurface()->Texture(NULL);
}

void
FOdysseyFlipbookEditorController::OnFlipbookChanged()
{
	SetTextureAtKeyframeIndex(mGUI->GetTimelineTab()->GetCurrentKeyframeIndex());
}

FOnSpriteCreated&
FOdysseyFlipbookEditorController::OnSpriteCreated()
{
	return mOnSpriteCreated;
}

FOnTextureCreated&
FOdysseyFlipbookEditorController::OnTextureCreated()
{
	return mOnTextureCreated;
}

FOnKeyframeRemoved&
FOdysseyFlipbookEditorController::OnKeyframeRemoved()
{
	return mOnKeyframeRemoved;
}

void
FOdysseyFlipbookEditorController::OnLayerStackCurrentLayerChanged(FOdysseyNTree< IOdysseyLayer* >* iCurrentLayer)
{
	//Add Image Layer Callback
    if( mData->LayerStack()->GetCurrentLayer() == NULL )
    {
		mData->PaintEngine()->Block(NULL);
        return;
    }

	IOdysseyLayer* layer = mData->LayerStack()->GetCurrentLayer()->GetNodeContent();

	if (!layer)
	{
		mData->PaintEngine()->Block(NULL);
		return;
	}

	if (layer->GetType() != IOdysseyLayer::eType::kImage) {
		mData->PaintEngine()->Block(NULL);
		return;
	}

	FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(layer);
	if (!imageLayer)
	{
		mData->PaintEngine()->Block(NULL);
		return;
	}

	mData->PaintEngine()->Block(imageLayer->GetBlock());
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Performance Handlers

FReply
FOdysseyFlipbookEditorController::OnClear()
{
    if(!mData->LayerStack())
        return FReply::Handled();

	if( mData->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
    mData->LayerStack()->mDrawingUndo->StartRecord();
	mData->LayerStack()->mDrawingUndo->SaveData( 0, 0, mData->LayerStack()->Width(), mData->LayerStack()->Height() );
	mData->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord
	
    FOdysseyPainterEditorController::OnClear();

	mData->LayerStack()->ClearCurrentLayer();
    InvalidateTextureFromData(mData->LayerStack()->GetResultBlock(), mData->Texture());
    return FReply::Handled();
}

FReply
FOdysseyFlipbookEditorController::OnFill()
{
    if(!mData->LayerStack())
        return FReply::Handled();

    if( mData->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
	mData->LayerStack()->mDrawingUndo->StartRecord();
	mData->LayerStack()->mDrawingUndo->SaveData( 0, 0, mData->LayerStack()->Width(), mData->LayerStack()->Height() );
	mData->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord

    FOdysseyPainterEditorController::OnFill();

	mData->LayerStack()->FillCurrentLayerWithColor(mData->PaintEngine()->GetColor() );
    return FReply::Handled();
}

FReply
FOdysseyFlipbookEditorController::OnClearUndo()
{
    if(!mData->LayerStack())
    {
        return FOdysseyPainterEditorController::OnClearUndo();
    }
    
	mData->LayerStack()->mDrawingUndo->Clear();
    return FOdysseyPainterEditorController::OnClearUndo();
}

FReply
FOdysseyFlipbookEditorController::OnUndoIliad()
{
    FOdysseyPainterEditorController::OnUndoIliad();
    
    if(!mData->LayerStack())
    {
        return FReply::Handled();
    }
	mData->LayerStack()->mDrawingUndo->LoadData();
    return FReply::Handled();
}


FReply
FOdysseyFlipbookEditorController::OnRedoIliad()
{
    FOdysseyPainterEditorController::OnRedoIliad();
    if(!mData->LayerStack())
    {
        return FReply::Handled();
    }
	mData->LayerStack()->mDrawingUndo->Redo();
    return FReply::Handled();
}

void
FOdysseyFlipbookEditorController::OnCreateNewLayer()
{
    FOdysseyPainterEditorController::OnCreateNewLayer();

    mData->LayerStack()->AddImageLayer(0);
    mData->LayerStack()->ComputeResultBlock();
    mGUI->GetLayerStackTab()->RefreshView();}

void
FOdysseyFlipbookEditorController::OnDuplicateCurrentLayer()
{
    FOdysseyPainterEditorController::OnDuplicateCurrentLayer();

    if( mData->LayerStack()->GetCurrentLayer() )
    {
        mData->LayerStack()->DuplicateLayer( mData->LayerStack()->GetCurrentLayer()->GetNodeContent() );
        mData->LayerStack()->ComputeResultBlock();
        mGUI->GetLayerStackTab()->RefreshView();
    }
}

void
FOdysseyFlipbookEditorController::OnDeleteCurrentLayer()
{
    FOdysseyPainterEditorController::OnDeleteCurrentLayer();

    if( mData->LayerStack()->GetCurrentLayer() )
    {
        mData->LayerStack()->DeleteLayer( mData->LayerStack()->GetCurrentLayer()->GetNodeContent() );
        mData->LayerStack()->ComputeResultBlock();
        mGUI->GetLayerStackTab()->RefreshView();
    }
}

void           
FOdysseyFlipbookEditorController::OnExportLayersAsTextures()
{
    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ExportLayerDialogTitle", "Export Layers As Texture" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath( mData->Texture()->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = mData->Texture()->GetName();
    saveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if( saveObjectPath != "" )
    {
        TArray< IOdysseyLayer* > layers = TArray<IOdysseyLayer*>();
		mData->LayerStack()->GetLayers()->DepthFirstSearchTree( &layers, false );

        for( int i = 0; i < layers.Num(); i++ )
        {
            if( !( layers[i]->GetType() == IOdysseyLayer::eType::kImage ) )
                continue;

            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*> ( layers[i] );

            FString assetPath = FPaths::GetPath( saveObjectPath ) + "/";
            FString packagePath = ( assetPath + imageLayer->GetName().ToString().Replace( TEXT( " " ), TEXT( "_" ) ) );
            UPackage* package = CreatePackage( nullptr, *packagePath );

            UTexture2D* object = NewObject<UTexture2D>( package, UTexture2D::StaticClass(), FName( *( FPaths::GetBaseFilename( saveObjectPath ) + TEXT( "_" ) + imageLayer->GetName().ToString() ) ), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone );
            object->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
            object->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
            object->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
            InitTextureWithBlockData(imageLayer->GetBlock(), object, mData->Texture()->Source.GetFormat());

            object->PostEditChange();
            object->UpdateResource();

            FAssetRegistryModule::AssetCreated( object );

            UPackage::SavePackage( package, object, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *( imageLayer->GetName().ToString() ) );
            
            package->MarkAsFullyLoaded();
            object->MarkPackageDirty();
        }
    }
}

void           
FOdysseyFlipbookEditorController::OnImportTexturesAsLayers()
{
    FOpenAssetDialogConfig openAssetDialogConfig;
    openAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ImportTextureDialogTitle", "Import Textures As Layers" );
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(mData->Texture()->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );

    for( int i = 0; i < assetsData.Num(); i++ )
    {
        UTexture2D* openedTexture = static_cast<UTexture2D*>( assetsData[i].GetAsset() );
        FOdysseyBlock* textureBlock = NewOdysseyBlockFromUTextureData( openedTexture );
		mData->LayerStack()->AddImageLayerFromData( textureBlock, mData->LayerStack()->GetLayers(), FName( *( openedTexture->GetName() ) ) );
        delete textureBlock;
    }

    mGUI->GetLayerStackTab()->RefreshView();
	mData->LayerStack()->ComputeResultBlock();
}


TSharedPtr<FOdysseyPainterEditorData>
FOdysseyFlipbookEditorController::GetData()
{
    return mData;
}

TSharedPtr<FOdysseyPainterEditorGUI>
FOdysseyFlipbookEditorController::GetGUI()
{
    return mGUI;
}

void
FOdysseyFlipbookEditorController::SetTextureAtKeyframeIndex(int32 iKeyframeIndex)
{
	UTexture2D* texture = mData->FlipbookWrapper()->GetKeyframeTexture(iKeyframeIndex);
	if (mGUI->GetTimelineTab()->IsScrubbing())
	{
		mData->PreviewSurface()->Texture(texture);
		mGUI->GetViewportTab()->SetSurface(mData->PreviewSurface());
		return;
	}

	//If we are not playing or scrubbing
	//Check if keyFrame changed
	if (texture != mData->Texture())
	{
		//disconnect the current layerstack
		if (mData->LayerStack())
		{
			mData->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);
		}

		//Set new keyframe
		mData->Texture(texture);

		//Init the new layerstack
		InitLayerStack();

		//Refresh Layer stack
		mGUI->GetLayerStackTab()->RefreshView();

		//Set display Surface
		mGUI->GetViewportTab()->SetSurface(mData->DisplaySurface());
	}
}

void
FOdysseyFlipbookEditorController::OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
	UPaperSprite* sprite = mData->FlipbookWrapper()->GetKeyframeSprite(mGUI->GetTimelineTab()->GetCurrentKeyframeIndex());
	if (sprite != iSprite)
		return;

	SetTextureAtKeyframeIndex(mGUI->GetTimelineTab()->GetCurrentKeyframeIndex());
}

#undef LOCTEXT_NAMESPACE

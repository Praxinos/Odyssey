// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
#include "TextureEditor/OdysseyTextureEditorState.h"

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
        mData->LayerStack()->OnStructureChanged().RemoveAll(this);
        mData->LayerStack()->OnImageResultChanged().RemoveAll(this);
	    mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().RemoveAll(this);
        mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().RemoveAll(this);

        TSharedPtr<IOdysseyLayer> layer = mData->LayerStack()->GetCurrentLayer();
        if ( layer && layer->GetType() == IOdysseyLayer::eType::kImage)
        {
            TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(layer);
            if (imageLayer)
            {
                //Set AlphaLock Delegate
                imageLayer->IsAlphaLockedChangedDelegate().RemoveAll(this);
            }
        }
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


	if (mData->LayerStack())
	{
		// Set Image Layer as the current Layer
		TArray<TSharedPtr<IOdysseyLayer>> layers;
		mData->LayerStack()->GetLayerRoot()->DepthFirstSearchTree(&layers, false);

		for (int i = 0; i < layers.Num(); i++)
		{
			if (layers[i]->GetType() != IOdysseyLayer::eType::kImage)
				continue;

			mData->LayerStack()->SetCurrentLayer(layers[i]);
			break;
		}
	}
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

	if (!(mData->LayerStack()->OnCurrentLayerChanged().IsBoundToObject(this)))
		mData->LayerStack()->OnCurrentLayerChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerStackCurrentLayerChanged);

    if( !(mData->LayerStack()->OnStructureChanged().IsBoundToObject(this)) )
	    mData->LayerStack()->OnStructureChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerStackStructureChanged);

    if( !(mData->LayerStack()->OnImageResultChanged().IsBoundToObject(this)) )
	    mData->LayerStack()->OnImageResultChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerStackImageResultChanged);

    if( !(mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().IsBoundToObject(this)) )
	    mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerIsLockedChanged);
        
    if( !(mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().IsBoundToObject(this)) )
	    mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerIsVisibleChanged);
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
FOdysseyFlipbookEditorController::OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(mData->LayerStack());
	FOdysseyPainterEditorController::OnPaintEnginePreviewBlockTilesChanged(iChangedTiles);
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		mData->LayerStack()->ComputeResultInBlockWithBlockAsCurrentLayer(mData->DisplaySurface()->Block()->GetBlock(), mData->PaintEngine()->PreviewBlock(), iChangedTiles[i]);
	}
    for (int i = 0; i < iChangedTiles.Num(); i++)
	{
        mData->DisplaySurface()->Block()->GetBlock()->Invalidate(iChangedTiles[i]);
    }
}

void
FOdysseyFlipbookEditorController::OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(mData->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesWillChange(iChangedTiles);
	mData->LayerStack()->mDrawingUndo->StartRecord();
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		mData->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
		//mData->LayerStack()->BlendOnCurrentLayer(mData->PaintEngine()->TempBuffer(), iChangedTiles[i], mData->PaintEngine()->GetOpacity(), mData->PaintEngine()->GetBlendingMode(), mData->PaintEngine()->GetAlphaMode());
	}
	mData->LayerStack()->mDrawingUndo->EndRecord();
}

void
FOdysseyFlipbookEditorController::OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(mData->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesChanged(iChangedTiles);
	// mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    // mData->DisplaySurface()->Invalidate();
	mData->Texture()->MarkPackageDirty(); 
}

void
FOdysseyFlipbookEditorController::OnPaintEngineStrokeAbort()
{
    check(mData->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineStrokeAbort();
	mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    mData->DisplaySurface()->Invalidate();
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
FOdysseyFlipbookEditorController::OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue)
{
    //Remove all delegates for AlphaLock and set alpha lock to a default value
    if (iOldValue && iOldValue->GetType() == IOdysseyLayer::eType::kImage) {
        TSharedPtr<FOdysseyImageLayer> oldImageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(iOldValue);
        if (oldImageLayer)
        {
            oldImageLayer->IsAlphaLockedChangedDelegate().RemoveAll(this);
            mData->PaintEngine()->SetAlphaModeModifier(mGUI->GetTopTab()->GetAlphaMode());
        }
	}

    mData->PaintEngine()->Block(NULL);

	//Add Image Layer Callback
    if( !mData->LayerStack() || mData->LayerStack()->GetCurrentLayer() == NULL )
        return;

	TSharedPtr<IOdysseyLayer> layer = mData->LayerStack()->GetCurrentLayer();
	if (!layer)
		return;

	if (layer->GetType() != IOdysseyLayer::eType::kImage)
		return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(layer);
	if (!imageLayer)
		return;

	mData->PaintEngine()->Block(imageLayer->GetBlock());
    mData->PaintEngine()->SetAlphaModeModifier(imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
    
    //Set AlphaLock Delegate
    imageLayer->IsAlphaLockedChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnCurrentLayerIsAlphaLockedChanged);
    mData->PaintEngine()->SetLock( imageLayer->IsLocked(true) || !imageLayer->IsVisible(true) );
}

void
FOdysseyFlipbookEditorController::OnCurrentLayerIsAlphaLockedChanged(bool iOldValue)
{
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mData->LayerStack()->GetCurrentLayer());
    mData->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
}

void
FOdysseyFlipbookEditorController::HandleAlphaModeModifierChanged( int32 iValue )
{
    if (!mData->LayerStack())
        return;

    if (!mData->LayerStack()->GetCurrentLayer())
        return;

    if( !( mData->LayerStack()->GetCurrentLayer()->GetType() == IOdysseyLayer::eType::kImage ) )
        return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mData->LayerStack()->GetCurrentLayer());
    mData->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
}

void
FOdysseyFlipbookEditorController::OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == mData->LayerStack()->GetCurrentLayer() || mData->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        mData->PaintEngine()->SetLock(mData->LayerStack()->GetCurrentLayer()->IsLocked(true) || !mData->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyFlipbookEditorController::OnLayerIsVisibleChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == mData->LayerStack()->GetCurrentLayer() || mData->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        mData->PaintEngine()->SetLock(mData->LayerStack()->GetCurrentLayer()->IsLocked(true) || !mData->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyFlipbookEditorController::OnLayerStackStructureChanged()
{
    if (mData->Texture())
    {
        mData->Texture()->MarkPackageDirty();
    }

    if( mData->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( mData->LayerStack() );
        mData->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
    }
}

void
FOdysseyFlipbookEditorController::OnLayerStackImageResultChanged()
{
    if (mData->Texture())
    {
        mData->PaintEngine()->Flush();
        mData->Texture()->MarkPackageDirty();
        mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
        mData->DisplaySurface()->Invalidate();
    }
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
    mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    mData->DisplaySurface()->Invalidate();

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
    mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    mData->DisplaySurface()->Invalidate();
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
    mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    mData->DisplaySurface()->Invalidate();
    return FReply::Handled();
}

void
FOdysseyFlipbookEditorController::OnCreateNewLayer()
{
    FOdysseyPainterEditorController::OnCreateNewLayer();

	FName layerName = mData->LayerStack()->GetLayerRoot()->GetNextLayerName();
    int w = mData->LayerStack()->Width();
    int h = mData->LayerStack()->Height();
    ::ul3::tFormat format = mData->LayerStack()->Format();
	TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, FVector2D(w, h), format));
    mData->LayerStack()->AddLayer(imageLayer);
    mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    mData->DisplaySurface()->Invalidate();
    mGUI->GetLayerStackTab()->RefreshView();}

void
FOdysseyFlipbookEditorController::OnDuplicateCurrentLayer()
{
    FOdysseyPainterEditorController::OnDuplicateCurrentLayer();

    if( mData->LayerStack()->GetCurrentLayer() )
    {
        mData->LayerStack()->DuplicateLayer( mData->LayerStack()->GetCurrentLayer() );
        mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
        mData->DisplaySurface()->Invalidate();
        mGUI->GetLayerStackTab()->RefreshView();
    }
}

void
FOdysseyFlipbookEditorController::OnDeleteCurrentLayer()
{
    FOdysseyPainterEditorController::OnDeleteCurrentLayer();

    if( mData->LayerStack()->GetCurrentLayer() )
    {
        FText Title = LOCTEXT("TitleDeletingCurrentLayer", "Deleting current layer");
        if (FMessageDialog::Open(EAppMsgType::OkCancel, LOCTEXT("DeletingCurrentLayer", "Are you sure you want to delete this layer ?"), &Title) == EAppReturnType::Ok)
        {
            mData->LayerStack()->DeleteLayer(mData->LayerStack()->GetCurrentLayer());
            mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
            mData->DisplaySurface()->Invalidate();
            mGUI->GetLayerStackTab()->RefreshView();
        }
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
		TArray< TSharedPtr<IOdysseyLayer> > layers;
		mData->LayerStack()->GetLayerRoot()->DepthFirstSearchTree(&layers, false);

        for( int i = 0; i < layers.Num(); i++ )
        {
            if( !( layers[i]->GetType() == IOdysseyLayer::eType::kImage ) )
                continue;

            TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer> ( layers[i] );

            FString assetPath = FPaths::GetPath( saveObjectPath ) + "/";
            FString packagePath = ( assetPath + imageLayer->GetName().ToString().Replace( TEXT( " " ), TEXT( "_" ) ) );
            UPackage* package = CreatePackage( *packagePath );

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
        FOdysseyBlock* textureBlock = NewOdysseyBlockFromUTextureData( openedTexture, mData->LayerStack()->Format() );

		FName layerName = mData->LayerStack()->GetLayerRoot()->GetNextLayerName();
		TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(FName(*(openedTexture->GetName())), textureBlock));
		mData->LayerStack()->AddLayer(imageLayer);
    }

    mGUI->GetLayerStackTab()->RefreshView();
    mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    mData->DisplaySurface()->Invalidate();
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
        mGUI->GetTextureDetailsTab()->SetTexture(texture);
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
			TSharedPtr<IOdysseyLayer> currentLayer = mData->LayerStack()->GetCurrentLayer();
			if (currentLayer->GetType() == IOdysseyLayer::eType::kImage)
			{
				TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(currentLayer);
				if (imageLayer)
				{
					imageLayer->IsAlphaLockedChangedDelegate().RemoveAll(this);
				}
			}
			mData->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);
            mData->LayerStack()->OnStructureChanged().RemoveAll(this);
            mData->LayerStack()->OnImageResultChanged().RemoveAll(this);
	        mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().RemoveAll(this);
            mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().RemoveAll(this);
		}

		//Set new keyframe
		mData->Texture(texture);

		//Init the new layerstack
		InitLayerStack();

        OnLayerStackCurrentLayerChanged(nullptr);

		//Refresh Layer stack
		mGUI->GetLayerStackTab()->RefreshView();

		//Set display Surface
		mGUI->GetViewportTab()->SetSurface(mData->DisplaySurface());
        
        //Set the new texture in the texture details panel
        mGUI->GetTextureDetailsTab()->SetTexture(texture);
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

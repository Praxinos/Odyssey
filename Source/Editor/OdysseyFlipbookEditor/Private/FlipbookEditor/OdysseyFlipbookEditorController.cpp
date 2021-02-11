// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditorController.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "Models/OdysseyFlipbookEditorCommands.h"
#include "SOdysseyAboutScreen.h"
#include "OdysseyBrushAssetBase.h"

#include "OdysseyFlipbookEditorGUI.h"
#include "OdysseyFlipbookWrapper.h"

#include "SOdysseySurfaceViewport.h"
#include "TextureEditor/OdysseyTextureEditorState.h"

#include "OdysseyFlipbookEditorTimelineTab.h"
#include "SOdysseyFlipbookTimelineView.h"

#include "PaperFlipbook.h"
#include "PaperSprite.h"



#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorController
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorController::~FOdysseyFlipbookEditorController()
{
    GetEditor()->TextureWrapper().OnPreTextureChangeDelegate().RemoveAll(this);
    GetEditor()->TextureWrapper().OnPostTextureChangeDelegate().RemoveAll(this);
	GetEditor()->FlipbookWrapper()->OnSpriteTextureChanged().RemoveAll(this);
}

FOdysseyFlipbookEditorController::FOdysseyFlipbookEditorController(FOdysseyFlipbookEditor* iEditor, TSharedPtr<FOdysseyFlipbookEditorGUI>& iGUI)
	: mEditor(iEditor)
	, mGUI(iGUI)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyFlipbookEditorController::Init()
{
	mOnSpriteTextureChangedHandle = GetEditor()->FlipbookWrapper()->OnSpriteTextureChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnSpriteTextureChanged);

	//----

	// Register our commands. This will only register them if not previously registered
	//FOdysseyFlipbookEditorCommands::Register();

    // Init Painter Editor
    FOdysseyPainterEditorController::InitOdysseyPainterEditorController(mEditor->Toolkit()->GetToolkitCommands());

    // Bind each command to its function
    BindCommands(mEditor->Toolkit()->GetToolkitCommands());

    if ( !(GetEditor()->TextureWrapper().OnPreTextureChangeDelegate().IsBoundToObject(this)) )
        GetEditor()->TextureWrapper().OnPreTextureChangeDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnPreTextureChange);

    if ( !(GetEditor()->TextureWrapper().OnPostTextureChangeDelegate().IsBoundToObject(this)) )
        GetEditor()->TextureWrapper().OnPostTextureChangeDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnPostTextureChange);

    //Make like if the texture changed, to set all callbacks correctly
    OnPostTextureChange(nullptr);
}

void
FOdysseyFlipbookEditorController::OnPreTextureChange(UTexture2D* iNewTexture)
{
    if (!GetEditor()->LayerStack())
		return;

    GetEditor()->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);
    GetEditor()->LayerStack()->OnStructureChanged().RemoveAll(this);
    GetEditor()->LayerStack()->OnImageResultChanged().RemoveAll(this);
    GetEditor()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().RemoveAll(this);
    GetEditor()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().RemoveAll(this);

    TSharedPtr<IOdysseyLayer> layer = GetEditor()->LayerStack()->GetCurrentLayer();
    if ( layer && layer->GetType() == IOdysseyLayer::eType::kImage)
    {
        TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetEditor()->LayerStack()->GetCurrentLayer());
        if (imageLayer)
        {
            //Set AlphaLock Delegate
            imageLayer->IsAlphaLockedChangedDelegate().RemoveAll(this);
        }
    }
}

void
FOdysseyFlipbookEditorController::OnPostTextureChange(UTexture2D* iOldTexture)
{
    if (!GetEditor()->LayerStack())
        return;

    if( !(GetEditor()->LayerStack()->OnCurrentLayerChanged().IsBoundToObject(this)) )
	    GetEditor()->LayerStack()->OnCurrentLayerChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerStackCurrentLayerChanged);

    if( !(GetEditor()->LayerStack()->OnStructureChanged().IsBoundToObject(this)) )
	    GetEditor()->LayerStack()->OnStructureChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerStackStructureChanged);

    if( !(GetEditor()->LayerStack()->OnImageResultChanged().IsBoundToObject(this)) )
	    GetEditor()->LayerStack()->OnImageResultChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerStackImageResultChanged);

    if( !(GetEditor()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().IsBoundToObject(this)) )
	    GetEditor()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerIsLockedChanged);
        
    if( !(GetEditor()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().IsBoundToObject(this)) )
	    GetEditor()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerIsVisibleChanged);
    	
    if (GetEditor()->LayerStack()->GetLayerRoot() == GetEditor()->LayerStack()->GetCurrentLayer())
    {
        // Set Image Layer as the current Layer
        TArray<TSharedPtr<IOdysseyLayer>> layers;
        GetEditor()->LayerStack()->GetLayerRoot()->DepthFirstSearchTree( &layers, false );

        for (int i = 0; i < layers.Num(); i++)
        {
            if (layers[i]->GetType() != IOdysseyLayer::eType::kImage)
                continue;
            
            GetEditor()->LayerStack()->SetCurrentLayer(layers[i]);
            break;
        }
    }
    else
    {
        OnLayerStackCurrentLayerChanged(nullptr);
    }

    //Refresh Layer stack
    mGUI->GetLayerStackTab()->RefreshView();

    //Set display Surface
    //mGUI->GetViewportTab()->SetSurface(GetEditor()->DisplaySurface());
    
    //Set the new texture in the texture details panel
    mGUI->GetTextureDetailsTab()->SetTexture(GetEditor()->Texture());
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyFlipbookEditorController::BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands)
{
	// mGUI->GetTimelineTab()->BindCommands(iToolkitCommands);
	FOdysseyPainterEditorController::BindCommands(iToolkitCommands);
}

void
FOdysseyFlipbookEditorController::OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(GetEditor()->LayerStack());
	FOdysseyPainterEditorController::OnPaintEnginePreviewBlockTilesChanged(iChangedTiles);
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		GetEditor()->LayerStack()->ComputeResultInBlockWithBlockAsCurrentLayer(GetEditor()->DisplaySurface()->Block()->GetBlock(), GetEditor()->PaintEngine()->PreviewBlock(), iChangedTiles[i]);
	}
    for (int i = 0; i < iChangedTiles.Num(); i++)
	{
        GetEditor()->DisplaySurface()->Block()->GetBlock()->Invalidate(iChangedTiles[i]);
    }
}

void
FOdysseyFlipbookEditorController::OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(GetEditor()->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesWillChange(iChangedTiles);
	GetEditor()->LayerStack()->mDrawingUndo->StartRecord();
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		GetEditor()->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
		//GetEditor()->LayerStack()->BlendOnCurrentLayer(GetEditor()->PaintEngine()->TempBuffer(), iChangedTiles[i], GetEditor()->PaintEngine()->GetOpacity(), GetEditor()->PaintEngine()->GetBlendingMode(), GetEditor()->PaintEngine()->GetAlphaMode());
	}
	GetEditor()->LayerStack()->mDrawingUndo->EndRecord();
}

void
FOdysseyFlipbookEditorController::OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(GetEditor()->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesChanged(iChangedTiles);
	// GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
    // GetEditor()->DisplaySurface()->Invalidate();
	GetEditor()->Texture()->MarkPackageDirty(); 
}

void
FOdysseyFlipbookEditorController::OnPaintEngineStrokeAbort()
{
    check(GetEditor()->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineStrokeAbort();
	GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
    GetEditor()->DisplaySurface()->Invalidate();
	//TODO: Check how to abort undo recording
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
            GetEditor()->PaintEngine()->SetAlphaModeModifier(mGUI->GetTopTab()->GetAlphaMode());
        }
	}

    GetEditor()->PaintEngine()->Block(NULL);

	//Add Image Layer Callback
    if( !GetEditor()->LayerStack() || GetEditor()->LayerStack()->GetCurrentLayer() == NULL )
        return;

	TSharedPtr<IOdysseyLayer> layer = GetEditor()->LayerStack()->GetCurrentLayer();
	if (!layer)
		return;

	if (layer->GetType() != IOdysseyLayer::eType::kImage)
		return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(layer);
	if (!imageLayer)
		return;

	GetEditor()->PaintEngine()->Block(imageLayer->GetBlock());
    GetEditor()->PaintEngine()->SetAlphaModeModifier(imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
    
    //Set AlphaLock Delegate
    imageLayer->IsAlphaLockedChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnCurrentLayerIsAlphaLockedChanged);
    GetEditor()->PaintEngine()->SetLock( imageLayer->IsLocked(true) || !imageLayer->IsVisible(true) );
}

void
FOdysseyFlipbookEditorController::OnCurrentLayerIsAlphaLockedChanged(bool iOldValue)
{
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetEditor()->LayerStack()->GetCurrentLayer());
    GetEditor()->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
}

void
FOdysseyFlipbookEditorController::HandleAlphaModeModifierChanged( int32 iValue )
{
    if (!GetEditor()->LayerStack())
        return;

    if (!GetEditor()->LayerStack()->GetCurrentLayer())
        return;

    if( !( GetEditor()->LayerStack()->GetCurrentLayer()->GetType() == IOdysseyLayer::eType::kImage ) )
        return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetEditor()->LayerStack()->GetCurrentLayer());
    GetEditor()->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
}

void
FOdysseyFlipbookEditorController::OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == GetEditor()->LayerStack()->GetCurrentLayer() || GetEditor()->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        GetEditor()->PaintEngine()->SetLock(GetEditor()->LayerStack()->GetCurrentLayer()->IsLocked(true) || !GetEditor()->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyFlipbookEditorController::OnLayerIsVisibleChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == GetEditor()->LayerStack()->GetCurrentLayer() || GetEditor()->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        GetEditor()->PaintEngine()->SetLock(GetEditor()->LayerStack()->GetCurrentLayer()->IsLocked(true) || !GetEditor()->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyFlipbookEditorController::OnLayerStackStructureChanged()
{
    if (GetEditor()->Texture())
    {
        GetEditor()->Texture()->MarkPackageDirty();
    }

    if( GetEditor()->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( GetEditor()->LayerStack() );
        GetEditor()->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
    }
}

void
FOdysseyFlipbookEditorController::OnLayerStackImageResultChanged()
{
    if (GetEditor()->Texture())
    {
        GetEditor()->PaintEngine()->Flush();
        GetEditor()->Texture()->MarkPackageDirty();
        GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
        GetEditor()->DisplaySurface()->Invalidate();
    }
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Performance Handlers

FReply
FOdysseyFlipbookEditorController::OnClear()
{
    if(!GetEditor()->LayerStack())
        return FReply::Handled();

	if( GetEditor()->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
    GetEditor()->LayerStack()->mDrawingUndo->StartRecord();
	GetEditor()->LayerStack()->mDrawingUndo->SaveData( 0, 0, GetEditor()->LayerStack()->Width(), GetEditor()->LayerStack()->Height() );
	GetEditor()->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord
	
    FOdysseyPainterEditorController::OnClear();

	GetEditor()->LayerStack()->ClearCurrentLayer();

    return FReply::Handled();
}

FReply
FOdysseyFlipbookEditorController::OnFill()
{
    if(!GetEditor()->LayerStack())
        return FReply::Handled();

    if( GetEditor()->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
	GetEditor()->LayerStack()->mDrawingUndo->StartRecord();
	GetEditor()->LayerStack()->mDrawingUndo->SaveData( 0, 0, GetEditor()->LayerStack()->Width(), GetEditor()->LayerStack()->Height() );
	GetEditor()->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord

    FOdysseyPainterEditorController::OnFill();

	GetEditor()->LayerStack()->FillCurrentLayerWithColor(GetEditor()->PaintEngine()->GetColor() );
    GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
    GetEditor()->DisplaySurface()->Invalidate();

    return FReply::Handled();
}

FReply
FOdysseyFlipbookEditorController::OnClearUndo()
{
    if(!GetEditor()->LayerStack())
    {
        return FOdysseyPainterEditorController::OnClearUndo();
    }
    
	GetEditor()->LayerStack()->mDrawingUndo->Clear();
    return FOdysseyPainterEditorController::OnClearUndo();
}

FReply
FOdysseyFlipbookEditorController::OnUndoIliad()
{
    FOdysseyPainterEditorController::OnUndoIliad();
    
    if(!GetEditor()->LayerStack())
    {
        return FReply::Handled();
    }
    
	GetEditor()->LayerStack()->mDrawingUndo->LoadData();
    GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
    GetEditor()->DisplaySurface()->Invalidate();
    return FReply::Handled();
}


FReply
FOdysseyFlipbookEditorController::OnRedoIliad()
{
    FOdysseyPainterEditorController::OnRedoIliad();
    if(!GetEditor()->LayerStack())
    {
        return FReply::Handled();
    }
    
	GetEditor()->LayerStack()->mDrawingUndo->Redo();
    GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
    GetEditor()->DisplaySurface()->Invalidate();
    return FReply::Handled();
}

void
FOdysseyFlipbookEditorController::OnCreateNewLayer()
{
    FOdysseyPainterEditorController::OnCreateNewLayer();

	FName layerName = GetEditor()->LayerStack()->GetLayerRoot()->GetNextLayerName();
    int w = GetEditor()->LayerStack()->Width();
    int h = GetEditor()->LayerStack()->Height();
    ::ul3::tFormat format = GetEditor()->LayerStack()->Format();
	TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, FVector2D(w, h), format));
    GetEditor()->LayerStack()->AddLayer(imageLayer);
    GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
    GetEditor()->DisplaySurface()->Invalidate();
    mGUI->GetLayerStackTab()->RefreshView();}

void
FOdysseyFlipbookEditorController::OnDuplicateCurrentLayer()
{
    FOdysseyPainterEditorController::OnDuplicateCurrentLayer();

    if( GetEditor()->LayerStack()->GetCurrentLayer() )
    {
        GetEditor()->LayerStack()->DuplicateLayer( GetEditor()->LayerStack()->GetCurrentLayer() );
        GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
        GetEditor()->DisplaySurface()->Invalidate();
        mGUI->GetLayerStackTab()->RefreshView();
    }
}

void
FOdysseyFlipbookEditorController::OnDeleteCurrentLayer()
{
    FOdysseyPainterEditorController::OnDeleteCurrentLayer();

    if( GetEditor()->LayerStack()->GetCurrentLayer() )
    {
        FText Title = LOCTEXT("TitleDeletingCurrentLayer", "Deleting current layer");
        if (FMessageDialog::Open(EAppMsgType::OkCancel, LOCTEXT("DeletingCurrentLayer", "Are you sure you want to delete this layer ?"), &Title) == EAppReturnType::Ok)
        {
            GetEditor()->LayerStack()->DeleteLayer(GetEditor()->LayerStack()->GetCurrentLayer());
            GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
            GetEditor()->DisplaySurface()->Invalidate();
            mGUI->GetLayerStackTab()->RefreshView();
        }
    }
}

void           
FOdysseyFlipbookEditorController::OnExportLayersAsTextures()
{
    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ExportLayerDialogTitle", "Export Layers As Texture" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath( GetEditor()->Texture()->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = GetEditor()->Texture()->GetName();
    saveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if( saveObjectPath != "" )
    {
		TArray< TSharedPtr<IOdysseyLayer> > layers;
		GetEditor()->LayerStack()->GetLayerRoot()->DepthFirstSearchTree(&layers, false);

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
            InitTextureWithBlockData(imageLayer->GetBlock(), object, GetEditor()->Texture()->Source.GetFormat());

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
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(GetEditor()->Texture()->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );

    for( int i = 0; i < assetsData.Num(); i++ )
    {
        UTexture2D* openedTexture = static_cast<UTexture2D*>( assetsData[i].GetAsset() );
        FOdysseyBlock* textureBlock = NewOdysseyBlockFromUTextureData( openedTexture, GetEditor()->LayerStack()->Format() );

		FName layerName = GetEditor()->LayerStack()->GetLayerRoot()->GetNextLayerName();
		TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(FName(*(openedTexture->GetName())), textureBlock));
		GetEditor()->LayerStack()->AddLayer(imageLayer);
    }

    mGUI->GetLayerStackTab()->RefreshView();
    GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
    GetEditor()->DisplaySurface()->Invalidate();
}

FOdysseyFlipbookEditor*
FOdysseyFlipbookEditorController::GetEditor()
{
    return mEditor;
}

TSharedPtr<FOdysseyPainterEditorGUI>
FOdysseyFlipbookEditorController::GetGUI()
{
    return mGUI;
}

void
FOdysseyFlipbookEditorController::SetTextureAtKeyframeIndex(int32 iKeyframeIndex)
{
	UTexture2D* texture = GetEditor()->FlipbookWrapper()->GetKeyframeTexture(iKeyframeIndex);
	if (GetEditor()->GetGUI()->GetTimelineTab()->Timeline()->IsScrubbing())
	{
		GetEditor()->PreviewSurface()->Texture(texture);
        GetEditor()->GetGUI()->GetTextureDetailsTab()->SetTexture(texture);
		//mGUI->GetViewportTab()->SetSurface(GetEditor()->PreviewSurface());
		return;
	}

    GetEditor()->Texture(texture);
}

void
FOdysseyFlipbookEditorController::OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
    UTexture2D* texture = iSprite->GetSourceTexture();
	UPaperFlipbook* flipbook = GetEditor()->FlipbookWrapper()->Flipbook();
	for (int i = 0; i < flipbook->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = GetEditor()->FlipbookWrapper()->GetKeyframeSprite(i);
		if (sprite == iSprite)
		{
			if (iOldTexture)
				mEditor->Toolkit()->RemoveEditingObject(iOldTexture);

			if (texture)
				mEditor->Toolkit()->AddEditingObject(texture);
		}
	}


	UPaperSprite* sprite = GetEditor()->FlipbookWrapper()->GetKeyframeSprite(mGUI->GetTimelineTab()->Timeline()->GetCurrentKeyframeIndex());
	if (sprite != iSprite)
		return;

	SetTextureAtKeyframeIndex(mGUI->GetTimelineTab()->Timeline()->GetCurrentKeyframeIndex());
}

#undef LOCTEXT_NAMESPACE

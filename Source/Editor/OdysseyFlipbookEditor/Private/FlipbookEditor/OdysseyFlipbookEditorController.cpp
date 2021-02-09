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
    GetFlipbookEditorData()->TextureWrapper().OnPreTextureChangeDelegate().RemoveAll(this);
    GetFlipbookEditorData()->TextureWrapper().OnPostTextureChangeDelegate().RemoveAll(this);
	GetFlipbookEditorData()->FlipbookWrapper()->OnSpriteTextureChanged().RemoveAll(this);
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
	mOnSpriteTextureChangedHandle = GetFlipbookEditorData()->FlipbookWrapper()->OnSpriteTextureChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnSpriteTextureChanged);

	//----

	// Register our commands. This will only register them if not previously registered
	FOdysseyFlipbookEditorCommands::Register();

    // Init Painter Editor
    FOdysseyPainterEditorController::InitOdysseyPainterEditorController(mEditor->GetToolkit()->GetToolkitCommands());

    // Bind each command to its function
    BindCommands(mEditor->GetToolkit()->GetToolkitCommands());

    if ( !(GetFlipbookEditorData()->TextureWrapper().OnPreTextureChangeDelegate().IsBoundToObject(this)) )
        GetFlipbookEditorData()->TextureWrapper().OnPreTextureChangeDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnPreTextureChange);

    if ( !(GetFlipbookEditorData()->TextureWrapper().OnPostTextureChangeDelegate().IsBoundToObject(this)) )
        GetFlipbookEditorData()->TextureWrapper().OnPostTextureChangeDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnPostTextureChange);
}

void
FOdysseyFlipbookEditorController::OnPreTextureChange(UTexture2D* iNewTexture)
{
    if (!GetFlipbookEditorData()->LayerStack())
		return;

    GetFlipbookEditorData()->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);
    GetFlipbookEditorData()->LayerStack()->OnStructureChanged().RemoveAll(this);
    GetFlipbookEditorData()->LayerStack()->OnImageResultChanged().RemoveAll(this);
    GetFlipbookEditorData()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().RemoveAll(this);
    GetFlipbookEditorData()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().RemoveAll(this);

    TSharedPtr<IOdysseyLayer> layer = GetFlipbookEditorData()->LayerStack()->GetCurrentLayer();
    if ( layer && layer->GetType() == IOdysseyLayer::eType::kImage)
    {
        TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetFlipbookEditorData()->LayerStack()->GetCurrentLayer());
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
    if (!GetFlipbookEditorData()->LayerStack())
        return;

    if( !(GetFlipbookEditorData()->LayerStack()->OnCurrentLayerChanged().IsBoundToObject(this)) )
	    GetFlipbookEditorData()->LayerStack()->OnCurrentLayerChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerStackCurrentLayerChanged);

    if( !(GetFlipbookEditorData()->LayerStack()->OnStructureChanged().IsBoundToObject(this)) )
	    GetFlipbookEditorData()->LayerStack()->OnStructureChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerStackStructureChanged);

    if( !(GetFlipbookEditorData()->LayerStack()->OnImageResultChanged().IsBoundToObject(this)) )
	    GetFlipbookEditorData()->LayerStack()->OnImageResultChanged().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerStackImageResultChanged);

    if( !(GetFlipbookEditorData()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().IsBoundToObject(this)) )
	    GetFlipbookEditorData()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerIsLockedChanged);
        
    if( !(GetFlipbookEditorData()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().IsBoundToObject(this)) )
	    GetFlipbookEditorData()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerIsVisibleChanged);
    	
    if (GetFlipbookEditorData()->LayerStack()->GetLayerRoot() == GetFlipbookEditorData()->LayerStack()->GetCurrentLayer())
    {
        // Set Image Layer as the current Layer
        TArray<TSharedPtr<IOdysseyLayer>> layers;
        GetFlipbookEditorData()->LayerStack()->GetLayerRoot()->DepthFirstSearchTree( &layers, false );

        for (int i = 0; i < layers.Num(); i++)
        {
            if (layers[i]->GetType() != IOdysseyLayer::eType::kImage)
                continue;
            
            GetFlipbookEditorData()->LayerStack()->SetCurrentLayer(layers[i]);
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
    mGUI->GetViewportTab()->SetSurface(GetFlipbookEditorData()->DisplaySurface());
    
    //Set the new texture in the texture details panel
    mGUI->GetTextureDetailsTab()->SetTexture(GetFlipbookEditorData()->Texture());
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyFlipbookEditorController::BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands)
{
	mGUI->GetTimelineTab()->BindCommands(iToolkitCommands);
	FOdysseyPainterEditorController::BindCommands(iToolkitCommands);
}

void
FOdysseyFlipbookEditorController::OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(GetFlipbookEditorData()->LayerStack());
	FOdysseyPainterEditorController::OnPaintEnginePreviewBlockTilesChanged(iChangedTiles);
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		GetFlipbookEditorData()->LayerStack()->ComputeResultInBlockWithBlockAsCurrentLayer(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock(), GetFlipbookEditorData()->PaintEngine()->PreviewBlock(), iChangedTiles[i]);
	}
    for (int i = 0; i < iChangedTiles.Num(); i++)
	{
        GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock()->Invalidate(iChangedTiles[i]);
    }
}

void
FOdysseyFlipbookEditorController::OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(GetFlipbookEditorData()->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesWillChange(iChangedTiles);
	GetFlipbookEditorData()->LayerStack()->mDrawingUndo->StartRecord();
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		GetFlipbookEditorData()->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
		//GetFlipbookEditorData()->LayerStack()->BlendOnCurrentLayer(GetFlipbookEditorData()->PaintEngine()->TempBuffer(), iChangedTiles[i], GetFlipbookEditorData()->PaintEngine()->GetOpacity(), GetFlipbookEditorData()->PaintEngine()->GetBlendingMode(), GetFlipbookEditorData()->PaintEngine()->GetAlphaMode());
	}
	GetFlipbookEditorData()->LayerStack()->mDrawingUndo->EndRecord();
}

void
FOdysseyFlipbookEditorController::OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    check(GetFlipbookEditorData()->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesChanged(iChangedTiles);
	// GetFlipbookEditorData()->LayerStack()->ComputeResultInBlock(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock());
    // GetFlipbookEditorData()->DisplaySurface()->Invalidate();
	GetFlipbookEditorData()->Texture()->MarkPackageDirty(); 
}

void
FOdysseyFlipbookEditorController::OnPaintEngineStrokeAbort()
{
    check(GetFlipbookEditorData()->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineStrokeAbort();
	GetFlipbookEditorData()->LayerStack()->ComputeResultInBlock(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock());
    GetFlipbookEditorData()->DisplaySurface()->Invalidate();
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
	mGUI->GetViewportTab()->SetSurface(GetFlipbookEditorData()->DisplaySurface());

	//Cleanup Preview Surface
	GetFlipbookEditorData()->PreviewSurface()->Texture(NULL);
}

void
FOdysseyFlipbookEditorController::OnFlipbookChanged()
{
	SetTextureAtKeyframeIndex(mGUI->GetTimelineTab()->GetCurrentKeyframeIndex());
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
            GetFlipbookEditorData()->PaintEngine()->SetAlphaModeModifier(mGUI->GetTopTab()->GetAlphaMode());
        }
	}

    GetFlipbookEditorData()->PaintEngine()->Block(NULL);

	//Add Image Layer Callback
    if( !GetFlipbookEditorData()->LayerStack() || GetFlipbookEditorData()->LayerStack()->GetCurrentLayer() == NULL )
        return;

	TSharedPtr<IOdysseyLayer> layer = GetFlipbookEditorData()->LayerStack()->GetCurrentLayer();
	if (!layer)
		return;

	if (layer->GetType() != IOdysseyLayer::eType::kImage)
		return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(layer);
	if (!imageLayer)
		return;

	GetFlipbookEditorData()->PaintEngine()->Block(imageLayer->GetBlock());
    GetFlipbookEditorData()->PaintEngine()->SetAlphaModeModifier(imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
    
    //Set AlphaLock Delegate
    imageLayer->IsAlphaLockedChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnCurrentLayerIsAlphaLockedChanged);
    GetFlipbookEditorData()->PaintEngine()->SetLock( imageLayer->IsLocked(true) || !imageLayer->IsVisible(true) );
}

void
FOdysseyFlipbookEditorController::OnCurrentLayerIsAlphaLockedChanged(bool iOldValue)
{
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetFlipbookEditorData()->LayerStack()->GetCurrentLayer());
    GetFlipbookEditorData()->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
}

void
FOdysseyFlipbookEditorController::HandleAlphaModeModifierChanged( int32 iValue )
{
    if (!GetFlipbookEditorData()->LayerStack())
        return;

    if (!GetFlipbookEditorData()->LayerStack()->GetCurrentLayer())
        return;

    if( !( GetFlipbookEditorData()->LayerStack()->GetCurrentLayer()->GetType() == IOdysseyLayer::eType::kImage ) )
        return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetFlipbookEditorData()->LayerStack()->GetCurrentLayer());
    GetFlipbookEditorData()->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
}

void
FOdysseyFlipbookEditorController::OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == GetFlipbookEditorData()->LayerStack()->GetCurrentLayer() || GetFlipbookEditorData()->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        GetFlipbookEditorData()->PaintEngine()->SetLock(GetFlipbookEditorData()->LayerStack()->GetCurrentLayer()->IsLocked(true) || !GetFlipbookEditorData()->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyFlipbookEditorController::OnLayerIsVisibleChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == GetFlipbookEditorData()->LayerStack()->GetCurrentLayer() || GetFlipbookEditorData()->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        GetFlipbookEditorData()->PaintEngine()->SetLock(GetFlipbookEditorData()->LayerStack()->GetCurrentLayer()->IsLocked(true) || !GetFlipbookEditorData()->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyFlipbookEditorController::OnLayerStackStructureChanged()
{
    if (GetFlipbookEditorData()->Texture())
    {
        GetFlipbookEditorData()->Texture()->MarkPackageDirty();
    }

    if( GetFlipbookEditorData()->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( GetFlipbookEditorData()->LayerStack() );
        GetFlipbookEditorData()->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
    }
}

void
FOdysseyFlipbookEditorController::OnLayerStackImageResultChanged()
{
    if (GetFlipbookEditorData()->Texture())
    {
        GetFlipbookEditorData()->PaintEngine()->Flush();
        GetFlipbookEditorData()->Texture()->MarkPackageDirty();
        GetFlipbookEditorData()->LayerStack()->ComputeResultInBlock(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock());
        GetFlipbookEditorData()->DisplaySurface()->Invalidate();
    }
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Performance Handlers

FReply
FOdysseyFlipbookEditorController::OnClear()
{
    if(!GetFlipbookEditorData()->LayerStack())
        return FReply::Handled();

	if( GetFlipbookEditorData()->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
    GetFlipbookEditorData()->LayerStack()->mDrawingUndo->StartRecord();
	GetFlipbookEditorData()->LayerStack()->mDrawingUndo->SaveData( 0, 0, GetFlipbookEditorData()->LayerStack()->Width(), GetFlipbookEditorData()->LayerStack()->Height() );
	GetFlipbookEditorData()->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord
	
    FOdysseyPainterEditorController::OnClear();

	GetFlipbookEditorData()->LayerStack()->ClearCurrentLayer();

    return FReply::Handled();
}

FReply
FOdysseyFlipbookEditorController::OnFill()
{
    if(!GetFlipbookEditorData()->LayerStack())
        return FReply::Handled();

    if( GetFlipbookEditorData()->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
	GetFlipbookEditorData()->LayerStack()->mDrawingUndo->StartRecord();
	GetFlipbookEditorData()->LayerStack()->mDrawingUndo->SaveData( 0, 0, GetFlipbookEditorData()->LayerStack()->Width(), GetFlipbookEditorData()->LayerStack()->Height() );
	GetFlipbookEditorData()->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord

    FOdysseyPainterEditorController::OnFill();

	GetFlipbookEditorData()->LayerStack()->FillCurrentLayerWithColor(GetFlipbookEditorData()->PaintEngine()->GetColor() );
    GetFlipbookEditorData()->LayerStack()->ComputeResultInBlock(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock());
    GetFlipbookEditorData()->DisplaySurface()->Invalidate();

    return FReply::Handled();
}

FReply
FOdysseyFlipbookEditorController::OnClearUndo()
{
    if(!GetFlipbookEditorData()->LayerStack())
    {
        return FOdysseyPainterEditorController::OnClearUndo();
    }
    
	GetFlipbookEditorData()->LayerStack()->mDrawingUndo->Clear();
    return FOdysseyPainterEditorController::OnClearUndo();
}

FReply
FOdysseyFlipbookEditorController::OnUndoIliad()
{
    FOdysseyPainterEditorController::OnUndoIliad();
    
    if(!GetFlipbookEditorData()->LayerStack())
    {
        return FReply::Handled();
    }
    
	GetFlipbookEditorData()->LayerStack()->mDrawingUndo->LoadData();
    GetFlipbookEditorData()->LayerStack()->ComputeResultInBlock(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock());
    GetFlipbookEditorData()->DisplaySurface()->Invalidate();
    return FReply::Handled();
}


FReply
FOdysseyFlipbookEditorController::OnRedoIliad()
{
    FOdysseyPainterEditorController::OnRedoIliad();
    if(!GetFlipbookEditorData()->LayerStack())
    {
        return FReply::Handled();
    }
    
	GetFlipbookEditorData()->LayerStack()->mDrawingUndo->Redo();
    GetFlipbookEditorData()->LayerStack()->ComputeResultInBlock(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock());
    GetFlipbookEditorData()->DisplaySurface()->Invalidate();
    return FReply::Handled();
}

void
FOdysseyFlipbookEditorController::OnCreateNewLayer()
{
    FOdysseyPainterEditorController::OnCreateNewLayer();

	FName layerName = GetFlipbookEditorData()->LayerStack()->GetLayerRoot()->GetNextLayerName();
    int w = GetFlipbookEditorData()->LayerStack()->Width();
    int h = GetFlipbookEditorData()->LayerStack()->Height();
    ::ul3::tFormat format = GetFlipbookEditorData()->LayerStack()->Format();
	TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, FVector2D(w, h), format));
    GetFlipbookEditorData()->LayerStack()->AddLayer(imageLayer);
    GetFlipbookEditorData()->LayerStack()->ComputeResultInBlock(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock());
    GetFlipbookEditorData()->DisplaySurface()->Invalidate();
    mGUI->GetLayerStackTab()->RefreshView();}

void
FOdysseyFlipbookEditorController::OnDuplicateCurrentLayer()
{
    FOdysseyPainterEditorController::OnDuplicateCurrentLayer();

    if( GetFlipbookEditorData()->LayerStack()->GetCurrentLayer() )
    {
        GetFlipbookEditorData()->LayerStack()->DuplicateLayer( GetFlipbookEditorData()->LayerStack()->GetCurrentLayer() );
        GetFlipbookEditorData()->LayerStack()->ComputeResultInBlock(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock());
        GetFlipbookEditorData()->DisplaySurface()->Invalidate();
        mGUI->GetLayerStackTab()->RefreshView();
    }
}

void
FOdysseyFlipbookEditorController::OnDeleteCurrentLayer()
{
    FOdysseyPainterEditorController::OnDeleteCurrentLayer();

    if( GetFlipbookEditorData()->LayerStack()->GetCurrentLayer() )
    {
        FText Title = LOCTEXT("TitleDeletingCurrentLayer", "Deleting current layer");
        if (FMessageDialog::Open(EAppMsgType::OkCancel, LOCTEXT("DeletingCurrentLayer", "Are you sure you want to delete this layer ?"), &Title) == EAppReturnType::Ok)
        {
            GetFlipbookEditorData()->LayerStack()->DeleteLayer(GetFlipbookEditorData()->LayerStack()->GetCurrentLayer());
            GetFlipbookEditorData()->LayerStack()->ComputeResultInBlock(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock());
            GetFlipbookEditorData()->DisplaySurface()->Invalidate();
            mGUI->GetLayerStackTab()->RefreshView();
        }
    }
}

void           
FOdysseyFlipbookEditorController::OnExportLayersAsTextures()
{
    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ExportLayerDialogTitle", "Export Layers As Texture" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath( GetFlipbookEditorData()->Texture()->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = GetFlipbookEditorData()->Texture()->GetName();
    saveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if( saveObjectPath != "" )
    {
		TArray< TSharedPtr<IOdysseyLayer> > layers;
		GetFlipbookEditorData()->LayerStack()->GetLayerRoot()->DepthFirstSearchTree(&layers, false);

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
            InitTextureWithBlockData(imageLayer->GetBlock(), object, GetFlipbookEditorData()->Texture()->Source.GetFormat());

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
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(GetFlipbookEditorData()->Texture()->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );

    for( int i = 0; i < assetsData.Num(); i++ )
    {
        UTexture2D* openedTexture = static_cast<UTexture2D*>( assetsData[i].GetAsset() );
        FOdysseyBlock* textureBlock = NewOdysseyBlockFromUTextureData( openedTexture, GetFlipbookEditorData()->LayerStack()->Format() );

		FName layerName = GetFlipbookEditorData()->LayerStack()->GetLayerRoot()->GetNextLayerName();
		TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(FName(*(openedTexture->GetName())), textureBlock));
		GetFlipbookEditorData()->LayerStack()->AddLayer(imageLayer);
    }

    mGUI->GetLayerStackTab()->RefreshView();
    GetFlipbookEditorData()->LayerStack()->ComputeResultInBlock(GetFlipbookEditorData()->DisplaySurface()->Block()->GetBlock());
    GetFlipbookEditorData()->DisplaySurface()->Invalidate();
}

TSharedPtr<FOdysseyFlipbookEditorData>
FOdysseyFlipbookEditorController::GetFlipbookEditorData()
{
    return mEditor->GetData();
}

TSharedPtr<FOdysseyPainterEditorData>
FOdysseyFlipbookEditorController::GetData()
{
    return mEditor->GetData();
}

TSharedPtr<FOdysseyPainterEditorGUI>
FOdysseyFlipbookEditorController::GetGUI()
{
    return mGUI;
}

void
FOdysseyFlipbookEditorController::SetTextureAtKeyframeIndex(int32 iKeyframeIndex)
{
	UTexture2D* texture = GetFlipbookEditorData()->FlipbookWrapper()->GetKeyframeTexture(iKeyframeIndex);
	if (mGUI->GetTimelineTab()->IsScrubbing())
	{
		GetFlipbookEditorData()->PreviewSurface()->Texture(texture);
        mGUI->GetTextureDetailsTab()->SetTexture(texture);
		mGUI->GetViewportTab()->SetSurface(GetFlipbookEditorData()->PreviewSurface());
		return;
	}

    GetFlipbookEditorData()->Texture(texture);
}

void
FOdysseyFlipbookEditorController::OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
    UTexture2D* texture = iSprite->GetSourceTexture();
	UPaperFlipbook* flipbook = GetFlipbookEditorData()->FlipbookWrapper()->Flipbook();
	for (int i = 0; i < flipbook->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = GetFlipbookEditorData()->FlipbookWrapper()->GetKeyframeSprite(i);
		if (sprite == iSprite)
		{
			if (iOldTexture)
				mEditor->GetToolkit()->RemoveEditingObject(iOldTexture);

			if (texture)
				mEditor->GetToolkit()->AddEditingObject(texture);
		}
	}


	UPaperSprite* sprite = GetFlipbookEditorData()->FlipbookWrapper()->GetKeyframeSprite(mGUI->GetTimelineTab()->GetCurrentKeyframeIndex());
	if (sprite != iSprite)
		return;

	SetTextureAtKeyframeIndex(mGUI->GetTimelineTab()->GetCurrentKeyframeIndex());
}

void
FOdysseyFlipbookEditorController::OnSpriteCreated(UPaperSprite* iSprite)
{
	mEditor->GetToolkit()->AddEditingObject(iSprite);
}

void
FOdysseyFlipbookEditorController::OnTextureCreated(UTexture2D* iTexture)
{
	mEditor->GetToolkit()->AddEditingObject(iTexture);
}

void
FOdysseyFlipbookEditorController::OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe)
{
	if (!iKeyframe.Sprite)
		return;

	mEditor->GetToolkit()->RemoveEditingObject(iKeyframe.Sprite);

	UTexture2D* texture = iKeyframe.Sprite->GetSourceTexture();
	if (!texture)
		return;

	mEditor->GetToolkit()->RemoveEditingObject(texture);
}

void
FOdysseyFlipbookEditorController::OnToolkitInitialized()
{
	TSharedPtr<SDockTab> OwnerTab = mEditor->GetToolkit()->GetTabManager()->GetOwnerTab();
	TSharedPtr<SWindow> parentWindow = NULL;
	if (OwnerTab.IsValid())
	{
		parentWindow = FSlateApplication::Get().FindWidgetWindow(OwnerTab.ToSharedRef());
	}
	else
	{
		parentWindow = FGlobalTabmanager::Get()->GetRootWindow();
	}

	SetTimelineNavigationShortcuts(parentWindow);
}

void
FOdysseyFlipbookEditorController::SetTimelineNavigationShortcuts(TSharedPtr<SWidget> iWidget)
{
	TSharedPtr<FNavigationMetaData> navigationMetaData = MakeShareable(new FNavigationMetaData());
	navigationMetaData->SetNavigationCustom(EUINavigation::Left, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Right, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Next, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Previous, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	iWidget->AddMetadata(navigationMetaData.ToSharedRef());
}

#undef LOCTEXT_NAMESPACE

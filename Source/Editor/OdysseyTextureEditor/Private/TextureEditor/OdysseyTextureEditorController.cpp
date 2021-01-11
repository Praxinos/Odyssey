// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorController.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "Models/OdysseyTextureEditorCommands.h"
#include "SOdysseyAboutScreen.h"
#include "OdysseyBrushAssetBase.h"
#include "TextureEditor/OdysseyTextureEditorData.h"
#include "TextureEditor/OdysseyTextureEditorState.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorController
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorController::~FOdysseyTextureEditorController()
{
    mData->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);
    mData->LayerStack()->OnStructureChanged().RemoveAll(this);
    mData->LayerStack()->OnImageResultChanged().RemoveAll(this);
	mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().RemoveAll(this);
    mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().RemoveAll(this);

	TSharedPtr<IOdysseyLayer> layer = mData->LayerStack()->GetCurrentLayer();
	if ( layer && layer->GetType() == IOdysseyLayer::eType::kImage)
    {
        TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mData->LayerStack()->GetCurrentLayer());
        if (imageLayer)
        {
            //Set AlphaLock Delegate
            imageLayer->IsAlphaLockedChangedDelegate().RemoveAll(this);
        }
    }
}

FOdysseyTextureEditorController::FOdysseyTextureEditorController(TSharedPtr<FOdysseyTextureEditorData>& iData, TSharedPtr<FOdysseyTextureEditorGUI>& iGUI)
	: mData(iData)
	, mGUI(iGUI)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyTextureEditorController::Init(const TSharedRef<FUICommandList>& iToolkitCommands)
{
    // Init Painter Editor
    FOdysseyPainterEditorController::InitOdysseyPainterEditorController(iToolkitCommands);

    // Register our commands. This will only register them if not previously registered
    FOdysseyTextureEditorCommands::Register();

    // Bind each command to its function
    BindCommands(iToolkitCommands);

	// Set LayerStack CB
    if( !(mData->LayerStack()->OnCurrentLayerChanged().IsBoundToObject(this)) )
	    mData->LayerStack()->OnCurrentLayerChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerStackCurrentLayerChanged);

    if( !(mData->LayerStack()->OnStructureChanged().IsBoundToObject(this)) )
	    mData->LayerStack()->OnStructureChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerStackStructureChanged);

    if( !(mData->LayerStack()->OnImageResultChanged().IsBoundToObject(this)) )
	    mData->LayerStack()->OnImageResultChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerStackImageResultChanged);

    if( !(mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().IsBoundToObject(this)) )
	    mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().AddRaw(this, &FOdysseyTextureEditorController::OnLayerIsLockedChanged);
        
    if( !(mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().IsBoundToObject(this)) )
	    mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().AddRaw(this, &FOdysseyTextureEditorController::OnLayerIsVisibleChanged);
    	
    // Set Image Layer as the current Layer
    TArray<TSharedPtr<IOdysseyLayer>> layers;
    mData->LayerStack()->GetLayerRoot()->DepthFirstSearchTree( &layers, false );

    for (int i = 0; i < layers.Num(); i++)
    {
        if (layers[i]->GetType() != IOdysseyLayer::eType::kImage)
            continue;
        
        mData->LayerStack()->SetCurrentLayer(layers[i]);
        break;
    }
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyTextureEditorController::BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands)
{
    FOdysseyPainterEditorController::BindCommands(iToolkitCommands);
}

void
FOdysseyTextureEditorController::OnBrushSelected( UOdysseyBrush* iBrush )
{
    FOdysseyPainterEditorController::OnBrushSelected( iBrush );

    if( mData->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( mData->LayerStack() );
        mData->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
    }
}

void
FOdysseyTextureEditorController::OnBrushChanged( UBlueprint* iBrush )
{
    FOdysseyPainterEditorController::OnBrushChanged( iBrush );

    UOdysseyBrush* check_brush = dynamic_cast<UOdysseyBrush*>( iBrush );
    if( !check_brush )
        return;
}

void
FOdysseyTextureEditorController::OnBrushCompiled( UBlueprint* iBrush )
{
    FOdysseyPainterEditorController::OnBrushCompiled( iBrush );

    UOdysseyBrush* check_brush = dynamic_cast<UOdysseyBrush*>( iBrush );
    if( !check_brush )
        return;

    if( mData->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( mData->LayerStack() );
        mData->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
    }
}

void
FOdysseyTextureEditorController::OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
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
FOdysseyTextureEditorController::OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles)
{
    FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesWillChange(iChangedTiles);
	mData->LayerStack()->mDrawingUndo->StartRecord();
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		mData->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
        // mData->LayerStack()->BlendOnCurrentLayer(mData->PaintEngine()->TempBuffer(), iChangedTiles[i], mData->PaintEngine()->GetOpacity(), mData->PaintEngine()->GetBlendingMode(), mData->PaintEngine()->GetAlphaMode());
	}
    mData->LayerStack()->mDrawingUndo->EndRecord();
}

void
FOdysseyTextureEditorController::OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesChanged(iChangedTiles);
	// mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
	// mData->DisplaySurface()->Invalidate();
	mData->Texture()->MarkPackageDirty();
}

void
FOdysseyTextureEditorController::OnPaintEngineStrokeAbort()
{
    FOdysseyPainterEditorController::OnPaintEngineStrokeAbort();
	mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
	mData->DisplaySurface()->Invalidate();
}

void
FOdysseyTextureEditorController::OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue)
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
    if( mData->LayerStack()->GetCurrentLayer() == NULL )
        return;

	TSharedPtr<IOdysseyLayer> layer = mData->LayerStack()->GetCurrentLayer();
	if (layer->GetType() != IOdysseyLayer::eType::kImage)
		return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mData->LayerStack()->GetCurrentLayer());
	if (!imageLayer) 
        return;

    mData->PaintEngine()->Block(imageLayer->GetBlock());
    mData->PaintEngine()->SetAlphaModeModifier(imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
    
    //Set AlphaLock Delegate
    imageLayer->IsAlphaLockedChangedDelegate().AddRaw(this, &FOdysseyTextureEditorController::OnCurrentLayerIsAlphaLockedChanged);
    mData->PaintEngine()->SetLock( imageLayer->IsLocked(true) || !imageLayer->IsVisible(true) );
}

void
FOdysseyTextureEditorController::OnCurrentLayerIsAlphaLockedChanged(bool iOldValue)
{
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mData->LayerStack()->GetCurrentLayer());
    mData->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
}

void
FOdysseyTextureEditorController::HandleAlphaModeModifierChanged( int32 iValue )
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
FOdysseyTextureEditorController::OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == mData->LayerStack()->GetCurrentLayer() || mData->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        mData->PaintEngine()->SetLock(mData->LayerStack()->GetCurrentLayer()->IsLocked(true) || !mData->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyTextureEditorController::OnLayerIsVisibleChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == mData->LayerStack()->GetCurrentLayer() || mData->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        mData->PaintEngine()->SetLock(mData->LayerStack()->GetCurrentLayer()->IsLocked(true) || !mData->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyTextureEditorController::OnLayerStackStructureChanged()
{
    mData->Texture()->MarkPackageDirty();

    if( mData->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( mData->LayerStack() );
        mData->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
    }
}

void
FOdysseyTextureEditorController::OnLayerStackImageResultChanged()
{
    mData->PaintEngine()->Flush();
    mData->Texture()->MarkPackageDirty();
    mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
	mData->DisplaySurface()->Invalidate();
}

void
FOdysseyTextureEditorController::OnExportLayersAsTextures()
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
		mData->LayerStack()->GetLayerRoot()->DepthFirstSearchTree( &layers, false );

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
FOdysseyTextureEditorController::OnImportTexturesAsLayers()
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
		TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(FName(*(openedTexture->GetName())), textureBlock));
		mData->LayerStack()->AddLayer( imageLayer );
    }

    mGUI->GetLayerStackTab()->RefreshView();
	
	mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
	mData->DisplaySurface()->Invalidate();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Performance Handlers

FReply
FOdysseyTextureEditorController::OnClear()
{
    if( mData->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
    mData->LayerStack()->mDrawingUndo->StartRecord();
	mData->LayerStack()->mDrawingUndo->SaveData( 0, 0, mData->LayerStack()->Width(), mData->LayerStack()->Height() );
	mData->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord
	
    FOdysseyPainterEditorController::OnClear();

	mData->LayerStack()->ClearCurrentLayer();
    
	mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
	mData->DisplaySurface()->Invalidate();
    return FReply::Handled();
}

FReply
FOdysseyTextureEditorController::OnFill()
{
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
FOdysseyTextureEditorController::OnClearUndo()
{
	mData->LayerStack()->mDrawingUndo->Clear();
    return FOdysseyPainterEditorController::OnClearUndo();
}

FReply
FOdysseyTextureEditorController::OnUndoIliad()
{
    FOdysseyPainterEditorController::OnUndoIliad();
	mData->LayerStack()->mDrawingUndo->LoadData();
	mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
	mData->DisplaySurface()->Invalidate();

    return FReply::Handled();
}


FReply
FOdysseyTextureEditorController::OnRedoIliad()
{
    FOdysseyPainterEditorController::OnRedoIliad();
	mData->LayerStack()->mDrawingUndo->Redo();
	mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
	mData->DisplaySurface()->Invalidate();

    return FReply::Handled();
}

void
FOdysseyTextureEditorController::OnCreateNewLayer()
{
    FOdysseyPainterEditorController::OnCreateNewLayer();

    FName name = mData->LayerStack()->GetLayerRoot()->GetNextLayerName();
    int w = mData->LayerStack()->Width();
    int h = mData->LayerStack()->Height();
    ::ul3::tFormat format = mData->LayerStack()->Format();
	TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(name, FVector2D(w, h), format));
    mData->LayerStack()->AddLayer(imageLayer, 0);
	mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
	mData->DisplaySurface()->Invalidate();
    mGUI->GetLayerStackTab()->RefreshView();
}

void
FOdysseyTextureEditorController::OnDuplicateCurrentLayer()
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
FOdysseyTextureEditorController::OnDeleteCurrentLayer()
{
    FOdysseyPainterEditorController::OnDeleteCurrentLayer();


    if( mData->LayerStack()->GetCurrentLayer() )
    {
		FText Title = LOCTEXT("TitleDeletingCurrentLayer", "Deleting current layer");
        if( FMessageDialog::Open(EAppMsgType::OkCancel,  LOCTEXT("DeletingCurrentLayer", "Are you sure you want to delete this layer ?" ), &Title ) == EAppReturnType::Ok )
        {
            mData->LayerStack()->DeleteLayer( mData->LayerStack()->GetCurrentLayer() );
            mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
			mData->DisplaySurface()->Invalidate();
            mGUI->GetLayerStackTab()->RefreshView();
        }
    }
}


TSharedPtr<FOdysseyPainterEditorData>
FOdysseyTextureEditorController::GetData()
{
    return mData;
}

TSharedPtr<FOdysseyPainterEditorGUI>
FOdysseyTextureEditorController::GetGUI()
{
    return mGUI;
}

#undef LOCTEXT_NAMESPACE

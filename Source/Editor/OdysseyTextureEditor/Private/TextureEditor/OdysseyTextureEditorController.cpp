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
    GetTextureEditorData()->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);
    GetTextureEditorData()->LayerStack()->OnStructureChanged().RemoveAll(this);
    GetTextureEditorData()->LayerStack()->OnImageResultChanged().RemoveAll(this);
	GetTextureEditorData()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().RemoveAll(this);
    GetTextureEditorData()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().RemoveAll(this);

	TSharedPtr<IOdysseyLayer> layer = GetTextureEditorData()->LayerStack()->GetCurrentLayer();
	if ( layer && layer->GetType() == IOdysseyLayer::eType::kImage)
    {
        TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetTextureEditorData()->LayerStack()->GetCurrentLayer());
        if (imageLayer)
        {
            //Set AlphaLock Delegate
            imageLayer->IsAlphaLockedChangedDelegate().RemoveAll(this);
        }
    }
}

FOdysseyTextureEditorController::FOdysseyTextureEditorController(FOdysseyTextureEditor* iEditor, TSharedPtr<FOdysseyTextureEditorGUI>& iGUI)
	: mEditor(iEditor)
	, mGUI(iGUI)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyTextureEditorController::Init()
{
    // Init Painter Editor
    FOdysseyPainterEditorController::InitOdysseyPainterEditorController(mEditor->GetToolkit()->GetToolkitCommands());

    // Register our commands. This will only register them if not previously registered
    FOdysseyTextureEditorCommands::Register();

    // Bind each command to its function
    BindCommands(mEditor->GetToolkit()->GetToolkitCommands());

	// Set LayerStack CB
    if( !(GetTextureEditorData()->LayerStack()->OnCurrentLayerChanged().IsBoundToObject(this)) )
	    GetTextureEditorData()->LayerStack()->OnCurrentLayerChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerStackCurrentLayerChanged);

    if( !(GetTextureEditorData()->LayerStack()->OnStructureChanged().IsBoundToObject(this)) )
	    GetTextureEditorData()->LayerStack()->OnStructureChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerStackStructureChanged);

    if( !(GetTextureEditorData()->LayerStack()->OnImageResultChanged().IsBoundToObject(this)) )
	    GetTextureEditorData()->LayerStack()->OnImageResultChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerStackImageResultChanged);

    if( !(GetTextureEditorData()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().IsBoundToObject(this)) )
	    GetTextureEditorData()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().AddRaw(this, &FOdysseyTextureEditorController::OnLayerIsLockedChanged);
        
    if( !(GetTextureEditorData()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().IsBoundToObject(this)) )
	    GetTextureEditorData()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().AddRaw(this, &FOdysseyTextureEditorController::OnLayerIsVisibleChanged);
    	
    // Set Image Layer as the current Layer
    TArray<TSharedPtr<IOdysseyLayer>> layers;
    GetTextureEditorData()->LayerStack()->GetLayerRoot()->DepthFirstSearchTree( &layers, false );

    for (int i = 0; i < layers.Num(); i++)
    {
        if (layers[i]->GetType() != IOdysseyLayer::eType::kImage)
            continue;
        
        GetTextureEditorData()->LayerStack()->SetCurrentLayer(layers[i]);
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

    if( GetTextureEditorData()->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( GetTextureEditorData()->LayerStack() );
        GetTextureEditorData()->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
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

    if( GetTextureEditorData()->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( GetTextureEditorData()->LayerStack() );
        GetTextureEditorData()->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
    }
}

void
FOdysseyTextureEditorController::OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    FOdysseyPainterEditorController::OnPaintEnginePreviewBlockTilesChanged(iChangedTiles);
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		GetTextureEditorData()->LayerStack()->ComputeResultInBlockWithBlockAsCurrentLayer(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock(), GetTextureEditorData()->PaintEngine()->PreviewBlock(), iChangedTiles[i]);
	}
    for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		GetTextureEditorData()->DisplaySurface()->Block()->GetBlock()->Invalidate(iChangedTiles[i]);
    }
}

void
FOdysseyTextureEditorController::OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles)
{
    FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesWillChange(iChangedTiles);
	GetTextureEditorData()->LayerStack()->mDrawingUndo->StartRecord();
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		GetTextureEditorData()->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
        // GetTextureEditorData()->LayerStack()->BlendOnCurrentLayer(GetTextureEditorData()->PaintEngine()->TempBuffer(), iChangedTiles[i], GetTextureEditorData()->PaintEngine()->GetOpacity(), GetTextureEditorData()->PaintEngine()->GetBlendingMode(), GetTextureEditorData()->PaintEngine()->GetAlphaMode());
	}
    GetTextureEditorData()->LayerStack()->mDrawingUndo->EndRecord();
}

void
FOdysseyTextureEditorController::OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    FOdysseyPainterEditorController::OnPaintEngineEditedBlockTilesChanged(iChangedTiles);
	// GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
	// GetTextureEditorData()->DisplaySurface()->Invalidate();
	GetTextureEditorData()->Texture()->MarkPackageDirty();
}

void
FOdysseyTextureEditorController::OnPaintEngineStrokeAbort()
{
    FOdysseyPainterEditorController::OnPaintEngineStrokeAbort();
	GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
	GetTextureEditorData()->DisplaySurface()->Invalidate();
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
            GetTextureEditorData()->PaintEngine()->SetAlphaModeModifier(mGUI->GetTopTab()->GetAlphaMode());
        }
	}
    GetTextureEditorData()->PaintEngine()->Block(NULL);

	//Add Image Layer Callback
    if( GetTextureEditorData()->LayerStack()->GetCurrentLayer() == NULL )
        return;

	TSharedPtr<IOdysseyLayer> layer = GetTextureEditorData()->LayerStack()->GetCurrentLayer();
	if (layer->GetType() != IOdysseyLayer::eType::kImage)
		return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetTextureEditorData()->LayerStack()->GetCurrentLayer());
	if (!imageLayer) 
        return;

    GetTextureEditorData()->PaintEngine()->Block(imageLayer->GetBlock());
    GetTextureEditorData()->PaintEngine()->SetAlphaModeModifier(imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
    
    //Set AlphaLock Delegate
    imageLayer->IsAlphaLockedChangedDelegate().AddRaw(this, &FOdysseyTextureEditorController::OnCurrentLayerIsAlphaLockedChanged);
    GetTextureEditorData()->PaintEngine()->SetLock( imageLayer->IsLocked(true) || !imageLayer->IsVisible(true) );
}

void
FOdysseyTextureEditorController::OnCurrentLayerIsAlphaLockedChanged(bool iOldValue)
{
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetTextureEditorData()->LayerStack()->GetCurrentLayer());
    GetTextureEditorData()->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
}

void
FOdysseyTextureEditorController::HandleAlphaModeModifierChanged( int32 iValue )
{
    if (!GetTextureEditorData()->LayerStack())
        return;

    if (!GetTextureEditorData()->LayerStack()->GetCurrentLayer())
        return;

    if( !( GetTextureEditorData()->LayerStack()->GetCurrentLayer()->GetType() == IOdysseyLayer::eType::kImage ) )
        return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetTextureEditorData()->LayerStack()->GetCurrentLayer());
    GetTextureEditorData()->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetTopTab()->GetAlphaMode());
}

void
FOdysseyTextureEditorController::OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == GetTextureEditorData()->LayerStack()->GetCurrentLayer() || GetTextureEditorData()->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        GetTextureEditorData()->PaintEngine()->SetLock(GetTextureEditorData()->LayerStack()->GetCurrentLayer()->IsLocked(true) || !GetTextureEditorData()->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyTextureEditorController::OnLayerIsVisibleChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == GetTextureEditorData()->LayerStack()->GetCurrentLayer() || GetTextureEditorData()->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        GetTextureEditorData()->PaintEngine()->SetLock(GetTextureEditorData()->LayerStack()->GetCurrentLayer()->IsLocked(true) || !GetTextureEditorData()->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyTextureEditorController::OnLayerStackStructureChanged()
{
    GetTextureEditorData()->Texture()->MarkPackageDirty();

    if( GetTextureEditorData()->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( GetTextureEditorData()->LayerStack() );
        GetTextureEditorData()->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
    }
}

void
FOdysseyTextureEditorController::OnLayerStackImageResultChanged()
{
    GetTextureEditorData()->PaintEngine()->Flush();
    GetTextureEditorData()->Texture()->MarkPackageDirty();
    GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
	GetTextureEditorData()->DisplaySurface()->Invalidate();
}

void
FOdysseyTextureEditorController::OnExportLayersAsTextures()
{
    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ExportLayerDialogTitle", "Export Layers As Texture" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath( GetTextureEditorData()->Texture()->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = GetTextureEditorData()->Texture()->GetName();
    saveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if( saveObjectPath != "" )
    {
        TArray< TSharedPtr<IOdysseyLayer> > layers;
		GetTextureEditorData()->LayerStack()->GetLayerRoot()->DepthFirstSearchTree( &layers, false );

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
            InitTextureWithBlockData(imageLayer->GetBlock(), object, GetTextureEditorData()->Texture()->Source.GetFormat());

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
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(GetTextureEditorData()->Texture()->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );

    for( int i = 0; i < assetsData.Num(); i++ )
    {
        UTexture2D* openedTexture = static_cast<UTexture2D*>( assetsData[i].GetAsset() );
        FOdysseyBlock* textureBlock = NewOdysseyBlockFromUTextureData( openedTexture, GetTextureEditorData()->LayerStack()->Format() );
		TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(FName(*(openedTexture->GetName())), textureBlock));
		GetTextureEditorData()->LayerStack()->AddLayer( imageLayer );
    }

    mGUI->GetLayerStackTab()->RefreshView();
	
	GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
	GetTextureEditorData()->DisplaySurface()->Invalidate();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Performance Handlers

FReply
FOdysseyTextureEditorController::OnClear()
{
    if( GetTextureEditorData()->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
    GetTextureEditorData()->LayerStack()->mDrawingUndo->StartRecord();
	GetTextureEditorData()->LayerStack()->mDrawingUndo->SaveData( 0, 0, GetTextureEditorData()->LayerStack()->Width(), GetTextureEditorData()->LayerStack()->Height() );
	GetTextureEditorData()->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord
	
    FOdysseyPainterEditorController::OnClear();

	GetTextureEditorData()->LayerStack()->ClearCurrentLayer();
    
	GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
	GetTextureEditorData()->DisplaySurface()->Invalidate();
    return FReply::Handled();
}

FReply
FOdysseyTextureEditorController::OnFill()
{
    if( GetTextureEditorData()->LayerStack()->GetCurrentLayer() == NULL )
        return FReply::Handled();

    //Record
	GetTextureEditorData()->LayerStack()->mDrawingUndo->StartRecord();
	GetTextureEditorData()->LayerStack()->mDrawingUndo->SaveData( 0, 0, GetTextureEditorData()->LayerStack()->Width(), GetTextureEditorData()->LayerStack()->Height() );
	GetTextureEditorData()->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord

    FOdysseyPainterEditorController::OnFill();

	GetTextureEditorData()->LayerStack()->FillCurrentLayerWithColor(GetTextureEditorData()->PaintEngine()->GetColor() );
	GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
	GetTextureEditorData()->DisplaySurface()->Invalidate();

    return FReply::Handled();
}

FReply
FOdysseyTextureEditorController::OnClearUndo()
{
	GetTextureEditorData()->LayerStack()->mDrawingUndo->Clear();
    return FOdysseyPainterEditorController::OnClearUndo();
}

FReply
FOdysseyTextureEditorController::OnUndoIliad()
{
    FOdysseyPainterEditorController::OnUndoIliad();
	GetTextureEditorData()->LayerStack()->mDrawingUndo->LoadData();
	GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
	GetTextureEditorData()->DisplaySurface()->Invalidate();

    return FReply::Handled();
}


FReply
FOdysseyTextureEditorController::OnRedoIliad()
{
    FOdysseyPainterEditorController::OnRedoIliad();
	GetTextureEditorData()->LayerStack()->mDrawingUndo->Redo();
	GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
	GetTextureEditorData()->DisplaySurface()->Invalidate();

    return FReply::Handled();
}

void
FOdysseyTextureEditorController::OnCreateNewLayer()
{
    FOdysseyPainterEditorController::OnCreateNewLayer();

    FName name = GetTextureEditorData()->LayerStack()->GetLayerRoot()->GetNextLayerName();
    int w = GetTextureEditorData()->LayerStack()->Width();
    int h = GetTextureEditorData()->LayerStack()->Height();
    ::ul3::tFormat format = GetTextureEditorData()->LayerStack()->Format();
	TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(name, FVector2D(w, h), format));
    GetTextureEditorData()->LayerStack()->AddLayer(imageLayer, 0);
	GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
	GetTextureEditorData()->DisplaySurface()->Invalidate();
    mGUI->GetLayerStackTab()->RefreshView();
}

void
FOdysseyTextureEditorController::OnDuplicateCurrentLayer()
{
    FOdysseyPainterEditorController::OnDuplicateCurrentLayer();

    if( GetTextureEditorData()->LayerStack()->GetCurrentLayer() )
    {
        GetTextureEditorData()->LayerStack()->DuplicateLayer( GetTextureEditorData()->LayerStack()->GetCurrentLayer() );
        GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
		GetTextureEditorData()->DisplaySurface()->Invalidate();
        mGUI->GetLayerStackTab()->RefreshView();
    }
}

void
FOdysseyTextureEditorController::OnDeleteCurrentLayer()
{
    FOdysseyPainterEditorController::OnDeleteCurrentLayer();


    if( GetTextureEditorData()->LayerStack()->GetCurrentLayer() )
    {
		FText Title = LOCTEXT("TitleDeletingCurrentLayer", "Deleting current layer");
        if( FMessageDialog::Open(EAppMsgType::OkCancel,  LOCTEXT("DeletingCurrentLayer", "Are you sure you want to delete this layer ?" ), &Title ) == EAppReturnType::Ok )
        {
            GetTextureEditorData()->LayerStack()->DeleteLayer( GetTextureEditorData()->LayerStack()->GetCurrentLayer() );
            GetTextureEditorData()->LayerStack()->ComputeResultInBlock(GetTextureEditorData()->DisplaySurface()->Block()->GetBlock());
			GetTextureEditorData()->DisplaySurface()->Invalidate();
            mGUI->GetLayerStackTab()->RefreshView();
        }
    }
}

TSharedPtr<FOdysseyTextureEditorData>
FOdysseyTextureEditorController::GetTextureEditorData()
{
    return mEditor->GetData();
}

TSharedPtr<FOdysseyPainterEditorData>
FOdysseyTextureEditorController::GetData()
{
    return mEditor->GetData();
}

TSharedPtr<FOdysseyPainterEditorGUI>
FOdysseyTextureEditorController::GetGUI()
{
    return mGUI;
}

#undef LOCTEXT_NAMESPACE

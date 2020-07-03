// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTextureEditorController.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "Models/OdysseyTextureEditorCommands.h"
#include "SOdysseyAboutScreen.h"
#include "OdysseyBrushAssetBase.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorController
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorController::~FOdysseyTextureEditorController()
{
    mData->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);

    TArray<IOdysseyLayer*> layersArray = TArray<IOdysseyLayer*>();
    mData->LayerStack()->GetLayers()->DepthFirstSearchTree( &layersArray, false );

    for( int i = 0; i < layersArray.Num(); i++ )
    {
        layersArray[i]->OnIsLockedChanged().RemoveAll(this);
        layersArray[i]->OnIsVisibleChanged().RemoveAll(this);
    
        if (layersArray[i]->GetType() == IOdysseyLayer::eType::kImage) 
        {
    		FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(layersArray[i]);
            imageLayer->OnIsAlphaLockedChanged().RemoveAll(this);
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
	// Add Menu Extender
    GetMenuExtenders().Add(CreateMenuExtenders(iToolkitCommands));

    // Init Painter Editor
    FOdysseyPainterEditorController::InitOdysseyPainterEditorController(iToolkitCommands);

    // Register our commands. This will only register them if not previously registered
    FOdysseyTextureEditorCommands::Register();

    // Bind each command to its function
    BindCommands(iToolkitCommands);

	// Set LayerStack CB
    if( !(mData->LayerStack()->OnCurrentLayerChanged().IsBound()) )
	    mData->LayerStack()->OnCurrentLayerChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerStackCurrentLayerChanged);

    if( !mData->LayerStack()->GetCurrentLayer() )
        return;
    	
    // Set Image Layer in PaintEngine
	IOdysseyLayer* layer = mData->LayerStack()->GetCurrentLayer()->GetNodeContent();

    if( layer )
    {
        if( !(layer->OnIsLockedChanged().IsBound()) )
            layer->OnIsLockedChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerIsLockedChanged);

        if( !(layer->OnIsVisibleChanged().IsBound()) )
            layer->OnIsVisibleChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerIsVisibleChanged);
    }

	if (layer->GetType() == IOdysseyLayer::eType::kImage) {
		FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(mData->LayerStack()->GetCurrentLayer()->GetNodeContent());
		if (imageLayer) {
			mData->PaintEngine()->Block(imageLayer->GetBlock());

			//Add Image Layer Callback
            if( !(imageLayer->OnIsAlphaLockedChanged().IsBound()) )
			    imageLayer->OnIsAlphaLockedChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerIsAlphaLockedChanged);
		}
	}
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyTextureEditorController::BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands)
{
	iToolkitCommands->MapAction(
        FOdysseyTextureEditorCommands::Get().ImportTexturesAsLayers,
        FExecuteAction::CreateSP( this, &FOdysseyTextureEditorController::OnImportTexturesAsLayers ),
        FCanExecuteAction() );

	iToolkitCommands->MapAction(
        FOdysseyTextureEditorCommands::Get().ExportLayersAsTextures,
        FExecuteAction::CreateSP( this, &FOdysseyTextureEditorController::OnExportLayersAsTextures ),
        FCanExecuteAction() );
}

TSharedPtr<FExtender>
FOdysseyTextureEditorController::CreateMenuExtenders(const TSharedRef<FUICommandList>& iToolkitCommands)
{
    FExtender* extender = new FExtender();

	extender->AddMenuExtension(
        "FileLoadAndSave",
        EExtensionHook::After,
		iToolkitCommands,
        FMenuExtensionDelegate::CreateStatic< FOdysseyTextureEditorController& >( &FOdysseyTextureEditorController::FillImportExportMenu, *this) );

    return MakeShareable(extender);
}

//static
void
FOdysseyTextureEditorController::FillImportExportMenu( FMenuBuilder& ioMenuBuilder, FOdysseyTextureEditorController& iOdysseyTextureEditor )
{
    ioMenuBuilder.BeginSection( "FileOdysseyTexture", LOCTEXT( "OdysseyTexture", "OdysseyTexture" ) );
    {
        ioMenuBuilder.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ImportTexturesAsLayers );
        ioMenuBuilder.AddMenuEntry( FOdysseyTextureEditorCommands::Get().ExportLayersAsTextures );
    }
}

void
FOdysseyTextureEditorController::OnPaintEngineStrokeChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    FOdysseyPainterEditorController::OnPaintEngineStrokeChanged(iChangedTiles);
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		mData->LayerStack()->ComputeResultBlockWithTempBlock(iChangedTiles[i], mData->PaintEngine()->TempBlock());
	}
}

void
FOdysseyTextureEditorController::OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
    FOdysseyPainterEditorController::OnPaintEngineStrokeWillEnd(iChangedTiles);
	mData->LayerStack()->mDrawingUndo->StartRecord();
	for (int i = 0; i < iChangedTiles.Num(); i++)
	{
		mData->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
	}
}

void
FOdysseyTextureEditorController::OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
    FOdysseyPainterEditorController::OnPaintEngineStrokeEnd(iChangedTiles);
	mData->LayerStack()->mDrawingUndo->EndRecord();
	mData->LayerStack()->ComputeResultBlock();
}

void
FOdysseyTextureEditorController::OnPaintEngineStrokeAbort()
{
    FOdysseyPainterEditorController::OnPaintEngineStrokeAbort();
	mData->LayerStack()->ComputeResultBlock();
}

void
FOdysseyTextureEditorController::OnLayerStackCurrentLayerChanged(FOdysseyNTree< IOdysseyLayer* >* iCurrentLayer)
{
	//Add Image Layer Callback
	//imageLayer->OnIsAlphaLockedChanged().AddRaw(this, &FOdysseyTextureEditorController::OnCurrentLayerIsAlphaLockedChanged);
    if( mData->LayerStack()->GetCurrentLayer() == NULL )
    {
		mData->PaintEngine()->Block(NULL);
        return;
    }

	IOdysseyLayer* layer = mData->LayerStack()->GetCurrentLayer()->GetNodeContent();

    if( layer )
    {
        OnLayerIsLockedChanged(layer);
        if( !(layer->OnIsLockedChanged().IsBound()) )
            layer->OnIsLockedChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerIsLockedChanged);
       
        OnLayerIsVisibleChanged(layer);
        if( !(layer->OnIsVisibleChanged().IsBound()) )
            layer->OnIsVisibleChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerIsVisibleChanged);
    }

	if (layer->GetType() != IOdysseyLayer::eType::kImage) {
		mData->PaintEngine()->Block(NULL);
		return;
	}

	FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*>(mData->LayerStack()->GetCurrentLayer()->GetNodeContent());
	if (imageLayer) {
		mData->PaintEngine()->Block(imageLayer->GetBlock());

        OnLayerIsAlphaLockedChanged(imageLayer);
        if( !(imageLayer->OnIsAlphaLockedChanged().IsBound()) )
		    imageLayer->OnIsAlphaLockedChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerIsAlphaLockedChanged);
	}
}

void
FOdysseyTextureEditorController::OnLayerIsAlphaLockedChanged(FOdysseyImageLayer* iLayer)
{
    if( iLayer == mData->LayerStack()->GetCurrentLayer()->GetNodeContent() )
    {
	    if (iLayer->IsAlphaLocked())
	    {
		    mData->PaintEngine()->SetAlphaModeModifier(::ul3::AM_BACK);
	    }
	    else
	    {
		    mData->PaintEngine()->SetAlphaModeModifier(mData->AlphaMode());
	    }
    }
}

void
FOdysseyTextureEditorController::OnLayerIsLockedChanged(IOdysseyLayer* iLayer)
{
    //simple case. If the current layer can't be drawn on, we just tell the paint engine to not draw, and be done with it.
    bool allowedToPaint = mData->LayerStack()->GetCurrentLayer()->GetNodeContent()->IsVisible() && !mData->LayerStack()->GetCurrentLayer()->GetNodeContent()->IsLocked();
    if( !allowedToPaint )
    {
        mData->PaintEngine()->SetIsAllowedToPaint(false);
        return;
    }

    //harder case. If we can draw on the current layer, we check all parent folders of the current layer
    FOdysseyNTree< IOdysseyLayer* >* checkedNode = mData->LayerStack()->GetCurrentLayer()->GetParent();
    while( checkedNode->GetParent() != NULL ) //If parent is null, we're at the root of the layer stack
    {
        allowedToPaint = checkedNode->GetNodeContent()->IsVisible() && !checkedNode->GetNodeContent()->IsLocked();
        if( !allowedToPaint )
        {
            mData->PaintEngine()->SetIsAllowedToPaint(false);
            return;
        }
        checkedNode = checkedNode->GetParent();
    }

    mData->PaintEngine()->SetIsAllowedToPaint(true);
}

void
FOdysseyTextureEditorController::OnLayerIsVisibleChanged(IOdysseyLayer* iLayer)
{
    //simple case. If the current layer can't be drawn on, we just tell the paint engine to not draw, and be done with it.
    bool allowedToPaint = mData->LayerStack()->GetCurrentLayer()->GetNodeContent()->IsVisible() && !mData->LayerStack()->GetCurrentLayer()->GetNodeContent()->IsLocked();
    if( !allowedToPaint )
    {
        mData->PaintEngine()->SetIsAllowedToPaint(false);
        return;
    }

    //harder case. If we can draw on the current layer, we check all parent folders of the current layer
    FOdysseyNTree< IOdysseyLayer* >* checkedNode = mData->LayerStack()->GetCurrentLayer()->GetParent();
    while( checkedNode->GetParent() != NULL ) //If parent is null, we're at the root of the layer stack
    {
        allowedToPaint = checkedNode->GetNodeContent()->IsVisible() && !checkedNode->GetNodeContent()->IsLocked();
        if( !allowedToPaint )
        {
            mData->PaintEngine()->SetIsAllowedToPaint(false);
            return;
        }
        checkedNode = checkedNode->GetParent();
    }

    mData->PaintEngine()->SetIsAllowedToPaint(true);
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
            object->Source.Init( imageLayer->GetBlock()->Width(), imageLayer->GetBlock()->Height(), 1, 1, TSF_BGRA8 );
            object->PostEditChange();

            CopyBlockDataIntoUTexture( imageLayer->GetBlock(), object );

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
        FOdysseyBlock* textureBlock = NewOdysseyBlockFromUTextureData( openedTexture );
		mData->LayerStack()->AddImageLayerFromData( textureBlock, mData->LayerStack()->GetLayers(), FName( *( openedTexture->GetName() ) ) );
        delete textureBlock;
    }

    mGUI->GetLayerStackTab()->RefreshView();
	mData->LayerStack()->ComputeResultBlock();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- Paint engine driving methods

void
FOdysseyTextureEditorController::HandleAlphaModeModifierChanged( int32 iValue )
{
    if( !(mData->LayerStack()->GetCurrentLayer()) )
        return;

    return FOdysseyPainterEditorController::HandleAlphaModeModifierChanged(iValue);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Performance Handlers

void
FOdysseyTextureEditorController::HandlePerformanceLiveUpdateChanged( bool iValue )
{
    mData->LiveUpdateInfo()->enabled = iValue;
    mData->DisplaySurface()->Invalidate();
}

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
    InvalidateTextureFromData(mData->LayerStack()->GetResultBlock(), mData->Texture());
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
    return FReply::Handled();
}

FReply
FOdysseyTextureEditorController::OnClearUndo()
{
	mData->LayerStack()->mDrawingUndo->Clear();
    return FOdysseyPainterEditorController::OnClearUndo();
}

void
FOdysseyTextureEditorController::UndoIliad()
{
    FOdysseyPainterEditorController::UndoIliad();
	mData->LayerStack()->mDrawingUndo->LoadData();
}


void
FOdysseyTextureEditorController::RedoIliad()
{
    FOdysseyPainterEditorController::RedoIliad();
	mData->LayerStack()->mDrawingUndo->Redo();
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

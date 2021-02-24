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

#include "OdysseyPainterEditorTopTab.h"
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

    if (!(GetEditor()->PaintEngine()->OnStrokeWillEnd().IsBoundToObject(this)))
        GetEditor()->PaintEngine()->OnStrokeWillEnd().AddRaw(this, &FOdysseyFlipbookEditorController::OnPaintEngineStrokeWillEnd);

    if (!(GetEditor()->PaintEngine()->OnStrokeEnd().IsBoundToObject(this)))
        GetEditor()->PaintEngine()->OnStrokeEnd().AddRaw(this, &FOdysseyFlipbookEditorController::OnPaintEngineStrokeEnd);

    GetEditor()->PaintEngine()->IsLocked(TAttribute<bool>(this, &FOdysseyFlipbookEditorController::PaintEngineIsLocked));

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

    /*if( !(GetEditor()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().IsBoundToObject(this)) )
	    GetEditor()->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerIsLockedChanged);
        
    if( !(GetEditor()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().IsBoundToObject(this)) )
	    GetEditor()->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnLayerIsVisibleChanged); */
    	
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

/*

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
} */

/* void
FOdysseyFlipbookEditorController::OnPaintEngineStrokeAbort()
{
    check(GetEditor()->LayerStack());
    FOdysseyPainterEditorController::OnPaintEngineStrokeAbort();
	GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock());
    GetEditor()->DisplaySurface()->Invalidate();
	//TODO: Check how to abort undo recording
} */

void
FOdysseyFlipbookEditorController::OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
    if (!GetEditor()->LayerStack())
        return;

    //TODO: Manage Undo directly in PaintEngine
    GetEditor()->LayerStack()->mDrawingUndo->StartRecord();
    for (int i = 0; i < iChangedTiles.Num(); i++)
    {
        GetEditor()->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
        // GetEditor()->LayerStack()->BlendOnCurrentLayer(GetEditor()->PaintEngine()->TempBuffer(), iChangedTiles[i], GetEditor()->PaintEngine()->GetOpacity(), GetEditor()->PaintEngine()->GetBlendingMode(), GetEditor()->PaintEngine()->GetAlphaMode());
    }
    GetEditor()->LayerStack()->mDrawingUndo->EndRecord();
}

void
FOdysseyFlipbookEditorController::OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
    if (!GetEditor()->Texture())
        return;

    //TODO: Move To Editor
    GetEditor()->Texture()->MarkPackageDirty();
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
            GetEditor()->PaintEngine()->SetAlphaModeModifier(GetEditor()->SelectedAlphaMode());
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
    GetEditor()->PaintEngine()->SetAlphaModeModifier(imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : GetEditor()->SelectedAlphaMode());
    
    //Set AlphaLock Delegate
    imageLayer->IsAlphaLockedChangedDelegate().AddRaw(this, &FOdysseyFlipbookEditorController::OnCurrentLayerIsAlphaLockedChanged);
    // GetEditor()->PaintEngine()->SetLock( imageLayer->IsLocked(true) || !imageLayer->IsVisible(true) );
}

void
FOdysseyFlipbookEditorController::OnCurrentLayerIsAlphaLockedChanged(bool iOldValue)
{
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetEditor()->LayerStack()->GetCurrentLayer());
    GetEditor()->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : GetEditor()->SelectedAlphaMode());
}

/* void
FOdysseyFlipbookEditorController::HandleAlphaModeModifierChanged( int32 iValue )
{
    if (!GetEditor()->LayerStack())
        return;

    if (!GetEditor()->LayerStack()->GetCurrentLayer())
        return;

    if( !( GetEditor()->LayerStack()->GetCurrentLayer()->GetType() == IOdysseyLayer::eType::kImage ) )
        return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetEditor()->LayerStack()->GetCurrentLayer());
    GetEditor()->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetTopTab()->PaintModifiers()->GetAlphaMode());
} */

/* void
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
} */

void
FOdysseyFlipbookEditorController::OnLayerStackStructureChanged()
{
    if (GetEditor()->Texture())
    {
        GetEditor()->Texture()->MarkPackageDirty();
    }

    if( GetEditor()->PaintEngine()->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( GetEditor()->LayerStack() );
        GetEditor()->PaintEngine()->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
    }
}

void
FOdysseyFlipbookEditorController::OnLayerStackImageResultChanged(const ::ul3::FRect& iRect)
{
    if (GetEditor()->Texture())
    {
        // GetEditor()->PaintEngine()->Flush();
        GetEditor()->Texture()->MarkPackageDirty();
        GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock(), iRect);
        GetEditor()->DisplaySurface()->Invalidate(iRect);
    }
}

FOdysseyFlipbookEditor*
FOdysseyFlipbookEditorController::GetEditor() const
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

bool
FOdysseyFlipbookEditorController::PaintEngineIsLocked() const
{
    if (!GetEditor()->LayerStack())
        return false;

    TSharedPtr<IOdysseyLayer> layer = GetEditor()->LayerStack()->GetCurrentLayer();
    if (!layer)
        return false;

    return layer->IsLocked(true) || !layer->IsVisible(true);
}

#undef LOCTEXT_NAMESPACE

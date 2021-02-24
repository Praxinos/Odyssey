// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorController.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "Models/OdysseyTextureEditorCommands.h"
#include "SOdysseyAboutScreen.h"
#include "OdysseyBrushAssetBase.h"
#include "TextureEditor/OdysseyTextureEditorState.h"
#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyPainterEditorTopTab.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorController
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorController::~FOdysseyTextureEditorController()
{
    GetEditor()->TextureWrapper().OnPreTextureChangeDelegate().RemoveAll(this);
    GetEditor()->TextureWrapper().OnPostTextureChangeDelegate().RemoveAll(this);
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
    // Bind each command to its function
    BindCommands(mEditor->Toolkit()->GetToolkitCommands());

    if ( !(GetEditor()->TextureWrapper().OnPreTextureChangeDelegate().IsBoundToObject(this)) )
        GetEditor()->TextureWrapper().OnPreTextureChangeDelegate().AddRaw(this, &FOdysseyTextureEditorController::OnPreTextureChange);

    if ( !(GetEditor()->TextureWrapper().OnPostTextureChangeDelegate().IsBoundToObject(this)) )
        GetEditor()->TextureWrapper().OnPostTextureChangeDelegate().AddRaw(this, &FOdysseyTextureEditorController::OnPostTextureChange);

    if (!(GetEditor()->PaintEngine()->OnStrokeWillEnd().IsBoundToObject(this)))
        GetEditor()->PaintEngine()->OnStrokeWillEnd().AddRaw(this, &FOdysseyTextureEditorController::OnPaintEngineStrokeWillEnd);

    if (!(GetEditor()->PaintEngine()->OnStrokeEnd().IsBoundToObject(this)))
        GetEditor()->PaintEngine()->OnStrokeEnd().AddRaw(this, &FOdysseyTextureEditorController::OnPaintEngineStrokeEnd);

    GetEditor()->PaintEngine()->IsLocked(TAttribute<bool>(this, &FOdysseyTextureEditorController::PaintEngineIsLocked));

    //Make like if the texture changed, to set all callbacks correctly
    OnPostTextureChange(nullptr);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyTextureEditorController::BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands)
{
}

void
FOdysseyTextureEditorController::OnPreTextureChange(UTexture2D* iNewTexture)
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
FOdysseyTextureEditorController::OnPostTextureChange(UTexture2D* iOldTexture)
{
    if (!GetEditor()->LayerStack())
        return;

    if( !(GetEditor()->LayerStack()->OnCurrentLayerChanged().IsBoundToObject(this)) )
	    GetEditor()->LayerStack()->OnCurrentLayerChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerStackCurrentLayerChanged);

    if( !(GetEditor()->LayerStack()->OnStructureChanged().IsBoundToObject(this)) )
	    GetEditor()->LayerStack()->OnStructureChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerStackStructureChanged);

    if( !(GetEditor()->LayerStack()->OnImageResultChanged().IsBoundToObject(this)) )
	    GetEditor()->LayerStack()->OnImageResultChanged().AddRaw(this, &FOdysseyTextureEditorController::OnLayerStackImageResultChanged);
    	
    
    if (GetEditor()->LayerStack()->GetLayerRoot() == GetEditor()->LayerStack()->GetCurrentLayer())
    {
        // Set Image Layer as the current Layer
        TArray<TSharedPtr<IOdysseyLayer>> layers;
        GetEditor()->LayerStack()->GetLayerRoot()->DepthFirstSearchTree(&layers, false);

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
        //Call this anyway because, as we changed of texture, we naturally also changed of layerstack
        OnLayerStackCurrentLayerChanged(nullptr);
    }
}

void
FOdysseyTextureEditorController::OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
    if (!GetEditor()->LayerStack())
        return;
    //TODO: Manage Undo directly in PaintEngine
    GetEditor()->LayerStack()->mDrawingUndo->StartRecord();
    for (int i = 0; i < iChangedTiles.Num(); i++)
    {
        GetEditor()->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
    }
    GetEditor()->LayerStack()->mDrawingUndo->EndRecord();
}

void
FOdysseyTextureEditorController::OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
    if (!GetEditor()->Texture())
        return;
    //TODO: Move To Editor
    GetEditor()->Texture()->MarkPackageDirty();
}

void
FOdysseyTextureEditorController::OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue)
{
    //TODO: Find a good way to sync PaintEngine to LayerStack and other foreign parameters

    //Remove all delegates for AlphaLock and set alpha lock to a default value
    if (iOldValue && iOldValue->GetType() == IOdysseyLayer::eType::kImage) {
        TSharedPtr<FOdysseyImageLayer> oldImageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(iOldValue);
        if (oldImageLayer)
        {
            oldImageLayer->IsAlphaLockedChangedDelegate().RemoveAll(this);
            GetEditor()->PaintEngine()->SetAlphaModeModifier(mEditor->SelectedAlphaMode());
        }
	}
    GetEditor()->PaintEngine()->Block(NULL);

	//Add Image Layer Callback
    if( GetEditor()->LayerStack()->GetCurrentLayer() == NULL )
        return;

	TSharedPtr<IOdysseyLayer> layer = GetEditor()->LayerStack()->GetCurrentLayer();
	if (layer->GetType() != IOdysseyLayer::eType::kImage)
		return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetEditor()->LayerStack()->GetCurrentLayer());
	if (!imageLayer) 
        return;

    GetEditor()->PaintEngine()->Block(imageLayer->GetBlock());
    GetEditor()->PaintEngine()->SetAlphaModeModifier(imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : mEditor->SelectedAlphaMode());
    
    //Set AlphaLock Delegate
    imageLayer->IsAlphaLockedChangedDelegate().AddRaw(this, &FOdysseyTextureEditorController::OnCurrentLayerIsAlphaLockedChanged);
}

void
FOdysseyTextureEditorController::OnCurrentLayerIsAlphaLockedChanged(bool iOldValue)
{
    //TODO: Find a good way to sync PaintEngine to LayerStack and other foreign parameters
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(GetEditor()->LayerStack()->GetCurrentLayer());
    GetEditor()->PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mEditor->SelectedAlphaMode());
}

void
FOdysseyTextureEditorController::OnLayerStackStructureChanged()
{
    //TODO: Move to TextureWrapper
    GetEditor()->Texture()->MarkPackageDirty();

    //TODO: Remove when States use Editor
    if( GetEditor()->PaintEngine()->BrushInstance() )
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState( GetEditor()->LayerStack() );
        GetEditor()->PaintEngine()->BrushInstance()->AddOrReplaceState( FOdysseyTextureEditorState::GetId(), layer_state );
    }
}

void
FOdysseyTextureEditorController::OnLayerStackImageResultChanged(const ::ul3::FRect& iRect)
{
    //TODO: Move to TextureWrapper
    GetEditor()->Texture()->MarkPackageDirty();
    GetEditor()->LayerStack()->ComputeResultInBlock(GetEditor()->DisplaySurface()->Block()->GetBlock(), iRect);
	GetEditor()->DisplaySurface()->Invalidate(iRect);
}

bool
FOdysseyTextureEditorController::PaintEngineIsLocked() const
{
    if (!GetEditor()->LayerStack())
        return false;

    TSharedPtr<IOdysseyLayer> layer = GetEditor()->LayerStack()->GetCurrentLayer();
    if (!layer)
        return false;

    return layer->IsLocked(true) || !layer->IsVisible(true);
}

FOdysseyTextureEditor*
FOdysseyTextureEditorController::GetEditor() const
{
    return mEditor;
}

TSharedPtr<FOdysseyPainterEditorGUI>
FOdysseyTextureEditorController::GetGUI()
{
    return mGUI;
}

#undef LOCTEXT_NAMESPACE

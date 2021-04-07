// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditor.h"

#include "OdysseyBrushAssetBase.h"
#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyTextureEditorDrawingState.h"


#define LOCTEXT_NAMESPACE "OdysseyTextureEditor"

/////////////////////////////////////////////////////
// FOdysseyTextureEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditor::~FOdysseyTextureEditor()
{
}

FOdysseyTextureEditor::FOdysseyTextureEditor() :
	FOdysseyPainterEditor(),
	mSelectedAlphaMode(::ul3::AM_NORMAL)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyTextureEditor::InitData()
{
	FOdysseyPainterEditor::InitData();

	//--- Init Data

    FOdysseyTextureEditorDrawingState* drawingState = new FOdysseyTextureEditorDrawingState(this);
	
    TAttribute<bool> paintEngineIsLockedAttr;
    paintEngineIsLockedAttr.BindRaw(this, &FOdysseyTextureEditor::PaintEngineIsLocked);
    PaintEngine()->IsLocked(paintEngineIsLockedAttr);
    PaintEngine()->AddDrawingState(drawingState);

    //Make like if the texture changed, to set all callbacks correctly
    OnPostTextureChange();

	//--- Init Listeners

    TextureWrapper()->OnPreTextureChangeDelegate().AddRaw(this, &FOdysseyTextureEditor::OnPreTextureChange);
    TextureWrapper()->OnPostTextureChangeDelegate().AddRaw(this, &FOdysseyTextureEditor::OnPostTextureChange);
    PaintEngine()->OnPaintEnd().AddRaw(this, &FOdysseyTextureEditor::OnPaintEnginePaintEnd);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Undo

void
FOdysseyTextureEditor::Undo()
{
    FOdysseyPainterEditor::Undo();

    if(!LayerStack())
        return;

    if (!LayerStack()->mDrawingUndo->HasUndo())
        return;
    
	LayerStack()->mDrawingUndo->LoadData();
}

void
FOdysseyTextureEditor::Redo()
{
    FOdysseyPainterEditor::Redo();

    if(!LayerStack())
        return;

    if (!LayerStack()->mDrawingUndo->HasRedo())
        return;
    
	LayerStack()->mDrawingUndo->Redo();
}

void
FOdysseyTextureEditor::ClearUndo()
{
    if(!LayerStack())
        return;
    
	LayerStack()->mDrawingUndo->Clear();
    FOdysseyPainterEditor::ClearUndo();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UTexture*
FOdysseyTextureEditor::Texture() const
{
    return nullptr;
}

FOdysseyTextureWrapper*
FOdysseyTextureEditor::TextureWrapper() const
{
    return nullptr;
}

IOdysseySurfaceEditable*
FOdysseyTextureEditor::DisplaySurface() const
{
	return TextureWrapper()->Surface();
}

FOdysseyLayerStack*
FOdysseyTextureEditor::LayerStack() const
{
    return TextureWrapper()->LayerStack();
}

::ul3::eAlphaMode
FOdysseyTextureEditor::SelectedAlphaMode() const
{
	return mSelectedAlphaMode;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyTextureEditor::SelectedAlphaMode(::ul3::eAlphaMode iMode)
{
	mSelectedAlphaMode = iMode;

	//Make sure we set the right value in the Paint Engine according to the editor state
    if (!LayerStack())
        return;

    if (!LayerStack()->GetCurrentLayer())
        return;

    if (LayerStack()->GetCurrentLayer()->GetType() != IOdysseyLayer::eType::kImage)
        return;

    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(LayerStack()->GetCurrentLayer());
    if (imageLayer && imageLayer->IsAlphaLocked())
    {
        PaintEngine()->SetAlphaModeModifier(::ul3::AM_BACK);
    }
    else
    {
		PaintEngine()->SetAlphaModeModifier(mSelectedAlphaMode);
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

bool
FOdysseyTextureEditor::OnCloseRequested()
{
	FOdysseyPainterEditor::OnCloseRequested();

    //TODO: Move in the right place
    if (LayerStack())
        LayerStack()->mDrawingUndo->Clear();

    TextureWrapper()->Finalize();
	return true;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Attributes

bool
FOdysseyTextureEditor::PaintEngineIsLocked() const
{
	FOdysseyLayerStack* layerstack = LayerStack();
    if (!layerstack)
		return true;

    TSharedPtr<IOdysseyLayer> layer = layerstack->GetCurrentLayer();
    if (!layer)
        return true;

    return layer->IsLocked(true) || !layer->IsVisible(true);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Listeners

void
FOdysseyTextureEditor::OnPreTextureChange()
{
	FOdysseyLayerStack* layerstack = LayerStack();
    if (!layerstack)
		return;

	//Remove delegates
    layerstack->OnCurrentLayerChanged().RemoveAll(this);
    layerstack->OnStructureChanged().RemoveAll(this);
    layerstack->OnImageResultChanged().RemoveAll(this);

    TSharedPtr<IOdysseyLayer> layer = layerstack->GetCurrentLayer();
    if ( layer && layer->GetType() == IOdysseyLayer::eType::kImage)
    {
        TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(layerstack->GetCurrentLayer());
        if (imageLayer)
        {
            //Set AlphaLock Delegate
            imageLayer->IsAlphaLockedChangedDelegate().RemoveAll(this);
        }
    }
}

void
FOdysseyTextureEditor::OnPostTextureChange()
{
    FOdysseyLayerStack* layerstack = LayerStack();
    if (!layerstack)
		return;

	//Add Delegates
    layerstack->OnCurrentLayerChanged().AddRaw(this, &FOdysseyTextureEditor::OnLayerStackCurrentLayerChanged);
    layerstack->OnStructureChanged().AddRaw(this, &FOdysseyTextureEditor::OnLayerStackStructureChanged);
    layerstack->OnImageResultChanged().AddRaw(this, &FOdysseyTextureEditor::OnLayerStackImageResultChanged);
    	  
    if ( layerstack->GetCurrentLayer() == layerstack->GetLayerRoot() )
    {
        // Set Image Layer as the current Layer
        TArray<TSharedPtr<IOdysseyLayer>> layers;
        layerstack->GetLayerRoot()->DepthFirstSearchTree(&layers, false);

        for (int i = 0; i < layers.Num(); i++)
        {
            if (layers[i]->GetType() != IOdysseyLayer::eType::kImage)
                continue;

            layerstack->SetCurrentLayer(layers[i]);
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
FOdysseyTextureEditor::OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue)
{
    //TODO: Find a good way to sync PaintEngine to LayerStack and other foreign parameters

    //Remove all delegates for AlphaLock and set alpha lock to a default value
    if (iOldValue && iOldValue->GetType() == IOdysseyLayer::eType::kImage) {
        TSharedPtr<FOdysseyImageLayer> oldImageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(iOldValue);
        if (oldImageLayer)
        {
            oldImageLayer->IsAlphaLockedChangedDelegate().RemoveAll(this);
            PaintEngine()->SetAlphaModeModifier(mSelectedAlphaMode);
        }
	}
    PaintEngine()->Block(NULL);

	FOdysseyLayerStack* layerstack = LayerStack();
    if (!layerstack)
		return;

	//Add Image Layer Callback
    if( layerstack->GetCurrentLayer() == NULL )
        return;

	TSharedPtr<IOdysseyLayer> layer = layerstack->GetCurrentLayer();
	if (layer->GetType() != IOdysseyLayer::eType::kImage)
		return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(layerstack->GetCurrentLayer());
	if (!imageLayer) 
        return;

    PaintEngine()->Block(imageLayer->GetBlock());

    //Set AlphaLock Delegate
    PaintEngine()->SetAlphaModeModifier(imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : mSelectedAlphaMode);
    imageLayer->IsAlphaLockedChangedDelegate().AddRaw(this, &FOdysseyTextureEditor::OnCurrentLayerIsAlphaLockedChanged);
}

void
FOdysseyTextureEditor::OnCurrentLayerIsAlphaLockedChanged(bool iOldValue)
{
	FOdysseyLayerStack* layerstack = LayerStack();
    if (!layerstack)
		return;

    //TODO: Find a good way to sync PaintEngine to LayerStack and other foreign parameters
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(layerstack->GetCurrentLayer());
    PaintEngine()->SetAlphaModeModifier( (imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mSelectedAlphaMode);
}

void
FOdysseyTextureEditor::OnLayerStackStructureChanged()
{
    //TODO: Move to TextureWrapper
    Texture()->MarkPackageDirty();
}

void
FOdysseyTextureEditor::OnLayerStackImageResultChanged(const ::ul3::FRect& iRect)
{
	FOdysseyLayerStack* layerstack = LayerStack();
    if (!layerstack)
		return;
		
    //TODO: Move to TextureWrapper
    Texture()->MarkPackageDirty();
    LayerStack()->ComputeResultInBlock(DisplaySurface()->Block()->GetBlock(), iRect);
	DisplaySurface()->Invalidate(iRect);
}

void
FOdysseyTextureEditor::OnPaintEnginePaintEnd(const TArray<::ul3::FRect>& iChangedTiles)
{
    if (iChangedTiles.Num() <= 0)
        return;

    if (!Texture())
        return;

    FOdysseyLayerStack* layerstack = LayerStack();
    if (!layerstack)
        return;

    //TODO: Manage Undo directly in PaintEngine

    //PATCH: To make the Undo work
    //TODO: Remove this when another Undo system will be made
    //As the PaintEngine edits directly the layer block, we need to replace it with the original block to store the changed tiles
    //Then we come back to the layer block
    if (!layerstack->GetCurrentLayer())
        return;

    if (layerstack->GetCurrentLayer()->GetType() != IOdysseyLayer::eType::kImage)
        return;

    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(layerstack->GetCurrentLayer());
    imageLayer->SetBlock(PaintEngine()->OriginalBlock(), false, false);

    layerstack->mDrawingUndo->StartRecord();
    for (int i = 0; i < iChangedTiles.Num(); i++)
    {
        layerstack->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
    }
    layerstack->mDrawingUndo->EndRecord();

    imageLayer->SetBlock(PaintEngine()->EditedBlock(), false, false);
    //TODO: Move To Editor
    Texture()->MarkPackageDirty();
}

#undef LOCTEXT_NAMESPACE
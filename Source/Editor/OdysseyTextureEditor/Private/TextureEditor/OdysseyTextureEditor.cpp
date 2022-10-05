// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureEditor.h"

#include "OdysseyBrushAssetBase.h"
#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBlendParameters.h"
#include "Tools/DrawingTool/OdysseyDrawingTool.h"
#include "BrushContext/OdysseyTextureEditorBrushContext.h"

#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditor"

/////////////////////////////////////////////////////
// FOdysseyTextureEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditor::~FOdysseyTextureEditor()
{
}

FOdysseyTextureEditor::FOdysseyTextureEditor() :
	FOdysseyPainterEditor()
{
    mBrushContexts.Add(new FOdysseyTextureEditorBrushContext(this));
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyTextureEditor::InitData()
{
	FOdysseyPainterEditor::InitData();

	//--- Init Data

    //Make like if the texture changed, to set all callbacks correctly
    OnPostTextureChange();

	//--- Init Listeners

    TextureWrapper()->OnPreTextureChangeDelegate().AddRaw(this, &FOdysseyTextureEditor::OnPreTextureChange);
    TextureWrapper()->OnPostTextureChangeDelegate().AddRaw(this, &FOdysseyTextureEditor::OnPostTextureChange);
    PaintEngine().OnCommitDelegate().AddRaw(this, &FOdysseyTextureEditor::OnPaintEngineCommit);
    PaintEngine().OnPreUpdateDelegate().BindRaw(this, &FOdysseyTextureEditor::OnPaintEnginePreUpdate);
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

    PaintEngine().Reset();
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

    PaintEngine().Reset();
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
    if ( !layer )
        return;

    layer->LockChangedDelegate().RemoveAll( this );
    layer->VisibilityChangedDelegate().RemoveAll( this );
}

void
FOdysseyTextureEditor::OnPostTextureChange()
{
    FOdysseyLayerStack* layerstack = LayerStack();
    if( !layerstack )
        return;

    //Add Delegates
    layerstack->OnCurrentLayerChanged().AddRaw( this, &FOdysseyTextureEditor::OnLayerStackCurrentLayerChanged );
    layerstack->OnStructureChanged().AddRaw( this, &FOdysseyTextureEditor::OnLayerStackStructureChanged );
    layerstack->OnImageResultChanged().AddRaw( this, &FOdysseyTextureEditor::OnLayerStackImageResultChanged );

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
    if (iOldValue)
    {
        iOldValue->LockChangedDelegate().RemoveAll(this);
        iOldValue->VisibilityChangedDelegate().RemoveAll(this);
    }

    PaintEngine().Block(NULL);

	FOdysseyLayerStack* layerstack = LayerStack();
    if (!layerstack)
		return;

	//Add Image Layer Callback
	TSharedPtr<IOdysseyLayer> layer = layerstack->GetCurrentLayer();
    if( !layer )
		return;

    layer->LockChangedDelegate().AddRaw(this, &FOdysseyTextureEditor::OnCurrentLayerLockChanged);
    layer->VisibilityChangedDelegate().AddRaw(this, &FOdysseyTextureEditor::OnCurrentLayerVisibilityChanged);

    if(layer->GetType() != IOdysseyLayer::eType::kImage)
        return;

	TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(layer);
	if (!imageLayer) 
        return;
    
    PaintEngine().Block(imageLayer->GetBlock());
    
    SetSelectedToolDrawingLocked();
}

void
FOdysseyTextureEditor::OnLayerStackStructureChanged()
{
    //TODO: Move to TextureWrapper
    Texture()->MarkPackageDirty();
}

void
FOdysseyTextureEditor::OnLayerStackImageResultChanged( const ::ULIS::FRectI* iRects, const uint32 iNumRects )
{
    FOdysseyLayerStack* layerstack = LayerStack();
    if( !layerstack )
        return;

    //TODO: Move to TextureWrapper
    Texture()->MarkPackageDirty();
    LayerStack()->ComputeResultInBlock( DisplaySurface()->Block(), iRects, iNumRects );
    DisplaySurface()->Invalidate( iRects, iNumRects );
}

void
FOdysseyTextureEditor::OnCurrentLayerLockChanged(bool iOldValue)
{
    SetSelectedToolDrawingLocked();
}

void
FOdysseyTextureEditor::OnCurrentLayerVisibilityChanged(bool iOldValue)
{
    SetSelectedToolDrawingLocked();
}

void
FOdysseyTextureEditor::SetSelectedToolDrawingLocked()
{
    FOdysseyLayerStack* layerstack = LayerStack();
    if (!layerstack)
		return;

    TSharedPtr<IOdysseyLayer> layer = layerstack->GetCurrentLayer();
    if (!layer)
        return;

    if(layer->GetType() != IOdysseyLayer::eType::kImage)
        return;

    Cast<UOdysseyDrawingTool>(mSelectedTool)->IsDrawingLocked(layer->IsLocked(true) || !layer->IsVisible(true));
}

void
FOdysseyTextureEditor::OnPaintEngineCommit(const TArray<::ULIS::FRectI>& iChangedTiles)
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
    ::ULIS::FBlock* block = imageLayer->GetBlock();
    imageLayer->SetBlock(PaintEngine().OriginalBlock(), false, false);

    layerstack->mDrawingUndo->StartRecord();
    layerstack->mDrawingUndo->SaveData(iChangedTiles);
    layerstack->mDrawingUndo->EndRecord();

    imageLayer->SetBlock(block, false, false);
    //TODO: Move To Editor
    Texture()->MarkPackageDirty();
}

FOdysseyBlendParameters
FOdysseyTextureEditor::OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters)
{
    FOdysseyBlendParameters blendParameters = iBlendParameters;

    //Make sure we set the right value in the Paint Engine according to the editor state
    if (!LayerStack())
        return blendParameters;

    if (!LayerStack()->GetCurrentLayer())
        return blendParameters;

    if (LayerStack()->GetCurrentLayer()->GetType() != IOdysseyLayer::eType::kImage)
        return blendParameters;

    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(LayerStack()->GetCurrentLayer());
    if (imageLayer && imageLayer->IsAlphaLocked())
        blendParameters.AlphaMode = EOdysseyAlphaMode(::ULIS::Alpha_Back);

    return blendParameters;
}

#undef LOCTEXT_NAMESPACE
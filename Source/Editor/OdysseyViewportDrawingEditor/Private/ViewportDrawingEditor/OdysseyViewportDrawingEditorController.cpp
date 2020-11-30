// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorController.h"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorController
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorController::~FOdysseyViewportDrawingEditorController()
{
    mData->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);
    mData->LayerStack()->OnStructureChanged().RemoveAll(this);
    mData->LayerStack()->OnImageResultChanged().RemoveAll(this);
    mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().RemoveAll(this);
}

FOdysseyViewportDrawingEditorController::FOdysseyViewportDrawingEditorController( TSharedPtr<FOdysseyViewportDrawingEditorData>& iData, TSharedPtr<SOdysseyViewportDrawingEditorGUI>& iGUI )
    : mData(iData)
    , mGUI(iGUI)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyViewportDrawingEditorController::Init(/*const TSharedRef<FUICommandList>& iToolkitCommands*/)
{
    /*
    mMenuExtenders.Add(CreateMenuExtender(iToolkitCommands));

    // Add Menu Extender
    //GetMenuExtenders().Add(CreateMenuExtenders(iToolkitCommands));

    // Register our commands. This will only register them if not previously registered
    FOdysseyPainterEditorCommands::Register();

    // Build commands
    FOdysseyPainterEditorController::BindCommands(iToolkitCommands);
    */
    //Add PaintEngine Callbacks
    if( mData->Texture() )
    {
        if(!(mData->PaintEngine()->OnPreviewBlockTilesChanged().IsBound()))
            mData->PaintEngine()->OnPreviewBlockTilesChanged().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnPaintEnginePreviewBlockTilesChanged);

        if(!(mData->PaintEngine()->OnEditedBlockTilesWillChange().IsBound()))
            mData->PaintEngine()->OnEditedBlockTilesWillChange().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnPaintEngineEditedBlockTilesWillChange);

        if(!(mData->PaintEngine()->OnEditedBlockTilesChanged().IsBound()))
            mData->PaintEngine()->OnEditedBlockTilesChanged().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnPaintEngineEditedBlockTilesChanged);

        if(!(mData->PaintEngine()->OnStrokeAbort().IsBound()))
            mData->PaintEngine()->OnStrokeAbort().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnPaintEngineStrokeAbort);

        // Set LayerStack CB
        if(!(mData->LayerStack()->OnCurrentLayerChanged().IsBound()))
            mData->LayerStack()->OnCurrentLayerChanged().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnLayerStackCurrentLayerChanged);

        if(!(mData->LayerStack()->OnStructureChanged().IsBound()))
            mData->LayerStack()->OnStructureChanged().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnLayerStackStructureChanged);

        if(!(mData->LayerStack()->OnImageResultChanged().IsBound()))
            mData->LayerStack()->OnImageResultChanged().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnLayerStackImageResultChanged);

        if(!(mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().IsBound()))
            mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnLayerIsLockedChanged);

        // Set Image Layer as the current Layer
        TArray<TSharedPtr<IOdysseyLayer>> layers;
        mData->LayerStack()->GetLayerRoot()->DepthFirstSearchTree(&layers,false);

        for(int i = 0; i < layers.Num(); i++)
        {
            if(layers[i]->GetType() != IOdysseyLayer::eType::kImage)
                continue;

            mData->LayerStack()->SetCurrentLayer(layers[i]);
            break;
        }
    }
}

void
FOdysseyViewportDrawingEditorController::ClearLayerStackDelegates(/*const TSharedRef<FUICommandList>& iToolkitCommands*/)
{
    if(mData->LayerStack())
    {
        mData->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);
        mData->LayerStack()->OnStructureChanged().RemoveAll(this);
        mData->LayerStack()->OnImageResultChanged().RemoveAll(this);
        mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().RemoveAll(this);
    }
}

TSharedPtr<FOdysseyViewportDrawingEditorData>
FOdysseyViewportDrawingEditorController::GetData()
{
    return mData;
}


TSharedPtr<SOdysseyViewportDrawingEditorGUI> 
FOdysseyViewportDrawingEditorController::GetGUI()
{
    return mGUI;
}

void
FOdysseyViewportDrawingEditorController::OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    for(int i = 0; i < iChangedTiles.Num(); i++)
    {
        mData->LayerStack()->ComputeResultInBlockWithBlockAsCurrentLayer(mData->DisplaySurface()->Block()->GetBlock(),mData->PaintEngine()->PreviewBlock(),iChangedTiles[i]);
    }
    for(int i = 0; i < iChangedTiles.Num(); i++)
    {
        mData->DisplaySurface()->Block()->GetBlock()->Invalidate(iChangedTiles[i]);
    }
}

void
FOdysseyViewportDrawingEditorController::OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles)
{
    mData->LayerStack()->mDrawingUndo->StartRecord();
    for(int i = 0; i < iChangedTiles.Num(); i++)
    {
        mData->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x,iChangedTiles[i].y,iChangedTiles[i].w,iChangedTiles[i].h);
        // mData->LayerStack()->BlendOnCurrentLayer(mData->PaintEngine()->TempBuffer(), iChangedTiles[i], mData->PaintEngine()->GetOpacity(), mData->PaintEngine()->GetBlendingMode(), mData->PaintEngine()->GetAlphaMode());
    }
    mData->LayerStack()->mDrawingUndo->EndRecord();
}

void
FOdysseyViewportDrawingEditorController::OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    // mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    // mData->DisplaySurface()->Invalidate();
    mData->Texture()->MarkPackageDirty();
}

void
FOdysseyViewportDrawingEditorController::OnPaintEngineStrokeAbort()
{
    mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    mData->DisplaySurface()->Invalidate();
}


//Delegates

void FOdysseyViewportDrawingEditorController::OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue)
{
    //Remove all delegates for AlphaLock and set alpha lock to a default value
    if(iOldValue && iOldValue->GetType() == IOdysseyLayer::eType::kImage) {
        TSharedPtr<FOdysseyImageLayer> oldImageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(iOldValue);
        if(oldImageLayer)
        {
            oldImageLayer->IsAlphaLockedChangedDelegate().RemoveAll(this);
            mData->PaintEngine()->SetAlphaModeModifier(mGUI->GetPaintModifiers()->GetAlphaMode());
        }
    }
    mData->PaintEngine()->Block(NULL);

    //Add Image Layer Callback
    if(mData->LayerStack()->GetCurrentLayer() == NULL)
        return;

    TSharedPtr<IOdysseyLayer> layer = mData->LayerStack()->GetCurrentLayer();
    if(layer->GetType() != IOdysseyLayer::eType::kImage)
        return;

    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mData->LayerStack()->GetCurrentLayer());
    if(!imageLayer)
        return;

    mData->PaintEngine()->Block(imageLayer->GetBlock());
    mData->PaintEngine()->SetAlphaModeModifier(imageLayer->IsAlphaLocked() ? ::ul3::AM_BACK : mGUI->GetPaintModifiers()->GetAlphaMode());

    //Set AlphaLock Delegate
    imageLayer->IsAlphaLockedChangedDelegate().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnCurrentLayerIsAlphaLockedChanged);
    mData->PaintEngine()->SetLock(imageLayer->IsLocked());
}

void FOdysseyViewportDrawingEditorController::OnLayerStackStructureChanged()
{/*
    mData->Texture()->MarkPackageDirty();

    if(mData->BrushInstance())
    {
        FOdysseyTextureEditorState* layer_state = new FOdysseyTextureEditorState(mData->LayerStack());
        mData->BrushInstance()->AddOrReplaceState(FOdysseyTextureEditorState::GetId(),layer_state);
    }*/
}

void FOdysseyViewportDrawingEditorController::OnLayerStackImageResultChanged()
{
    mData->Texture()->MarkPackageDirty();
    mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    mData->DisplaySurface()->Invalidate();
}

void FOdysseyViewportDrawingEditorController::OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer,bool iOldValue)
{
    if(iLayer == mData->LayerStack()->GetCurrentLayer() || mData->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        mData->PaintEngine()->SetLock(iLayer->IsLocked());
    }
}

void FOdysseyViewportDrawingEditorController::OnCurrentLayerIsAlphaLockedChanged(bool iOldValue)
{
    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(mData->LayerStack()->GetCurrentLayer());
    mData->PaintEngine()->SetAlphaModeModifier((imageLayer && imageLayer->IsAlphaLocked()) ? ::ul3::AM_BACK : mGUI->GetPaintModifiers()->GetAlphaMode());
}

void
FOdysseyViewportDrawingEditorController::OnBrushSelected(UOdysseyBrush* iBrush)
{
    mData->Brush(iBrush);

    if(mData->BrushInstance())
    {
        mData->BrushInstance()->RemoveFromRoot();
        mData->BrushInstance(NULL);
    }

    if(mData->Brush())
    {
        //@todo: check
        //mData->Brush()->OnChanged().AddSP( this, &SOdysseyViewportDrawingEditorWidget::OnBrushChanged );

        mData->Brush()->OnCompiled().AddSP(this,&FOdysseyViewportDrawingEditorController::OnBrushCompiled);

        UOdysseyBrushAssetBase* brushInstance = NewObject< UOdysseyBrushAssetBase >(GetTransientPackage(),mData->Brush()->GeneratedClass);
        brushInstance->AddToRoot();
        mData->BrushInstance(brushInstance);

        mData->PaintEngine()->SetBrushInstance(mData->BrushInstance());
        GetGUI()->GetBrushExposedParameters()->Refresh(mData->BrushInstance());

        FOdysseyBrushPreferencesOverrides& overrides = mData->BrushInstance()->Preferences;
        if(overrides.bOverride_Step)          GetGUI()->GetStrokeOptions()->SetStrokeStep(overrides.Step);
        if(overrides.bOverride_Adaptative)    GetGUI()->GetStrokeOptions()->SetStrokeAdaptative(overrides.SizeAdaptative);
        if(overrides.bOverride_PaintOnTick)   GetGUI()->GetStrokeOptions()->SetStrokePaintOnTick(overrides.PaintOnTick);
        if(overrides.bOverride_Type)          GetGUI()->GetStrokeOptions()->SetInterpolationType((int32)overrides.Type);
        if(overrides.bOverride_Method)        GetGUI()->GetStrokeOptions()->SetSmoothingMethod((int32)overrides.Method);
        if(overrides.bOverride_Strength)      GetGUI()->GetStrokeOptions()->SetSmoothingStrength(overrides.Strength);
        if(overrides.bOverride_Enabled)       GetGUI()->GetStrokeOptions()->SetSmoothingEnabled(overrides.Enabled);
        if(overrides.bOverride_RealTime)      GetGUI()->GetStrokeOptions()->SetSmoothingRealTime(overrides.RealTime);
        if(overrides.bOverride_CatchUp)       GetGUI()->GetStrokeOptions()->SetSmoothingCatchUp(overrides.CatchUp);
        if(overrides.bOverride_Size)          GetGUI()->GetPaintModifiers()->SetSize(overrides.Size);
        if(overrides.bOverride_Opacity)       GetGUI()->GetPaintModifiers()->SetOpacity(overrides.Opacity);
        if(overrides.bOverride_Flow)          GetGUI()->GetPaintModifiers()->SetFlow(overrides.Flow);
        if(overrides.bOverride_BlendingMode)  GetGUI()->GetPaintModifiers()->SetBlendingMode((::ul3::eBlendingMode)overrides.BlendingMode);
        if(overrides.bOverride_AlphaMode)     GetGUI()->GetPaintModifiers()->SetAlphaMode((::ul3::eAlphaMode)overrides.AlphaMode);
    }
}

void
FOdysseyViewportDrawingEditorController::OnBrushCompiled(UBlueprint* iBrush)
{
    UOdysseyBrush* check_brush = dynamic_cast<UOdysseyBrush*>(iBrush);

    // Reload instance
    if(check_brush)
    {
        if(mData->BrushInstance())
        {
            if(mData->BrushInstance()->IsValidLowLevel())
                mData->BrushInstance()->RemoveFromRoot();

            mData->BrushInstance(NULL);
        }

        //brush->OnCompiled().AddSP( this, &FOdysseyPainterEditorToolkit::OnBrushCompiled );
        UOdysseyBrushAssetBase* brushInstance = NewObject< UOdysseyBrushAssetBase >(GetTransientPackage(),mData->Brush()->GeneratedClass);
        brushInstance->AddToRoot();
        mData->BrushInstance(brushInstance);

        mData->PaintEngine()->SetBrushInstance(mData->BrushInstance());
        GetGUI()->GetBrushExposedParameters()->Refresh(mData->BrushInstance());
    }
}

void
FOdysseyViewportDrawingEditorController::OnEditedTextureChanged(UTexture2D* iTexture)
{
    if( iTexture )
    {
        mData->Init(iTexture);
        mGUI->RefreshLayerStackView(mData->LayerStack());
    }
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Brush Handlers
void
FOdysseyViewportDrawingEditorController::HandleBrushParameterChanged()
{
    mData->PaintEngine()->TriggerStateChanged();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Color Handlers
void
FOdysseyViewportDrawingEditorController::HandleSelectorColorChanged(const ::ul3::FPixelValue& iColor)
{
    if(GetGUI()->GetColorSliders())
        GetGUI()->GetColorSliders()->SetColor(iColor);

    mData->PaintEngine()->SetColor(iColor);
}

void
FOdysseyViewportDrawingEditorController::HandleSlidersColorChanged(const ::ul3::FPixelValue& iColor)
{
    if(GetGUI()->GetColorSelector())
        GetGUI()->GetColorSelector()->SetColor(iColor);

    mData->PaintEngine()->SetColor(iColor);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Modifiers Handlers
void
FOdysseyViewportDrawingEditorController::HandleSizeModifierChanged(int32 iValue)
{
    mData->PaintEngine()->SetSizeModifier(iValue);
}

void
FOdysseyViewportDrawingEditorController::HandleOpacityModifierChanged(int32 iValue)
{
    mData->PaintEngine()->SetOpacityModifier(iValue);
}

void
FOdysseyViewportDrawingEditorController::HandleFlowModifierChanged(int32 iValue)
{
    mData->PaintEngine()->SetFlowModifier(iValue);
}

void
FOdysseyViewportDrawingEditorController::HandleBlendingModeModifierChanged(int32 iValue)
{
    mData->PaintEngine()->SetBlendingModeModifier(static_cast<::ul3::eBlendingMode>(iValue));
}

void
FOdysseyViewportDrawingEditorController::HandleAlphaModeModifierChanged(int32 iValue)
{
    mData->PaintEngine()->SetAlphaModeModifier(static_cast<::ul3::eAlphaMode>(iValue));
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------- Stroke Options Handlers
void
FOdysseyViewportDrawingEditorController::HandleStrokeStepChanged(int32 iValue)
{
    mData->PaintEngine()->SetStrokeStep(iValue);
}

void
FOdysseyViewportDrawingEditorController::HandleStrokeAdaptativeChanged(bool iValue)
{
    mData->PaintEngine()->SetStrokeAdaptative(iValue);
}

void
FOdysseyViewportDrawingEditorController::HandleStrokePaintOnTickChanged(bool iValue)
{
    mData->PaintEngine()->SetStrokePaintOnTick(iValue);
}

void
FOdysseyViewportDrawingEditorController::HandleInterpolationTypeChanged(int32 iValue)
{
    mData->PaintEngine()->SetInterpolationType(static_cast<EOdysseyInterpolationType>(iValue));
}

void
FOdysseyViewportDrawingEditorController::HandleSmoothingMethodChanged(int32 iValue)
{
    mData->PaintEngine()->SetSmoothingMethod(static_cast<EOdysseySmoothingMethod>(iValue));
}

void
FOdysseyViewportDrawingEditorController::HandleSmoothingStrengthChanged(int32 iValue)
{
    mData->PaintEngine()->SetSmoothingStrength(iValue);
}

void
FOdysseyViewportDrawingEditorController::HandleSmoothingEnabledChanged(bool iValue)
{
    mData->PaintEngine()->SetSmoothingEnabled(iValue);
}

void
FOdysseyViewportDrawingEditorController::HandleSmoothingRealTimeChanged(bool iValue)
{
    mData->PaintEngine()->SetSmoothingRealTime(iValue);
}

void
FOdysseyViewportDrawingEditorController::HandleSmoothingCatchUpChanged(bool iValue)
{
    mData->PaintEngine()->SetSmoothingCatchUp(iValue);
}
// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorController.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "PhysicsEngine/PhysicsSettings.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorController"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorController
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorController::~FOdysseyViewportDrawingEditorController()
{
    if( mData )
    {
        if(mData->Texture())
        {
            GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetClosed(mData->Texture(),this);
        }

        if(mData->LayerStack())
        {
            mData->LayerStack()->OnCurrentLayerChanged().RemoveAll(this);
            mData->LayerStack()->OnStructureChanged().RemoveAll(this);
            mData->LayerStack()->OnImageResultChanged().RemoveAll(this);
            mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().RemoveAll(this);
            mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().RemoveAll(this);
        }

        mData.Reset();
        mData = 0;
    }
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
        if(!(mData->PaintEngine()->OnPreviewBlockTilesChanged().IsBoundToObject(this)))
            mData->PaintEngine()->OnPreviewBlockTilesChanged().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnPaintEnginePreviewBlockTilesChanged);

        if(!(mData->PaintEngine()->OnEditedBlockTilesWillChange().IsBoundToObject(this)))
            mData->PaintEngine()->OnEditedBlockTilesWillChange().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnPaintEngineEditedBlockTilesWillChange);

        if(!(mData->PaintEngine()->OnEditedBlockTilesChanged().IsBoundToObject(this)))
            mData->PaintEngine()->OnEditedBlockTilesChanged().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnPaintEngineEditedBlockTilesChanged);

        if(!(mData->PaintEngine()->OnStrokeAbort().IsBoundToObject(this)))
            mData->PaintEngine()->OnStrokeAbort().AddRaw(this,&FOdysseyViewportDrawingEditorController::OnPaintEngineStrokeAbort);

        // Set LayerStack CB
        if (mData->LayerStack())
        {
            if (!(mData->LayerStack()->OnCurrentLayerChanged().IsBoundToObject(this)))
                mData->LayerStack()->OnCurrentLayerChanged().AddRaw(this, &FOdysseyViewportDrawingEditorController::OnLayerStackCurrentLayerChanged);

            if (!(mData->LayerStack()->OnStructureChanged().IsBoundToObject(this)))
                mData->LayerStack()->OnStructureChanged().AddRaw(this, &FOdysseyViewportDrawingEditorController::OnLayerStackStructureChanged);

            if (!(mData->LayerStack()->OnImageResultChanged().IsBoundToObject(this)))
                mData->LayerStack()->OnImageResultChanged().AddRaw(this, &FOdysseyViewportDrawingEditorController::OnLayerStackImageResultChanged);

            if (!(mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().IsBound()))
                mData->LayerStack()->GetLayerRoot()->ChildIsLockedChangedDelegate().AddRaw(this, &FOdysseyViewportDrawingEditorController::OnLayerIsLockedChanged);
        
            if( !(mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().IsBoundToObject(this)) )
                mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().AddRaw(this, &FOdysseyViewportDrawingEditorController::OnLayerIsVisibleChanged);

            // Set Image Layer as the current Layer
            TArray<TSharedPtr<IOdysseyLayer>> layers;
            mData->LayerStack()->GetLayerRoot()->DepthFirstSearchTree(&layers, false);

            for (int i = 0; i < layers.Num(); i++)
            {
                if (layers[i]->GetType() != IOdysseyLayer::eType::kImage)
                    continue;

                mData->LayerStack()->SetCurrentLayer(layers[i]);
                break;
            }
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
        mData->LayerStack()->GetLayerRoot()->ChildIsVisibleChangedDelegate().RemoveAll(this);
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
    if (mData->LayerStack())
    {
        for (int i = 0; i < iChangedTiles.Num(); i++)
        {
            mData->LayerStack()->ComputeResultInBlockWithBlockAsCurrentLayer(mData->DisplaySurface()->Block()->GetBlock(), mData->PaintEngine()->PreviewBlock(), iChangedTiles[i]);
        }
    }

    if (mData->DisplaySurface())
    {
        for (int i = 0; i < iChangedTiles.Num(); i++)
        {
            mData->DisplaySurface()->Block()->GetBlock()->Invalidate(iChangedTiles[i]);
        }
    }
}

void
FOdysseyViewportDrawingEditorController::OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles)
{
    if (mData->LayerStack())
    {
        mData->LayerStack()->mDrawingUndo->StartRecord();
        for (int i = 0; i < iChangedTiles.Num(); i++)
        {
            mData->LayerStack()->mDrawingUndo->SaveData(iChangedTiles[i].x, iChangedTiles[i].y, iChangedTiles[i].w, iChangedTiles[i].h);
            // mData->LayerStack()->BlendOnCurrentLayer(mData->PaintEngine()->TempBuffer(), iChangedTiles[i], mData->PaintEngine()->GetOpacity(), mData->PaintEngine()->GetBlendingMode(), mData->PaintEngine()->GetAlphaMode());
        }
        mData->LayerStack()->mDrawingUndo->EndRecord();
    }
}

void
FOdysseyViewportDrawingEditorController::OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles)
{
    // mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    // mData->DisplaySurface()->Invalidate();
    if (mData->Texture())
    {
        mData->Texture()->MarkPackageDirty();
    }
}

void
FOdysseyViewportDrawingEditorController::OnPaintEngineStrokeAbort()
{
    if (mData->LayerStack())
    {
        mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    }

    if (mData->DisplaySurface())
    {
        mData->DisplaySurface()->Invalidate();
    }
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
    mData->PaintEngine()->SetLock( imageLayer->IsLocked(true) || !imageLayer->IsVisible(true) );
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
    if (mData->Texture())
    {
        mData->Texture()->MarkPackageDirty();
    }

    if (mData->LayerStack())
    {
        mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    }

    if (mData->DisplaySurface())
    {
        mData->DisplaySurface()->Invalidate();
    }
}

void FOdysseyViewportDrawingEditorController::OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer,bool iOldValue)
{
    if(mData->LayerStack() && iLayer == mData->LayerStack()->GetCurrentLayer() || mData->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        mData->PaintEngine()->SetLock(mData->LayerStack()->GetCurrentLayer()->IsLocked(true) || !mData->LayerStack()->GetCurrentLayer()->IsVisible(true));
    }
}

void
FOdysseyViewportDrawingEditorController::OnLayerIsVisibleChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue)
{
    if (iLayer == mData->LayerStack()->GetCurrentLayer() || mData->LayerStack()->GetCurrentLayer()->HasForParent(iLayer))
    {
        mData->PaintEngine()->SetLock(mData->LayerStack()->GetCurrentLayer()->IsLocked(true) || !mData->LayerStack()->GetCurrentLayer()->IsVisible(true));
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
        mData->PaintEngine()->SetBrushInstance(NULL);
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
    //PATCH: this ensures that when we change the selected actor but we still edit the samle texture, the texture is synched
    // it avoids the texture to refresh itself to a previous state
    mData->PaintEngine()->Flush();
    mData->SyncTextureAndInvalidate();

    if (mData->Texture() == iTexture)
        return;
    
    if (mData->Texture())
    {
        GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetClosed( mData->Texture(), this );
    }

    mData->Init(iTexture);
    mGUI->RefreshLayerStackView(mData->LayerStack());
    Init();
    
    if (iTexture)
    {
        GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetOpened(mData->Texture(), this);
    }
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ Tools Actions

FReply
FOdysseyViewportDrawingEditorController::OnClear()
{
    if (!mData->LayerStack())
        return FReply::Handled();

    if(mData->LayerStack()->GetCurrentLayer() == NULL)
        return FReply::Handled();

    //Record
    mData->LayerStack()->mDrawingUndo->StartRecord();
    mData->LayerStack()->mDrawingUndo->SaveData(0,0,mData->LayerStack()->Width(),mData->LayerStack()->Height());
    mData->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord

    GetData()->PaintEngine()->AbortStroke();

    mData->LayerStack()->ClearCurrentLayer();

    mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    mData->DisplaySurface()->Invalidate();
    return FReply::Handled();
}

FReply
FOdysseyViewportDrawingEditorController::OnFill()
{
    if (!mData->LayerStack())
        return FReply::Handled();

    if(mData->LayerStack()->GetCurrentLayer() == NULL)
        return FReply::Handled();

    //Record
    mData->LayerStack()->mDrawingUndo->StartRecord();
    mData->LayerStack()->mDrawingUndo->SaveData(0,0,mData->LayerStack()->Width(),mData->LayerStack()->Height());
    mData->LayerStack()->mDrawingUndo->EndRecord();
    //EndRecord

    GetData()->PaintEngine()->AbortStroke();

    mData->LayerStack()->FillCurrentLayerWithColor(mData->PaintEngine()->GetColor());
    mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
    mData->DisplaySurface()->Invalidate();

    return FReply::Handled();
}

FReply
FOdysseyViewportDrawingEditorController::OnUndoIliad()
{
    GetData()->PaintEngine()->InterruptStrokeAndStampInPlace();
    if (mData->LayerStack())
    {
        mData->LayerStack()->mDrawingUndo->LoadData();
        mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
        mData->DisplaySurface()->Invalidate();
    }

    return FReply::Handled();
}


FReply
FOdysseyViewportDrawingEditorController::OnRedoIliad()
{
    GetData()->PaintEngine()->InterruptStrokeAndStampInPlace();
    if (mData->LayerStack())
    {
        mData->LayerStack()->mDrawingUndo->Redo();
        mData->LayerStack()->ComputeResultInBlock(mData->DisplaySurface()->Block()->GetBlock());
        mData->DisplaySurface()->Invalidate();
    }

    return FReply::Handled();
}

FReply
FOdysseyViewportDrawingEditorController::OnClearUndo()
{
    if (mData->LayerStack())
    {
        mData->LayerStack()->mDrawingUndo->Clear();
    }
    return FReply::Handled();
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
FOdysseyViewportDrawingEditorController::HandlePaintColorChange(eOdysseyEventState::Type iEventState,const ::ul3::FPixelValue& iColor)
{
    GetData()->PaintColor(iColor);

    if(iEventState == eOdysseyEventState::kSet)
    {
        GetData()->PaintEngine()->SetColor(iColor);
    }
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

FName
FOdysseyViewportDrawingEditorController::GetEditorName() const
{
    return FName("OdysseyViewportDrawingEditor");
}

void
FOdysseyViewportDrawingEditorController::FocusWindow(UObject* ObjectToFocusOn)
{
    //---
}

bool
FOdysseyViewportDrawingEditorController::CloseWindow()
{
    //---
    if( mData->Texture() )
    {
        mData->PaintEngine()->Flush();
        mData->SyncTextureAndInvalidate();
        mData->ApplyPropertiesBackup();

        //TODO: Move in the right place

        if (mData->LayerStack())
        {
            mData->LayerStack()->mDrawingUndo->Clear();
        }
    }

	return true;
}

bool
FOdysseyViewportDrawingEditorController::IsPrimaryEditor() const
{
    return true; //I don't know what this means
}

void
FOdysseyViewportDrawingEditorController::InvokeTab(const struct FTabId& TabId)
{
    //---
}

FName
FOdysseyViewportDrawingEditorController::GetToolbarTabId() const
{
    return NAME_None;
}

TSharedPtr<class FTabManager>
FOdysseyViewportDrawingEditorController::GetAssociatedTabManager()
{
    return TSharedPtr<class FTabManager>();
}

double
FOdysseyViewportDrawingEditorController::GetLastActivationTime()
{
    return 0.0;
}

void
FOdysseyViewportDrawingEditorController::RemoveEditingAsset(UObject* Asset)
{
    //---
}

void
FOdysseyViewportDrawingEditorController::EdModeEnter()
{
    if( !UPhysicsSettings::Get()->bSupportUVFromHitResults )
    {
        FText Title = LOCTEXT("TitleCollisionUVNoSupport","CollisionUVNoSupport");
        FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("Enable FindCollisionUV","'Support UV From Hit Results' doesn't seem to be enabled. Enable it from project settings in order to use this paint editor properly."),&Title);
    }

    UObject* texture = mData->Texture();
    if (texture)
    {
        GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetOpened( texture, this);
    }
}

void
FOdysseyViewportDrawingEditorController::EdModeExit()
{
    UObject* texture = mData->Texture();
    if (texture)
    {
        ClearLayerStackDelegates();
        mData->PaintEngine()->Flush();
        mData->SyncTextureAndInvalidate();
        mData->ApplyPropertiesBackup();

        //TODO: Move in the right place

        if (mData->LayerStack())
        {
            mData->LayerStack()->mDrawingUndo->Clear();
        }

        GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetClosed(texture, this);
    }
}

#undef LOCTEXT_NAMESPACE

// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditor.h"
#include "Engine/Texture2D.h"
#include "OdysseyTextureEditorGUI.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "Misc/OdysseyHandle.h"

/**
 * Implements an Editor for textures.
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditor
    : public FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditor();
    FOdysseyTextureEditor(); //Non Initialized constructor

public:
    virtual void InitData(UObject* iEditedObject) override;
    virtual void InitTools() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu(FToolMenuOwner iOwner, FName iMenuName) override;
    virtual void Tick(float iDeltaTime) override;
    virtual FOdysseyMediaProvider GetCurrentMediaProvider() override;

    UOdysseyPainterEditorTool* FindDefaultToolForCurrentLayer();
    void SelectDefaultTool();
    virtual void OnSelectedToolChanged() override;

public:
    // Getters
	virtual UTexture2D*				    Texture() const;
    virtual UOdysseyTextureLayerStack*	LayerStack() const;
	virtual UTexture*                   DisplayTexture() const override;
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetDisplayBlock() override;
    
    virtual void				        SetTexture(UTexture2D* iTexture);
    UOdysseyTextureLayerStackUserData*  TextureUserData() const;

    //TSharedPtr<FOdysseyRasterBlock> GetCurrentRasterBlock() const;
    //bool IsRasterBlockReadOnly() const;
    //FOdysseyVectorEngine* GetCurrentVectorEngine() const;

public:
    // Overrides
    virtual bool OnCloseRequested() override;

public:
    // Overrides
    virtual FOdysseyTextureEditorGUI* GetGUI() override;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

public:
    // Events
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);

public:
    //Common Actions
    virtual void Clear() override;

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
    //void UpdateToolContext();

private:
    UTexture2D* mTexture;
	TSharedPtr<FOdysseyTextureEditorGUI> mGUI;

    TSharedPtr<IOdysseyHandle> mLayerStackPreloadHandle;
    TMap<UClass*, UOdysseyPainterEditorTool*> mCurrentToolPerLayerClass;
};

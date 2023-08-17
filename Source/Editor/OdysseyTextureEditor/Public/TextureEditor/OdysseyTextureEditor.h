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
    UOdysseyPainterEditorTool* FindDefaultToolForCurrentLayer();
    virtual void SelectDefaultTool() override;
    virtual void OnSelectedToolChanged() override;
    UTexture2D* Texture() const;

public:
    // Overrides
    virtual void OnSourceInactivated() override;
    virtual void OnSourceActivated() override;

public:
    // Overrides
    virtual FOdysseyTextureEditorGUI* GetGUI() override;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

public:
    // Events
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);

private:
	TSharedPtr<FOdysseyTextureEditorGUI> mGUI;
    TMap<UClass*, UOdysseyPainterEditorTool*> mCurrentToolPerLayerClass;
};

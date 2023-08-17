// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "PainterEditor/OdysseyPainterEditorExtension.h"

class FOdysseyPainterEditor;
class FOdysseyTextureEditorGUI;
class FOdysseyTextureEditorSource;

class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorExtension
    : public FOdysseyPainterEditorExtension
{   
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorExtension();
    FOdysseyTextureEditorExtension( FOdysseyPainterEditor* iEditor );

public:
    virtual void Initialize() override;
    virtual void Finalize() override;

public:
    FOdysseyTextureEditorGUI* GetGUI();
    UTexture2D* Texture() const;

private:
    void OnSourceChanged();
    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);

private:
    TSharedPtr<FOdysseyTextureEditorSource> mTextureSource;
	TSharedPtr<FOdysseyTextureEditorGUI> mGUI;
};

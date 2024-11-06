// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "PainterEditor/OdysseyPainterEditorExtension.h"

class FOdysseyPainterEditor;
class FOdysseyTextureEditorGUI;
class FOdysseyTextureEditorSource;
class FOdysseyLayerStackEditorBrushContext;
class UOdysseyLayerStack;
class UOdysseyTextureLayerStack;

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
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender ) override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder) override;

public:
    UTexture2D* Texture() const;
    TSharedPtr<FOdysseyTextureEditorSource> GetTextureSource() const;
    UOdysseyTextureLayerStack* GetLayerStack() const;

private:
    void OnSourceChanged();
    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);

    void ConfigureTools();

private:
    TSharedPtr<FOdysseyTextureEditorSource> mTextureSource;
    TSharedPtr<FOdysseyTextureEditorGUI> mGUI;
    TSharedPtr<FOdysseyLayerStackEditorBrushContext> mLayerStackBrushEditorContext;
};

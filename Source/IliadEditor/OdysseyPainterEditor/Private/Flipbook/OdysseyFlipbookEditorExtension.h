// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyFlipbookWrapper.h"
#include "OdysseySurfaceTexture2D.h"
#include "OdysseyPainterEditorExtension.h"

class FOdysseyPainterEditor;
class FOdysseyFlipbookEditorGUI;

class FOdysseyFlipbookEditorExtension
    : public FOdysseyPainterEditorExtension
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorExtension();
    FOdysseyFlipbookEditorExtension( FOdysseyPainterEditor* iEditor );

public:
    virtual void Initialize() override;
    virtual void Finalize() override;
    virtual void BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder) override;

public:
    void SetFlipbook(UPaperFlipbook* iFlipbook);
    UPaperFlipbook* GetFlipbook();

public:
    TSharedPtr<FOdysseyFlipbookWrapper>    FlipbookWrapper();
    UTexture*                    PreviewTexture();
    void                        PreviewTexture(UTexture2D* iTexture);

protected:
    virtual void OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture);
    virtual void SetTextureAtKeyframeIndex(int32 iKeyframeIndex);

private:
    TSharedPtr<FOdysseyFlipbookWrapper> mFlipbookWrapper;
    FOdysseySurfaceTexture2D mPreviewSurface;

    TSharedPtr<FOdysseyFlipbookEditorGUI> mGUI;
};

// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "PainterEditor/OdysseyPainterEditorSource.h"

#include "OdysseyTextureLayerStack.h"

class UOdysseyTextureLayerStackUserData;

/**
 * Base class for an Editor Source
 */
class FOdysseyTextureEditorSource
    : public FOdysseyPainterEditorSource
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorSource();
    FOdysseyTextureEditorSource(UTexture2D* iTexture);

public:
    static const FGuid& StaticId();

public:
    virtual const FGuid& Id() const override;
    virtual int Width() const override;
    virtual int Height() const override;
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual UTexture* DisplayTexture() const override;
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetDisplayBlock() override;
    virtual FOdysseyMediaProvider GetCurrentMediaProvider() override;

    virtual void Clear() override;
    virtual void ClearFromCopyBlock(TSharedPtr<::ULIS::FBlock> iBlock) override;
    virtual void PasteBlockToCurrentLayer(TSharedPtr<::ULIS::FBlock> iBlock) override;
    virtual void PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iBlock ) override;

public:
    UTexture2D* GetTexture() const;
    virtual UOdysseyTextureLayerStack* GetLayerStack() const override;
    UOdysseyTextureLayerStackUserData* TextureUserData() const;
    void InitTextureUserData();

public:
    UTexture2D* mTexture;
};

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyAnimationImageRenderingAbility.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "OdysseyVectorCell.h"

class FOdysseyVectorBlock;
class FOdysseyMediaVector;
class FOdysseyVectorEngine;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorRoot;

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageVector
    : public FOdysseyAnimationCell, public IOdysseyVectorCell
{    
public:
    static TSharedRef<FOdysseyAnimationCellImageVector> Create(UOdysseyAnimationLayerImageVector* iLayer, int iLength, int iWidth, int iHeight);
    static const FName& StaticType();

public:
    virtual ~FOdysseyAnimationCellImageVector();
    FOdysseyAnimationCellImageVector(UOdysseyAnimationLayerImageVector* iLayer, int iLength);

    void Init(int iWidth, int iHeight);
    virtual TSharedPtr<FOdysseyAnimationCell> Clone(UOdysseyAnimationLayer* iLayer, int iLength) const override;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;
    virtual TSharedPtr<FOdysseyAnimationCell> CreateCellFromFrame(uint32 iFrameIndex) const override;

public:
    virtual const FName& GetType() const override;
    virtual void Serialize(FArchive& Ar) override;
    virtual void PostLoad() override;
        
    FOdysseyVectorEngine* GetEngine() const;
    FOdysseyVectorRoot* GetRoot() const;
    TSharedPtr<FOdysseyVectorBlock> GetVectorBlock() const;
    uint32 GetWidth();
    uint32 GetHeight();
    FGuid GetVectorBlockId();
    void SetWidth( uint32 iWidth );
    void SetHeight( uint32 iHeight );
    void SetVectorBlockId( FGuid iVectorBlockID );

public:
    // Event Listeners
    void OnVectorSceneSignal( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags );
    void OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer);
    void OnIsWireframeChanged(UOdysseyAnimationLayerImageVector* iLayer);


public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
    FCriticalSection* GetImageRenderingMutex() const;
    bool IsImageRenderingGameThreadOnly() const;
    virtual UOdysseyAnimationLayerImageVector* GetLayer() const override;

public:
    // Implements Interface IOdysseyVectorAnimationCell
    virtual FOdysseyVectorEngine* GetEngine() override;
    virtual int32 GetIndex() override;
    virtual uint32 GetLength() override;
    virtual uint32 GetFrame() override;

private:
    void OnVectorBlockInvalidated(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

private:
    //Import/Export
    friend class FOdysseyAnimationCellImageVectorExport;
    friend class FOdysseyAnimationCellImageVectorImport;

private:
    FOdysseyVectorRoot* mRoot;
    FGuid mVectorBlockId;
    TSharedPtr<FOdysseyVectorBlock> mVectorBlock; //A automatically cached block containing the render of mEngine
    uint32 mWidth;
    uint32 mHeight;
    mutable FCriticalSection mImageRenderingMutex;
    mutable TWeakPtr<FOdysseyMediaVector> mMediaVector;
};

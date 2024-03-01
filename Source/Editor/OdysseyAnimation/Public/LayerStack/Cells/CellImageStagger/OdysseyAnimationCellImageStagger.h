// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/OdysseyAnimationCell.h"

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageStagger
    : public FOdysseyAnimationCell
{    
public:
    enum class eBehaviour
    {
        Loop = 1, //needs to be 1 for compatibility reasons when Hold was an option
        PingPong
    };

public:
    static TSharedRef<FOdysseyAnimationCellImageStagger> Create(UOdysseyAnimationLayer* iLayer, int iLength);
    static const FName& StaticType();

public:
    virtual ~FOdysseyAnimationCellImageStagger();
    FOdysseyAnimationCellImageStagger(UOdysseyAnimationLayer* iLayer, int iLength);
    void Init();

public:
    virtual TSharedPtr<FOdysseyAnimationCell> Clone(UOdysseyAnimationLayer* iLayer, int iLength) const override;
    virtual const FName& GetType() const override;
    virtual void Serialize(FArchive& Ar);
    virtual TSharedPtr<FOdysseyAnimationCell> CreateCellFromFrame(uint32 iFrameIndex) const override;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
    bool IsImageRenderingGameThreadOnly() const;

public:
    TSharedPtr<FOdysseyAnimationCell> GetReferenceCellAtFrame(int iFrameIndex, int* oCellFrameIndex) const;
    int GetStaggerFrame(int iFrameIndex) const;
    int GetReach() const;
    eBehaviour GetBehaviour() const;

private:
    //Import/Export
    friend class FOdysseyAnimationCellImageStaggerExport;
    friend class FOdysseyAnimationCellImageStaggerImport;
    friend class FOdysseyAnimationCellImageStaggerMutator;
    friend class FOdysseySetBehaviourMutation;
    friend class FOdysseySetReachMutation;

private:
    eBehaviour mBehaviour;
    uint32 mReach;
};
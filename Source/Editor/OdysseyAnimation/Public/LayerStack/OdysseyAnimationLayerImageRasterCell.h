// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class ODYSSEYANIMATION_API FOdysseyAnimationLayerImageRasterCell
    : public FOdysseyAnimationLayerCell
{

    
public:
    static TSharedPtr<FOdysseyAnimationLayerImageRasterCell> Create(UOdysseyLayer*, int iWidth, int iHeight, ::ULIS::eFormat iFormat);
    static const FName& StaticType();

public:
    ~FOdysseyAnimationLayerImageRasterCell();
    FOdysseyAnimationLayerImageRasterCell(UOdysseyLayer* iLayer);

    void Init(int iWidth, int iHeight, ::ULIS::eFormat iFormat);

public:
    virtual const FName& GetType() const override;
    virtual void PostLoad() override;
    virtual void PostDuplicate() override;
    virtual TSharedPtr<IOdysseyHandle> Preload(int iFrameIndex) override;
    virtual void Serialize(FArchive& Ar);

private:
    void InitAbilities();
    void InitDelegates();
    void OnBlockChanged(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockCommited(const TArray<::ULIS::FRectI>& iRects);
    void OnBlockPtrChanged();

private:
    //Abilities
    class FImageProviderAbility;
    class FImageRasterEditingAbility;

    friend class FImageProviderAbility;
    friend class FImageRasterEditingAbility;

private:
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
};
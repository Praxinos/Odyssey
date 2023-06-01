// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageRaster
    : public FOdysseyAnimationCell
{

    
public:
    static TSharedPtr<FOdysseyAnimationCellImageRaster> Create(UObject* iOwner, int iWidth, int iHeight, ::ULIS::eFormat iFormat);
    static TSharedPtr<FOdysseyAnimationCellImageRaster> Create(UObject* iOwner, TSharedPtr<::ULIS::FBlock> iBlock);
    static const FName& StaticType();

public:
    virtual ~FOdysseyAnimationCellImageRaster();
    FOdysseyAnimationCellImageRaster(UObject* iOwner);

    void Init(int iWidth, int iHeight, ::ULIS::eFormat iFormat);
    void Init(TSharedPtr<::ULIS::FBlock> iBlock);
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;

public:
    virtual const FName& GetType() const override;
    virtual void PostLoad() override;
    virtual void PostDuplicate() override;
    virtual void Serialize(FArchive& Ar);

private:
    void InitAbilities();

private:
    UObject* mOwner;
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
};
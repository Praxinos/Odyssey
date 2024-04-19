// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationImageRenderingAbility.h"
#include "OdysseyMediaProvider.h"

class ODYSSEYANIMATION_API FOdysseyAnimationCell
    : public TSharedFromThis<FOdysseyAnimationCell>
    , public FOdysseyAnimationImageRenderingAbility
{
//public:
    //DECLARE_MULTICAST_DELEGATE_OneParam(FOnLengthChanged, TSharedRef<FOdysseyAnimationCell>)
    //static FOnLengthChanged& OnLengthChanged();

public:
    virtual ~FOdysseyAnimationCell();
    FOdysseyAnimationCell(int iLength, UOdysseyAnimationLayer* iLayer);

public:
    //void SetLength(int iLength);
    int GetLength() const;
    int GetMarkId() const;
    virtual UOdysseyAnimationLayer* GetLayer() const;
    virtual TSharedPtr<FOdysseyAnimationCell> Clone(UOdysseyAnimationLayer* iLayer, int iLength) const = 0;
    virtual const FName& GetType() const = 0;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const;
    virtual TSharedPtr<FOdysseyAnimationCell> CreateCellFromFrame(uint32 iFrameIndex) const;

public:
    virtual void PostDuplicate();
    virtual void PostLoad();
    virtual void Serialize(FArchive& Ar);

private:
    //Import/Export
    friend class FOdysseyAnimationCellExport;
    friend class FOdysseyAnimationCellImport;
    friend class FOdysseySetCellLengthMutation;
    friend class FOdysseySetCellMarkIdMutation;

protected:
    UOdysseyAnimationLayer* mLayer;
    uint32 mLength;
    int mMarkId;
};

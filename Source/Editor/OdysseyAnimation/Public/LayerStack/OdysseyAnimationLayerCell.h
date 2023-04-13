// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAbility.h"

class ODYSSEYANIMATION_API FOdysseyAnimationLayerCell
    : public TSharedFromThis<FOdysseyAnimationLayerCell>
{
public:
    ~FOdysseyAnimationLayerCell();
    FOdysseyAnimationLayerCell(UOdysseyLayer* iLayer);

public:
    UOdysseyLayer* GetLayer();
    void SetLength(uint32 iLength);
    uint32 GetLength() const;
    virtual const FName& GetType() const = 0;

public:
    virtual void PostLoad();
    virtual void PostDuplicate();
    virtual TSharedPtr<IOdysseyHandle> Preload(int iFrameIndex);
    virtual void Serialize(FArchive& Ar);

public:
    template<class T> void HasAbility() const;
    template<class T> TSharedPtr<T> GetAbility();

protected:
    template<class T> void SetAbility(TSharedRef<T> iAbility);

private:
    UOdysseyLayer* mLayer;
    uint32 mLength;
    TMap<FGuid, TSharedPtr<IOdysseyAbility>> mAbilities;
};

template<class T>
void
FOdysseyAnimationLayerCell::HasAbility() const
{
    return mAbilities.Contains(T::Id());
}

template<class T>
TSharedPtr<T>
FOdysseyAnimationLayerCell::GetAbility()
{
    TSharedPtr<IOdysseyAbility>* ability = mAbilities.Find(T::Id());
    if ( !ability )
        return nullptr;

    return StaticCastSharedPtr<T>(*ability);
}

template<class T>
void
FOdysseyAnimationLayerCell::SetAbility(TSharedRef<T> iAbility)
{
    if (mAbilities.Contains(T::Id()))
    {
        mAbilities[T::Id()] = iAbility;
        return;
    }
    
    mAbilities.Add(T::Id(), iAbility);
}
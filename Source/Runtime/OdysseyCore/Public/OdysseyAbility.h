// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

/**
 * @brief A base class for all abilities
 * 
 * Empty for now, but still useful
 */
class ODYSSEYCORE_API IOdysseyAbility
{
    //Each ability should define a static function returning a unique Id
    //Like the following
    //static const FGuid& Id()
    //{
    //    static FGuid Id(0xb3da1256, 0x7366410b, 0xacfdc6e3, 0x54ff7e5d);
    //    return Id;
    //}
};

class ODYSSEYCORE_API FOdysseyAbilityContainer
{
public:
    ~FOdysseyAbilityContainer();
    FOdysseyAbilityContainer();

public:
    template<class T> bool HasAbility() const;
    template<class T> TSharedPtr<T> GetAbility();

protected:
    template<class T> void SetAbility(TSharedRef<T> iAbility);

protected:
    TMap<FGuid, TSharedPtr<IOdysseyAbility>> mAbilities;
};

template<class T>
bool
FOdysseyAbilityContainer::HasAbility() const
{
    return mAbilities.Contains(T::Id());
}

template<class T>
TSharedPtr<T>
FOdysseyAbilityContainer::GetAbility()
{
    TSharedPtr<IOdysseyAbility>* ability = mAbilities.Find(T::Id());
    if ( !ability )
        return nullptr;

    return StaticCastSharedPtr<T>(*ability);
}

template<class T>
void
FOdysseyAbilityContainer::SetAbility(TSharedRef<T> iAbility)
{
    if (mAbilities.Contains(T::Id()))
    {
        mAbilities[T::Id()] = iAbility;
        return;
    }
    
    mAbilities.Add(T::Id(), iAbility);
}
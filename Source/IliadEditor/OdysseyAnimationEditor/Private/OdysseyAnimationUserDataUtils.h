// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "Templates/SubclassOf.h"

#include "OdysseyAnimation.h"

namespace OdysseyAnimationUserData
{
    /**
     * @brief Returns the asset user data of class T attached to an animation, creating it if needed
     *
     * @param iAnimation The animation owning the user data
     * @return The user data of the animation, or nullptr if iAnimation is null
     */
    template<class T>
    T* FindOrAdd(const UOdysseyAnimation* iAnimation)
    {
        if (!iAnimation)
            return nullptr;

        // Creating the editor user data lazily does not change the animation content, so it is allowed from const accessors
        UOdysseyAnimation* animation = const_cast<UOdysseyAnimation*>(iAnimation);

        T* userData = animation->GetAssetUserData<T>();
        if (!userData)
        {
            userData = NewObject<T>(animation, NAME_None, RF_Transactional);
            animation->AddAssetUserData(userData);
        }

        return userData;
    }
}

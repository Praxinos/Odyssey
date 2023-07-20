// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyMedia.h"

class ODYSSEYMEDIA_API FOdysseyMediaProvider
{
public:
    FOdysseyMediaProvider();

public:
    DECLARE_DELEGATE_RetVal(TSharedPtr<IOdysseyMedia>, FCreateMediaDelegate)

public:
    void Add(const FGuid& iMediaId, const FCreateMediaDelegate& iCreateMediaDelegate);
    void Add(TSharedPtr<IOdysseyMedia> iMedia);

public:
    //IOdysseyMedia API
    template<class T> bool HasMedia() const;
    template<class T> TArray<TSharedPtr<T>> GetMedias() const;
    template<class T> TArray<TSharedPtr<T>> GetOrCreateMedias() const;

private:
    struct FMediaFactory {
        mutable TSharedPtr<IOdysseyMedia> mMedia;
        FCreateMediaDelegate mCreateMediaDelegate;
    };
    TMap<FGuid, TArray<FMediaFactory>> mMediaFactories;
};

template<class T>
bool
FOdysseyMediaProvider::HasMedia() const
{
    return mMediaFactories.Contains(T::StaticId());
}

template<class T>
TArray<TSharedPtr<T>>
FOdysseyMediaProvider::GetMedias() const
{
    TArray<TSharedPtr<T>> medias;
    const TArray<FMediaFactory>& factories = mMediaFactories[T::StaticId()];
    for (const FMediaFactory& factory : factories)
    {
        if (!factory.mMedia)
            continue;

        medias.Add(StaticCastSharedPtr<T>(factory.mMedia));
    }
    return medias;
}

template<class T>
TArray<TSharedPtr<T>>
FOdysseyMediaProvider::GetOrCreateMedias() const
{
    TArray<TSharedPtr<T>> medias;
    const TArray<FMediaFactory>& factories = mMediaFactories[T::StaticId()];
    for (const FMediaFactory& factory : factories)
    {
        if (!factory.mMedia)
        {
            if (!factory.mCreateMediaDelegate.IsBound())
                continue;

            factory.mMedia = factory.mCreateMediaDelegate.Execute();
            
            if (!factory.mMedia)
                continue;
        }

        medias.Add(StaticCastSharedPtr<T>(factory.mMedia));
    }
    return medias;
}


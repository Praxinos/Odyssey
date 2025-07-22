// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    bool IsLocked() const;
    void IsLocked(bool iIsLocked);

public:
    //IOdysseyMedia API
    template<class T> bool HasMedia() const;
    template<class T> int GetMediaCount() const;
    template<class T> TArray<TSharedPtr<T>> GetMedias() const;
    template<class T> TArray<TSharedPtr<T>> GetOrCreateMedias() const;

private:
    struct FMediaFactory {
        mutable TSharedPtr<IOdysseyMedia> mMedia;
        FCreateMediaDelegate mCreateMediaDelegate;
    };
    TMap<FGuid, TArray<FMediaFactory>> mMediaFactories;
    bool mIsLocked;
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

template<class T>
int
FOdysseyMediaProvider::GetMediaCount() const
{
    if (!mMediaFactories.Contains(T::StaticId()))
        return 0;


    const TArray<FMediaFactory>& factories = mMediaFactories[T::StaticId()];
    return factories.Num();
}

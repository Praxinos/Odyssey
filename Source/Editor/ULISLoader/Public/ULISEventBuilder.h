// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class ULISLOADER_API FULISEventBuilder
{
public:
    ::ULIS::FEvent Build();

    FULISEventBuilder& OnComplete( TFunction<void(const ::ULIS::FRectI&)> iFunction );
    FULISEventBuilder& RetainBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>& iBlock);

private:
    struct FParams
    {
        TArray<TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>> mRetainedBlocks;
        TFunction<void(const ::ULIS::FRectI&)> mOnComplete;
    } mParams;
};
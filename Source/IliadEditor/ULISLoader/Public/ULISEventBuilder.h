// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class ULISLOADER_API FULISEventBuilder
{
public:
    DECLARE_DELEGATE_OneParam( FOnComplete, const ::ULIS::FRectI& );

public:
    ::ULIS::FEvent Build();

    FULISEventBuilder& OnComplete( const FOnComplete& iFunction );
    FULISEventBuilder& RetainBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock);

private:
    struct FParams
    {
        TArray<TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>> mRetainedBlocks;
        FOnComplete mOnComplete;
    } mParams;
};

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "ULISEventBuilder.h"

#include <ULIS>

::ULIS::FEvent
FULISEventBuilder::Build()
{
    FParams* params = new FParams();
    *params = mParams;

    return ::ULIS::FEvent(
        ::ULIS::FOnEventComplete(
            [params](const ::ULIS::FRectI& iRect)
            {
                params->mOnComplete.ExecuteIfBound(iRect);
                delete params; //will release all retained blocks
            }
        )
    );
}

FULISEventBuilder&
FULISEventBuilder::OnComplete( const FOnComplete& iFunction )
{
    mParams.mOnComplete = iFunction;
    return *this;
}

FULISEventBuilder&
FULISEventBuilder::RetainBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>& iBlock)
{
    mParams.mRetainedBlocks.Add(iBlock);
    return *this;
}
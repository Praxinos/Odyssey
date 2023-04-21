// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Abilities/IOdysseyAnimationImageRenderingAbility.h"


IOdysseyAnimationImageRenderingAbility::FOnChanged&
IOdysseyAnimationImageRenderingAbility::OnChanged()
{
    static FOnChanged onChanged;
    return onChanged;
}

IOdysseyAnimationImageRenderingAbility::FOnChanged&
IOdysseyAnimationImageRenderingAbility::OnCommited()
{
    static FOnChanged onCommited;
    return onCommited;
}

IOdysseyAnimationImageRenderingAbility::FOnCompositionChanged&
IOdysseyAnimationImageRenderingAbility::OnCompositionChanged()
{
    static FOnCompositionChanged onCompositionChanged;
    return onCompositionChanged;
}

IOdysseyAnimationImageRenderingAbility::FOnCompositionChanged&
IOdysseyAnimationImageRenderingAbility::OnCompositionCommited()
{
    static FOnCompositionChanged onCompositionCommited;
    return onCompositionCommited;
}

IOdysseyAnimationImageRenderingAbility::IOdysseyAnimationImageRenderingAbility()
    : mId(FGuid::NewGuid())
{
}
    
const FGuid&
IOdysseyAnimationImageRenderingAbility::GetId() const
{
    return mId;
}

TArray<::ULIS::FEvent>
IOdysseyAnimationImageRenderingAbility::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return RenderInBlock(ioBlock, iFrame, iRect, iPos, iWaitList);
}

TSharedPtr<::ULIS::FBlock>
IOdysseyAnimationImageRenderingAbility::RenderInNewBlock(int iFrame, ::ULIS::eFormat iFormat, const ::ULIS::FRectI& iRect, TArray<::ULIS::FEvent>& oEvents)
{
    TSharedRef<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iRect.w, iRect.h, iFormat);
    oEvents = RenderInBlock(block, iFrame, iRect, ::ULIS::FVec2I(0), {});
    return block;
}

TSharedPtr<IOdysseyHandle>
IOdysseyAnimationImageRenderingAbility::Preload(int iFrame)
{
    return nullptr;
}
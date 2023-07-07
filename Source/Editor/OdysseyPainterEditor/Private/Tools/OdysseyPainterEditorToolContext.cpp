// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/OdysseyPainterEditorToolContext.h"

FOdysseyPainterEditorToolContext::FParams::FParams()
    : mRasterBlock(nullptr)
    , mCanProvideRasterBlockOnDemand(false)
    , mIsRasterBlockReadOnly(false)
    , mVectorEngine(nullptr)
    , mOnProvideRasterBlockDelegate()
{
}

bool
FOdysseyPainterEditorToolContext::FParams::Set(const FParams& iParams)
{
    bool hasChanged = false;
    hasChanged |= mRasterBlock != iParams.mRasterBlock;
    hasChanged |= mCanProvideRasterBlockOnDemand != iParams.mCanProvideRasterBlockOnDemand;
    hasChanged |= mIsRasterBlockReadOnly != iParams.mIsRasterBlockReadOnly;
    hasChanged |= mVectorEngine != iParams.mVectorEngine;

    mRasterBlock = iParams.mRasterBlock;
    mCanProvideRasterBlockOnDemand = iParams.mCanProvideRasterBlockOnDemand;
    mIsRasterBlockReadOnly = iParams.mIsRasterBlockReadOnly;
    mVectorEngine = iParams.mVectorEngine;
    mOnProvideRasterBlockDelegate = iParams.mOnProvideRasterBlockDelegate;

    return hasChanged;
}

FOdysseyPainterEditorToolContext::~FOdysseyPainterEditorToolContext()
{

}

FOdysseyPainterEditorToolContext::FOdysseyPainterEditorToolContext(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
    , mParams()
{

}

void
FOdysseyPainterEditorToolContext::Set( const FParams& iParams )
{
    bool hasChanged = mParams.Set(iParams);
    if (hasChanged)
        mOnChanged.Broadcast();
}

void
FOdysseyPainterEditorToolContext::Unset()
{
    bool hasChanged = mParams.Set(FParams());
    if (hasChanged)
        mOnChanged.Broadcast();
}

FOdysseyPainterEditor*
FOdysseyPainterEditorToolContext::GetEditor() const
{
    return mEditor;
}

TSharedPtr<FOdysseyRasterBlock>
FOdysseyPainterEditorToolContext::GetRasterBlock() const
{
    return mParams.mRasterBlock;
}

bool
FOdysseyPainterEditorToolContext::CanProvideRasterBlockOnDemand() const
{
    return mParams.mCanProvideRasterBlockOnDemand;
}

bool
FOdysseyPainterEditorToolContext::IsRasterBlockReadOnly() const
{
    return mParams.mIsRasterBlockReadOnly;
}

FOdysseyVectorEngine*
FOdysseyPainterEditorToolContext::GetVectorEngine() const
{
    return mParams.mVectorEngine;
}

FOdysseyPainterEditorToolContext::FOnProvideRasterBlock&
FOdysseyPainterEditorToolContext::OnProvideRasterBlockDelegate()
{
    return mParams.mOnProvideRasterBlockDelegate;
}

FOdysseyPainterEditorToolContext::FOnChanged&
FOdysseyPainterEditorToolContext::OnChanged()
{
    return mOnChanged;
}

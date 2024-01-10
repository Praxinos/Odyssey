// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/LightTable/OdysseyAnimationLightTableMutator.h"

FOdysseyAnimationLightTableMutator::FOdysseyAnimationLightTableMutator(TSharedPtr<FOdysseyAnimationLightTable> iLightTable)
    : FOdysseyMutator(iLightTable->GetOwnerLayer(), "FOdysseyAnimationLightTableMutator", false) //false means we never generate undo, we could also pass nullptr instead of the ownerlayer here
    , mLightTable(iLightTable)
{
    GetRootMutation()->OnChanged().BindLambda(
        [lightTable = mLightTable]()
        {
            //TODO: Finding the rect to invalidate is annoying, please find another way, so that it is transparent for the dev
            lightTable->ImageRenderingChanged(true);
        }
    );

    GetRootMutation()->OnCommited().BindLambda(
        [lightTable = mLightTable]()
        {
            lightTable->ImageRenderingChanged();
        }
    );
}

struct FSetSourceLayerData
{
    UOdysseyAnimationLayer* mNewSourceLayer;
    UOdysseyAnimationLayer* mOldSourceLayer;
};

void
FOdysseyAnimationLightTableMutator::SetSourceLayer(UOdysseyAnimationLayer* iLayer)
{
    TSharedRef<FSetSourceLayerData> data = MakeShared<FSetSourceLayerData>();
    data->mNewSourceLayer = iLayer;
    data->mOldSourceLayer = mLightTable->mSourceLayer;

    TSharedPtr<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetSourceLayerData>>(
        data,
        FOdysseyMutation<FSetSourceLayerData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetSourceLayerData> iData) { lighttable->mSourceLayer = iData->mNewSourceLayer; }),
        FOdysseyMutation<FSetSourceLayerData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetSourceLayerData> iData) { lighttable->mSourceLayer = iData->mOldSourceLayer; })
    );

    AddAndApplyMutation(mutation);
}

struct FSetDisplayPositionData
{
    EOdysseyLightTableDisplayPosition mNewDisplayPosition;
    EOdysseyLightTableDisplayPosition mOldDisplayPosition;
};

void
FOdysseyAnimationLightTableMutator::SetDisplayPosition(EOdysseyLightTableDisplayPosition iDisplayPosition)
{
    TSharedRef<FSetDisplayPositionData> data = MakeShared<FSetDisplayPositionData>();
    data->mNewDisplayPosition = iDisplayPosition;
    data->mOldDisplayPosition = mLightTable->mDisplayPosition;

    

    TSharedPtr<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetDisplayPositionData>>(
        data,
        FOdysseyMutation<FSetDisplayPositionData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetDisplayPositionData> iData) { lighttable->mDisplayPosition = iData->mNewDisplayPosition; }),
        FOdysseyMutation<FSetDisplayPositionData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetDisplayPositionData> iData) { lighttable->mDisplayPosition = iData->mOldDisplayPosition; })
    );

    AddAndApplyMutation(mutation);
}

struct FSetDisplayModeData
{
    EOdysseyLightTableDisplayMode mNewDisplayMode;
    EOdysseyLightTableDisplayMode mOldDisplayMode;
};

void
FOdysseyAnimationLightTableMutator::SetDisplayMode(EOdysseyLightTableDisplayMode iDisplayMode)
{
    TSharedRef<FSetDisplayModeData> data = MakeShared<FSetDisplayModeData>();
    data->mNewDisplayMode = iDisplayMode;
    data->mOldDisplayMode = mLightTable->mDisplayMode;

    TSharedRef<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetDisplayModeData>>(
        data,
        FOdysseyMutation<FSetDisplayModeData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetDisplayModeData> iData) { lighttable->mDisplayMode = iData->mNewDisplayMode; }),
        FOdysseyMutation<FSetDisplayModeData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetDisplayModeData> iData) { lighttable->mDisplayMode = iData->mOldDisplayMode; })
    );

    AddAndApplyMutation(mutation);
}

struct FSetKeyIsActivatedData
{
    int mIndex;
    bool mNewIsActivated;
    bool mOldIsActivated;
};


void
FOdysseyAnimationLightTableMutator::SetKeyIsActivated(int iIndex, bool iIsActivated)
{
    TSharedRef<FSetKeyIsActivatedData> data = MakeShared<FSetKeyIsActivatedData>();
    data->mIndex = iIndex;
    data->mNewIsActivated = iIsActivated;
    data->mOldIsActivated = mLightTable->mKeysData[iIndex].mIsActivated;

    TSharedRef<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetKeyIsActivatedData>>(
        data,
        FOdysseyMutation<FSetKeyIsActivatedData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeyIsActivatedData> iData) { lighttable->mKeysData[iData->mIndex].mIsActivated = iData->mNewIsActivated; }),
        FOdysseyMutation<FSetKeyIsActivatedData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeyIsActivatedData> iData) { lighttable->mKeysData[iData->mIndex].mIsActivated = iData->mOldIsActivated; })
    );

    AddAndApplyMutation(mutation);
}

struct FSetKeyFrameOffsetData
{
    int mIndex;
    int mNewOffset;
    int mOldOffset;
};

void
FOdysseyAnimationLightTableMutator::SetKeyFrameOffset(int iIndex, int iOffset)
{
    TSharedRef<FSetKeyFrameOffsetData> data = MakeShared<FSetKeyFrameOffsetData>();
    data->mIndex = iIndex;
    data->mNewOffset = iOffset;
    data->mOldOffset = mLightTable->mKeysData[iIndex].mOffset;

    TSharedRef<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetKeyFrameOffsetData>>(
        data,
        FOdysseyMutation<FSetKeyFrameOffsetData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeyFrameOffsetData> iData) { lighttable->mKeysData[iData->mIndex].mOffset = iData->mNewOffset; }),
        FOdysseyMutation<FSetKeyFrameOffsetData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeyFrameOffsetData> iData) { lighttable->mKeysData[iData->mIndex].mOffset = iData->mOldOffset; })
    );

    AddAndApplyMutation(mutation);
}

struct FSetKeyOpacityData
{
    int mIndex;
    float mNewOpacity;
    float mOldOpacity;
};

void
FOdysseyAnimationLightTableMutator::SetKeyOpacity(int iIndex, float iOpacity)
{
    TSharedRef<FSetKeyOpacityData> data = MakeShared<FSetKeyOpacityData>();
    data->mIndex = iIndex;
    data->mNewOpacity = FMath::Clamp(iOpacity, 0.f, 1.f);
    data->mOldOpacity = mLightTable->mKeysData[iIndex].mOpacity;

    TSharedRef<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetKeyOpacityData>>(
        data,
        FOdysseyMutation<FSetKeyOpacityData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeyOpacityData> iData) { lighttable->mKeysData[iData->mIndex].mOpacity = iData->mNewOpacity; }),
        FOdysseyMutation<FSetKeyOpacityData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeyOpacityData> iData) { lighttable->mKeysData[iData->mIndex].mOpacity = iData->mOldOpacity; })
    );

    AddAndApplyMutation(mutation);
}

struct FSetKeysColorData
{
    FLinearColor mNewColor;
    FLinearColor mOldColor;
};

void
FOdysseyAnimationLightTableMutator::SetNextKeysColor(const FLinearColor& iColor)
{
    TSharedRef<FSetKeysColorData> data = MakeShared<FSetKeysColorData>();
    data->mNewColor = iColor;
    data->mOldColor = mLightTable->mNextKeysColor;

    TSharedRef<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetKeysColorData>>(
        data,
        FOdysseyMutation<FSetKeysColorData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeysColorData> iData) { lighttable->mNextKeysColor = iData->mNewColor; }),
        FOdysseyMutation<FSetKeysColorData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeysColorData> iData) { lighttable->mNextKeysColor = iData->mOldColor; })
    );

    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationLightTableMutator::SetPreviousKeysColor(const FLinearColor& iColor)
{
    TSharedRef<FSetKeysColorData> data = MakeShared<FSetKeysColorData>();
    data->mNewColor = iColor;
    data->mOldColor = mLightTable->mPreviousKeysColor;

    TSharedRef<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetKeysColorData>>(
        data,
        FOdysseyMutation<FSetKeysColorData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeysColorData> iData) { lighttable->mPreviousKeysColor = iData->mNewColor; }),
        FOdysseyMutation<FSetKeysColorData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeysColorData> iData) { lighttable->mPreviousKeysColor = iData->mOldColor; })
    );

    AddAndApplyMutation(mutation);
}
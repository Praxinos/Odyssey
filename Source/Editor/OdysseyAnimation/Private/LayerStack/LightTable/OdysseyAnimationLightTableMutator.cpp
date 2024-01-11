// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/LightTable/OdysseyAnimationLightTableMutator.h"

FOdysseyAnimationLightTableMutator::FOdysseyAnimationLightTableMutator(TSharedPtr<FOdysseyAnimationLightTable> iLightTable)
    : FOdysseyMutator(iLightTable->GetLayer(), "FOdysseyAnimationLightTableMutator", false) //false means we never generate undo, we could also pass nullptr instead of the ownerlayer here
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
    data->mOldIsActivated = mLightTable->GetKeyIsActivated(iIndex);

    TSharedRef<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetKeyIsActivatedData>>(
        data,
        FOdysseyMutation<FSetKeyIsActivatedData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeyIsActivatedData> iData) { lighttable->GetKey(iData->mIndex)->mIsActivated = iData->mNewIsActivated; }),
        FOdysseyMutation<FSetKeyIsActivatedData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeyIsActivatedData> iData) { lighttable->GetKey(iData->mIndex)->mIsActivated = iData->mOldIsActivated; })
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
    data->mOldOpacity = mLightTable->GetKeyOpacity(iIndex);

    TSharedRef<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetKeyOpacityData>>(
        data,
        FOdysseyMutation<FSetKeyOpacityData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeyOpacityData> iData) { lighttable->GetKey(iData->mIndex)->mOpacity = iData->mNewOpacity; }),
        FOdysseyMutation<FSetKeyOpacityData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeyOpacityData> iData) { lighttable->GetKey(iData->mIndex)->mOpacity = iData->mOldOpacity; })
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

struct FSetKeysContrastData
{
    float mNewContrast;
    float mOldContrast;
};

void
FOdysseyAnimationLightTableMutator::SetNextKeysContrast(float iContrast)
{
    TSharedRef<FSetKeysContrastData> data = MakeShared<FSetKeysContrastData>();
    data->mNewContrast = iContrast;
    data->mOldContrast = mLightTable->mNextKeysContrast;

    TSharedRef<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetKeysContrastData>>(
        data,
        FOdysseyMutation<FSetKeysContrastData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeysContrastData> iData) { lighttable->mNextKeysContrast = iData->mNewContrast; }),
        FOdysseyMutation<FSetKeysContrastData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeysContrastData> iData) { lighttable->mNextKeysContrast = iData->mOldContrast; })
    );

    AddAndApplyMutation(mutation);
}

void
FOdysseyAnimationLightTableMutator::SetPreviousKeysContrast(float iContrast)
{
    TSharedRef<FSetKeysContrastData> data = MakeShared<FSetKeysContrastData>();
    data->mNewContrast = iContrast;
    data->mOldContrast = mLightTable->mPreviousKeysContrast;

    TSharedRef<IOdysseyMutation> mutation = MakeShared<FOdysseyMutation<FSetKeysContrastData>>(
        data,
        FOdysseyMutation<FSetKeysContrastData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeysContrastData> iData) { lighttable->mPreviousKeysContrast = iData->mNewContrast; }),
        FOdysseyMutation<FSetKeysContrastData>::FMutationDelegate::CreateLambda([lighttable = mLightTable](TSharedPtr<FSetKeysContrastData> iData) { lighttable->mPreviousKeysContrast = iData->mOldContrast; })
    );

    AddAndApplyMutation(mutation);
}
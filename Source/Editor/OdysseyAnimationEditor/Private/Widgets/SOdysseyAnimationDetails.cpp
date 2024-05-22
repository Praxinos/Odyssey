// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyAnimationDetails.h"
#include "OdysseyAnimation.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void SOdysseyAnimationDetails::Construct(const FArguments& InArgs)
{
    mAnimation = InArgs._Animation;

    FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	mAnimationPropertiesWidget = PropertyModule.CreateDetailView(Args);
	mAnimationPropertiesWidget->SetObject(mAnimation.Get());

    ChildSlot
    [
        mAnimationPropertiesWidget.ToSharedRef()
    ];
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

void
SOdysseyAnimationDetails::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (animation != mCurrentAnimation)
    {
        mCurrentAnimation = animation;
        mAnimationPropertiesWidget->SetObject(animation);
    }
}

#undef LOCTEXT_NAMESPACE

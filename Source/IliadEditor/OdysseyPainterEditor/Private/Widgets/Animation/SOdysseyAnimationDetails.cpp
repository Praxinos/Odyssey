// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/Animation/SOdysseyAnimationDetails.h"
#include "OdysseyAnimation.h"
#include "DetailsViewArgs.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"

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

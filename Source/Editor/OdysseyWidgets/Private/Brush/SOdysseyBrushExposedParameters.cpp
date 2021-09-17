// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Brush/SOdysseyBrushExposedParameters.h"

#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"


#define LOCTEXT_NAMESPACE "OdysseyBrushExposedParameters"


//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushExposedParameters
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyBrushExposedParameters::Construct( const FArguments& InArgs )
{
    mBrushInstance = InArgs._BrushInstance;
    mCurrentBrushInstance = nullptr;
    OnParameterChangedCallback = InArgs._OnParameterChanged;

    // Create a details view
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FNotifyHook* NotifyHook = this;
    FDetailsViewArgs DetailsViewArgs(/*bUpdateFromSelection=*/ false, /*bLockable=*/ false, /*bAllowSearch=*/ false, FDetailsViewArgs::HideNameArea, /*bHideSelectionTip=*/ true, NotifyHook );
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
    details_view = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

    details_view->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateLambda(
            [](const FPropertyAndParent& iPropertyAndParent) -> bool
            {
            return !(iPropertyAndParent.Objects.Num() == 1 && iPropertyAndParent.Property.GetNameCPP() == "Preferences"); //Brush Overrides
            }
        )
    );

    this->ChildSlot
    [
        details_view.ToSharedRef()
    ];
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides
void
SOdysseyBrushExposedParameters::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    UOdysseyBrushAssetBase* brushInstance = mBrushInstance.Get();
    if (brushInstance != mCurrentBrushInstance)
    {
        mCurrentBrushInstance = brushInstance;
        details_view->SetObject(brushInstance);
    }
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- FNotifyHook Interface
void
SOdysseyBrushExposedParameters::NotifyPreChange( FProperty* PropertyAboutToChange )
{
}


void
SOdysseyBrushExposedParameters::NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged )
{
    //Call the callback only if the change is not an interactive change (while moving a slider), but when it is a real change (when releasing a slider)
    if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
        return;

    OnParameterChangedCallback.ExecuteIfBound();
}


#undef LOCTEXT_NAMESPACE


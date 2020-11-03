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
    OnParameterChangedCallback = InArgs._OnParameterChanged;

    // Create a details view
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FNotifyHook* NotifyHook = this;
    FDetailsViewArgs DetailsViewArgs(/*bUpdateFromSelection=*/ false, /*bLockable=*/ false, /*bAllowSearch=*/ false, FDetailsViewArgs::HideNameArea, /*bHideSelectionTip=*/ true, NotifyHook );
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
    details_view = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

    this->ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        details_view.ToSharedRef()
                    ]
            ]
    ];
}


void
SOdysseyBrushExposedParameters::Refresh( UOdysseyBrushAssetBase* iValue )
{
    details_view->SetObject( iValue );
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
    OnParameterChangedCallback.ExecuteIfBound();
}


#undef LOCTEXT_NAMESPACE


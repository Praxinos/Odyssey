// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/Widgets/SOdysseyBrushExposedParameters.h"

#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Tools/RasterDrawingTool/OdysseyRasterDrawingTool.h"


#define LOCTEXT_NAMESPACE "OdysseyBrushExposedParameters"


//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushExposedParameters
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyBrushExposedParameters::Construct( const FArguments& InArgs )
{
    mBrushInstance = InArgs._BrushInstance;
    mTool = InArgs._Tool;
    mCurrentBrushInstance = nullptr;
    OnParameterChangedCallback = InArgs._OnParameterChanged;

    // Create a details view
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FNotifyHook* NotifyHook = this;
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.NotifyHook = NotifyHook;
    DetailsViewArgs.bAllowMultipleTopLevelObjects = true;
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
    details_view = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

    details_view->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateLambda(
            [this](const FPropertyAndParent& iPropertyAndParent) -> bool
            {   
                if (iPropertyAndParent.Property.GetNameCPP() == "BlendParameters")
                    return true;

                if (iPropertyAndParent.Objects.Num() > 0 && iPropertyAndParent.Objects[0] == mTool)
                    return false;

                if (iPropertyAndParent.Property.HasAnyPropertyFlags(CPF_DisableEditOnInstance))
                    return false;
                return true;
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
        TArray<UObject*> objects;
        objects.Add(mTool);
        objects.Add(mCurrentBrushInstance);
        details_view->SetObjects(objects);
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


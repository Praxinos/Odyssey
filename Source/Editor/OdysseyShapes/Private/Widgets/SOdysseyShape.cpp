// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Widgets/SOdysseyShape.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"


#define LOCTEXT_NAMESPACE "SOdysseyShape"

/////////////////////////////////////////////////////
// SOdysseyShape
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyShape::Construct( const FArguments& InArgs )
{
    mShape = InArgs._Shape;
    mCurrentShape = nullptr;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true, nullptr);
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
    mDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    mDetailsView->SetIsPropertyVisibleDelegate(
        FIsPropertyVisible::CreateLambda(
            [](const FPropertyAndParent& iPropertyAndParent) -> bool
            {
                return !iPropertyAndParent.Property.HasAnyPropertyFlags(CPF_DisableEditOnInstance); //Brush Overrides
            }
        )
    );

    this->ChildSlot
    [
        mDetailsView.ToSharedRef()
    ];
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

void
SOdysseyShape::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    UOdysseyShape* Shape = mShape.Get();
    if (Shape != mCurrentShape)
    {
        mCurrentShape = Shape;
        mDetailsView->SetObject(mShape.Get());
    }
}

#undef LOCTEXT_NAMESPACE


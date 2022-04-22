// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Line/OdysseyHUDLine.h"

UOdysseyHUDLine::UOdysseyHUDLine(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

TSharedPtr<SWidget> UOdysseyHUDLine::CreateWidget()
{
    FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    FDetailsViewArgs args;

    mDetailsView = propertyModule.CreateDetailView(args);
    mDetailsView->SetObject(this);

    return mDetailsView;
}

void UOdysseyHUDLine::Draw(FViewport* iViewport, FCanvas* ioCanvas)
{

}

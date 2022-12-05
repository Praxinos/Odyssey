// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyToolSet.h"
#include "UObject/OdysseyObjectEditorUtils.h"


void
UOdysseyToolSet::AddTool(UOdysseyTool* iTool)
{
    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, "Tools");
    Tools.Add(iTool);
    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, "Tools", EPropertyChangeType::ArrayAdd);
}

void
UOdysseyToolSet::RemoveTool(UOdysseyTool* iTool)
{
    if (SelectedTool == iTool)
        SelectedTool->Inactivate();

    FOdysseyObjectEditorUtils::SetPropertyValue(this, "SelectedTool", nullptr, EPropertyChangeType::ValueSet);
    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, "Tools");
    Tools.Remove(iTool);
    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, "Tools", EPropertyChangeType::ArrayRemove);
}

/* const TSet<UOdysseyTool*>
UOdysseyToolSet::GetTools() const
{
    return Tools;
} */

void
UOdysseyToolSet::SelectTool(UOdysseyTool* iTool)
{
    if (SelectedTool == iTool)
        return;

    if (SelectedTool)
        SelectedTool->Inactivate();

    FOdysseyObjectEditorUtils::SetPropertyValue(this, "SelectedTool", iTool);

    if (SelectedTool)
        SelectedTool->Activate();
}

UOdysseyTool*
UOdysseyToolSet::GetSelectedTool() const
{
    return SelectedTool;
}

void
UOdysseyToolSet::SelectedToolChanged()
{
    // OnSelectedToolChanged().Broadcast(this);
}

void
UOdysseyToolSet::ToolsChanged()
{
    //OnToolsChanged().Broadcast(this);
}

void
UOdysseyToolSet::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == "SelectedTool" )
        SelectedToolChanged();
    if ( iPropertyName == "Tools" )
        ToolsChanged();
}

void
UOdysseyToolSet::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyToolSet::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
    }
}
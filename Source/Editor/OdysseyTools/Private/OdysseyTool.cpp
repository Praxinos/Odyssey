// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTool.h"
#include "Misc/TransactionObjectEvent.h"

UOdysseyTool::UOdysseyTool()
{

}

UOdysseyTool::FOnIsActivableChanged&
UOdysseyTool::OnIsActivableChanged()
{
    static FOnIsActivableChanged onIsActivableChanged;
    return onIsActivableChanged;
}

void
UOdysseyTool::Activate()
{

}

void
UOdysseyTool::Inactivate()
{

}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

bool
UOdysseyTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool
UOdysseyTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

void
UOdysseyTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void
UOdysseyTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}

bool
UOdysseyTool::OnKeyDown(const FKey& iKey)
{
    return false;
}

bool
UOdysseyTool::OnKeyUp(const FKey& iKey)
{
    return false;
}

void
UOdysseyTool::Tick(float iDeltaTime)
{

}

void
UOdysseyTool::Flush()
{

}

void
UOdysseyTool::Commit()
{

}

void
UOdysseyTool::BindShortcuts(FBaseToolkit* iToolkit)
{

}

void
UOdysseyTool::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{

}

void
UOdysseyTool::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == "IsActivable")
        OnIsActivableChanged().Broadcast(this);
}

void
UOdysseyTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyTool::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
    }
}
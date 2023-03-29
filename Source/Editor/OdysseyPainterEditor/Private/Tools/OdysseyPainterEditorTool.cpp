// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/OdysseyPainterEditorTool.h"

#include "Misc/TransactionObjectEvent.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorTool::~UOdysseyPainterEditorTool()
{
}

UOdysseyPainterEditorTool::UOdysseyPainterEditorTool()
    : mEditor (nullptr)
{
}

void
UOdysseyPainterEditorTool::SetEditor(FOdysseyPainterEditor* iEditor)
{
    mEditor = iEditor;
}

void
UOdysseyPainterEditorTool::Activate()
{

}

void
UOdysseyPainterEditorTool::Inactivate()
{
    Flush(); //Finish everything
    Commit(); //Commit the jobs that has been done
}

bool
UOdysseyPainterEditorTool::IsActivable() const
{
    return true;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

bool
UOdysseyPainterEditorTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool
UOdysseyPainterEditorTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

void
UOdysseyPainterEditorTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void
UOdysseyPainterEditorTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}

bool
UOdysseyPainterEditorTool::OnKeyDown(const FKey& iKey)
{
    return false;
}

bool
UOdysseyPainterEditorTool::OnKeyUp(const FKey& iKey)
{
    return false;
}

void
UOdysseyPainterEditorTool::Tick(float iDeltaTime)
{

}

void
UOdysseyPainterEditorTool::Flush()
{

}

void
UOdysseyPainterEditorTool::Commit()
{

}

void
UOdysseyPainterEditorTool::BindShortcuts(FBaseToolkit* iToolkit)
{

}

void
UOdysseyPainterEditorTool::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{

}

void
UOdysseyPainterEditorTool::PropertyChanged(const FName& iPropertyName)
{
}

void
UOdysseyPainterEditorTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyPainterEditorTool::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);
    
    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
    }
}
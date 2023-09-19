// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/OdysseyPainterEditorTool.h"
#include "HUDViewportElement/OdysseyHUDElement.h"

#include "Misc/TransactionObjectEvent.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorTool::~UOdysseyPainterEditorTool()
{
    mHUD->RemoveFromRoot();
}

UOdysseyPainterEditorTool::UOdysseyPainterEditorTool()
    : mEditor (nullptr)
    , mIsActivated(false)
{
    mHUD = NewObject<UOdysseyHUDElement>(GetTransientPackage(), NAME_None, RF_Transient);
    mHUD->AddToRoot();
    mHUD->Init(FName("RootHUD"));
}

void
UOdysseyPainterEditorTool::SetEditor(FOdysseyPainterEditor* iEditor)
{
    mEditor = iEditor;
}

FOdysseyPainterEditor*
UOdysseyPainterEditorTool::GetEditor() const
{
    return mEditor;
}

/* void
UOdysseyPainterEditorTool::OnToolContextChanged()
{
	//If not activable => Inactivate
	if (!IsActivable())
	{
		Inactivate(); //close the tool
		return;
	}

	//Reload the tool to edit the new layer
	Unload();
	Load();
} */

void
UOdysseyPainterEditorTool::Activate()
{    
    //mToolContext->OnChanged().AddUObject(this, &UOdysseyPainterEditorTool::OnToolContextChanged );
    mIsActivated = true;
    Load();
}

void
UOdysseyPainterEditorTool::Inactivate()
{
	//mToolContext->OnChanged().RemoveAll(this);
    Flush(); //Finish everything
    Commit(); //Commit the jobs that has been done
    Unload();
    mIsActivated = false;
}

void
UOdysseyPainterEditorTool::Load()
{

}

void
UOdysseyPainterEditorTool::Unload()
{
    mHUD->EmptyHUDElements();
    mEditor->HUDSystem()->ClearHUDSurface();
}

bool
UOdysseyPainterEditorTool::IsActivable() const
{
    return true;
}

bool
UOdysseyPainterEditorTool::IsActivated() const
{
    return mIsActivated;
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

TSharedRef<SWidget>
UOdysseyPainterEditorTool::CreateTopTabWidget()
{
    return SNullWidget::NullWidget;
}

UOdysseyHUDElement* UOdysseyPainterEditorTool::GetHUD()
{
    return mHUD;
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
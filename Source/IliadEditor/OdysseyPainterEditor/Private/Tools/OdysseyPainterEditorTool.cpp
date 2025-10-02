// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyHUDElement.h"

#include "Framework/Application/SlateApplication.h"
#include "Misc/TransactionObjectEvent.h"
#include "OdysseyPainterEditor.h"
#include "Misc/OdysseyUndoDelegates.h"
#include "OdysseyPainterEditorToolInputProcessor.h"
#include "OdysseyAnimationPlayer.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorTool::~UOdysseyPainterEditorTool()
{
}

UOdysseyPainterEditorTool::UOdysseyPainterEditorTool()
    : mEditor (nullptr)
    , mIsActivated(false)
{
    mHUD = MakeShared<FOdysseyHUDElement>();
    mHUD->SetIsVisible(MakeAttributeUObject(this, &UOdysseyPainterEditorTool::IsHUDVisible));
    mInputProcessor = MakeShared<FOdysseyPainterEditorToolInputProcessor>(this);
}

bool
UOdysseyPainterEditorTool::SupportsColorType(EOdysseyPainterEditorColorType iType)
{
    return iType == EOdysseyPainterEditorColorType::Raw;
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

void
UOdysseyPainterEditorTool::PostInitProperties()
{
    Super::PostInitProperties();
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
    // register IInputProcessor interface for handling global key press
    FSlateApplication::Get().RegisterInputPreProcessor(mInputProcessor);

    //mToolContext->OnChanged().AddUObject(this, &UOdysseyPainterEditorTool::OnToolContextChanged );
    mIsActivated = true;

    mCommandList = MakeShared<FUICommandList>();
    BindShortcuts(mCommandList);

    const TSharedRef<FUICommandList> toolkitCommandList = GetEditor()->GetToolkit()->GetToolkitCommands();
    toolkitCommandList->Append(mCommandList.ToSharedRef());

    Load();
}

void
UOdysseyPainterEditorTool::Inactivate()
{
    // unregister IInputProcessor interface
    FSlateApplication::Get().UnregisterInputPreProcessor(mInputProcessor);

    //mToolContext->OnChanged().RemoveAll(this);
    Flush(); //Finish everything

    mCommandList = nullptr;

    Unload();
    mIsActivated = false;
}

void
UOdysseyPainterEditorTool::Load()
{
    mEditor->HUDSystem()->AddElement(mHUD);
}

void
UOdysseyPainterEditorTool::Unload()
{
    mHUD->EmptyElements();
    mEditor->HUDSystem()->RemoveElement(mHUD);
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
UOdysseyPainterEditorTool::OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    return false;
}

bool
UOdysseyPainterEditorTool::OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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

void UOdysseyPainterEditorTool::BindShortcuts(TSharedPtr<FUICommandList> iCommandList)
{

}

void
UOdysseyPainterEditorTool::ExtendMenu( TSharedRef<FExtender> iExtender )
{

}

void
UOdysseyPainterEditorTool::ExtendToolbar( UToolMenu* iToolMenu )
{
}

TSharedPtr<FOdysseyHUDElement> UOdysseyPainterEditorTool::GetHUD()
{
    return mHUD;
}

bool
UOdysseyPainterEditorTool::IsHUDVisible() const
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();

    return ( player == nullptr ) || ( player->GetStatus() == EOdysseyAnimationPlayerStatus::Stopped );
}

EMouseCursor::Type UOdysseyPainterEditorTool::GetMouseCursor() const
{
    return EMouseCursor::Crosshairs;
}

bool
UOdysseyPainterEditorTool::OnKeyDownGlobal(const FKeyEvent& InKeyEvent)
{
    return false; //false means Unreal will continue as if we did nothing
}

bool
UOdysseyPainterEditorTool::OnKeyUpGlobal(const FKeyEvent& InKeyEvent)
{
    return false; //false means Unreal will continue as if we did nothing
}

FText
UOdysseyPainterEditorTool::GetTooltip() const
{
    return FText();
}

void
UOdysseyPainterEditorTool::PropertyChanged(const FName& iPropertyName)
{
}

void
UOdysseyPainterEditorTool::PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive)
{
}

void
UOdysseyPainterEditorTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    PropertyChanged(PropertyChangedEvent.GetPropertyName(), PropertyChangedEvent.GetMemberPropertyName(), PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive);
    PostPropertyChanged(PropertyChangedEvent.GetMemberPropertyName(), PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive);
}

void
UOdysseyPainterEditorTool::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    if (iIsInteractive)
        return;

    PropertyChanged(iPropertyName);
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
        PropertyChanged(propertyName, propertyName, false);
        FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
            [this, propertyName](bool iIsRedo)
            {
                PostPropertyChanged(propertyName, false);
            }
        );
    }
}

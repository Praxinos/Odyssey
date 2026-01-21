// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane
#include "ArianeEditorTool.h"
#include "ArianeEditor.h"
/* Gary
#include "ArianeEditorToolInputProcessor.h"
*/
// Unreal
#include "Framework/Application/SlateApplication.h"
#include "Misc/TransactionObjectEvent.h"


UArianeEditorTool::~UArianeEditorTool()
{
}

UArianeEditorTool::UArianeEditorTool()
    : mEditor (nullptr)
    , mContextMenuFunc ( nullptr )
{
/* Gary
    mInputProcessor = MakeShared<FArianeEditorToolInputProcessor>(this);
*/
}

UArianeEditorTool::UArianeEditorTool( FContextMenuFunc iContextMenufunc )
    : mEditor (nullptr)
    , mContextMenuFunc ( iContextMenufunc )
{
/* Gary
    mInputProcessor = MakeShared<FArianeEditorToolInputProcessor>(this);
*/
}

void
UArianeEditorTool::SetEditor(FArianeEditor* iEditor)
{
    mEditor = iEditor;
}

FArianeEditor*
UArianeEditorTool::GetEditor() const
{
    return mEditor;
}

void
UArianeEditorTool::PostInitProperties()
{
    Super::PostInitProperties();
}

void UArianeEditorTool::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
}

void
UArianeEditorTool::Activate()
{
    // register IInputProcessor interface for handling global key press
/* Gary
    FSlateApplication::Get().RegisterInputPreProcessor(mInputProcessor);


    mCommandList = MakeShared<FUICommandList>();
    BindShortcuts(mCommandList);

    const TSharedRef<FUICommandList> toolkitCommandList = GetEditor()->GetToolkit()->GetToolkitCommands();
    toolkitCommandList->Append(mCommandList.ToSharedRef());
*/

    Load();
}

void
UArianeEditorTool::Inactivate()
{
    // unregister IInputProcessor interface
/* Gary
    FSlateApplication::Get().UnregisterInputPreProcessor(mInputProcessor);


    //mToolContext->OnChanged().RemoveAll(this);
    Flush(); //Finish everything

    mCommandList = nullptr;
*/
    Unload();
}

void
UArianeEditorTool::Load()
{
}

void
UArianeEditorTool::Unload()
{
}

bool
UArianeEditorTool::IsActivable() const
{
    return true;
}

bool
UArianeEditorTool::IsActivated() const
{
    return mEditor->GetCurrentTool() == this;
}

bool UArianeEditorTool::OnMouseDown( const FVector2D& iViewportCoords, const FKey& iKey, bool iRepeat = false )
{
    return false;
}

void
UArianeEditorTool::OnMouseHover( const FVector2D& iViewportCoords )
{
}

void
UArianeEditorTool::OnMouseDrag( const FVector2D& iViewportCoords )
{
}

bool
UArianeEditorTool::OnMouseUp( const FVector2D& iViewportCoords, const FKey& iKey )
{
    return false;
}

void
UArianeEditorTool::Tick( float iDeltaTime )
{
}

void
UArianeEditorTool::PopupContextMenu()
{
    TSharedPtr<SWidget> contextMenu = CreateContextMenu();

    TSharedPtr<SWindow> window = FSlateApplication::Get().GetActiveTopLevelWindow();
    if (!window)
        return;

    FSlateApplication::Get().PushMenu( window.ToSharedRef(),
                                       FWidgetPath(),
                                       contextMenu.ToSharedRef(),
                                       FSlateApplication::Get().GetCursorPos(),
                                       FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu) );
}

TSharedPtr<SWidget>
UArianeEditorTool::CreateContextMenu()
{
    FMenuBuilder menu( true, mCommandList );

    menu.BeginSection("Context Menu");
    if( mContextMenuFunc.IsBound() )
        mContextMenuFunc.Execute( menu );
    menu.EndSection();

    return menu.MakeWidget();
}

void
UArianeEditorTool::ExtendContextMenu( FMenuBuilder& menu )
{
}

/* Gary

bool
UArianeEditorTool::OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    return false;
}

bool
UArianeEditorTool::OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool
UArianeEditorTool::OnKeyDown(const FKey& iKey)
{
    return false;
}

bool
UArianeEditorTool::OnKeyUp(const FKey& iKey)
{
    return false;
}

bool
UArianeEditorTool::OnKeyDownGlobal(const FKeyEvent& InKeyEvent)
{
    return false; //false means Unreal will continue as if we did nothing
}

bool
UArianeEditorTool::OnKeyUpGlobal(const FKeyEvent& InKeyEvent)
{
    return false; //false means Unreal will continue as if we did nothing
}

void UArianeEditorTool::BindShortcuts(TSharedPtr<FUICommandList> iCommandList)
{
}

void
UArianeEditorTool::ExtendMenu( TSharedRef<FExtender> iExtender )
{

}

void
UArianeEditorTool::ExtendToolbar( UToolMenu* iToolMenu )
{
}
*/

EMouseCursor::Type UArianeEditorTool::GetMouseCursor() const
{
    return EMouseCursor::Crosshairs;
}


FText
UArianeEditorTool::GetTooltip() const
{
    return FText();
}

void
UArianeEditorTool::PropertyChanged(const FName& iPropertyName)
{
}

void
UArianeEditorTool::PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive)
{
}

void
UArianeEditorTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty( PropertyChangedEvent );

    PropertyChanged( PropertyChangedEvent.GetPropertyName()
                   , PropertyChangedEvent.GetMemberPropertyName()
                   , PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive );

    PostPropertyChanged( PropertyChangedEvent.GetMemberPropertyName()
                       , PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive );
}

void
UArianeEditorTool::PropertyChanged( const FName& iPropertyName
                                  , const FName& iMemberPropertyName
                                  , bool iIsInteractive )
{
    if (iIsInteractive)
        return;

    PropertyChanged(iPropertyName);
}

void
UArianeEditorTool::PostTransacted( const FTransactionObjectEvent& iTransactionEvent )
{
    Super::PostTransacted(iTransactionEvent);

/* Gary
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
*/
}

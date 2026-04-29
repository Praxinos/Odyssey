// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDCircle.h"
#include "OdysseyHUDLine.h"

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

void UOdysseyPainterEditorTool::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{

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
UOdysseyPainterEditorTool::Reset()
{
    UOdysseyPainterEditorTool* toolDefaultObject = GetClass()->GetDefaultObject<UOdysseyPainterEditorTool>();
    UEngine::FCopyPropertiesForUnrelatedObjectsParams copyParams;
    copyParams.bDoDelta = false;
    UEngine::CopyPropertiesForUnrelatedObjects(toolDefaultObject, this, copyParams);
}

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
    CancelRIM();

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

void
UOdysseyPainterEditorTool::BeginInteractiveMode()
{
    mIsInInteractiveMode = true;
}

void
UOdysseyPainterEditorTool::EndInteractiveMode()
{
    mIsInInteractiveMode = false;
}

bool
UOdysseyPainterEditorTool::IsInInteractiveMode() const
{
    return mIsInInteractiveMode;
}

bool
UOdysseyPainterEditorTool::HasRadius() const
{
    return false;
}

void
UOdysseyPainterEditorTool::SetRadius(float Radius)
{
}

float
UOdysseyPainterEditorTool::GetRadius() const
{
    return 0.f;
}

EPainterEditorToolRadiusReference
UOdysseyPainterEditorTool::GetRadiusReference() const
{
    return EPainterEditorToolRadiusReference::Texture;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

bool
UOdysseyPainterEditorTool::ProcessMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (mIsRIMActive)
        return true;

    if( GetEditor() )
        GetEditor()->SaveMainToolToRecentTools();

    return OnMouseDown(iPointInTexture, iKey);
}

bool
UOdysseyPainterEditorTool::ProcessMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    if (mIsRIMActive)
        return true;

    return OnMouseClick(iPointInTexture, iKey);
}

bool
UOdysseyPainterEditorTool::ProcessMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (mIsRIMActive)
        return true;

    return OnMouseDoubleClick(iPointInTexture, iKey);
}

bool
UOdysseyPainterEditorTool::ProcessMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (mIsRIMActive)
    {
        EndRIM();
        return true;
    }

    return OnMouseUp(iPointInTexture, iKey);
}

void
UOdysseyPainterEditorTool::ProcessMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if (mIsRIMActive)
    {
        RIMOnMouseMove(iPointInTexture);
    }
    else
    {
        OnMouseHover(iPointInTexture);
    }

    mPreviousMousePosition = iPointInTexture;
}

void
UOdysseyPainterEditorTool::ProcessMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (mIsRIMActive)
    {
        RIMOnMouseDrag(iPointInTexture);
    }
    else
    {
        OnMouseDrag(iPointInTexture);
    }

    mPreviousMousePosition = iPointInTexture;
}

bool
UOdysseyPainterEditorTool::ProcessKeyDown(const FKey& iKey)
{
    if (mIsRIMActive)
    {
        if (iKey == EKeys::Escape)
        {
            CancelRIM();
        }
        return true;
    }

    return OnKeyDown(iKey);
}

bool
UOdysseyPainterEditorTool::ProcessKeyUp(const FKey& iKey)
{
    if( mIsRIMActive )
        return true;

    return OnKeyUp(iKey);
}

bool
UOdysseyPainterEditorTool::ProcessKeyUpGlobal(const FKeyEvent& InKeyEvent)
{
    if( mIsRIMActive )
    {
        //return false to let other parts of Unreal know that the key has been released
        //otherwise if we activate RIM a second time, the shortcut is not triggered
        return false;
    }

    return OnKeyUpGlobal(InKeyEvent);
}

bool
UOdysseyPainterEditorTool::ProcessKeyDownGlobal(const FKeyEvent& InKeyEvent)
{
    return OnKeyDownGlobal(InKeyEvent);
}

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

void
UOdysseyPainterEditorTool::StartRadiusInteractiveModifier()
{
    if (!HasRadius())
        return;

    if (mIsRIMActive)
        return;

    //If mouse is down, we consider don't start this tool
    //Unreal still sends shortcuts while the mouse is down
    //If we start this tool while the mouse is down, the tool currently using the mouse
    //will not receive the up of the mouse, which lead to lots of problems
    if ( !FSlateApplication::Get().GetPressedMouseButtons().IsEmpty() )
        return;

    mIsRIMActive = true;

    mRIMStartRadius = GetRadius();

    mRIMHUD = MakeShared<FOdysseyHUDCircle>(
        GetRIMCenter(mPreviousMousePosition),
        mRIMStartRadius
    );

    mRIMHorizontalHUD = MakeShared<FOdysseyHUDLine>(mRIMHUD->GetCenter() + FVector2D(-mRIMHUD->GetRadius(), 0), mRIMHUD->GetCenter() + FVector2D(mRIMHUD->GetRadius(), 0));
    mRIMVerticalHUD = MakeShared<FOdysseyHUDLine>(mRIMHUD->GetCenter() + FVector2D(0, -mRIMHUD->GetRadius()), mRIMHUD->GetCenter() + FVector2D(0, mRIMHUD->GetRadius()));

    mRIMHUD->AddElement(mRIMHorizontalHUD);
    mRIMHUD->AddElement(mRIMVerticalHUD);

    //We need customization to set the color, but we could do this easier
    //by removing customization and make its behaviour directly part of the HUD system
    FOdysseyHUDElement::FHUDCustomization customization;
    customization.mColors.Add(FLinearColor::Red);
    mRIMHUD->SetCustomization(customization);

    //Replace the HUD with the RIMHUD
    mEditor->HUDSystem()->RemoveElement(mHUD);
    mEditor->HUDSystem()->AddElement(mRIMHUD);

    switch(GetRadiusReference())
    {
        case EPainterEditorToolRadiusReference::Texture:
        {
            mRIMHUD->SetReference(EOdysseyHUDReference::Texture);
            mRIMHorizontalHUD->SetReference(EOdysseyHUDReference::Texture);
            mRIMVerticalHUD->SetReference(EOdysseyHUDReference::Texture);
        }
        break;

        case EPainterEditorToolRadiusReference::HUD:
        {
            mRIMHUD->SetReference(EOdysseyHUDReference::HUD);
            mRIMHorizontalHUD->SetReference(EOdysseyHUDReference::HUD);
            mRIMVerticalHUD->SetReference(EOdysseyHUDReference::HUD);
        }
        break;
    }

    BeginInteractiveMode();

    //UE_LOG(LogTemp, Warning, TEXT("StartRadiusInteractiveModifier x=%f, y=%f", mPreviousMousePosition.X, mPreviousMousePosition.Y));
}

void
UOdysseyPainterEditorTool::EndRIM()
{
    if (!mIsRIMActive)
        return;

    EndInteractiveMode();
    SetRadius(GetRadius()); //Force a non interactive call to validate the Radius

    mIsRIMActive = false;

    //Put the tool's HUD in place again
    mEditor->HUDSystem()->RemoveElement(mRIMHUD);
    mEditor->HUDSystem()->AddElement(mHUD);

    mRIMStartRadius = 0.f;
    mRIMHUD = nullptr;
    mRIMHorizontalHUD = nullptr;
    mRIMVerticalHUD = nullptr;
}

void
UOdysseyPainterEditorTool::CancelRIM()
{
    if (!mIsRIMActive)
        return;

    SetRadius(mRIMStartRadius); //Cancel by setting mRIMStartRadius again
    EndInteractiveMode();


    mIsRIMActive = false;

    //Put the tool's HUD in place again
    mEditor->HUDSystem()->RemoveElement(mRIMHUD);
    mEditor->HUDSystem()->AddElement(mHUD);

    mRIMStartRadius = 0.f;
    mRIMHUD = nullptr;
    mRIMHorizontalHUD = nullptr;
    mRIMVerticalHUD = nullptr;
}

FVector2D
UOdysseyPainterEditorTool::GetRIMCenter(const FVector2D& iMousePositionInTexture) const
{
    FVector2D delta(GetRadius() * UE_DOUBLE_INV_SQRT_2, GetRadius() * UE_DOUBLE_INV_SQRT_2);
    FVector2D center = FVector2D(iMousePositionInTexture);
    switch(GetRadiusReference())
    {
        case EPainterEditorToolRadiusReference::Texture:
        {
            center -= delta;
        }
        break;

        case EPainterEditorToolRadiusReference::HUD:
        {
            center = mEditor->HUDSystem()->TextureToHUD( FVector2D(iMousePositionInTexture) );
            center -= delta;
        }
        break;
    }

    return center;
}

void
UOdysseyPainterEditorTool::RIMOnMouseMove(const FOdysseyPoint& iPointInTexture)
{
    FVector2D center = GetRIMCenter(iPointInTexture);

    mRIMHUD->SetCenter(center);
    mRIMHorizontalHUD->SetStartPoint(mRIMHUD->GetCenter() + FVector2D(-mRIMHUD->GetRadius(), 0));
    mRIMHorizontalHUD->SetEndPoint(mRIMHUD->GetCenter() + FVector2D(mRIMHUD->GetRadius(), 0));
    mRIMVerticalHUD->SetStartPoint(mRIMHUD->GetCenter() + FVector2D(0, -mRIMHUD->GetRadius()));
    mRIMVerticalHUD->SetEndPoint(mRIMHUD->GetCenter() + FVector2D(0, mRIMHUD->GetRadius()));
}

void
UOdysseyPainterEditorTool::RIMOnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FVector2D center(0,0);
    FVector2D mousePosition(0,0);

    switch(GetRadiusReference())
    {
        case EPainterEditorToolRadiusReference::Texture:
        {
            center =  mRIMHUD->GetCenter();
            mousePosition =  iPointInTexture;
        }
        break;

        case EPainterEditorToolRadiusReference::HUD:
        {
            center =  mRIMHUD->GetCenter();
            mousePosition =  mEditor->HUDSystem()->TextureToHUD( iPointInTexture );
        }
        break;
    }

    float distance = FMath::CeilToFloat( FVector2D::Distance(mousePosition, center) );
    float radius = distance;

    mRIMHUD->SetRadius(radius);

    mRIMHorizontalHUD->SetStartPoint(mRIMHUD->GetCenter() + FVector2D(-mRIMHUD->GetRadius(), 0));
    mRIMHorizontalHUD->SetEndPoint(mRIMHUD->GetCenter() + FVector2D(mRIMHUD->GetRadius(), 0));
    mRIMVerticalHUD->SetStartPoint(mRIMHUD->GetCenter() + FVector2D(0, -mRIMHUD->GetRadius()));
    mRIMVerticalHUD->SetEndPoint(mRIMHUD->GetCenter() + FVector2D(0, mRIMHUD->GetRadius()));

    SetRadius(radius);
}

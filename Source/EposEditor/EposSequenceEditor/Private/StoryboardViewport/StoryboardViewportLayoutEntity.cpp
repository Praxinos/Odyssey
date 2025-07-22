// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "StoryboardViewport/StoryboardViewportLayoutEntity.h"

#include "Widgets/SWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Framework/Application/SlateApplication.h"
#include "LevelViewportLayout.h"
#include "SLevelViewport.h"
#include "StoryboardViewport/SStoryboardLevelViewport.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"

//---

FStoryboardViewportLayoutEntity::FStoryboardViewportLayoutEntity( const FAssetEditorViewportConstructionArgs& iArgs, TSharedPtr<ILevelEditor> InLevelEditor )
    : Widget( SNew( SStoryboardLevelViewport )
              .ParentLayout( iArgs.ParentLayout )
              .ParentLevelEditor( InLevelEditor )
              .LayoutName( iArgs.ConfigKey )
              )
{
}

//---

TSharedPtr<SLevelViewport>
FStoryboardViewportLayoutEntity::AsLevelViewport() const //override
{
    return Widget->GetLevelViewport();
}

FLevelEditorViewportClient&
FStoryboardViewportLayoutEntity::GetLevelViewportClient() const //override
{
    return Widget->GetLevelViewport()->GetLevelViewportClient();
}

bool
FStoryboardViewportLayoutEntity::IsPlayInEditorViewportActive() const //override
{
    return Widget->GetLevelViewport()->IsPlayInEditorViewportActive();
}

void
FStoryboardViewportLayoutEntity::RegisterGameViewportIfPIE() //override
{
    return Widget->GetLevelViewport()->RegisterGameViewportIfPIE();
}

//---

TSharedRef<SWidget>
FStoryboardViewportLayoutEntity::AsWidget() const //override
{
    return Widget;
}

void
FStoryboardViewportLayoutEntity::SetKeyboardFocus() //override
{
    FSlateApplication::Get().SetKeyboardFocus( Widget->GetLevelViewport() );
}

void
FStoryboardViewportLayoutEntity::OnLayoutDestroyed() //override
{
    if (IsPlayInEditorViewportActive() || GetLevelViewportClient().IsSimulateInEditorViewport() )
    {
        GUnrealEd->EndPlayMap();
    }
}

void
FStoryboardViewportLayoutEntity::SaveConfig( const FString& iConfigString ) //override
{
    return Widget->GetLevelViewport()->SaveConfig( iConfigString );
}

FName
FStoryboardViewportLayoutEntity::GetType() const //override
{
    static FName sStoryboardName("Storyboard");
    return sStoryboardName;
}

void
FStoryboardViewportLayoutEntity::TakeHighResScreenShot() const //override
{
}

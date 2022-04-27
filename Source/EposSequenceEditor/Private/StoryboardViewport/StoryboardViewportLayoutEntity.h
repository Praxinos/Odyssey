// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "AssetEditorViewportLayout.h"
#include "LevelViewportLayout.h"

//---

class FLevelEditorViewportClient;
class SLevelViewport;
class SStoryboardLevelViewport;
class SWidget;

//---

class FStoryboardViewportLayoutEntity
    : public ILevelViewportLayoutEntity
{
public:
    FStoryboardViewportLayoutEntity( const FAssetEditorViewportConstructionArgs& iArgs, TSharedPtr<ILevelEditor> InLevelEditor );

    //~ ILevelViewportLayoutEntity interface
    virtual TSharedPtr<SLevelViewport> AsLevelViewport() const override;
    virtual FLevelEditorViewportClient& GetLevelViewportClient() const override;
    virtual bool IsPlayInEditorViewportActive() const override;
    virtual void RegisterGameViewportIfPIE() override;

    //~ IEditorViewportLayoutEntity interface
    virtual TSharedRef<SWidget> AsWidget() const override;
    virtual void SetKeyboardFocus() override;
    virtual void OnLayoutDestroyed() override;
    virtual void SaveConfig( const FString& iConfigString ) override;
    virtual FName GetType() const override;
    virtual void TakeHighResScreenShot() const override;

protected:
    /** This entity's widget */
    TSharedRef<SStoryboardLevelViewport> Widget;
};

// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

class FBaseToolkit;

class ODYSSEYEDITOR_API FOdysseyEditorContextMenu :
    public TSharedFromThis<FOdysseyEditorContextMenu>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyEditorContextMenu();
    FOdysseyEditorContextMenu(FName iID, FText iDisplayName, FSlateIcon iIcon);

public:
    void Init();

public:
    virtual TSharedPtr<SWidget> CreateWidget() = 0;
    virtual void BindShortcuts(FBaseToolkit* iToolkit);

public:
    const FName&                        ID() const;
    const FText&                        DisplayName() const;
    const FSlateIcon&                   Icon() const;
    virtual const TSharedPtr<SWidget>&  Widget() const;

protected:
    FMenuBuilder mMenu;

private:
    FName mID;
    FText mDisplayName;
    FSlateIcon mIcon;
    TSharedPtr<SWidget> mWidget;
};

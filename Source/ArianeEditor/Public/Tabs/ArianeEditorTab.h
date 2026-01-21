// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once


#include "Textures/SlateIcon.h"

class FBaseToolkit;
class FTabManager;

class ARIANEEDITOR_API FArianeEditorTab :
    public TSharedFromThis<FArianeEditorTab>
{
public:
    // Construction / Destruction
    virtual ~FArianeEditorTab();
    FArianeEditorTab(FText iDisplayName, FSlateIcon iIcon);

public:
    virtual void Init();
    void Open();
    void Close();
    bool IsOpen() const;
    const FText& GetName() const;
    const FSlateIcon& GetIcon() const;

public:
    virtual bool CanOpen() const;
    virtual const FName& GetId() const = 0;
    virtual TSharedPtr<SWidget> CreateWidget() = 0;
    virtual void BindShortcuts(FBaseToolkit* iToolkit);
    virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& iArgs);
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender );

    void SetTabManager(TSharedPtr< FTabManager > iTabManager);
    TSharedPtr< FTabManager > GetTabManager() const;
    void Register(TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef);
    void Unregister();

public:
    const FText&                        DisplayName() const;
    const FSlateIcon&                   Icon() const;
    virtual const TSharedPtr<SWidget>&  Widget() const;

private:
    FText mDisplayName;
    FSlateIcon mIcon;
    TSharedPtr<SWidget> mWidget;
    TSharedPtr<FTabManager> mTabManager;
};

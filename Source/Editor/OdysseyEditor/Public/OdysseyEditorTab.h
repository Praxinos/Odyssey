// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class ODYSSEYEDITOR_API FOdysseyEditorTab :
    public TSharedFromThis<FOdysseyEditorTab>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyEditorTab();
    FOdysseyEditorTab(FText iDisplayName, FSlateIcon iIcon);

public:
    virtual void Init();
    void Open();
    void Close();
    bool IsOpened() const;
    const FText& GetName() const;
    const FSlateIcon& GetIcon() const;
    FMinorTabConfig GetMinorTabConfig();

    bool ShouldOpenByDefault() const;
    void ShouldOpenByDefault(bool iShouldOpenByDefault);

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
    bool mShouldOpenByDefault;
    TSharedPtr<SWidget> mWidget;
    TWeakPtr< FTabManager > mTabManager;
};


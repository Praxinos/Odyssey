// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class ODYSSEYEDITOR_API FOdysseyEditorTab :
	public TSharedFromThis<FOdysseyEditorTab>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyEditorTab();
    FOdysseyEditorTab(FName iID, FText iDisplayName, FSlateIcon iIcon);

public:
    void Init();

public:
    virtual TSharedPtr<SWidget> CreateWidget() = 0;
    virtual void BindShortcuts(FBaseToolkit* iToolkit);
    virtual void OnToolkitInitialized(FBaseToolkit* iToolkit);
    virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& iArgs);
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName );

public:
    const FName&                        ID() const;
    const FText&                        DisplayName() const;
    const FSlateIcon&                   Icon() const;
    virtual const TSharedPtr<SWidget>&  Widget() const;

private:
    FName mID;
    FText mDisplayName;
    FSlateIcon mIcon;
    TSharedPtr<SWidget> mWidget;
};


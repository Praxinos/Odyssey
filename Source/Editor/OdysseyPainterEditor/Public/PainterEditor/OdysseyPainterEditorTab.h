// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorTab :
	public TSharedFromThis<FOdysseyPainterEditorTab>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorTab();
    FOdysseyPainterEditorTab(FName iID, FText iDisplayName, FSlateIcon iIcon);

public:
    void Init();

public:
    virtual TSharedPtr<SWidget> CreateWidget() = 0;
    virtual void BindShortcuts();
    virtual void OnToolkitInitialized();
    virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& iArgs);
    virtual void FillExtender(TSharedPtr<FExtender>& ioExtender);

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


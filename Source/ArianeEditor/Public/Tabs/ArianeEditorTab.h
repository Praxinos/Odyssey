// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "Textures/SlateIcon.h"

class FBaseToolkit;
class FTabManager;
class SWidget;

class ARIANEEDITOR_API FArianeEditorTab :
    public TSharedFromThis<FArianeEditorTab>
{
public:
    // Construction / Destruction
    virtual ~FArianeEditorTab();
    FArianeEditorTab( FText DisplayName, FSlateIcon InIcon);

public:
    /** Init the tab */
    virtual void Init();

    /** Open the tab */
    void Open();

    /** Close the tab */
    void Close();

    /** Check if the tab is open */
    bool IsOpen() const;

    /**
    * @brief Get the tab's name
    * @return the tab's name
    */
    const FText& GetName() const;

    /**
    * @brief Get the tab's icon
    * @return the tab's icon
    */
    const FSlateIcon& GetIcon() const;

    /** Check if the tab can open */
    virtual bool CanOpen() const;

    /**
    * @brief Get the tab's ID
    * @return the tab's ID
    */
    virtual const FName& GetId() const = 0;

    /**
    * @brief Create the tab's widget
    * @return the tab's widget
    */
    virtual TSharedPtr<SWidget> CreateWidget() = 0;

    /**
    * @brief Bind shortcuts
    * @param Toolkit the toolkit
    */
    virtual void BindShortcuts(FBaseToolkit* Toolkit);

    /**
    * @brief Spawn the tab
    * @param Args
    */
    virtual TSharedRef<SDockTab> SpawnTab( const FSpawnTabArgs& Args );

    virtual void ExtendMenu( TSharedRef<FExtender> Extender );

    void Register(TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef);
    void Unregister();

    /** Get the tab's widget */
    virtual const TSharedPtr<SWidget>&  GetWidget() const;

    bool IsOpened();

private:
    static TSharedPtr< FTabManager > GetTabManager();


private:
    FText DisplayName;
    FSlateIcon Icon;
    TSharedPtr<SWidget> Widget;
};

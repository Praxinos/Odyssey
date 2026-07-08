// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

#include "SOdysseyViewport.h"
#include "OdysseyEditorTab.h"
#include "OdysseyEventState.h"
#include "Input/OdysseyPoint.h"

class FOdysseyPainterEditor;
class SOdysseyViewport;
class FOdysseyPainterEditorViewportClient;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorViewportTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorViewportTab();
    FOdysseyPainterEditorViewportTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void OnTabClosed(TSharedRef<SDockTab> DockTab) override;
    virtual void CloseTab() override;

    virtual void BuildOptionsMenu( FMenuBuilder& ioMenuBuilder );
    virtual void BuildBackgroundSubMenu( FMenuBuilder& ioMenuBuilder );
    virtual FReply OnColorBlockClicked( TSharedPtr<SButton> iWidget, FLinearColor iInitialColor, FOnLinearColorValueChanged OnColorChanged, FSimpleDelegate OnColorCommitted ) const;
    virtual void BuildBackgroundViewportEntry( FMenuBuilder& ioMenuBuilder );
    virtual void BuildBackgroundCheckboardColorOneEntry( FMenuBuilder& ioMenuBuilder );
    virtual void BuildBackgroundCheckboardColorTwoEntry( FMenuBuilder& ioMenuBuilder );
    virtual void BuildBackgroundCheckboardSizeEntry( FMenuBuilder& ioMenuBuilder );
    virtual void BuildBackgroundCheckboardPresets( FMenuBuilder& ioMenuBuilder );

public:
    // Public Setters
    void SetTexture(const TAttribute<UTexture*>& mTexture);
    void SetDefaultTexture();

public:
    // Public Getters
    TSharedPtr<SOdysseyViewport> GetViewport();

protected:
    // Widget Getters
    virtual UTexture* Texture() const;

protected:
    // Event Listeners
    virtual bool OnViewportMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnViewportMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnViewportMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnViewportMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnViewportMouseHover(const FOdysseyPoint& iPointInTexture);
    virtual void OnViewportMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnViewportKeyDown(const FKey& iKey);
    virtual bool OnViewportKeyUp(const FKey& iKey);

protected:
    //HUD
    //virtual void OnViewportSizeChanged(FViewport* iViewport, uint32 iUnused);

private:
    TAttribute<UTexture*> mTexture;
    FOdysseyPainterEditor* mEditor;

    TSharedPtr<SOdysseyViewport> mViewport;
    TSharedPtr<FOdysseyPainterEditorViewportClient> mViewportClient;
};

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorTopTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorTopTab();
    FOdysseyPainterEditorTopTab( FOdysseyPainterEditor* iEditor );

    // Spawner callback
virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& iArgs) override;

    // Public Getters
public:
    virtual bool IsEraserButtonActive() const;
    virtual bool IsPackageEdited() const;

    // Public Setters
public:
    virtual void SetToolDefaultBlendingMode( ::ULIS::eBlendMode );
    virtual void SetToolDefaultAlphaMode( ::ULIS::eAlphaMode );

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts( FBaseToolkit* iToolkit ) override;

protected:
    // Widget Getters

    virtual int OnGetSize() const;
    virtual float OnGetOpacity() const;
    virtual float OnGetFlow() const;
    virtual ::ULIS::eBlendMode BlendingMode() const;
    virtual ::ULIS::eAlphaMode AlphaMode() const;

protected:
    // Event Listeners
    virtual void OnSizeChanged( int32 iValue );
    virtual void OnOpacityChanged( int32 iValue );
    virtual void OnFlowChanged( int32 iValue );
    virtual void OnBlendingModeChanged( int32 iValue );
    virtual void OnAlphaModeChanged( int32 iValue );
    virtual FReply OnSaveButtonClicked();
    virtual FReply OnUndoButtonClicked();
    virtual FReply OnRedoButtonClicked();
    virtual FReply OnEraserButtonClicked();

protected:
    // Methods
    virtual void ToggleEraserButton();
    virtual void SetAlphaModeShortcut( ::ULIS::eAlphaMode iAlphaMode );
    virtual void SetAlphaMode( ::ULIS::eAlphaMode iAlphaMode );
    virtual void SetBlendingMode(::ULIS::eBlendMode iBlendingMode);
    virtual void AddSize( int32 iValue );

private:
    FOdysseyPainterEditor* mEditor;
    ::ULIS::eBlendMode mToolDefaultBlendingMode;
    ::ULIS::eAlphaMode mToolDefaultAlphaMode;
    bool mIsEraserButtonActive;
};


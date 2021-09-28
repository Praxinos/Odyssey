// IDDN FR.001.250001.004.S.X.2019.000.00000
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
    virtual void SetToolDefaultBlendingMode( ::ul3::eBlendingMode );
    virtual void SetToolDefaultAlphaMode( ::ul3::eAlphaMode );

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts( FBaseToolkit* iToolkit ) override;

protected:
    // Widget Getters

    virtual int OnGetSize() const;
    virtual float OnGetOpacity() const;
    virtual float OnGetFlow() const;
    virtual ::ul3::eBlendingMode BlendingMode() const;
    virtual ::ul3::eAlphaMode AlphaMode() const;

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
    virtual void SetAlphaMode( ::ul3::eAlphaMode iAlphaMode );
    virtual void AddSize( int32 iValue );

private:
    FOdysseyPainterEditor* mEditor;
    ::ul3::eBlendingMode mToolDefaultBlendingMode;
    ::ul3::eAlphaMode mToolDefaultAlphaMode;
    bool mIsEraserButtonActive;
};


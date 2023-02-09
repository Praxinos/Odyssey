// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"
#include "SOdysseyPaintModifiers.h"

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

    // Setter to change this tab's widget size from pure pixel value to percentage of mesh
    void    SetMeshMaxSize(float iValue = -1);


protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts( FBaseToolkit* iToolkit ) override;

protected:
    // Widget Getters

    virtual bool IsPackageEdited() const;
    virtual bool IsEraserButtonActive() const;
    virtual float OnGetSize() const;
    virtual float OnGetOpacity() const;
    virtual float OnGetFlow() const;
    virtual ::ULIS::eBlendMode BlendingMode() const;
    virtual ::ULIS::eAlphaMode AlphaMode() const;

protected:
    // Event Listeners
    virtual void OnSizeChanged( float iValue, EPropertyChangeType::Type iChangeType);
    virtual void OnOpacityChanged( int32 iValue, EPropertyChangeType::Type iChangeType);
    virtual void OnFlowChanged( int32 iValue, EPropertyChangeType::Type iChangeType);
    virtual void OnBlendingModeChanged( int32 iValue );
    virtual void OnAlphaModeChanged( int32 iValue );
    virtual FReply OnSaveButtonClicked();
    virtual FReply OnUndoButtonClicked();
    virtual FReply OnRedoButtonClicked();
    virtual FReply OnEraserButtonClicked();

protected:
    // Methods
    virtual void ToggleEraserButton();
    virtual void SetAlphaMode( ::ULIS::eAlphaMode iAlphaMode );
    virtual void SetBlendingMode(::ULIS::eBlendMode iBlendingMode);
    virtual void AddSize( int32 iValue );

private:
    FOdysseyPainterEditor* mEditor;
    TSharedPtr<SOdysseyPaintModifiers> mWidget;
};


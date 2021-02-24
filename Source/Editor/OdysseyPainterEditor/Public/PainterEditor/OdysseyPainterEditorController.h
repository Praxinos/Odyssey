// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include <ULIS3>
#include "OdysseyStylusInputSettings.h"
#include "OdysseyEventState.h"

class FOdysseyPainterEditorToolkit;
class FOdysseyPainterEditorGUI;
class UOdysseyBrush;
class UBlueprint;

/**
 * Implements an Editor Controller for textures.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorController :
	public TSharedFromThis<FOdysseyPainterEditorController>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorController();
    FOdysseyPainterEditorController();

public:
    // Init
	void InitOdysseyPainterEditorController(const TSharedRef<FUICommandList>& iToolkitCommands);

public:
    // Virtual Methods
    virtual FOdysseyPainterEditor* GetEditor() const = 0;
    virtual TSharedPtr<FOdysseyPainterEditorGUI> GetGUI() = 0;

public:
	virtual void OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles);
	virtual void OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles);
	virtual void OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles);
	virtual void OnPaintEngineStrokeAbort();

    virtual void OnAboutIliad();
    virtual void OnVisitPraxinosWebsite();
    virtual void OnVisitPraxinosForums();

    //Shortcut Actions
    void           OnSwitchTabletAPI();

protected:
    // Commands building
    virtual void BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands);

private:
    TSharedRef<SWidget> GenerateTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iItem );
    void ChangeSelectionTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iNewSelection, ESelectInfo::Type iSelectInfo );
    FText GetComboBoxTabletAPISelectedAsText() const;

private:
    TSharedPtr<EOdysseyStylusInputDriver>       mComboBoxTabletAPISelected;
};


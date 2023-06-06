// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Styling/SlateTypes.h"
#include "Color/SOdysseyColorSlider.h"
#include "OdysseyPalette.h"

/**
 * The Model of the MeshSelector
 */
class ODYSSEYWIDGETS_API FOdysseyPalette :  public TSharedFromThis<FOdysseyPalette>
{

    friend class SOdysseyPalette; // For the access to private callbacks

public:
    // Construction / Destruction
    FOdysseyPalette();
    ~FOdysseyPalette();

public:
    //Getter / Setter
    UOdysseyPalette* GetColorPalette() const;
    void SetColorPalette( UOdysseyPalette* iColorPalette );
    void AddNewPaletteEntry();

private:
    //CallBacks


private:
    UOdysseyPalette* mPalette;
};

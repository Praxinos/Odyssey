// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPalette.h"

/**
 * The Model of the MeshSelector
 */
class ODYSSEYPALETTE_API FOdysseyPalette :  public TSharedFromThis<FOdysseyPalette>
{

    friend class SOdysseyPalette; // For the access to private callbacks

public:
    // Construction / Destruction
    FOdysseyPalette();
    ~FOdysseyPalette();

public:
    //Getter / Setter
    UOdysseyPalette* GetPalette() const;
    void SetPalette( UOdysseyPalette* iColorPalette );

private:
    //CallBacks


private:
    UOdysseyPalette* mPalette;
};

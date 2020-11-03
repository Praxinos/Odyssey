// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "WintabStylusInputInterface.h"

#if PLATFORM_WINDOWS

// Done in its own file

#elif PLATFORM_MAC

// Done in its own file too

#else

TSharedPtr<IStylusInputInterfaceInternal>
CreateStylusInputInterfaceWintab()
{
    return TSharedPtr<IStylusInputInterfaceInternal>();
}

#endif

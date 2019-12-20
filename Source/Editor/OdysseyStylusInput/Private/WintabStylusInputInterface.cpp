// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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

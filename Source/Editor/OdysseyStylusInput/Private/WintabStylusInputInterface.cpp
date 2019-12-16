// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "WintabStylusInputInterface.h"

#if PLATFORM_WINDOWS

// Done in its own file

#else

TSharedPtr<IStylusInputInterfaceInternal>
CreateStylusInputInterfaceWintab()
{
    return TSharedPtr<IStylusInputInterfaceInternal>();
}

#endif

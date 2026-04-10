// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane headers
#include "ArianeLayerInvalidationFlags.h"

#include "ArianeLayerFolderInvalidationFlags.generated.h"

USTRUCT()
struct ARIANE_API FArianeLayerFolderInvalidationFlags : public FArianeLayerInvalidationFlags
{
    GENERATED_BODY()

    public:
        static const uint32 StaticClass() { return 0x63b8056a; }; // value is crc32 FArianeLayerFolderInvalidationFlags
        virtual uint32 GetClass() { return StaticClass(); };
        virtual bool HasBaseClass( uint32 BaseClass ) const;

    public:
        virtual FArianeLayerFolderInvalidationFlags& AND( const FArianeLayerFolderInvalidationFlags& RHS );
        virtual FArianeLayerFolderInvalidationFlags& OR( const FArianeLayerFolderInvalidationFlags& RHS );
        virtual FArianeLayerFolderInvalidationFlags& SetAll();
        virtual FArianeLayerFolderInvalidationFlags& Clear();
        virtual bool HasAny();

    public:
        FArianeLayerFolderInvalidationFlags& SetHierarchy() { Hierarchy = 1; return *this; };

    public:
        uint32 Hierarchy : 1 = 0;
};

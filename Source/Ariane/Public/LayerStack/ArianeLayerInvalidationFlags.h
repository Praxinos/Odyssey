// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"

#include "ArianeLayerInvalidationFlags.generated.h"

USTRUCT()
struct ARIANE_API FArianeLayerInvalidationFlags
{
    GENERATED_BODY()

    public:
        static const uint32 StaticClass() { return  0xdfaa58f0; }; // value is crc32 FArianeLayerInvalidationFlags
        virtual uint32 GetClass() { return StaticClass(); };
        virtual bool HasBaseClass( uint32 BaseClass ) const;

    public:
        virtual FArianeLayerInvalidationFlags& AND( const FArianeLayerInvalidationFlags& RHS );
        virtual FArianeLayerInvalidationFlags& OR( const FArianeLayerInvalidationFlags& RHS );
        virtual FArianeLayerInvalidationFlags& SetAll();
        virtual FArianeLayerInvalidationFlags& Clear();
        virtual bool HasAny();

    public:
        FArianeLayerInvalidationFlags& SetTransform() { Transform = 1; return *this; };
        FArianeLayerInvalidationFlags& SetSelected()  { Selected  = 1; return *this; };

    public:
        uint32 Selected  : 1 = 0;
        uint32 Transform : 1 = 0;
};

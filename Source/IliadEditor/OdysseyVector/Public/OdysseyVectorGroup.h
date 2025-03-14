// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"

//#include "OdysseyVectorGroup.generated.h"

class ODYSSEYVECTOR_API FOdysseyVectorGroup : public FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0xd4c8357b; // value is crc32 FOdysseyVectorGroup

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID ) override;

        /**
         * @brief destructor
         */
        virtual ~FOdysseyVectorGroup(){};

        /**
         * @brief constructor
         */
        FOdysseyVectorGroup( const FString& iName );

        /**
         * @brief Init a group
         * @param iName
         */
        void Init( FString& iName );
        virtual void UpdateBBox();

    protected:
       /**
         * @brief Pick this shape.
         * @param iFlags selection flags from the engine.
         */
        virtual bool PickShape( const ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ) override;

       /**
         * @brief Copy this group (for copy-paste operations).
         * @return a newly allocated group.
         */
        virtual FOdysseyVectorObject* CopyShape( uint64 iCopyFlags ) override;

       /**
         * @brief Update this ellipse (update cached data if any).
         * @param iFlags update flags from the engine.
         */
        virtual void UpdateShape( uint32 iUpdateFlags ) override;
};

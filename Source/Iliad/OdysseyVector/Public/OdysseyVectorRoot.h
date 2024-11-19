// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorEngine.h"

class IOdysseyVectorLayer;
class IOdysseyVectorCell;
class FOdysseyVectorGroupPaint;

//#include "OdysseyVectorRoot.generated.h"

class ODYSSEYVECTOR_API FOdysseyVectorRoot : public FOdysseyVectorObject
{
    private:
        static const uint32 mStaticClass = 0x78514e2d; // value is crc32 FOdysseyVectorRoot

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID ) override;

        /**
         * @brief destructor
         */
        virtual ~FOdysseyVectorRoot(){};

        /**
         * @brief constructor
         */
        FOdysseyVectorRoot( IOdysseyVectorLayer* iLayer
                          , IOdysseyVectorCell* iCell
                          , FOdysseyVectorGroupPaint* iScene
                          , uint32 iPreferredWidth
                          , uint32 iPreferredHeight );

        void SetScene( FOdysseyVectorGroupPaint* iScene );
        FOdysseyVectorGroupPaint* GetScene();
        virtual void Invalidate( uint64 iInvalidationFlags ) override;
        FOdysseyVectorEngine* GetEngine();

    protected:
        FOdysseyVectorEngine mEngine;
       FOdysseyVectorGroupPaint* mScene;
};

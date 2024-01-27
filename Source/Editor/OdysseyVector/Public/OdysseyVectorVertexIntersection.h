#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertex.h"

class FOdysseyVectorIntersection;
class FOdysseyVectorObject;

class ODYSSEYVECTOR_API FOdysseyVectorVertexIntersection : public FOdysseyVectorVertex
{
    private:
        static const uint32 mStaticClass =  0x29459195; // value is crc32 FOdysseyVectorVertexIntersection
    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };

        /**
         * @brief Destructor.
         */
        ~FOdysseyVectorVertexIntersection();

        /**
         * @brief Constructor.
         */
        FOdysseyVectorVertexIntersection( FOdysseyVectorObject* iOwner
                                        , uint32 iID
                                        , double iX
                                        , double iY
                                        , FOdysseyVectorIntersection* iIntersection0
                                        , FOdysseyVectorIntersection* iIntersection1 );

        void SetIntersection( FOdysseyVectorIntersection* iIntersection0
                            , FOdysseyVectorIntersection* iIntersection1 );

        FOdysseyVectorIntersection* GetIntersection( uint32 iIndex );

    protected:
        FOdysseyVectorIntersection* mIntersection[2];
};

// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane Headers
#include "ArianeObject.h"

class FArianeSegment;
class FArianeVertex;

class ARIANE_API FArianePath : public FArianeObject
{
    public:
        ~FArianePath();
        FArianePath();

    public:
        void AddVertex( FArianeVertex* iVertex );
        void AddSegment( FArianeSegment* iSegment );

        void RemoveVertex( FArianeVertex* iVertex );
        void RemoveSegment( FArianeSegment* iSegment );

        const TArray<FArianeSegment*>& GetSegments();
        const TArray<FArianeVertex*>& GetVertices();

    protected:
        TArray<FArianeSegment*> Segments;
        TArray<FArianeVertex*> Vertices;

};

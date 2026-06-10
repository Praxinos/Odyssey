// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

namespace FArianeEditorHUD
{
    static const uint32 VERTEXRADIUS = 4;
    static const uint32 HANDLERADIUS = 3;

    enum class EMode : uint8
    {
        Object =        1  ,
        Vertex = ( 1 << 0 ),
    };

    struct ARIANEEDITOR_API FDrawingFlags
    {
        public:
            FDrawingFlags& SetPathVertex()        { PathVertex         = true; return *this; };
            FDrawingFlags& SetPathSegment()       { PathSegment        = true; return *this; };
            FDrawingFlags& SetPathSegmentHandle() { PathSegmentHandle  = true; return *this; };
            FDrawingFlags& SetPathVertexHandle()  { PathVertexHandle   = true; return *this; };
            FDrawingFlags& SetPathVertexValence0(){ PathVertexValence0 = true; return *this; };
            FDrawingFlags& SetPathVertexValence1(){ PathVertexValence1 = true; return *this; };
            FDrawingFlags& SetPathVertexValence2(){ PathVertexValence2 = true; return *this; };

        public:
            EMode Mode = EMode::Object;
            bool PathVertex         : 1 = 0;
            bool PathSegment        : 1 = 0;
            bool PathSegmentHandle  : 1 = 0;
            bool PathVertexHandle   : 1 = 0;
            bool PathVertexValence0 : 1 = 0;
            bool PathVertexValence1 : 1 = 0;
            bool PathVertexValence2 : 1 = 0;
    };
};

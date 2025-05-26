// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyCanvasUtils.h"

#include "BatchedElements.h"
#include "CanvasTypes.h"
#include "Math/TransformCalculus2D.h"

class FCanvas;
class FBatchedElementParameters;

namespace Odyssey {
namespace CanvasUtils {

void
DrawTransformedQuad(FCanvas* iCanvas, FBatchedElementParameters* iShader, const FVector2D& iPosition, const FVector2D& iSourceSize, const FOdysseyImageAnchor& iHandlePosition, const FVector2D& iScale, float iRotationInDegrees, const FVector2D& iDestinationTextureSize)
{
    FVector2D destinationSize = iSourceSize * iScale;
    FVector2D destinationPosition = iHandlePosition.GetPosition(iPosition, destinationSize);

    FVector2f topLeft(destinationPosition.X, destinationPosition.Y);
    FVector2f bottomRight(destinationPosition.X + destinationSize.X, destinationPosition.Y + destinationSize.Y);
    FVector2f topRight(bottomRight.X, topLeft.Y);
    FVector2f bottomLeft(topLeft.X, bottomRight.Y);

    //Rotation
    FQuat2D rotation(FMath::DegreesToRadians(iRotationInDegrees));

    FVector2f translation = topLeft + (bottomRight - topLeft) / 2.f;
    FTransform2D transform(FVector2f(-translation.X, -translation.Y));
    transform = transform.Concatenate(FTransform2D(rotation));
    transform = transform.Concatenate(FVector2f(translation.X, translation.Y));

    FVector2f rotatedtopLeft = transform.TransformPoint(topLeft) / FVector2f(iDestinationTextureSize);
    FVector2f rotatedbottomRight = transform.TransformPoint(bottomRight) / FVector2f(iDestinationTextureSize);
    FVector2f rotatedtopRight = transform.TransformPoint(topRight) / FVector2f(iDestinationTextureSize);
    FVector2f rotatedbottomLeft = transform.TransformPoint(bottomLeft) / FVector2f(iDestinationTextureSize);

    topLeft /= FVector2f(iDestinationTextureSize);
    bottomRight /= FVector2f(iDestinationTextureSize);
    topRight /= FVector2f(iDestinationTextureSize);
    bottomLeft /= FVector2f(iDestinationTextureSize);

    FVector2f Tex0(topLeft.X, topLeft.Y);
    FVector2f Tex1(topRight.X, topRight.Y);
    FVector2f Tex2(bottomLeft.X, bottomLeft.Y);
    FVector2f Tex3(bottomRight.X, bottomRight.Y);

    // Vertex positions
    FVector4f Vert0(rotatedtopLeft.X * 2 - 1.0f, (1.0f - rotatedtopLeft.Y) * 2 - 1.0f, 0, 1);
    FVector4f Vert1(rotatedtopRight.X * 2 - 1.0f, (1.0f - rotatedtopRight.Y) * 2 - 1.0f, 0, 1);
    FVector4f Vert2(rotatedbottomLeft.X * 2 - 1.0f, (1.0f - rotatedbottomLeft.Y) * 2 - 1.0f, 0, 1);
    FVector4f Vert3(rotatedbottomRight.X * 2 - 1.0f, (1.0f - rotatedbottomRight.Y) * 2 - 1.0f, 0, 1);

    // Vertex color
    FLinearColor Col(1.f, 1.f, 1.0f);

    FBatchedElements* batchedElements = iCanvas->GetBatchedElements(FCanvas::ET_Triangle, iShader, nullptr, SE_BLEND_Opaque);
    FHitProxyId hitProxyId = iCanvas->GetHitProxyId();
    int32 V0 = batchedElements->AddVertexf(Vert0, Tex0, Col, hitProxyId);
    int32 V1 = batchedElements->AddVertexf(Vert1, Tex1, Col, hitProxyId);
    int32 V2 = batchedElements->AddVertexf(Vert2, Tex2, Col, hitProxyId);
    int32 V3 = batchedElements->AddVertexf(Vert3, Tex3, Col, hitProxyId);

    batchedElements->AddTriangle(V0, V1, V2, iShader, SE_BLEND_Opaque);
    batchedElements->AddTriangle(V1, V2, V3, iShader, SE_BLEND_Opaque);
}

} //CanvasUtils
} //Odyssey

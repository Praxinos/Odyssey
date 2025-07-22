// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "IMediaTextureSample.h"
#include "Media/OdysseyAnimationMediaTextureSampleConverter.h"

class FOdysseyAnimationMediaTextureSampleConverter;
class UTextureRenderTarget2D;

class ODYSSEYANIMATION_API FOdysseyAnimationMediaTextureSample
    : public IMediaTextureSample
    , public TSharedFromThis<FOdysseyAnimationMediaTextureSample>
{
public:
    //Constructor / Destructor
    virtual ~FOdysseyAnimationMediaTextureSample();
    FOdysseyAnimationMediaTextureSample(int iWidth, int iHeight, UTextureRenderTarget2D* iRenderTarget);

public:
    void SetTime(FMediaTimeStamp iTime);
    void SetDuration(FTimespan iDuration);

public:
    // Mandatrory IMediaTextureSample interface overrides

    /**
     * Get the sample's frame buffer.
     *
     * The returned buffer is only valid for the life time of this sample.
     *
     * @return Buffer containing the texels, or nullptr if the sample holds an FTexture.
     * @see GetDim, GetDuration, GetFormat, GetOutputDim, GetStride, GetTexture, GetTime
     */
    virtual const void* GetBuffer();

    /**
     * Get media texture sample converter
     * Allows us to provide directly the texture instead of having to copy it
     *
     * @return texture sample converter
     */
    virtual IMediaTextureSampleConverter* GetMediaTextureSampleConverter() override;

    /**
     * Get the width and height of the sample.
     *
     * The sample may be larger than the output dimensions, because
     * of horizontal or vertical padding required by some formats.
     *
     * @return Buffer dimensions (in texels).
     * @see GetBuffer, GetDuration, GetFormat, GetOutputDim, GetStride, GetTexture, GetTime
     */
    virtual FIntPoint GetDim() const;

    /**
     * Get the amount of time for which the sample is valid.
     *
     * A duration of zero indicates that the sample is valid until the
     * timecode of the next sample in the queue.
     *
     * @return Sample duration.
     * @see GetBuffer, GetDim, GetFormat, GetOutputDim, GetStride, GetTexture, GetTime
     */
    virtual FTimespan GetDuration() const;

    /**
     * Get the texture sample format.
     *
     * @return Sample format.
     * @see GetBuffer, GetDim, GetDuration, GetOutputDim, GetStride, GetTexture, GetTime
     */
    virtual EMediaTextureSampleFormat GetFormat() const;

    /**
     * Get the sample's desired output width and height.
     *
     * The output dimensions may be smaller than the frame buffer dimensions, because
     * of horizontal and/or vertical padding that may be required for some formats.
     *
     * @return Output dimensions (in pixels).
     * @see GetBuffer, GetDim, GetDuration, GetFormat, GetStride, GetTexture, GetTime
     */
    virtual FIntPoint GetOutputDim() const;

    /**
     * Get the horizontal stride (aka. pitch) of the sample's frame buffer.
     *
     * @return The buffer stride (in number of bytes).
     * @see GetBuffer, GetDim, GetDuration, GetFormat, GetOutputDim, GetTexture, GetTime
     */
    virtual uint32 GetStride() const;


#if WITH_ENGINE

    /**
     * Get the sample's texture resource.
     *
     * @return Texture resource, or nullptr if the sample holds a frame buffer.
     * @see GetBuffer, GetDim, GetDuration, GetFormat, GetOutputDim, GetStride, GetTime
     */
    virtual FRHITexture* GetTexture() const;

#endif //WITH_ENGINE

    /**
     * Get the sample time (in the player's local clock).
     *
     * This value is used primarily for debugging purposes.
     *
     * @return Sample time.
     * @see GetBuffer, GetDim, GetDuration, GetFormat, GetOutputDim, GetStride, GetTexture
     */
    virtual FMediaTimeStamp GetTime() const;
    /**
     * ATTENTION: Never actually called since at least 5 years ago
     *
     * Whether the sample can be held in a cache.
     *
     * Non-cacheable video samples become invalid when the next sample is available,
     * and only the latest sample should be kept by video sample consumers.
     *
     * @return true if cacheable, false otherwise.
     */
    virtual bool IsCacheable() const;

    /**
     * Whether the output of the sample is in sRGB color space.
     *
     * @return true if sRGB, false otherwise.
     */
    virtual bool IsOutputSrgb() const;

private:
    FIntPoint mDimensions;
    FMediaTimeStamp mTime;
    FTimespan mDuration;
    UTextureRenderTarget2D* mRenderTarget;
    FOdysseyAnimationMediaTextureSampleConverter mConverter;
};

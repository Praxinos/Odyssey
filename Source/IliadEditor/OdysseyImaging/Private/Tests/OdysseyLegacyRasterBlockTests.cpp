// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterBlock.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "UObject/Package.h"

#include "OdysseyFile.h"

namespace Odyssey::LegacyRasterBlockTests
{
constexpr EAutomationTestFlags TestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
/** Written after the raster block, to check where the reader stops. */
constexpr uint32 Marker = 0xC0FFEE42;

/** Writes a sub-chunk: its id, its length, then its content. */
void WriteChunk(FArchive& Ar, uint32 Id, TFunctionRef<void(FArchive&)> Content)
{
    TArray<uint8> Bytes;
    FMemoryWriter Writer(Bytes, true);
    Content(Writer);
    uint64 Length = Bytes.Num();
    Ar << Id;
    Ar << Length;
    Ar.Serialize(Bytes.GetData(), Bytes.Num());
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyLegacyRasterBlockTest, "Odyssey.Imaging.LegacyRasterBlock.Read", Odyssey::LegacyRasterBlockTests::TestFlags)
/**
 * Raster blocks saved in chunks are read up to their end, unknown chunks included; a block without pixels gives none.
 * Archives that do not load are left as they are.
 *
 * The pixels are bulk data loaded from the package file, which a memory archive cannot hold: that path is not covered here.
 */
bool FOdysseyLegacyRasterBlockTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::LegacyRasterBlockTests;
    UObject& Owner = *GetTransientPackage();

    // A chunked raster block without bulk data, followed by the marker.
    TArray<uint8> Bytes;
    {
        FMemoryWriter Writer(Bytes, true);
        WriteChunk(Writer, FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK, [](FArchive& Ar)
        {
            WriteChunk(Ar, FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_ID, [](FArchive& InAr) { FGuid Id = FGuid::NewGuid(); InAr << Id; });
            WriteChunk(Ar, FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_RESOLUTION, [](FArchive& InAr) { int32 Width = 8; int32 Height = 4; InAr << Width; InAr << Height; });
            WriteChunk(Ar, 0x12345678, [](FArchive& InAr) { uint32 Unknown = 7; InAr << Unknown; });
            WriteChunk(Ar, FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_FORMAT, [](FArchive& InAr) { int32 Format = ::ULIS::Format_BGRA8; InAr << Format; });
        });
        uint32 WrittenMarker = Marker;
        Writer << WrittenMarker;
    }
    {
        FMemoryReader Reader(Bytes, true);
        TestNull(TEXT("A raster block without bulk data gives no pixels"), Odyssey::ReadLegacyRasterBlock(Reader, Owner).Get());
        uint32 ReadMarker = 0;
        Reader << ReadMarker;
        TestEqual(TEXT("The reader stops at the end of the raster block"), ReadMarker, Marker);
    }

    // Archives that do not load are not touched.
    {
        TArray<uint8> Written;
        FMemoryWriter Writer(Written, true);
        TestNull(TEXT("A saving archive gives no pixels"), Odyssey::ReadLegacyRasterBlock(Writer, Owner).Get());
        TestEqual(TEXT("A saving archive is not written"), Written.Num(), 0);

        FMemoryReader Transient(Bytes, false);
        TestNull(TEXT("A non-persistent archive gives no pixels"), Odyssey::ReadLegacyRasterBlock(Transient, Owner).Get());
        TestEqual(TEXT("A non-persistent archive is not read"), Transient.Tell(), int64(0));
    }
    return true;
}

#endif

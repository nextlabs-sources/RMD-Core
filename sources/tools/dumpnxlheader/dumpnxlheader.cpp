/******************************************************************************
 *
 * NextLabs NXL Header Dump Tool
 *
 * Dump the header info of NXL format version 2 file.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "rmccore/format/nxlfmt.h"



enum class FileResult_t {
    Success,
    FileTooSmall,
    OtherError
};

static bool op_printFixedHeader = false;
static bool op_printSectionData = false;
static bool op_nested = false;
static int spacesPerIndentLevel;
static bool op_force = false;
static bool op_verbose = false;
static bool op_help = false;
static const char *fName = NULL;



static void printBanner(void)
{
    printf("\n");
    printf("NextLabs NXL Header Dump Tool\n\n");
    printf("\t\t\t**********************\n");
    printf("\t\t\t* INTERNAL USE ONLY! *\n");
    printf("\t\t\t**********************\n\n");
}

static void printUsage(const char *cmd)
{
    printBanner();
    printf("Usage: %s option... file\n", cmd);
    printf("\n");
    printf("Options:\n");
    printf("    --fixed             Print fixed header\n");
    printf("    --section           Print section data\n");
    printf("    --nested n          Print in nested format, indenting n spaces per level\n");
    printf("    --force             Continue printing even if file size is too small or\n");
    printf("                        header is invalid (Output may be invalid.)\n");
    //printf("    --verbose, -v       Verbose output\n");
    printf("    --help, -h, /?      This screen\n");
    printf("\n");
    printf("Dump the header of an NXL Format v2 file.\n");
    printf("\n");
    printf("Example:\n");
    printf("    %s --fixed foo.txt.nxl\n", cmd);
    printf("    %s --fixed --nested 4 --section foo.txt.nxl\n", cmd);
} /* printUsage */

static bool processOptions(int argc, const char *argv[])
{
    for (int i = 1; i < argc; i++) {
        const char *option = strstr(argv[i], "=");
        if (option != NULL) {
            option++;
        }

        if (_stricmp(argv[i], "--fixed") == 0) {
            op_printFixedHeader = true;
        } else if (_stricmp(argv[i], "--section") == 0) {
            op_printSectionData = true;
        } else if (_stricmp(argv[i], "--nested") == 0) {
            op_nested = true;
            spacesPerIndentLevel = atoi(argv[i + 1]);
            i++;
        } else if (_stricmp(argv[i], "--force") == 0) {
            op_force = true;
        } else if (_stricmp(argv[i], "--verbose") == 0 ||
                   _stricmp(argv[i], "-v") == 0) {
            op_verbose = true;
        } else if (_stricmp(argv[i], "--help") == 0 ||
                   _stricmp(argv[i], "-h") == 0 ||
                   _stricmp(argv[i], "/?") == 0) {
            op_help = true;
        } else if (argv[i][0] == L'-') {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            printUsage(argv[0]);
            return false;
        } else if (fName == NULL) {
            fName = argv[i];
        } else {
            fprintf(stderr, "Too many parameters: %s\n", argv[i]);
            printUsage(argv[0]);
            return false;
        }
    }

    if (op_help) {
        printUsage(argv[0]);
        return false;
    }

    if (fName == NULL) {
        fprintf(stderr, "Missing file.\n");
        printUsage(argv[0]);
        return false;
    }

    return true;
} /* processOptions */

void indent(int level)
{
    for (int i = 0; i < level * spacesPerIndentLevel; i++) {
        putchar(' ');
    }
}

void printByteArray(const unsigned char array[], int len)
{
    const int bytesPerLine = 16;

    for (int i = 0; i < len; i++) {
        if (i % bytesPerLine == 0) {
            printf("\t");
        }

        printf("%02hX", (unsigned short) array[i]);

        if ((i + 1) % bytesPerLine == 0 || i == len - 1) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
} /* printByteArray */

void printByteArrayIndented(const unsigned char array[], int len, int level)
{
    const int bytesPerLine = 16;

    for (int i = 0; i < len; i++) {
        if (i % bytesPerLine == 0) {
            indent(level);
        }

        printf("%02hX", (unsigned short) array[i]);

        if ((i + 1) % bytesPerLine == 0 || i == len - 1) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
} /* printByteArray */

FileResult_t readFixedHeader(NXL_HEADER2 &hdr)
{
    FILE *f;
    FileResult_t ret = FileResult_t::Success;

    if (fopen_s(&f, fName, "rb") != 0) {
        perror(fName);
        return FileResult_t::OtherError;
    }

    memset(&hdr, '\0', sizeof hdr);

    if (fread(&hdr, sizeof hdr, 1, f) != 1) {
        if (ferror(f) != 0) {
            perror(fName);
            ret = FileResult_t::OtherError;
        } else {
            fprintf(stderr, "Error: file size too small\n");
            ret = FileResult_t::FileTooSmall;
        }
    }

    fclose(f);
    return ret;
}

const char * const algorithmStr[] = {
    "NXL_ALGORITHM_NONE",
    "NXL_ALGORITHM_AES128",
    "NXL_ALGORITHM_AES256",
    "NXL_ALGORITHM_RSA1024",
    "NXL_ALGORITHM_RSA2048",
    "NXL_ALGORITHM_SHA1",
    "NXL_ALGORITHM_SHA256",
    "NXL_ALGORITHM_MD5"
};

const char * const secureModeStr[] = {
    NULL,
    "KF_CLIENT_SECURE_MODE",
    "KF_SERVER_SECURE_MODE",
    "KF_SPLIT_SECURE_MODE"
};

void printFixedHeader(const NXL_HEADER2 &hdr)
{
    //
    // NXL_HEADER2.fixed
    //

    // NXL_HEADER2.fixed.signature
    printf("NXL_HEADER2.fixed.signature.magic.code = 0x%016llX\n",
           hdr.fixed.signature.magic.code);
    printf("NXL_HEADER2.fixed.signature.magic.name = \"%s\"\n",
           hdr.fixed.signature.magic.name);
    printf("NXL_HEADER2.fixed.signature.version = %hu.%hu\n",
           (unsigned short) (hdr.fixed.signature.version >> 16),
           (unsigned short) (hdr.fixed.signature.version & 0xFFFF));
    printf("NXL_HEADER2.fixed.signature.message = \"%s\"\n",
           hdr.fixed.signature.message);

    // NXL_HEADER2.fixed.fileInfo
    printf("NXL_HEADER2.fixed.fileInfo.duid = {\n");
    printByteArray(hdr.fixed.fileInfo.duid, _countof(hdr.fixed.fileInfo.duid));
    printf("}\n");
    printf("NXL_HEADER2.fixed.fileInfo.flags = 0x%08X\n",
           hdr.fixed.fileInfo.flags);
    printf("NXL_HEADER2.fixed.fileInfo.alignment = %u\n",
           hdr.fixed.fileInfo.alignment);
    printf("NXL_HEADER2.fixed.fileInfo.algorithm = %s\n",
           algorithmStr[hdr.fixed.fileInfo.algorithm]);
    printf("NXL_HEADER2.fixed.fileInfo.blockSize = %u\n",
           hdr.fixed.fileInfo.blockSize);
    printf("NXL_HEADER2.fixed.fileInfo.contentOffset = %u\n",
           hdr.fixed.fileInfo.contentOffset);
    printf("NXL_HEADER2.fixed.fileInfo.ownerId = \"%s\"\n",
           hdr.fixed.fileInfo.ownerId);
    printf("NXL_HEADER2.fixed.fileInfo.extendedDataOffset = %u\n",
           hdr.fixed.fileInfo.extendedDataOffset);

    // NXL_HEADER2.fixed.keys
    printf("NXL_HEADER2.fixed.keys.modeFlags = %s, %s\n",
           secureModeStr[hdr.fixed.keys.modeFlags >> 24],
           (hdr.fixed.keys.modeFlags & 0x00FFFFFF) & KF_RECOVERY_KEY_ENABLED ?
           "KF_RECOVERY_KEY_ENABLED" : "");
    printf("NXL_HEADER2.fixed.keys.ivSeed = {\n");
    printByteArray(hdr.fixed.keys.ivSeed, _countof(hdr.fixed.keys.ivSeed));
    printf("}\n");
    printf("NXL_HEADER2.fixed.keys.tokenCek = {\n");
    printByteArray(hdr.fixed.keys.tokenCek, _countof(hdr.fixed.keys.tokenCek));
    printf("}\n");
    printf("NXL_HEADER2.fixed.keys.tokenCekChecksum = {\n");
    printByteArray(hdr.fixed.keys.tokenCekChecksum, _countof(hdr.fixed.keys.tokenCekChecksum));
    printf("}\n");
    printf("NXL_HEADER2.fixed.keys.recoveryCek = {\n");
    printByteArray(hdr.fixed.keys.recoveryCek, _countof(hdr.fixed.keys.recoveryCek));
    printf("}\n");
    printf("NXL_HEADER2.fixed.keys.recoveryCekChecksum = {\n");
    printByteArray(hdr.fixed.keys.recoveryCekChecksum, _countof(hdr.fixed.keys.recoveryCekChecksum));
    printf("}\n");
    printf("NXL_HEADER2.fixed.keys.publicKey1 = {\n");
    printByteArray(hdr.fixed.keys.publicKey1, _countof(hdr.fixed.keys.publicKey1));
    printf("}\n");
    printf("NXL_HEADER2.fixed.keys.publicKey2 = {\n");
    printByteArray(hdr.fixed.keys.publicKey2, _countof(hdr.fixed.keys.publicKey2));
    printf("}\n");
    printf("NXL_HEADER2.fixed.keys.tokenLevel = %u\n",
           hdr.fixed.keys.tokenLevel);
    printf("NXL_HEADER2.fixed.keys.extendedDataOffset = %u\n",
           hdr.fixed.keys.extendedDataOffset);

    // NXL_HEADER2.fixed.sections
    printf("NXL_HEADER2.fixed.sections.section_map = 0x%08X\n",
           hdr.fixed.sections.section_map);
    for (int i = 0; i < _countof(hdr.fixed.sections.record); i++) {
        if (hdr.fixed.sections.section_map & (1 << i)) {
            printf("NXL_HEADER2.fixed.sections.record[%d].name = \"%s\"\n",
                   i, hdr.fixed.sections.record[i].name);
            printf("NXL_HEADER2.fixed.sections.record[%d].flags = %s, %s\n",
                   i, 
                   hdr.fixed.sections.record[i].flags & NXL_SECTION_FLAG_ENCRYPTED ?
                   "NXL_SECTION_FLAG_ENCRYPTED" : "",
                   hdr.fixed.sections.record[i].flags & NXL_SECTION_FLAG_COMPRESSED ?
                   "NXL_SECTION_FLAG_COMPRESSED" : "");
            printf("NXL_HEADER2.fixed.sections.record[%d].offset = %u\n",
                   i, hdr.fixed.sections.record[i].offset);
            printf("NXL_HEADER2.fixed.sections.record[%d].size = %u\n",
                   i, hdr.fixed.sections.record[i].size);
            printf("NXL_HEADER2.fixed.sections.record[%d].dataSize = %hu\n",
                   i, hdr.fixed.sections.record[i].dataSize);
            printf("NXL_HEADER2.fixed.sections.record[%d].compressedSize = %hu\n",
                   i, hdr.fixed.sections.record[i].compressedSize);
            printf("NXL_HEADER2.fixed.sections.record[%d].checksum = {\n", i);
            printByteArray(hdr.fixed.sections.record[i].checksum,
                           _countof(hdr.fixed.sections.record[i].checksum));
            printf("}\n");
        }
    }

    // NXL_HEADER2.fixed.extendData
    printf("NXL_HEADER2.fixed.extendData = {\n");
    printByteArray(hdr.fixed.extendData, _countof(hdr.fixed.extendData));
    printf("}\n");



    //
    // NXL_HEADER2.dynamic
    //

    // NXL_HEADER2.dynamic.fixedHeaderHash
    printf("NXL_HEADER2.dynamic.fixedHeaderHash = {\n");
    printByteArray(hdr.dynamic.fixedHeaderHash,
                   _countof(hdr.dynamic.fixedHeaderHash));
    printf("}\n");

    // NXL_HEADER2.dynamic.contentLength
    printf("NXL_HEADER2.dynamic.contentLength = %lld\n",
           hdr.dynamic.contentLength);
}

void print_NXL_MAGIC(const NXL_MAGIC &magic, int level)
{
    indent(level); printf("code = 0x%016llX\n", magic.code);

    indent(level); printf("name = \"%s\"\n", magic.name);
}

void print_NXL_SIGNATURE2(const NXL_SIGNATURE2 &signature, int level)
{
    indent(level); printf("magic = {\n");
    print_NXL_MAGIC(signature.magic, level + 1);
    indent(level); printf("}\n");

    indent(level);
    printf("version = %hu.%hu\n",
           (unsigned short) (signature.version >> 16),
           (unsigned short) (signature.version & 0xFFFF));

    indent(level); printf("message = \"%s\"\n", signature.message);
}

void print_NXL_FILEINFO2(const NXL_FILEINFO2 &fileInfo, int level)
{
    indent(level); printf("duid = {\n");
    printByteArrayIndented(fileInfo.duid, _countof(fileInfo.duid), level + 1);
    indent(level); printf("}\n");

    indent(level); printf("flags = 0x%08X\n", fileInfo.flags);

    indent(level); printf("alignment = %u\n", fileInfo.alignment);

    indent(level); printf("algorithm = %s\n", algorithmStr[fileInfo.algorithm]);

    indent(level); printf("blockSize = %u\n", fileInfo.blockSize);

    indent(level); printf("contentOffset = %u\n", fileInfo.contentOffset);

    indent(level); printf("ownerId = \"%s\"\n", fileInfo.ownerId);

    indent(level); printf("extendedDataOffset = %u\n",
                          fileInfo.extendedDataOffset);
}

void print_NXL_KEYS(const NXL_KEYS &keys, int level)
{
    indent(level); printf("modeFlags = %s, %s\n",
                          secureModeStr[keys.modeFlags >> 24],
                          (keys.modeFlags & 0x00FFFFFF) & KF_RECOVERY_KEY_ENABLED ?
                          "KF_RECOVERY_KEY_ENABLED" : "");

    indent(level); printf("ivSeed = {\n");
    printByteArrayIndented(keys.ivSeed, _countof(keys.ivSeed), level + 1);
    indent(level); printf("}\n");

    indent(level); printf("tokenCek = {\n");
    printByteArrayIndented(keys.tokenCek, _countof(keys.tokenCek), level + 1);
    indent(level); printf("}\n");

    indent(level); printf("tokenCekChecksum = {\n");
    printByteArrayIndented(keys.tokenCekChecksum, _countof(keys.tokenCekChecksum), level + 1);
    indent(level); printf("}\n");

    indent(level); printf("recoveryCek = {\n");
    printByteArrayIndented(keys.recoveryCek, _countof(keys.recoveryCek), level + 1);
    indent(level); printf("}\n");

    indent(level); printf("recoveryCekChecksum = {\n");
    printByteArrayIndented(keys.recoveryCekChecksum, _countof(keys.recoveryCekChecksum), level + 1);
    indent(level); printf("}\n");

    indent(level); printf("publicKey1 = {\n");
    printByteArrayIndented(keys.publicKey1, _countof(keys.publicKey1), level + 1);
    indent(level); printf("}\n");

    indent(level); printf("publicKey2 = {\n");
    printByteArrayIndented(keys.publicKey2, _countof(keys.publicKey2), level + 1);
    indent(level); printf("}\n");

    indent(level); printf("tokenLevel = %u\n", keys.tokenLevel);

    indent(level); printf("extendedDataOffset = %u\n", keys.extendedDataOffset);
}

void print_NXL_SECTION2(const NXL_SECTION2 &record, int level)
{
    indent(level); printf("name = \"%s\"\n", record.name);

    indent(level); printf("flags = %s, %s\n",
                          record.flags & NXL_SECTION_FLAG_ENCRYPTED ?
                          "NXL_SECTION_FLAG_ENCRYPTED" : "",
                          record.flags & NXL_SECTION_FLAG_COMPRESSED ?
                          "NXL_SECTION_FLAG_COMPRESSED" : "");

    indent(level); printf("offset = %u\n", record.offset);

    indent(level); printf("size = %u\n", record.size);

    indent(level); printf("dataSize = %hu\n", record.dataSize);

    indent(level); printf("compressedSize = %hu\n", record.compressedSize);

    indent(level); printf("checksum = {\n");
    printByteArrayIndented(record.checksum, _countof(record.checksum), level + 1);
    indent(level); printf("}\n");
}

void print_NXL_SECTION_HEADER2(const NXL_SECTION_HEADER2 &sections, int level)
{
    indent(level); printf("section_map = 0x%08X\n", sections.section_map);

    for (int i = 0; i < _countof(sections.record); i++) {
        if (sections.section_map & (1 << i)) {
            indent(level); printf("record[%d] = {\n", i);
            print_NXL_SECTION2(sections.record[i], level + 1);
            indent(level); printf("}\n");
        }
    }
}

void print_NXL_FIXED_HEADER(const NXL_FIXED_HEADER &fixed, int level)
{
    indent(level); printf("signature = {\n");
    print_NXL_SIGNATURE2(fixed.signature, level + 1);
    indent(level); printf("}\n");

    indent(level); printf("fileInfo = {\n");
    print_NXL_FILEINFO2(fixed.fileInfo, level + 1);
    indent(level); printf("}\n");

    indent(level); printf("keys = {\n");
    print_NXL_KEYS(fixed.keys, level + 1);
    indent(level); printf("}\n");

    indent(level); printf("sections = {\n");
    print_NXL_SECTION_HEADER2(fixed.sections, level + 1);
    indent(level); printf("}\n");

    indent(level); printf("extendData = {\n");
    printByteArrayIndented(fixed.extendData, _countof(fixed.extendData),
                           level+ 1);
    indent(level); printf("}\n");
}

void print_NXL_DYNAMIC_HEADER(const NXL_DYNAMIC_HEADER &dynamic, int level)
{
    indent(level); printf("fixedHeaderHash = {\n");
    printByteArrayIndented(dynamic.fixedHeaderHash,
                           _countof(dynamic.fixedHeaderHash), level + 1);
    indent(level); printf("}\n");

    indent(level); printf("contentLength = %lld\n", dynamic.contentLength);
}

void print_NXL_HEADER2(const NXL_HEADER2 &hdr, int level)
{
    indent(level); printf("fixed = {\n");
    print_NXL_FIXED_HEADER(hdr.fixed, level + 1);
    indent(level); printf("}\n");

    indent(level); printf("dynamic = {\n");
    print_NXL_DYNAMIC_HEADER(hdr.dynamic, level + 1);
    indent(level); printf("}\n");
}

void printFixedHeaderIndented(const NXL_HEADER2 &hdr)
{
    printf("NXL_HEADER2 = {\n");
    print_NXL_HEADER2(hdr, 1);
    printf("}\n");
}

FileResult_t readSectionData(const NXL_SECTION2 &section,
                             std::vector<unsigned char> &buf)
{
    FILE *f;
    FileResult_t ret = FileResult_t::Success;

    if (fopen_s(&f, fName, "rb") != 0) {
        perror(fName);
        return FileResult_t::OtherError;
    }

    // fseek() returning success DOES NOT imply that the file is at least as
    // big as the seeked position.
    if (fseek(f, section.offset, SEEK_SET) != 0) {
        perror(fName);
        ret = FileResult_t::OtherError;
    } else {
        size_t bufSize = (section.flags & NXL_SECTION_FLAG_COMPRESSED ?
                          section.compressedSize : section.dataSize);
        buf.resize(bufSize);

        size_t bytesRead = fread(buf.data(), 1, bufSize, f);
        if (bytesRead != bufSize) {
            if (ferror(f) != 0) {
                perror(fName);
                ret = FileResult_t::OtherError;
            } else {
                fprintf(stderr, "Error: file size too small\n");
                buf.resize(bytesRead);
                ret = FileResult_t::FileTooSmall;
            }
        }
    }

    fclose(f);
    return ret;
}

void printSectionData(int sectionIndex, const NXL_SECTION2 &section,
                      const std::vector<unsigned char> &buf)
{
    printf("Section %d: %s\n", sectionIndex, section.name);
    for (const char &c : buf) {
        putchar(c);
    }
    printf("\n");
}

int main(int argc, const char *argv[])
{
    if (!processOptions(argc, argv)) {
        goto exit;
    }

    if (op_verbose) {
        printBanner();
    }

    FileResult_t res;

    NXL_HEADER2 hdr;
    res = readFixedHeader(hdr);

    if (hdr.fixed.signature.magic.code != NXL_MAGIC_CODE_2 ||
        hdr.fixed.signature.version >> 16 != NXL_VERSION_30 >> 16) {
        fprintf(stderr, "Error: file not in valid NXL Format v2\n");

        if (!op_force) {
            goto exit;
        }
    }

    if (op_printFixedHeader &&
        (res == FileResult_t::Success ||
         (res == FileResult_t::FileTooSmall && op_force))) {
        if (op_nested) {
            printFixedHeaderIndented(hdr);
        } else {
            printFixedHeader(hdr);
        }
    }

    printf("\n");

    if (op_printSectionData) {
        for (int i = 0; i < _countof(hdr.fixed.sections.record); i++) {
            if (hdr.fixed.sections.section_map & (1 << i)) {
                std::vector<unsigned char> buf;
                res = readSectionData(hdr.fixed.sections.record[i], buf);

                if (res == FileResult_t::Success ||
                    (res == FileResult_t::FileTooSmall && op_force)) {
                    printSectionData(i, hdr.fixed.sections.record[i], buf);
                    printf("\n");
                }
            }
        }
    }

    if (op_verbose) {
        printf("Done.\n");
    }

exit:
    return 0;
}

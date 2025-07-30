#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "CReedSolomon.h"
#include "DES.h"

void split_source_packet(const char* input_file, const char* filename);
void make_transport_file(const char* spfilename, const char* tpfilename);
void tptojpeg(const char* tpfilename, const char* jpegfilename, unsigned long long key);
void tptoGIF(const char* tpfilename, const char* giffilename, unsigned long long key);

int main(void)
{
    FILE* fpCADU;
    FILE* fpPN;
    FILE* fpIMG;
    FILE* fpADD;

    unsigned char data[1024];
    unsigned char pn[1020];
    int i;
    int error;
    int total_error = 0;

    fpCADU = fopen("C:\\kari_final_project\\final_project\\LRIT_CADU.dat", "rb");
    fpPN = fopen("C:\\kari_final_project\\final_project\\pnCode1020.dat", "rb");
    fpIMG = fopen("C:\\kari_final_project\\final_project\\LRIT_CADU_IMG.dat", "wb");
    fpADD = fopen("C:\\kari_final_project\\final_project\\LRIT_CADU_ADD.dat", "wb");
    fread(pn, 1, 1020, fpPN);

    while (fread(data, 1, 1024, fpCADU))
    {
        // De-Randomizing
        for (i = 0; i < 1020; i++)
            data[i + 4] ^= pn[i];

        // RS Decoding
        CReedSolomon rs;
        rs.init(8, 16, 112, 11, 0, 4, 0, 1);
        rs.Decode(data + 4);
        error = rs.CorrectableErrorsInFrame();
        total_error += error;

        if (data[5] == 0xc0)
            fwrite(data, 1, 1024, fpIMG);
        else if (data[5] == 0xc5)
            fwrite(data, 1, 1024, fpADD);
    }
    fclose(fpCADU);
    fclose(fpPN);
    fclose(fpIMG);
    fclose(fpADD);

    printf("¿¡·¯ °³¼ö: %d\n", total_error);

    //img file process
    split_source_packet(
        "C:\\kari_final_project\\final_project\\LRIT_CADU_IMG.dat",
        "C:\\kari_final_project\\final_project\\packet\\img\\img_sp"
    );

    make_transport_file(
        "C:\\kari_final_project\\final_project\\packet\\img\\img_sp",
        "C:\\kari_final_project\\final_project\\transport\\img\\img_tp"
    );

    tptojpeg(
        "C:\\kari_final_project\\final_project\\transport\\img\\img_tp",
        "C:\\kari_final_project\\final_project\\LRIT\\img\\LRIT_",
        0x3B0779919DC237A4
        );

    //add file process
    split_source_packet(
        "C:\\kari_final_project\\final_project\\LRIT_CADU_ADD.dat",
        "C:\\kari_final_project\\final_project\\packet\\add\\add_sp"
    );

    make_transport_file(
        "C:\\kari_final_project\\final_project\\packet\\add\\add_sp",
        "C:\\kari_final_project\\final_project\\transport\\add\\add_tp"
    );

    tptoGIF(
        "C:\\kari_final_project\\final_project\\transport\\add\\add_tp",
        "C:\\kari_final_project\\final_project\\LRIT\\add\\add_",
        0x3B0779919DC237A4
    );
    return 0;
}

void split_source_packet(const char* input_file, const char* filename)
{
    FILE* fpIN = fopen(input_file, "rb");
    unsigned char data[1024];
    FILE* fpSP = NULL;
    int j = 0;
    char fname[256];

    while (fread(data, 1, 1024, fpIN))
    {
        int pos = data[10] * 256 + data[11];

        if (pos == 0)
        {
            sprintf(fname, "%s_%d.dat", filename, j);
            fpSP = fopen(fname, "wb");
            fwrite(data + 12, 1, 884, fpSP);
        }
        else if (pos == 2047)
        {
            fwrite(data + 12, 1, 884, fpSP);
        }
        else
        {
            fwrite(data + 12, 1, pos, fpSP);
            j++;
            fclose(fpSP);
            if (data[12 + pos] * 256 + data[pos + 13] != 0 && data[13 + pos] * 256 + data[pos + 14])
            {
                sprintf(fname, "%s_%d.dat", filename, j);
                fpSP = fopen(fname, "wb");
                fwrite(data + 12 + pos, 1, 884 - pos, fpSP);
            }
        }
    }
    fclose(fpIN);
    if (fpSP) fclose(fpSP);
}

void make_transport_file(const char* spfilename, const char* tpfilename)
{
    unsigned char buf[9000];
    FILE* fpSP = NULL;
    FILE* fpTP = NULL;
    char fname[256];
    char tpname[256];
    int j = 0;
    int t = 0;
    size_t len;

    while (1)
    {
        sprintf(fname, "%s_%d.dat", spfilename, j);
        fpSP = fopen(fname, "rb");
        if (!fpSP) break;

        len = fread(buf, 1, 9000, fpSP);
        fclose(fpSP);

        if (buf[2] == 64)
        {
            sprintf(tpname, "%s_%d.dat", tpfilename, t);
            fpTP = fopen(tpname, "wb");
            fwrite(buf + 6, 1, 8190, fpTP);
        }
        else if (buf[2] == 0)
        {
            fwrite(buf + 6, 1, 8190, fpTP);
        }
        else if (buf[2] == 128)
        {
            fwrite(buf + 6, 1, len - 8, fpTP);
            fclose(fpTP);
            fpTP = NULL;
            t++;
        }
        else
        {
            sprintf(tpname, "%s_%d.dat", tpfilename, t);
            fpTP = fopen(tpname, "wb");
            fwrite(buf + 6, 1, 8190, fpTP);
            fclose(fpTP);
            fpTP = NULL;
            t++;
        }
        j++;
    }
    if (fpTP) fclose(fpTP);
}

void tptojpeg(const char* tpfilename, const char* jpegfilename, unsigned long long key)
{
    int size = 0;
    int j = 0;
    
    unsigned char data[140000];
    char fname1[256];
    char fname2[256];
    FILE* fpTP = NULL;
    FILE* fpLRIT = NULL;

    while (1)
    {
        sprintf(fname1, "%s_%d.dat", tpfilename, j);
        fpTP = fopen(fname1, "rb");
        if (!fpTP) break;
        size = fread(data, 1, sizeof(data), fpTP);
        fclose(fpTP);

        int offset = 10;
        unsigned short primary_header_record_length = (data[11] << 8) | data[12];
        offset += primary_header_record_length;
        
        unsigned short type1_header_record_length = (data[offset+1]<<8) | data[offset+2] ;
        offset += type1_header_record_length;

        unsigned short type2_header_record_length = (data[offset + 1] << 8) | data[offset + 2];
        offset += type2_header_record_length;

        unsigned short type3_header_record_length = (data[offset + 1] << 8) | data[offset + 2];
        offset += type3_header_record_length;

        unsigned short type4_header_record_length = (data[offset + 1] << 8) | data[offset + 2];
        offset += type4_header_record_length;

        unsigned short type5_header_record_length = (data[offset + 1] << 8) | data[offset + 2];
        offset += type5_header_record_length;

        unsigned short type7_header_record_length = (data[offset + 1] << 8) | data[offset+ 2];
        int key_number_offset = offset + 3;
        unsigned short key_number =
            (data[key_number_offset + 0] << 24) |
            (data[key_number_offset + 1] << 16) |
            (data[key_number_offset + 2] << 8) |
            (data[key_number_offset + 3]);
        printf("IMG DATA KEY HEADER ÀÎµ¦½º(key_number): %x\n", key_number);
        offset += type7_header_record_length;
        

        

        unsigned short type128_header_record_length = (data[offset + 1] << 8) | data[offset + 2];
        offset += type128_header_record_length;

        if (size <= offset) break;
        if (key_number != 0)
        {
            DES des;
            des.setKey(key);
            des.decrypt(data + offset, size - offset);

            sprintf(fname2, "%s%d.jpeg", jpegfilename, j);
            fpLRIT = fopen(fname2, "wb");
            fwrite(data + offset, 1, size - offset, fpLRIT);
            fclose(fpLRIT);

            j++;
        }

        else if (key_number == 0)
        {
            sprintf(fname2, "%s%d.jpeg", jpegfilename, j);
            fpLRIT = fopen(fname2, "wb");
            fwrite(data + offset, 1, size - offset, fpLRIT);
            fclose(fpLRIT);

            j++;
        }
    }
}


void tptoGIF(const char* tpfilename, const char* giffilename, unsigned long long key)
{
    unsigned char data[140000];
    size_t size;
    int offset = 83;
    int j = 0;
    char fname1[256];
    char fname2[256];

    while (1)
    {
        sprintf(fname1, "%s_%d.dat", tpfilename, j);
        FILE* fpTP = fopen(fname1, "rb");
        if (!fpTP) break;

        size = fread(data, 1, sizeof(data), fpTP);
        fclose(fpTP);
        int offset = 10;
        unsigned short primary_header_record_length = (data[11] << 8) | data[12];
        offset += primary_header_record_length;

        unsigned short type4_header_record_length = (data[offset + 1] << 8) | data[offset + 2];
        offset += type4_header_record_length;

        unsigned short type5_header_record_length = (data[offset + 1] << 8) | data[offset + 2];
        offset += type5_header_record_length;

        unsigned short type7_header_record_length = (data[offset + 1] << 8) | data[offset + 2];
        int key_number_offset = offset + 3;
        unsigned int key_number =
            (data[key_number_offset + 0] << 24) |
            (data[key_number_offset + 1] << 16) |
            (data[key_number_offset + 2] << 8) |
            (data[key_number_offset + 3]);
        printf("ADD DATA KEY HEADER ÀÎµ¦½º(key_number): %x\n", key_number);
        offset += type7_header_record_length;

        if (size <= offset) break;
        if (key_number != 0)
        {
            DES des;
            des.setKey(key);
            des.decrypt(data + offset, size - offset);

            sprintf(fname2, "%s%d.GIF", giffilename, j);
            FILE* fpLRIT = fopen(fname2, "wb");
            fwrite(data + offset, 1, size - offset, fpLRIT);
            fclose(fpLRIT);

            j++;
        }

        else if (key_number == 0)
        {
            sprintf(fname2, "%s%d.GIF", giffilename, j);
            FILE* fpLRIT = fopen(fname2, "wb");
            fwrite(data + offset, 1, size - offset, fpLRIT);
            fclose(fpLRIT);

            j++;
        }
    }
}
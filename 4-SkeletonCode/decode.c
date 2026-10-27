#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];

        if (argv[3] == NULL)
        {
            strcpy(decInfo->secret_fname, "decode.txt");
        }
        else
        {
            strcpy(decInfo->secret_fname, argv[3]);
        }

        return e_success;
    }

    return e_failure;
}

Status open_files_for_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    decInfo->fptr_secret = fopen(decInfo->secret_fname, "wb");

    if (decInfo->fptr_secret == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_files_for_decode(decInfo) == e_success)
    {
        printf("Open files for decode is a success\n");
    }
    else
    {
        printf("Open files for decode is a failure\n");
        return e_failure;
    }

    if (decode_magic_string(decInfo) == e_success)
    {
        printf("Magic string decoding is a success\n");
    }
    else
    {
        printf("Magic string decoding is a failure\n");
        return e_failure;
    }

    if (decode_secret_file_extn_size(decInfo) == e_success)
    {
        printf("Extension size decoding is a success\n");
    }
    else
    {
        printf("Extension size decoding is a failure\n");
        return e_failure;
    }

    if (decode_secret_file_extn(decInfo) == e_success)
    {
        printf("Extension decoding is a success\n");
    }
    else
    {
        printf("Extension decoding is a failure\n");
        return e_failure;
    }

    if (decode_secret_file_size(decInfo) == e_success)
    {
        printf("Secret file size decoding is a success\n");
    }
    else
    {
        printf("Secret file size decoding is a failure\n");
        return e_failure;
    }

    if (decode_secret_file_data(decInfo) == e_success)
    {
        printf("Secret file data decoding is a success\n");
    }
    else
    {
        printf("Secret file data decoding is a failure\n");
        return e_failure;
    }

    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char data[3];

    for (int i = 0; i < 2; i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_stego_image);

        decode_byte_from_lsb(image_buffer, &data[i]);
    }

    data[2] = '\0';

    if (strcmp(data, MAGIC_STRING) == 0)
    {
        return e_success;
    }

    return e_failure;
}

Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    char ch = 0;

    for (int i = 0; i < 8; i++)
    {
        ch = ch | ((image_buffer[i] & 1) << i);
    }

    *data = ch;

    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char image_buffer[32];

    fread(image_buffer, 32, 1, decInfo->fptr_stego_image);

    decode_size_from_lsb(image_buffer, &decInfo->extn_size);

    return e_success;
}

Status decode_size_from_lsb(char *image_buffer, int *data)
{
    int value = 0;

    for (int i = 0; i < 32; i++)
    {
        value = value | ((image_buffer[i] & 1) << i);
    }

    *data = value;

    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char image_buffer[8];

    for (int i = 0; i < decInfo->extn_size; i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_stego_image);

        decode_byte_from_lsb(image_buffer,&decInfo->extn_secret_file[i]);
    }

    decInfo->extn_secret_file[decInfo->extn_size] = '\0';

    

    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[32];

    fread(image_buffer, 32, 1, decInfo->fptr_stego_image);

    decode_size_from_lsb(image_buffer,&decInfo->size_secret_file);

   

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char data;

    printf("Decoded secret data: ");

    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_stego_image);

        decode_byte_from_lsb(image_buffer, &data);

        fwrite(&data, 1, 1, decInfo->fptr_secret);

        printf("%c", data);
    }

    printf("\n");

    return e_success;
}



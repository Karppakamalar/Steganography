#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_FILE_SUFFIX 5

typedef struct _DecodeInfo
{

    char *stego_image_fname;
    FILE *fptr_stego_image;

    char secret_fname[100];
    FILE *fptr_secret;

    char extn_secret_file[MAX_FILE_SUFFIX];
    int size_secret_file;
    int extn_size;

} DecodeInfo;


// Func prototypes //

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status open_files_for_decode(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

Status decode_byte_from_lsb(char *image_buffer, char *data);

Status decode_size_from_lsb(char *image_buffer, int *data);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
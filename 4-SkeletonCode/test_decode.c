#include <stdio.h>
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    DecodeInfo decInfo;

    if (argc < 3)
    {
        printf("Usage: %s -d <stego.bmp> [output.txt]\n", argv[0]);
        return 1;
    }

    if (read_and_validate_decode_args(argv, &decInfo) == e_success)
    {
        printf("Read and validate decode args is a success\n");
    }
    else
    {
        printf("Read and validate decode args is a failure\n");
        return 1;
    }

    if (do_decoding(&decInfo) == e_success)
    {
        printf("Decoding is a success\n");
    }
    else
    {
        printf("Decoding is a failure\n");
    }

    return 0;
}
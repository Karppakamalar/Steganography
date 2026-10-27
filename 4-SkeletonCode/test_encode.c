#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>

int main(int argc, char *argv[])
{
    EncodeInfo E1;

    // Validate cmd args
    if(argc <= 3)
    {
        printf("lsb_steg: Encoding: lsb_steg -e <.bmp file> <.txt file> [output file]\n");
        return 0;
    }

    int ret = check_operation_type(argv);

    if(ret == e_encode)
    {
        printf("Selected encoding\n");

        if(read_and_validate_encode_args(argv, &E1) == e_success)
        {
            printf("read_and_validate_encode_args is a success\n");

            if(do_encoding(&E1) == e_success){
                printf("encoding is a success\n");
            }
            else{
            printf("encoding is a failure\n");
            }
        }
        else
        {
            printf("read_and_validate_encode_args is a failure\n");
        }
    }
    else if(ret == e_decode)
    {
        printf("Selected decoding\n");

    }
    else
    {
        printf("Invalid option\n");
    }

    return 0;
}
OperationType check_operation_type(char *argv[])
{
    // check argv[1] is -e or -d

    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

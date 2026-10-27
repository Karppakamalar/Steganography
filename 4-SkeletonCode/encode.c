#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
Status do_encoding(EncodeInfo *encInfo){

    if(open_files(encInfo) == e_success)
    {
        printf("Open files is a success\n");
    }
    else
    {
        printf("Open files is a failure\n");
        return e_failure;
    }
    if(check_capacity(encInfo)==e_success){
        printf("Check capacity is a success\n");
    }
    else{
        printf("Check capacity is a failure\n");
        return e_failure;
    }
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_success){
        printf("Copy bmp header is a success\n");
    }
    else{
        printf("Copy bmp header is a failure\n");
        return e_failure;
    }
    if(encode_magic_string("#*",encInfo)==e_success){
        printf("encoding magic string is a success\n");
    }
    else{
        printf("encoding magic string is a failure\n");
        return e_failure;
    }
    if(encode_secret_file_extn_size(4, encInfo) == e_success)
    {
        printf("Encoding secret file extension size is a success\n");
    }
    else
    {
        printf("Encoding secret file extension size is a failure\n");
        return e_failure;
    }
    if(encode_secret_file_extn(".txt", encInfo) == e_success){
        printf("Encoding secret file extension is a success\n");   
    }
    else{
        printf("Encoding secret file extension is a failure\n");
        return e_failure;
    }
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success){
        printf("Encoding secret file size is a success\n");
    }
    else{
        printf("Encoding secret file size is a failure\n");
        return e_failure;
    }
    char str[encInfo-> size_secret_file];
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    fread(str,encInfo->size_secret_file,1,encInfo->fptr_secret);
    str[encInfo->size_secret_file] = '\0';
    printf("%s\n",str);

    if(encode_secret_file_data(str,encInfo)==e_success){
        printf("Encoding secret file data is a success\n");
    }
    else{
        printf("Encoding secret file data is a failure\n");
    }
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success){
        printf("copying of remaing data is a success\n");
    }
    else{
        printf("copying of remaing data is a failure\n");
    }

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){
    char ch;
    while(fread(&ch,1,1,fptr_src)==1){
        fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}

Status encode_secret_file_data(char *str,EncodeInfo *encInfo){
    char image_buffer[8];
    for(int i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(image_buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(str[i], image_buffer);

        fwrite(image_buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;

}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char image_buffer[8];

    for(int i = 0; i < 4; i++)
    {
        fread(image_buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(file_extn[i], image_buffer);

        fwrite(image_buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    fread(image_buffer, 32, 1, encInfo->fptr_src_image);

    encode_size_to_lsb(file_size, image_buffer);

    fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_extn_size(int extn_size,EncodeInfo *encInfo){

    char image_buffer[32];

    fread(image_buffer, 32, 1, encInfo->fptr_src_image);

    encode_size_to_lsb(extn_size, image_buffer);

    fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);

    return e_success;

}

Status encode_size_to_lsb(int data,char *image_buffer){

    for(int i = 0; i < 32; i++)
    {
        image_buffer[i] =(image_buffer[i] & 0xFE) | ((data >> i) & 1);
    }

    return e_success;

}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo){
    char image_buffer[8];

    for(int i = 0; i < 2; i++)
    {
        fread(image_buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(magic_string[i], image_buffer);

        fwrite(image_buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;

}

Status encode_byte_to_lsb(char data, char *image_buffer){

    for(int i=0;i<8;i++){
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 1);
    }
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54];

    fseek(fptr_src_image, 0, SEEK_SET);

    fread(arr, 54, 1, fptr_src_image);

    fwrite(arr, 54, 1, fptr_dest_image);

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity =get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file =get_file_size(encInfo->fptr_secret);

    if(encInfo->image_capacity >
       (16 + 32 + 32 + 32 +
       (encInfo->size_secret_file * 8)))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);

    uint size = ftell(fptr);

    fseek(fptr, 0, SEEK_SET);

    return size;
}
 Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Check source image extension
    if(strstr(argv[2], ".bmp") != NULL)
    {
        encInfo->src_image_fname = argv[2];

        // Check secret file extension
        if(strstr(argv[3], ".txt") != NULL)
        {
            encInfo->secret_fname = argv[3];

            // Check whether output filename is given
            if(argv[4] == NULL)
            {
                encInfo->stego_image_fname = "default.bmp";
                return e_success;
            }
            else
            {
                // Validate output file extension
                if(strstr(argv[4], ".bmp") != NULL)
                {
                    encInfo->stego_image_fname = argv[4];
                }
                else
                {
                    return e_failure;
                }
            }

            return e_success;
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        return e_failure;
    }
}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

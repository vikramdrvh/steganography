#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

// Function to read and validate encoding arguments
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Check if the source image file has a .bmp extension
    if (strstr(argv[2], ".bmp")) 
    {
        encInfo->src_image_fname = argv[2]; // Assign the source image file name
    }
    else
    {
        return e_failure; // Return failure if the extension is not .bmp
    }

    // Check if the secret file has a valid extension
    if (strstr(argv[3], ".txt")) 
    {
        encInfo->secret_fname = argv[3]; // Assign the secret file name
        strcpy(encInfo->extn_secret_file, ".txt"); // Copy the extension to encInfo
    }
    else if (strstr(argv[3], ".c")) // Check for .c extension
    {
        encInfo->secret_fname = argv[3]; // Assign the secret file name
        strcpy(encInfo->extn_secret_file, ".c"); // Copy the extension to encInfo
    }
    else if (strstr(argv[3], ".sh")) // Check for .sh extension
    {
        encInfo->secret_fname = argv[3]; // Assign the secret file name
        strcpy(encInfo->extn_secret_file, ".sh"); // Copy the extension to encInfo
    }
    else
    {
        printf("Error: file is not matching with .txt/.c/.sh\n");
        return e_failure; // Return failure if extension is not valid
    }

    // Check if output file name is provided
    if (argv[4] != NULL) 
    {
        if (strstr(argv[4], ".bmp")) // Check if the output file has a .bmp extension
        {
            encInfo->stego_image_fname = argv[4]; // Assign the stego image file name
        }
        else
        {
            return e_failure; // Return failure if the extension is not .bmp
        }
    }
    else
    {
        printf("Output file not mentioned\nCreating default.bmp as default\n");
        encInfo->stego_image_fname = "default.bmp"; // Use default file name
    }
    return e_success; // Return success
}

// Function to perform encoding
Status do_encoding(EncodeInfo *encInfo) 
{
    int res = open_files(encInfo); // Open the required files
    if (res == e_success)
    {
        printf("Opened required files\n");
    }
    else
    {
        printf("Failed to open required files\n");
    }

    printf("Encoding started\n");
    check_capacity(encInfo); // Check if the image has enough capacity
    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image); // Copy BMP header
    encode_magic_string(MAGIC_STRING, encInfo); // Encode the magic string
    encode_secret_file_extension_size(strlen(encInfo->extn_secret_file), encInfo); // Encode secret file extension size
    encode_secret_file_extension(encInfo->extn_secret_file, encInfo); // Encode secret file extension
    encode_secret_file_size(encInfo->size_secret_file, encInfo); // Encode secret file size
    encode_secret_file_data(encInfo); // Encode secret file data
    copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image); // Copy remaining image data
    return e_success; // Return success
}

// Function to check if the image has enough capacity
Status check_capacity(EncodeInfo *encInfo) 
{
    printf("Checking image capacity\n");
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image); // Get image capacity
    printf("Image capacity: %u\n", encInfo->image_capacity);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret); // Get secret file size

    int size_extn_secret_file = strlen(encInfo->extn_secret_file); // Get size of secret file extension
    int magic_string_len = strlen(MAGIC_STRING); // Get size of magic string
    printf("Checking if image can handle the secret file\n");
    if (encInfo->image_capacity > (54 + (magic_string_len) * 8 + (size_extn_secret_file) * 8 + sizeof(size_extn_secret_file) + sizeof(encInfo->size_secret_file)) * 8 + (encInfo->size_secret_file) * 8) 
    {
        printf("Capacity is sufficient\n");
        return e_success; // Return success if capacity is sufficient
    }
    else
    {
        printf("Image capacity is insufficient\n");
        return e_failure; // Return failure if capacity is insufficient
    }
}

// Function to copy the BMP header
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image) 
{
    printf("Copying image header\n");
    rewind(fptr_src_image); // Rewind to the beginning of the source image
    char buffer[54]; // Buffer to hold the header
    fread(buffer, 54, 1, fptr_src_image); // Read the header from the source image
    fwrite(buffer, 54, 1, fptr_dest_image); // Write the header to the destination image
    printf("Image header copied\n");
    return e_success; // Return success
}

// Function to get the file size
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END); // Move to the end of the file
    return ftell(fptr); // Get the current position (file size)
}

// Function to encode the magic string into the image
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("Encoding magic string\n");
    char arr[8]; // Buffer to hold bits
    int size_of_magic_string = strlen(MAGIC_STRING); // Get the size of the magic string
    for (int i = 0; i < size_of_magic_string; i++)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image); // Read 8 bits from source image
        encode_byte_to_lsb(magic_string[i], arr); // Encode byte to LSB
        fwrite(arr, 8, 1, encInfo->fptr_stego_image); // Write 8 bits to stego image
    }
    printf("Magic string encoded\n");
    return e_success; // Return success
}

// Function to encode the size of the secret file extension
Status encode_secret_file_extension_size(int size, EncodeInfo *encInfo)
{
    printf("Encoding secret file extension size\n");
    char arr[32]; // Buffer to hold bits
    fread(arr, 32, 1, encInfo->fptr_src_image); // Read 32 bits from source image
    encode_size_to_lsb(size, arr); // Encode size to LSB
    fwrite(arr, 32, 1, encInfo->fptr_stego_image); // Write 32 bits to stego image
    printf("Secret file extension size encoded\n");
    return e_success; // Return success
}

// Function to encode the secret file extension
Status encode_secret_file_extension(const char *file_extension, EncodeInfo *encInfo)
{
    printf("Encoding secret file extension\n");
    int secret_extn_size = strlen(encInfo->extn_secret_file); // Get the size of the secret file extension
    char arr[8]; // Buffer to hold bits
    for (int i = 0; i < secret_extn_size; i++)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image); // Read 8 bits from source image
        encode_byte_to_lsb(file_extension[i], arr); // Encode byte to LSB
        fwrite(arr, 8, 1, encInfo->fptr_stego_image); // Write 8 bits to stego image
    }
    printf("Secret file extension encoded\n");
    return e_success; // Return success
}

// Function to encode the size of the secret file
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    printf("Encoding secret file size\n");
    file_size = encInfo->size_secret_file; // Assign secret file size
    char arr[32]; // Buffer to hold bits
    fread(arr, 32, 1, encInfo->fptr_src_image); // Read 32 bits from source image
    encode_size_to_lsb(file_size, arr); // Encode size to LSB
    fwrite(arr, 32, 1, encInfo->fptr_stego_image); // Write 32 bits to stego image
    printf("Secret file size encoded\n");
    return e_success; // Return success
}



// Function to encode the secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("Encode secret.txt File Data\n");
    rewind(encInfo->fptr_secret); // Go to the beginning of the secret file
    char arr[8], ch;
    while ((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image);    // Read 8 bits from source image
        encode_byte_to_lsb(ch, arr);                  // Encode byte to LSB
        fwrite(arr, 8, 1, encInfo->fptr_stego_image); // Write 8 bits to stego image
    }
    printf("Done\n");
}
// Function to copy remaining image data after encoding
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("Copying Left Over Data\n");
    char ch;
    while (fread(&ch, 1, 1, fptr_src))
        fwrite(&ch, 1, 1, fptr_dest); // Copy byte by byte
    printf("Done\n");
    printf("## Encoding Done Successfully ##\n");
}
// Function to encode a byte into the least significant bits
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 7; i >= 0; i--)
    {
        if (data & (1 << i))
            image_buffer[7 - i] |= 1; // Set LSB to 1
        else
            image_buffer[7 - i] &= ~1; // Set LSB to 0
    }
    return e_success;
}
// Function to encode a size into the least significant bits
Status encode_size_to_lsb(char data, char *image_buffer)
{
    for (int i = 31; i >= 0; i--)
    {
        if (data & (1 << i))
            image_buffer[31 - i] |= 1; // Set LSB to 1
        else
            image_buffer[31 - i] &= ~1; // Set LSB to 0
    }
    return e_success;
}
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

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
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
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
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
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


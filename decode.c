#include <stdio.h>
#include "decode.h"
#include <string.h>
#include "types.h"

// Function to read and validate decoding arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if the source image file has a .bmp extension
    if (strstr(argv[2], ".bmp"))
    {
        decInfo->encoded_src_image_fname = argv[2]; // Assign the encoded image file name
    }
    else
    {
        printf("INFO : No input .bmp file passed\n"); // Print error message if the file is not .bmp
        return e_failure; // Return failure status
    }
    return e_success; // Return success status
}

// Function to perform decoding
Status do_decoding(char **argv, DecodeInfo *decInfo)
{
    printf("Decoding Procedure Started \n"); // Indicate start of decoding procedure

    // Perform the decoding steps
    open_encoded_file(decInfo); // Open the encoded file
    decode_magic_string(decInfo); // Decode the magic string
    decode_secret_file_extension_size(decInfo); // Decode the secret file extension size
    decode_secret_file_extension(decInfo); // Decode the secret file extension
    open_decoded_file(argv, decInfo); // Open the decoded file
    decode_secret_file_size(decInfo); // Decode the secret file size
    decode_secret_file_data(decInfo); // Decode the secret file data

    printf("\n## Decoding Done Successfully ##\n\n");

    return e_success; // Return success status
}

// Function to open the encoded file
Status open_encoded_file(DecodeInfo *decInfo)
{
    printf("Opening required files\n"); // Indicate file opening

    // Open the encoded image file in read mode
    decInfo->fptr_encoded_image = fopen(decInfo->encoded_src_image_fname, "r");
    if (decInfo->fptr_encoded_image == NULL) // Check for file opening error
    {
        perror("Fopen"); // Print error message
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->encoded_src_image_fname); // Print file name
        return e_failure; // Return failure status
    }
    printf("Opened %s\n", decInfo->encoded_src_image_fname); // Indicate successful file opening
    return e_success; // Return success status
}

// Function to decode the magic string
Status decode_magic_string(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_encoded_image, 54, SEEK_SET); // Move the file pointer to the start of pixel data
    char password[20]; // Buffer to hold the password
    printf("Enter the password key : "); // Prompt for password
    scanf("%s", password); // Read the password from user
    printf("Decoding magic string \n"); // Indicate magic string decoding

    int len = strlen(password); // Get the length of the password
    char magic_str[len + 1]; // Buffer to hold the decoded magic string
    
    char arr[8]; // Buffer to hold bits

    // Decode each character of the password
    for (int i = 0; i < len; i++)
    {
        fread(arr, 8, 1, decInfo->fptr_encoded_image); // Read 8 bits from encoded image
        magic_str[i] = decode_byte_to_lsb(arr); // Decode byte to LSB
    }
    magic_str[len] = '\0'; // Null terminate the decoded string

    // Check if the decoded magic string matches the password
    if (strcmp(magic_str, password))
    {
        printf("Password key is wrong !!\n"); // Print error message if password doesn't match
        return e_failure; // Return failure status
    }
    else
    {
        printf("Password key is matched\n"); // Indicate successful password match
        return e_success; // Return success status
    }
}

// Function to decode the size of the secret file extension
Status decode_secret_file_extension_size(DecodeInfo *decInfo)
{
    printf("Decoding Output File Extension Size\n"); // Indicate extension size decoding
    char arr[32]; // Buffer to hold bits
    fread(arr, 32, 1, decInfo->fptr_encoded_image); // Read 32 bits from encoded image
    decInfo->file_extension_size = decode_size_to_lsb(arr); // Decode size to LSB
    return e_success; // Return success status
}

// Function to decode the secret file extension
Status decode_secret_file_extension(DecodeInfo *decInfo)
{
    printf("Decoding Output File Extension\n"); // Indicate extension decoding
    char arr[8]; // Buffer to hold bits

    // Decode each character of the file extension
    for (int i = 0; i < decInfo->file_extension_size; i++)
    {
        fread(arr, 8, 1, decInfo->fptr_encoded_image); // Read 8 bits from encoded image
        decInfo->file_extension[i] = decode_byte_to_lsb(arr); // Decode byte to LSB
    }
    decInfo->file_extension[decInfo->file_extension_size] = '\0'; // Null terminate the decoded extension
    return e_success; // Return success status
}

// Function to open the decoded file
Status open_decoded_file(char **argv, DecodeInfo *decInfo)
{
    // Check if output file name is provided
    if (argv[3] == NULL)
    {
        printf("Output File not mentioned. Creating decoded%s as default\n", decInfo->file_extension); // Indicate default file creation
        strcpy(decInfo->decoded_fname, "decoded"); // Assign default file name
    }
    else
    {
        strcpy(decInfo->decoded_fname, argv[3]); // Assign provided file name
    }
    strcat(decInfo->decoded_fname, decInfo->file_extension); // Append file extension to file name

    // Open the decoded file in write mode
    if ((decInfo->fptr_decoded_file = fopen(decInfo->decoded_fname, "w")) != NULL)
        printf("Opened %s\n", decInfo->decoded_fname); // Indicate successful file opening
    else
        return e_failure; // Return failure status

    return e_success; // Return success status
}

// Function to decode the size of the secret file
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    printf("Decoding %s File Size\n", decInfo->decoded_fname); // Indicate file size decoding
    char arr[32]; // Buffer to hold bits
    fread(arr, 32, 1, decInfo->fptr_encoded_image); // Read 32 bits from encoded image
    decInfo->decoded_size_of_the_file = decode_size_to_lsb(arr); // Decode size to LSB
    return e_success; // Return success status
}

// Function to decode the secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("Decoding %s file data\n", decInfo->decoded_fname); // Indicate file data decoding
    char arr[8], ch; // Buffer to hold bits and character

    // Decode each byte of the secret file data
    for (int i = 0; i < decInfo->decoded_size_of_the_file; i++)
    {
        fread(arr, 8, 1, decInfo->fptr_encoded_image); // Read 8 bits from encoded image
        ch = decode_byte_to_lsb(arr); // Decode byte to LSB
        fwrite(&ch, 1, 1, decInfo->fptr_decoded_file); // Write decoded byte to file
    }
    return e_success; // Return success status
}

// Function to decode a byte from the least significant bits
char decode_byte_to_lsb(char *image_buffer)
{
    char ch = 0; // Initialize character to zero

    // Decode each bit from the LSB of image buffer
    for (int i = 0; i < 8; i++)
    {
        if (image_buffer[i] & 1) // Check if LSB is set
            ch = ch | (1 << (7 - i)); // Set corresponding bit in character
    }
    return ch; // Return decoded character
}

// Function to decode a size from the least significant bits
int decode_size_to_lsb(char *image_buffer)
{
    int temp = 0; // Initialize integer to zero

    // Decode each bit from the LSB of image buffer
    for (int i = 0; i < 32; i++)
    {
        if (image_buffer[i] & 1) // Check if LSB is set
            temp = temp | (1 << (31 - i)); // Set corresponding bit in integer
    }
    return temp; // Return decoded size
}


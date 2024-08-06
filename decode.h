#ifndef DECODE_H
#define DECODE_H

#include "types.h"
typedef struct Decode
{
   /*Encoded source image info*/
    char *encoded_src_image_fname;
    FILE *fptr_encoded_image;

    /*magic string*/
    char *magic_string;

    /*Decoded file info*/
    int decoded_size_of_the_file;
    int file_extension_size;
    char file_extension[20];
    char decoded_fname[20];
    FILE *fptr_decoded_file;
} DecodeInfo;


/* validate the command line arguments*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*open the required files for decoding*/
Status open_encoded_file(DecodeInfo *decInfo);

/*start the decoding process*/
Status do_decoding(char **argv,DecodeInfo *decInfo);

/*Decode the magic string*/
Status decode_magic_string(DecodeInfo *decInfo);

/*decode secret file extn size*/
Status decode_secret_file_extension_size(DecodeInfo *decInfo);

/*decode secret file extension*/
Status decode_secret_file_extension(DecodeInfo *decInfo);

/*open a file to store the decoded data*/
Status open_decoded_file(char **argv,DecodeInfo *decInfo);

/*decode secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo);

/*decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/*decode a byte of data from encoded image*/
char decode_byte_to_lsb(char *image_buffer);

/*decode a int size data from the encoded data*/
int decode_size_to_lsb(char *image_buffer);

#endif

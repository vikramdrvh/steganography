# Image Steganography in C

This is a custom C implementation of **Image Steganography**—the art of hiding secret data in plain sight. If you've ever wanted to conceal a private message inside a standard image file without altering how the image looks to the human eye, this tool does exactly that.

The project works by manipulating the pixel data in a BMP image (like the included `beautiful.bmp`) to securely embed text from a file (like `secret.txt`). 

## Core Features
* **Message Encoding:** Reads a secret text message and seamlessly embeds it into the pixel data of a target BMP image.
* **Message Decoding:** Parses a previously modified BMP image to extract and reveal the hidden text using `decode.c`.
* **Lossless Preservation:** Modifies the image data so subtly that the output image looks visually identical to the original.
* **Modular Architecture:** Cleanly separates the encoding, decoding, and common functionalities into distinct headers (`encode.h`, `decode.h`, `common.h`, `types.h`).



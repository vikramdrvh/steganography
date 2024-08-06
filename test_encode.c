#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include"decode.h"

int main(int argc,char *argv[])
{
    if(argc < 3){
        printf("pass command line arugument\n"); // check for command line arugument
        return 1;
    }

    int ret = check_operation_type(argv);  //check operation function is used to check whether its in encoding or decoding all the command line aruguments are passed.

    if(ret == 0){ //check the return value since e_encode holds value has 0 so check against the value.
        printf("Selected encoding\n");
        
        EncodeInfo encInfo; //creating a structure variable named encInfo.

        read_and_validate_encode_args(argv,&encInfo);// call the function to validate all the cmd line aruguments.

        do_encoding(&encInfo);//on sucessful return of validation function, encoding function is called to start encoding procedure.
    }
    else if(ret == 1){
        printf("Selected decoding\n");

        DecodeInfo decInfo;

        read_and_validate_decode_args(argv,&decInfo);

        do_decoding(argv,&decInfo);
  
    }
    else{
        printf("printf error\n");
    }

}

OperationType check_operation_type(char *argv[]){
    if(strcmp(argv[1],"-e") == 0){ //here the first command line arugument is taken and checked whether its encoding.
      return e_encode; // this return e_encode means return 0 which is typedefed. 
    }
    else if(strcmp(argv[1],"-d") == 0){ //here the first arugument is checked whether its decoding.
        return e_decode;// this return e_decode means return 1 which is typedefed.
    }
    else{
        return e_unsupported;// this return e_unsupported means return 2(enum)which is typedefed.
    }
}

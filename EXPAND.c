#include <stdlib.h>
#include <stdio.h>
#include <string.h>
///decoding globals///
int N;
int S;
int L;
int W_size;
int LB_size;
int LEL;
int W_F;
int ob;
int EOFF;
char * window;
FILE * file;
int window_iter;

////////////////DEBUGGING FUNCTIONS/////////////////////////////
void print_parameter_info(){ 
//Prints values of N,L,S
    fprintf(stderr,"PARAMETER VALUES: N=%d , L=%d , S=%d\n",N,L,S);
}
void print_variable_info(){ 
//Prints basic predetermined variables
    fprintf(stderr,"WINDOW SIZE: %d, LOOKAHEAD SIZE: %d, W-F: %d, LEL: %d\n",W_size,LB_size,W_F,LEL);
}
void print_overall_bytes(){
//Prints how many completed bytes were packed from bitfactory and printed
    fprintf(stderr,"OVERALL BYTES AFTER ITERATION: %d\n",ob);
}
void print_window(){ 
//Prints the whole window in the current state and corresponding characters
    for (int i = 0; i < W_size; i++){
        fprintf(stderr,"%d: %c ,, %d\n",i,window[i],sizeof(window[i]));
}}
void DEBUG(){ 
//Calls any of the above debugging functions
    //print_window();
    //print_variable_info();
    //print_overall_bytes();
    //print_parameter_info();
}
////////////END OF DEBUGGING FUNCTIONS////////////////////////////


void operate_window(int length,int mode){
//Mode 0: Initiates window
//Mode 1: Shifts the window whenever matches (or nonmatches) are complete from the window
    switch(mode){
        case 0:
            for(int i = 0; i < W_size;  i++){ // initialize window to consist of W blanks 
                window[i] = 0;
            }
            break;
        case 1:
            for (int i = 0; i < W_size; i++){
                window[i] = window[i+length];
            }
            break;
}}

void bitdelivery(){
//Takes the file or stdin input characters, and shifts bits in order to find their true representations according the project description
//These representations also affect the window and what is stored in there.
    unsigned char cur_char = 0;
    unsigned char CODE = 0,STRLEN =0,LITERAL=0,LEN = 0;
    unsigned short OFFSET = 0;
    int strlen = 0;
    int mode = 0;
    //int LIT_count = 8;
    int byte_count = 8;
    cur_char = (unsigned char)fgetc(file);
    unsigned char * LB_pointer;
    unsigned char * S_pointer;
    while(mode != 4){ //currently causing problems!!!!!
        switch(mode){
            //Mode 0: Shift and compute L bits for Code/Len (if all zeros, set mode for Mode 1, else Mode 3)
            //Mode 1: Shift and compute S bits for Strlen (afterwards set for Mode 2, if strlen is zero set mode 4 for EOF then break)
            //Mode 2: Shift and compute eight (8) bits for Literals (iterate as many times as the value of strlen, afterwards set Mode 0)
            //Mode 3: Shift and compute N bits for Offset (store bits into an unsigned short, afterwards set Mode 0)
            //Mode 4: EOF sequence encountered, break
            case 0:
                CODE &= 0;
                for (int i = 0; i < L; i++){
                    CODE = (CODE << 1);
                    CODE |= ((cur_char & 0x80)>>7);
                    cur_char = (cur_char << 1);
                    byte_count--;
                    if (byte_count == 0){
                        cur_char = (unsigned char)fgetc(file);
                        byte_count = 8;
                }}
                fprintf(stderr,"CODE/LEN: %u\n",CODE);
                if (CODE == 0){
                    mode = 1;
                }
                else{
                    LEN = CODE+1;
                    mode = 3;
                }
                CODE &= 0;
                break;
            case 1:
                STRLEN &= 0;
                for (int i = 0; i<S; i++){
                    STRLEN = (STRLEN << 1);
                    STRLEN |= ((cur_char & 0x80)>>7);
                    cur_char = (cur_char << 1);
                    byte_count--;
                    if (byte_count == 0){
                        cur_char = (unsigned char)fgetc(file);
                        byte_count = 8;
                }}
                fprintf(stderr,"STRLEN: %u\n",STRLEN);
                if (STRLEN == 0){
                    mode = 4;
                }
                else{
                    strlen = (int)STRLEN;
                    mode = 2;
                }
                STRLEN &= 0;
                break;
            case 2:
                if (strlen == 0){
                    mode = 0;
                    break;
                }
                else{
                    LITERAL &= 0;
                    for (int i = 0; i < 8; i++){
                        LITERAL = (LITERAL << 1);
                        LITERAL |= ((cur_char & 0x80)>>7);
                        cur_char = (cur_char << 1);
                        byte_count--;
                        if (byte_count == 0){
                            cur_char = (unsigned char)fgetc(file);
                            byte_count = 8;
                    }}
                    window[W_F] = LITERAL;
                    operate_window(1,1);
                    fprintf(stderr,"LITERAL: %c\n",LITERAL);
                    fprintf(stdout,"%c",LITERAL);
                    LITERAL &= 0;
                    strlen--;
                    mode = 2;
                }
                break;
            case 3:
                OFFSET &= 0;
                for (int i = 0; i<N; i++){
                    OFFSET = (OFFSET << 1);
                    OFFSET |= (unsigned short)((cur_char & 0x80)>>7);
                    cur_char = (cur_char << 1);
                    byte_count--;
                    if (byte_count == 0){
                        cur_char = (unsigned char)fgetc(file);
                        byte_count = 8;
                }}
                fprintf(stderr,"OFFSET: %u\n",OFFSET);
                S_pointer = &window[W_F];
                for (int i = 0; i<OFFSET; i++){
                    S_pointer--;
                }
                for (int i = 0; i<LEN; i++){
                    window[W_F+i] = S_pointer[i];
                    fprintf(stdout,"%c",window[W_F+i]);
                }
                operate_window(LEN,1);
                OFFSET &= 0;
                mode = 0;
                break;
            case 4:
                break;
        }
    }
}

void decode(int N, int L, int S){
    W_size = (1 << N); // WINDOW SIZE
    LB_size = (1 << L); //LOOKAHEAD BUFFER SIZE
    LEL = ((1 << S)-1); //LONGEST ENCODED LITERAL
    W_F = W_size - LB_size; // (WINDOW SIZE) - (LOOKAHEAD BUFFER SIZE) = LINE DIVIDER
    EOFF = LB_size;
    window_iter = W_F;
    window = (unsigned char*)malloc(W_size * sizeof(unsigned char)); // initialize entire window

    operate_window(0,0);
    bitdelivery();
}


int main(int argc, char * argv[]){
    file = fopen(argv[1],"r");
    if (file == NULL){
        file = stdin;
    }
    N = fgetc(file);
    L = fgetc(file);
    S = fgetc(file);
    decode(N,L,S);
    fclose(file);
    DEBUG();
    return 0;         
    
}
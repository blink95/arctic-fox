#include <stdlib.h>
#include <stdio.h>
#include <string.h>
///encoding globals////
int N = 11;
int L = 4;
int S = 3;
int W_size;
int LB_size;
int LEL;
int W_F;
char * window;
FILE * file;
int EOFF;
int ob = 0;
///bitfactory() globals////
unsigned char factory = 0;
int bits_left = 8;


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
    print_overall_bytes();
    print_parameter_info();
}
////////////END OF DEBUGGING FUNCTIONS////////////////////////////



void bitfactory(int len, int offset,int mode,unsigned char *literal){ 
//Should export the encoding to stdout by packing the bits into 8-bit chars
//Mode 2: Token Double
//Mode 3: Token Tripple
//Mode 0: EOF case
    unsigned char len_temp = len-1; 
    unsigned char strlen = len;
    unsigned short offset_temp = (unsigned short)offset;
    unsigned char offset_high = (unsigned char)(offset_temp >> 8);
    unsigned char offset_low = (unsigned char)(offset_temp & 0x00FF);
    int high_front = N-8;
    switch(mode){

        case 2:

            ///////////////EXPORT LEN/////////////////
            if (bits_left < L){
                factory |= (len_temp >> (L-bits_left));
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                factory |= (len_temp << (8-(L-bits_left)));
                bits_left = 8-(L-bits_left);
         
            }
            else if (bits_left == L){
                factory |= len_temp;
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                bits_left = 8;
            }
            else{
                factory |= (len_temp << (bits_left-L));
                bits_left = bits_left-L;
            }

            //// EXPORT OFFSET_HIGH ////
            if (bits_left < high_front){
                factory |= (offset_high >> (high_front-bits_left));
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                factory |= (offset_high << (8-(high_front-bits_left)));
                bits_left = 8 -(high_front-bits_left);
            }
            else if (bits_left == high_front){
                factory |= offset_high;
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                bits_left = 8;
            }
            else{
                factory |= (offset_high << (bits_left-high_front));
                bits_left = bits_left - high_front;
            }

            //// EXPORT OFFSET_LOW ////
            if (bits_left == 8){
                factory |= offset_low;
                fprintf(stdout,"%c",factory);
                ob++;
                factory &=0;
                bits_left = 8;
            }
            else{
                factory |= (offset_low >> (8-bits_left));
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                factory |= (offset_low << bits_left);
            }
            break;

        case 3:

            ///////////// EXPORT CODE ///////////////
            if (bits_left < L){
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                bits_left = 8-(L-bits_left);
         
            }
            else if (bits_left == L){
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                bits_left = 8;
            }
            else{
                bits_left = bits_left-L;
            }

            /////////// EXPORT STRLEN //////////////
            if (bits_left < S){
                factory |= (strlen >> (S-bits_left));
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                factory |= (strlen << (8-(S-bits_left)));
                bits_left = 8-(S-bits_left);
         
            }
            else if (bits_left == S){
                factory |= strlen;
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                bits_left = 8;
            }
            else{
                factory |= (strlen << (bits_left-S));
                bits_left = bits_left-S;
            }

            //////////// EXPORT LITERALS ///////////////
            for (int i = 0; i<len;i++){
                unsigned char liter = literal[i];
                factory |= (liter >> (8-bits_left));
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                factory |= (liter << bits_left);
            }
            break;

        case 0:

            /////////// EXPORT EOF ZERO CODE ////////
            if (bits_left < L){
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                bits_left = 8-(L-bits_left);
         
            }
            else if (bits_left == L){
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                bits_left = 8;
            }
            else{
                bits_left = bits_left-L;
            }

            ////////// EXPORT EOF STRLEN ///////////
            if (bits_left < S){
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                bits_left = 8-(S-bits_left);
                fprintf(stdout,"%c",factory);
                ob++;
         
            }
            else if (bits_left == S){
                fprintf(stdout,"%c",factory);
                ob++;
                factory &= 0;
                bits_left = 8;
            }
            else{
                fprintf(stdout,"%c",factory);
                bits_left = bits_left-S;
                ob++;
            }
            break;
}}


void operate_window(int length,int mode){
//Mode 0: Initiates window
//Mode 1: Shifts the window whenever matches (or nonmatches) are complete from the window
    switch(mode){
        case 0:
            for(int i = 0; i < W_F;  i++){ // initialize window to consist of W-F blanks 
                window[i] = 0;
            }
            for(int i = W_F; i < W_size; i++){ // initialize window to consist of first F characters of the input after blanks
                unsigned char c;
                if(feof(file)){
                    window[i] = 0; //to check for EOF later??
                    EOFF--;
                } 
                else{
                    c = fgetc(file);
                    window[i] = c;
            }}
            break;
        case 1:
            for (int i = 0; i < W_size; i++){
                window[i] = window[i+length];
            }
            for (int i = (W_size-length); i < W_size; i++){
                unsigned char c;
                if(feof(file)){
                    window[i] = 0; //to check for EOF later??
                    EOFF--;
                } 
                else{
                    c = fgetc(file);
                    window[i] = c;
            }}
            break;
}}


void matching(){
//Iterate through the elements in the Lookahead Buffer and the Window to find matches and shift the window
//When matches are found, call the Bitfactory function to pack into characters and send to stdout
    unsigned char *LB_pointer;
    unsigned char *S_pointer;
    unsigned char * cont_literal;
    cont_literal = (unsigned char*)malloc(LEL * sizeof(unsigned char));
    int cont_literal_count = 0;
    LB_pointer = &window[W_F]; // pointer to first element in lookahead buffer
    S_pointer = &window[(W_F-1)]; // pointer to first element in search buffer (will go backwards as we iterate)
    int max_ML = 1; //stores ML
    int offset;
    int tcount = 0;
    while(EOFF > 0){
        if (tcount == W_F){ //where we should apply the longest match thus far etc.
            if (max_ML > 1){
                if (cont_literal_count !=0){
                    bitfactory(cont_literal_count,0,3,cont_literal);
                }
                bitfactory(max_ML,offset+1,2,cont_literal);
                cont_literal_count = 0;
            }
            else{
                if (LB_pointer[0] != EOF){
                    cont_literal[cont_literal_count] = LB_pointer[0];
                    cont_literal_count++;
                }
                if (cont_literal_count == LEL){
                    bitfactory(cont_literal_count,0,3,cont_literal);
                    cont_literal_count = 0;
                }
            }
            operate_window(max_ML,1);
            LB_pointer = &window[W_F]; // reset pointers to original positions
            S_pointer = &window[(W_F-1)];
            tcount = 0; 
            max_ML = 1;
        }
        else{
            if (S_pointer[0] == LB_pointer[0]){ // if the current Search pointer equals the first char of LB
                int ML = 1;
                for(ML; LB_pointer[ML] == S_pointer[ML]; ML++){
                }
                if (ML > max_ML){
                    max_ML = ML;
                    offset = tcount;
            }}
            S_pointer--;
            tcount++;
    }}
    if (cont_literal_count !=0){
        bitfactory(cont_literal_count-1,0,3,cont_literal);
        cont_literal_count = 0;
    }
    bitfactory(0,0,0,cont_literal); // EOF case
    free(cont_literal);
}


void encode(int N, int L, int S, char * filename){
//Open the file, initalize the window, and be the process of matching
    unsigned char N_decode = N; //one byte of information to be output later
    unsigned char L_decode = L;
    unsigned char S_decode = S;
    fprintf(stdout,"%c",N_decode);
    fprintf(stdout,"%c",L_decode);
    fprintf(stdout,"%c",S_decode);
    W_size = (1 << N); // WINDOW SIZE
    LB_size = (1 << L); //LOOKAHEAD BUFFER SIZE
    LEL = ((1 << S)-1); //LONGEST ENCODED LITERAL
    W_F = W_size - LB_size; // (WINDOW SIZE) - (LOOKAHEAD BUFFER SIZE) = LINE DIVIDER
    EOFF = LB_size;
    window = (unsigned char*)malloc(W_size * sizeof(unsigned char)); // initialize entire window
    file = fopen(filename,"rb");
    if (file == NULL){
        fprintf(stderr,"Error: Could not open file!\n");
        return;
    }  
    operate_window(0,0);
    matching();
    DEBUG();

}


int main(int argc, char * argv[]){
//Take parameter/value input from command line and begin the process of encoding
    if (argc > 5){
        fprintf(stderr,"Error: Too many arguments!\n");
        return 0;
    }
    if (argc > 2){
        char parameter;
        int value;
        for (int i = 1; i < argc-1; i++){ // check interval of commands for -P=V where P is N, S, or L and V is a valid value  
            sscanf(argv[i],"-%c=%d",&parameter,&value);
            switch(parameter){
                case 'n': case 'N':
                    if (value > 8 && value < 15){
                        N = value;
                        break;
                    }
                    fprintf(stderr,"Error: N value outside of allowable range, changed to default value of 11.\n");
                    break;
                case 's': case 'S':
                    if (value > 0 && value < 6){
                        S = value;
                        break;
                    }
                    fprintf(stderr,"Error: S value outside of allowable range, changed to default value of 3.\n");
                    break;
                case 'l': case 'L':
                    if (value > 2 && value < 5){
                        L = value;
                        break;
                    }
                    fprintf(stderr,"Error: L value outside of allowable range, changed to default value of 4.\n");
                    break;
                default:
                    fprintf(stderr,"Error: Parameter '-%s' not recognized.",parameter);
                    break;
    }}}
    encode(N,L,S,argv[argc-1]);
    fclose(file);
    free(window);
    return 0;
}
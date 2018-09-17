SOURCE:
	Information and specifics on the LZ77 Sliding Window and other Lempel-Ziv variations can be found at: http://orbit.dtu.dk/files/134976715/LIPIcs_CPM_2017_15.pdf

PARAMETERS OF THE LEMPEL-ZIV SLIDING WINDOW:
	
	N specifies the number of bits used to encode an offset into the window. N also determines the window size, W, which is 2N. The default value of N is 11. 

	L specifies the number of bits used to encode the maximum match length. L also determines the lookahead buffer size, F, which is 2L. The default value of L is 4. 

	S specifies the number of bits used to encode the maximum length of literal strings. S also determines the length of the longest encoded literal string, which is 2S-1. The default value of S is 3.

	The window consists of the W-F most recent input characters that have been processed and encoded plus the lookahead buffer, that contains the next F input characters that are yet to be processed.


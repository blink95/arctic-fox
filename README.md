Files compile with the Windows version of TDM-GCC-64

Makefile runs using GnuWin for make

//LZ.c outputs a compressed version of the input content.

//EXPAND.c recovers the original content from the compressed version.

//big.txt will act as our binary file for encoding/decoding testing, downloaded from https://norvig.com/big.txt

//MAKEFILE compiles both LZ.c and EXPAND.c into executables using TDM-GCC-64 and is run by using GnuWin make

CURRENT PROCEDURE:
	
	1. Compile both LZ.c and EXPAND.c using the MAKEFILE by typing 'make' into Windows Command Prompt (GnuWin make.exe must be installed)
	
	2. After successful compilation, type 'encode' into the command prompt (currently this encode.cmd is set to run the LZ executable on big.txt, but you can add your own binary file for testing by editing encode.cmd).
	
	3. Look into your current directory, you should now see a compressed file for the binary file that you encoded. This new file size is dependant on L, S, and N integer ranges.
	
	4. After successful encoding of the binary file, you may now type 'decode' into the command prompt (currently this decode.cmd is set to run the EXPAND executable on big.txt, but you can add your own binary file for testing by editing decode.cmd)
	
	5. Look into your current directory, you should now see a recovered file for the binary file that you decoded. This new file size should be exactly the same as the original binary file for testing. This recovered file should contain the exact materials as the original binary file for testing. 

EXAMPLES OF SOME COMMANDS (AFTER COMPILATION OF LZ AND EXPAND):

	LZ -N=12 -L=3 -S=4 big.txt > big-encoded.compressed
	EXPAND big-encoded.compressed > big-decoded.recover
	diff big.txt big-decoded.recover
	
	LZ big.txt | EXPAND > big-decoded.recover
	diff big.txt big-decoded.recover

# Shared-Memory
Write two  C programs that communicate with each other viasimple shared memory. 
The first program receiver.c  runs in an infinite loop receiving alpha numeric strings as input from the user, 
one line at a time. After reading one line from the standard input, 
this program sends this information to the other program only if the line contains the secret code "C00L". 
The sharing of data between the two processes should take place via shared memory. 
The second program processor.c creates an output file secrets.out and waits for user input to be sent by the receiver program. 
As soon as one line is received from the receiver, it counts the number of digits in 
that line and dumps the digit count along with the original line in the secrets.out file. 
This program also runs in an infinite loop.

Executing steps for receive.c:
1 open one terminal, run:  “gcc receive.c”
2 execute the object file for receive.c: “./a.out”

Executing steps for processor.c:
1 open the other terminal, run:  “gcc processor.c –o secret.out”
2 execute the object file for receive.c: “./secret.out”

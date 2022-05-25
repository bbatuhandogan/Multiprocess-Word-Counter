# Multiprocess-Word-Counter
For each given file, it creates a separate process with the fork() command and prints the number of lines, words and characters in the file. And if more than one file is given, it prints the total amount with shared memories.
Words are strings of characters separated by a non-zero space \t \n.

If the File extension is not given or the '+' character is given, the file in stdin will be processed.

If no print parameter is given, it is printed in the following order: line, word, character.

-l: Printing line count 

-w: Printing word count
                       
-c: Printing chracter count

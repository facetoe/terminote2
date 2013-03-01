##Terminote


###What is it?
Terminote is a simple note program for the terminal written in C. You can pipe data to it, insert a note and search notes among other things. It also has an interactive mode for managing your notes. 


###How do I install it?
Just `git clone` this repo, `cd` to the `src` directory and type `make`. 


###What does it run on?
I've tested it on Ubuntu and OS X. If you have trouble getting it to run on another *nix platform file a issue and I'll see if I can fix it.


###How do I use it?
Run terminote with the `-h` flag for the most up to date usage information. Here is a sample of its output:

Terminote version 1.0 - a command line note tool.

    Terminote reacts differently depending on how you call it.
    If you pipe data to it, or supply a command line argument, Terminote runs in non-interactive mode.
    If you call terminote with no arguments from the shell, terminote will enter interactive mode.

    ARGUMENTS: Arguments in capitals are destructive, lower case leaves notes intact.
      -h: Print this message and quit.
      -P: Prints the last note only, no path or number, then deletes it.
      -F: Prints the last note with full info (path/time/num) then deletes it.
      -N: Prints the note at the supplied note number and deletes it, if it exists. Requires an integer argument. 
      -D: Deletes the note at supplied note number, if it exists. Requires an integer argument.
      -R: Deletes all notes.
      -p: Prints the note at the supplied note number, leaving it intact. Requires an integer argument.
      -l: Prints all the notes leaving them intact.
      -f: Searches for notes containing supplied sub string and prints them, leaving them intact. Requires a string argument.
      -a: Appends a note to the list. Requires a string argument.

    CONTACT:
      Please email any bugs, requests or hate mail to facetoe@ymail.com, or file a bug at https://github.com/facetoe/terminote2

#Examples:

You can save the output of a command: 

`$ cal | ./terminote`


Then "pop" it out again with the `-P` flag for just the note, or the `-F` flag for all the info, eg:


    $ ./terminote -P   
         March 2013
    Su Mo Tu We Th Fr Sa
                    1  2
     3  4  5  6  7  8  9
    10 11 12 13 14 15 16
    17 18 19 20 21 22 23
    24 25 26 27 28 29 30
    31

    $ ./terminote -F   
    NoteNum: 1
    Path: /Users/fragmachine/git/terminote2/src
    Time: Fri Mar  1 18:18:30 2013

         March 2013
    Su Mo Tu We Th Fr Sa
                    1  2
     3  4  5  6  7  8  9
    10 11 12 13 14 15 16
    17 18 19 20 21 22 23
    24 25 26 27 28 29 30
    31

If you call terminote with no arguments, it enters interactive mode:

    $ ./terminote 

    Welcome to Terminote interactive:
    (w) to insert new note
    (a) and (d) to navigate through notes
    (e) to print all notes
    (g) to delete all notes
    (f) to search for notes
    (m) to print this message
    (q) to quit



###Why?
I wanted to learn about linked lists and C, so this is what I came up with.


###It doesn't work.
Please file an issue here on Github or email me at the address on my profile.


###Can you make it do X?
Maybe, file a feature request or email me and I'll give it a try, or you are welcome to fork terminote and implement it yourself.


###To do list:

* Redesign terminote to allow people to write plugins for it.

* Add functionality to edit notes.

* Make the note capacity dynamically increase in size up to a set size. 

* Copy notes from clipboard to terminote.

* Copy notes from terminote to clipboard.
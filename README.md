#Terminote


##What is it?
Terminote is a simple note program for the terminal written in C. You can pipe data to it, insert and search notes among other things. It also has an interactive mode for managing your notes. 

##Usage:

There are a few ways to input data into terminote. You can pipe data to it:

`⇒ ls /usr/bin | ./terminote2`

copy text from the clipboard (requires xclip): 

`⇒ ./terminote2 -c`

or add text from the command line:

`⇒ ./terminote2 -a "I must remember this important thing"`

When you want to view a note, you can read it in a nano-like ncurses interface by running terminote with no options. In interactive mode the 'a' and 'd' keys loop through the notes, the 'w' and 'e' keys jump to the bottom and the top of the message and 'Cntrl-f' opens the menu. Or, if you want to read the note from the shell, you have a few options. 

You can "pop" the last note with the `-P` flag. This prints the last note and then deletes it:

    ⇒ ./terminote2 -P
    Note Number: 3
    Path: /home/facetoe/git/terminote2/Debug
    Time: Sun Jul 21 15:39:27 2013
    Message:
    I must remember this important thing

If you want to keep the note, you can use the `-p` flag to print the note without deleting it. In terminote, all uppercase flags are destructive, while the lower case ones are not. 

If you want to print a specific note, you can use the `-n` or `-N` flag with the note number:

    ⇒ ./terminote2 -n 3
    Note Number: 3
    Path: /home/facetoe/git/terminote2/Debug
    Time: Sun Jul 21 15:53:01 2013
    Message:
    This is note number 3

Terminote also supports a couple of methods for searching stored notes. You can print all notes that contain a substring with the `-f` flag:

    ⇒ ./terminote2 -f debugging
    Note Number: 2
    Path: /home/facetoe/git/terminote2/Debug
    Time: Sun Jul 21 16:00:46 2013
    Message:
    If debugging is the process of removing software bugs, then programming must be the process of putting them in.

Or you can "grep" the stored notes with the `-g` flag, printing each line where the string occurs along with the note and line number:


    ⇒ ./terminote2 -g debug   
    Msg: 1: Line 827: isdv4-serial-debugger
    Msg: 1: Line 862: jsadebugd
    Msg: 1: Line 878: kdebugdialog
    Msg: 1: Line 911: kubuntu-debug-installer
    Msg: 2: Line 1: If debugging is the process of removing software bugs, then programming must be the process of putting them in.

For a full list of options, run terminote with the `-h` flag.


##How do I install it?
Just `git clone` this repo, `cd` to the `src` directory and type `make`. 


##What does it run on?
I've tested it on Mint, Ubuntu and OS X. If you have trouble getting it to run on another *nix platform please file an issue and I'll see if I can fix it.


##Why?
I wanted to learn about linked lists and C and this what I came up with.


##It doesn't work.
Please file an issue here on Github or email me at the address on my profile.


##Can you make it do X?
Maybe, file a feature request or email me and I'll give it a try, or you are welcome to fork terminote and implement it yourself.


##To do list:

* Redesign terminote to allow people to write plugins for it.

* Add functionality to edit notes in interactive mode.

* Add option to the grep function to respect word boundries.

* Add option to specify time range to search or print in.

* Add option to read from a file.

* Add option to write to specified file. 

/*
 * ui.c
 *
 *  Created on: 21/07/2013
 *      Author: facetoe
 */

#include "ui.h"

#define SIGWINCH 28 // Needed because OSX doesn't recognize SIGWINCH
struct sigaction sa; // For our signal handler

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct winsize wSize; // I'm using this instead of getmaxyx() because it didn't seem to always work.

/* Allocate and initialize a POSITION struct */
void initPosition( DISPLAY_DATA **disp ) {
    DISPLAY_DATA *tmp = malloc( sizeof(DISPLAY_DATA) );
    if ( !tmp ) {
        fprintf( stderr, "Error allocating memory in  initPosition\n" );
        exit( 1 );
    }
    tmp->NCOLS = 0;
    tmp->NROWS = 0;
    tmp->cursorRow = 0;
    tmp->cursorCol = 0;
    tmp->currMsg = NULL;
    *disp = tmp;
}

bool RECIEVED_SIGWINCH = false;
bool needsRefresh = false;

/* Variables for ncurses */
char *mainMenuStrings[] =
        { "Add", "Delete", "Quit", "Help", ( char * ) NULL, };

static ITEM **mainMenuItems;
static MENU *footerMenu;
static WINDOW *wins[3];

/* Enum for different windows */
enum {
    TOP, MID, BOT
};

/* Get the size of the terminal screen */
void getScrnSize( DISPLAY_DATA *disp ) {
    assert( disp != NULL );
    ioctl( 0, TIOCGWINSZ, &wSize );
    disp->NCOLS = wSize.ws_col;
    disp->NROWS = wSize.ws_row;
}

/* Setup ncurses */
void initNcurses() {
    initscr();
    raw();
    nonl();
    noecho();
    cbreak();
    keypad( stdscr, TRUE );
    start_color();
    init_pair( 1, COLOR_BLACK, COLOR_WHITE );
}

void initWins( DISPLAY_DATA *disp ) {
    getScrnSize( disp );
    wins[TOP] = newwin( 1, disp->NCOLS, 0, 0 );
    wins[MID] = newwin( disp->NROWS - 2, disp->NCOLS, 1, 0 );
    wins[BOT] = newwin( 1, disp->NCOLS, disp->NROWS - 1, 0 );

    /* Turn colors on */
    wattron( wins[TOP], COLOR_PAIR(1) );
    wbkgd( wins[TOP], COLOR_PAIR(1) );

    keypad( wins[MID], true );
}

/* Setup and print the top window to screen */
void showTopWin( DISPLAY_DATA *disp ) {

    wclear( wins[TOP] );

    /* If the we are not in the root node then update the top window with time, number and path info*/
    if ( disp->currMsg->messageNum != 0 ) {
        char noteStr[100];
        snprintf( noteStr, 100, "Note #%d", disp->currMsg->messageNum );
        mvwprintw( wins[TOP], 0,
                ( disp->NCOLS / 2 ) - ( strlen( noteStr ) / 2 ), noteStr );
        mvwprintw( wins[TOP], 0, 0, disp->currMsg->path );
        mvwprintw( wins[TOP], 0,
                ( disp->NCOLS - strlen( disp->currMsg->time ) ) - 2,
                disp->currMsg->time );
    } else {
        /* Otherwise just print the title and version */
        char title[50];
        sprintf( title, "Terminote %.1f", VERSION );
        mvwprintw( wins[TOP], 0, ( disp->NCOLS / 2 ) - ( strlen( title ) / 2 ),
                title );
    }
    wrefresh( wins[TOP] );
}

/* Print the current page */
void printPage( DISPLAY_DATA *disp, int numRows ) {
    LINE *tmp = disp->currMsg->pageTop;
    assert( tmp != NULL );

    for ( int i = 0; i < numRows && tmp->next; tmp = tmp->next, i++ ) {
        mvwprintw( wins[MID], i, 0, "%s", tmp->text );
    }
    disp->currMsg->pageBot = tmp;
    wrefresh( wins[MID] );
}

/* Print the start of the message */
void printTop( DISPLAY_DATA *disp ) {
    LINE *tmp = disp->currMsg->first;
    wclear( wins[MID] );

    disp->currMsg->pageTop = tmp;

    for ( int i = 0; i < disp->NROWS - 2 && tmp->next; tmp = tmp->next, i++ ) {
        mvwprintw( wins[MID], i, 0, "%s", tmp->text );
    }
    disp->currMsg->pageBot = tmp;
    disp->currMsg->currentLine = tmp;
    wrefresh( wins[MID] );
}

/* Print the bottom of the message */
void printBot( DISPLAY_DATA *disp ) {
    LINE *tmp = disp->currMsg->last;
    wclear( wins[MID] );

    for ( int i = 0; i < disp->NROWS - 3 && tmp->prev; tmp = tmp->prev, ++i )
        ;

    disp->currMsg->pageTop = tmp;

    for ( int i = 0; tmp->next; tmp = tmp->next, i++ ) {
        mvwprintw( wins[MID], i, 0, "%s", tmp->text );
    }

    disp->currMsg->pageBot = disp->currMsg->last;
    disp->currMsg->currentLine = disp->currMsg->last;
    wrefresh( wins[MID] );
}

/* Scrolls the page up  */
void scrollUpPage( DISPLAY_DATA *disp, WINDOW *win ) {

    /* If we are already at the top then print the page and return */
    if ( disp->currMsg->currentLine == disp->currMsg->pageTop ) {
        printPage( disp, disp->NROWS - 2 );
        return;
    }

    /* Start at the top of the page */
    LINE *tmp = disp->currMsg->pageTop;

    /* Rewind nScroll lines */
    for ( int i = 1; i < disp->NROWS - 2 && tmp->prev; tmp = tmp->prev, i++ )
        ;

    wclear( wins[MID] );
    disp->currMsg->pageTop = tmp;

    /* Print the lines to the screen */
    for ( int i = 0; i < disp->NROWS - 2 && tmp->next; tmp = tmp->next, i++ )
        mvwprintw( wins[MID], i, 0, "%s", tmp->text );

    /* Update the pageBot pointer */
    disp->currMsg->pageBot = tmp;
    disp->currMsg->currentLine = tmp;
    wrefresh( win );
}

/* Scrolls the page down */
void scrollDownPage( DISPLAY_DATA *disp, WINDOW *win ) {

    /* If the entire message will fit on the screen without scrolling, return */
    if ( disp->currMsg->numLines <= disp->NROWS - 2 ) {
        return;
    }

    /* If we are already at the bottom just return */
    if ( disp->currMsg->currentLine == disp->currMsg->last ) {
        return;
    }

    /* Start at the bottom of the page */
    LINE *tmp = disp->currMsg->pageBot;

    /* Update the pageTop pointer */
    disp->currMsg->pageTop = tmp;

    wclear( wins[MID] );

    /* Print nScroll lines to the screen */
    for ( int i = 0; i < ( disp->NROWS - 2 ) && tmp->next;
            tmp = tmp->next, i++ ) {
        mvwprintw( wins[MID], i, 0, "%s", tmp->text );

        /* Update the pointers */
        disp->currMsg->pageBot = tmp;
        disp->currMsg->currentLine = tmp;
    }

    wrefresh( win );
}

/* Setup and print the middle window to screen */
void showMidWin( DISPLAY_DATA *disp ) {
    wclear( wins[MID] );

    if ( disp->currMsg->messageNum == 0
            || disp->currMsg->root->totalMessages < 1 ) {
        char str[30];
        sprintf( str, "You had %d stored notes.",
                disp->currMsg->root->totalMessages );
        mvwprintw( wins[MID], 0, ( disp->NCOLS / 2 ) - ( strlen( str ) / 2 ),
                str );
        wrefresh( wins[MID] );
        return;
    }

    /* Position the cursor at the start of the message */
    wmove( wins[MID], 0, 0 );

    /* Print the message to the screen */
    printPage( disp, disp->NROWS - 2 );
}

/* Setup and print the bottom window to screen */
void showBotWin() {
    wclear( wins[BOT] );
    wattron( wins[BOT], COLOR_PAIR(2) );
    wbkgd( wins[BOT], COLOR_PAIR(2) );
    wrefresh( wins[BOT] );
}

/* Show the windows */
void showWins( DISPLAY_DATA *disp ) {
    getScrnSize( disp );
    showTopWin( disp );
    showBotWin();
    showMidWin( disp );
}

/* Initialize the menu but don't show it */
void initMainMenu() {
    /* Create items */
    int nItems;
    nItems = ARRAY_SIZE( mainMenuStrings );
    mainMenuItems = ( ITEM ** ) calloc( nItems, sizeof(ITEM *) );
    for ( int i = 0; i < nItems; ++i )
        mainMenuItems[i] = new_item( mainMenuStrings[i], mainMenuStrings[i] );

    /* Create menu */
    footerMenu = new_menu( ( ITEM ** ) mainMenuItems );
}

/* Setup and show the menu */
void setMainMenu( DISPLAY_DATA *disp ) {
    /* Set menu option not to show the description */
    menu_opts_off( footerMenu, O_SHOWDESC );

    int nItems = ARRAY_SIZE(mainMenuStrings);

    /* Set main window and sub window */
    set_menu_win( footerMenu, wins[BOT] );
    set_menu_sub( footerMenu, derwin( wins[BOT], 1, disp->NCOLS, 1, 1 ) );
    set_menu_format( footerMenu, 0, nItems );

    /* Get key events from the bottom window */
    keypad( wins[BOT], TRUE );

    /* Post the menu */
    post_menu( footerMenu );
    wrefresh( wins[BOT] );
}

/* Show the menu along the bottom of the screen */
void showMainMenu( DISPLAY_DATA *disp ) {
    getScrnSize( disp );
    showBotWin();
    setMainMenu( disp );
}

/* Hide the menu at the bottom of the screen */
void hideMainMenu() {
    unpost_menu( footerMenu );
    wrefresh( wins[BOT] );
}

/* Resize the terminal screen */
void resizeScreen( DISPLAY_DATA *disp ) {
    getScrnSize( disp );
    resizeterm( disp->NROWS, disp->NCOLS );
}

/* Sets the RECIEVED_SIGWINCH flag to true so we can deal with resizing the screen */
void hndSIGWINCH( int sig ) {
    if ( sig == SIGWINCH ) {
        RECIEVED_SIGWINCH = true;
    }
}

/* Set up the SIGWINCH handler */
void initSigaction() {
    memset( &sa, '\0', sizeof( sa ) );
    sa.sa_handler = hndSIGWINCH;
}

/* Prints the help message */
void printHelp() {
    wclear(wins[MID]);
    wprintw( wins[MID], "\nTerminote Help:\n\n"
            " Movement:\n"
            " <a> selects previous note\n"
            " <d> selects the next note\n"
            " <w> jumps to the top of the page\n"
            " <e> jumps to the bottom of the page\n"
            " <q> quits the program\n"
            " Arrows keys scroll and move the cursor\n\n"
            " Cntrl-F opens the menu");
    wrefresh(wins[MID]);
}

/* Free all memory and quit */
void quit( MESSAGE *msg ) {
    int n = ARRAY_SIZE(mainMenuStrings);
    for ( int i = 0; i < n; ++i )
        free_item( mainMenuItems[i] );
    free_menu( footerMenu );
    endwin();
    list_save( msg );
    list_destroy( &msg );
    exit( 0 );
}

void insertNoteFromEditor(DISPLAY_DATA *disp) {
    // get the user's editor
    char *editor = getenv( "EDITOR" );
    if ( editor == NULL ) {
        // default to nano
        editor = "/bin/nano";
    }
    // close ncurses
    endwin();

    // fork exec the editor
    pid_t pid=fork();

    if ( pid == 0 ) {
        execl( editor, editor, ".terminote_temp", NULL);
        exit( 127 );
    } else {
        waitpid( pid, 0, 0 );
    }
    // read the new message in
    FILE *f = fopen(".terminote_temp", "r");
    char tempc;
    int buffSize = 10, i = 0;
    char *input = malloc(buffSize * sizeof(char));

    while (fscanf( f, "%c", &tempc ) != EOF) {
        if ( i >= buffSize ) {
            buffSize *= 2;
            input = realloc(input, buffSize * sizeof(char));
        }
        input[i] = tempc;
        i++;
    }

    input[i] = '\0';

    fclose(f);
    remove(".terminote_temp");
    // append it to the list
    list_appendMessage(disp->currMsg, input);

    free(input);
}

/* Select and execute options from the menu */
void doMenu( DISPLAY_DATA *disp ) {
    /* Show the menu along the bottom of the screen */
    showBotWin();
    showMainMenu( disp );
    wnoutrefresh( wins[BOT] );
    doupdate();

    int ch;
    ITEM *currItem;
    bool keepGoing = true;
    while ( keepGoing ) {
        ch = wgetch( wins[BOT] );
        switch ( ch ) {
        case KEY_LEFT:
            menu_driver( footerMenu, REQ_PREV_ITEM );
            break;

        case KEY_RIGHT:
            menu_driver( footerMenu, REQ_NEXT_ITEM );
            break;

        case 13: /* Enter */
            currItem = current_item( footerMenu );
            if ( !strcmp( item_name( currItem ), "Quit" ) ) {
                quit( disp->currMsg );
            } else if ( !strcmp( item_name( currItem ), "Browse" ) ) {
                hideMainMenu();
                list_firstNode( &disp->currMsg );
                showWins( disp );
                keepGoing = false;
                break;
            } else if ( !strcmp( item_name( currItem ), "Help" ) ) {
                hideMainMenu();
                printHelp();
                keepGoing = false;
                break;
            } else if ( !strcmp( item_name( currItem ), "Delete" ) ) {
                list_deleteNode( disp->currMsg, disp->currMsg->messageNum );
                disp->currMsg->root->hasChanged = true;
                list_firstNode( &disp->currMsg );
                hideMainMenu();
                showWins( disp );
                keepGoing = false;
            } else if ( !strcmp( item_name( currItem ), "Add" ) ) {
                insertNoteFromEditor(disp);
                disp->currMsg->root->hasChanged = true; 
                hideMainMenu();
                showWins( disp );
                keepGoing = false;
            }

            keepGoing = false;
            break;

        default:
            /* Hide the menu and break out of the loop */
            hideMainMenu();
            keepGoing = false;
            return;
            break;
        }
    }
}

void clearPosition( DISPLAY_DATA *disp ) {
    disp->NCOLS = 0;
    disp->NROWS = 0;
    disp->cursorRow = 0;
    disp->cursorCol = 0;
}

/* run main GUI loop */
void run() {

    /* Load the list */
    MESSAGE *msg = NULL;
    list_init( &msg );
    list_load( msg );

    /* Set up the POSITION struct */
    DISPLAY_DATA *disp = NULL;
    initPosition( &disp );
    disp->currMsg = msg;

    /* Set up Ncurses */
    initNcurses();
    initMainMenu();
    initSigaction();
    initWins( disp );

    /* Print everything to the screen */
    showWins( disp );

    disp->currMsg->currentLine = msg->first;

    int ch;
    while ( ( ch = wgetch( wins[MID] ) ) ) {

        sigaction( SIGWINCH, &sa, NULL );

        if ( RECIEVED_SIGWINCH ) {
            resizeScreen( disp );
            initWins( disp );
            showWins( disp );
            RECIEVED_SIGWINCH = false;
        }

        switch ( ch ) {

        /* Change to next note in list struct */
        case 'd':
            clearPosition( disp );
            list_next( &disp->currMsg );
            disp->currMsg->currentLine = disp->currMsg->first;
            needsRefresh = true;
            break;

            /* Change to previous note in list struct */
        case 'a':
            clearPosition( disp );
            list_previous( &disp->currMsg );
            disp->currMsg->currentLine = disp->currMsg->first;
            needsRefresh = true;
            break;

            /* Show the menu along the bottom of the screen */
        case 6:
            doMenu( disp );
            break;

            /* Scroll up in the message */
        case KEY_UP:

            /* If we are in the root node, break to avoid a segfault */
            if ( disp->currMsg->messageNum == 0 )
                break;

            if ( disp->cursorRow > 0 ) {
                disp->cursorRow--;
                wmove( wins[MID], disp->cursorRow, disp->cursorCol );
                wrefresh( wins[MID] );
                break;
            }

            wins[MID] = newwin( disp->NROWS - 2, disp->NCOLS, 1, 0 );
            keypad( wins[MID], true );
            scrollUpPage( disp, wins[MID] );
            disp->cursorRow = ( disp->NROWS / 2 ) - 1;
            wmove( wins[MID], disp->cursorRow, disp->cursorCol );
            wrefresh( wins[MID] );
            break;

            /* Scroll down in the message */
        case KEY_DOWN:

            /* If we are in the root node, break to avoid a segfault */
            if ( disp->currMsg->messageNum == 0 )
                break;

            if ( disp->cursorRow <= disp->NROWS - 3 ) {
                disp->cursorRow++;
                wmove( wins[MID], disp->cursorRow, disp->cursorCol );
                wrefresh( wins[MID] );
                break;
            } else {
                disp->cursorRow = ( disp->NROWS / 2 ) - 1;
                scrollDownPage( disp, wins[MID] );
                wmove( wins[MID], disp->cursorRow, disp->cursorCol );
                break;
            }
            break;

        case KEY_LEFT:
            if ( disp->cursorCol <= 0 ) {
                disp->cursorRow--;
                disp->cursorCol = disp->NCOLS;
            } else {
                disp->cursorCol--;
            }
            wmove( wins[MID], disp->cursorRow, disp->cursorCol );
            wrefresh( wins[MID] );
            keypad( wins[MID], true );
            break;

        case KEY_RIGHT:
            if ( disp->cursorCol >= disp->NCOLS ) {
                disp->cursorRow++;
                disp->cursorCol = 0;
            } else {
                disp->cursorCol++;
            }
            wmove( wins[MID], disp->cursorRow, disp->cursorCol );
            wrefresh( wins[MID] );
            keypad( wins[MID], true );
            break;

        case 'w':
            /* If we are in the root node, break to avoid a segfault */
            if ( disp->currMsg->messageNum == 0 )
                break;

            disp->cursorCol = 0;
            disp->cursorRow = 0;
            printTop( disp );
            wmove( wins[MID], disp->cursorRow, disp->cursorCol );
            wrefresh( wins[MID] );
            break;

        case 'e':
            /* If we are in the root node, break to avoid a segfault */
            if ( disp->currMsg->messageNum == 0 )
                break;

            disp->cursorRow = disp->NROWS - 3;
            printBot( disp );
            wrefresh( wins[MID] );
            break;

            /* Free all memory and exit */
        case 'q':
            quit( msg );
            break;

        default:
            break;
        }

        if ( needsRefresh ) {
            showWins( disp );
            wmove( wins[MID], disp->cursorRow, disp->cursorCol );
            wrefresh( wins[MID] );
            needsRefresh = false;
        }

    }

}

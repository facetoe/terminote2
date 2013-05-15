/*
 ============================================================================
 Name        : listRewrite.c
 Author      : facetoe
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#define _POSIX_SOURCE // You need this here to get rid of the waring with sigaction
#include <getopt.h>
#include "helperFunctions.h"
#include "linkedList.h"
#include "options.h"
#include "defines.h"
#include "nonInteractive.h"


#include "ncurses.h"
#include "menu.h"

#include <sys/ioctl.h> // For terminal size
#include <signal.h> // For sigaction
char *path = "/home/facetoe/.terminote.data";

OPTIONS *opts = NULL;

#define SIGWINCH 28 // Needed because OSX doesn't recognize SIGWINCH
struct sigaction sa;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct winsize wSize; // I'm using this instead of getmaxyx() because it didn't seem to always work.

/* Terminal coordinates and scrollable message variables */
int NCOLS, NROWS;
static int nlines = 0;
static int cursorRow = 0;
static int cursorCol = 0;

bool RECIEVED_SIGWINCH = false;

bool inScrollMessage = false;
bool needsRefresh = false;

/* Variables for ncurses */
char *mainMenuStrings[] = { "New", "Browse", "Edit", "Delete", "Search", "Quit",
        "Help", ( char * ) NULL, };

static ITEM **mainMenuItems;
static MENU *footerMenu;
static WINDOW *wins[3];

/* Enum for different windows */
enum {
    TOP, MID, BOT
};

/* Get the size of the terminal screen */
void getScrnSize() {
    ioctl( 0, TIOCGWINSZ, &wSize );
    NCOLS = wSize.ws_col;
    NROWS = wSize.ws_row;
}

/* Setup ncurses */
void ui_initNcurses() {
    initscr();
    raw();
    nonl();
    noecho();
    cbreak();
    keypad( stdscr, TRUE );
    start_color();
    init_pair( 1, COLOR_BLACK, COLOR_WHITE );
}

void ui_initWins() {
    getScrnSize();
    wins[TOP] = newwin( 1, NCOLS, 0, 0 );
    wins[MID] = newwin( NROWS - 2, NCOLS, 1, 0 );
    wins[BOT] = newwin( 1, NCOLS, NROWS - 1, 0 );

    /* Turn colors on */
    wattron( wins[TOP], COLOR_PAIR(1) );
    wbkgd( wins[TOP], COLOR_PAIR(1) );

    keypad( wins[MID], true );
}

/* Setup and print the top window to screen */
void ui_showTopWin( MESSAGE *msg ) {

    wclear( wins[TOP] );

    /* If the we are not in the root node then update the top window with time, number and path info*/
    if ( msg->root->totalMessages > 0 ) {
        char noteStr[100];
        snprintf( noteStr, 100, "Note #%d", msg->messageNum );
        mvwprintw( wins[TOP], 0, ( NCOLS / 2 ) - ( strlen( noteStr ) / 2 ),
                noteStr );
        mvwprintw( wins[TOP], 0, 0, msg->path );
        mvwprintw( wins[TOP], 0, ( NCOLS - strlen( msg->time ) ) - 2,
                msg->time );
    } else {
        /* Otherwise just print the title and version */
        char title[50];
        sprintf( title, "Terminote %.1f", VERSION );
        mvwprintw( wins[TOP], 0, ( NCOLS / 2 ) - ( strlen( title ) / 2 ),
                title );
    }
    wrefresh( wins[TOP] );
}

void ui_printPage( MESSAGE *msg, int numRows ) {
    LINE *tmp = msg->pageTop;

    for ( int i = 0; i < numRows && tmp->next; tmp = tmp->next, i++ ) {
        mvwprintw( wins[MID], i, 0, tmp->currLine );
    }
    msg->pageBot = tmp;
    wrefresh( wins[MID] );
}

/* Setup and print the middle window to screen */
void ui_showMidWin( MESSAGE *msg ) {
    wclear( wins[MID] );

    if ( msg->messageNum == 0 || msg->root->totalMessages < 1 ) {
        char str[30];
        sprintf( str, "You had %d stored notes.", msg->root->totalMessages );
        mvwprintw( wins[MID], 0, ( NCOLS / 2 ) - ( strlen( str ) / 2 ), str );
        wrefresh( wins[MID] );
        return;
    } else if( msg->root->totalMessages > 0 ) {
        /* Print the message to the screen */
        ui_printPage( msg, NROWS - 2 );
    } else

    /* Position the cursor at the start of the message */
    wmove( wins[MID], 0, 0 );
}

/* Setup and print the bottom window to screen */
void ui_showBotWin() {
    wclear( wins[BOT] );
    wattron( wins[BOT], COLOR_PAIR(2) );
    wbkgd( wins[BOT], COLOR_PAIR(2) );
    wrefresh( wins[BOT] );
}

/* Show the windows */
void ui_showWins( MESSAGE *msg ) {
    getScrnSize( NCOLS, NROWS );
    ui_showTopWin( msg );
    ui_showBotWin();
    ui_showMidWin( msg );
}

/* Initialize the menu but don't show it */
void ui_initMainMenu() {
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
void ui_setMainMenu() {
    /* Set menu option not to show the description */
    menu_opts_off( footerMenu, O_SHOWDESC );

    int nItems = ARRAY_SIZE(mainMenuStrings);

    /* Set main window and sub window */
    set_menu_win( footerMenu, wins[BOT] );
    set_menu_sub( footerMenu, derwin( wins[BOT], 1, NCOLS, 1, 1 ) );
    set_menu_format( footerMenu, 0, nItems );

    /* Get key events from the bottom window */
    keypad( wins[BOT], TRUE );

    /* Post the menu */
    post_menu( footerMenu );
    wrefresh( wins[BOT] );
}

/* Show the menu along the bottom of the screen */
void ui_showMainMenu() {
    getScrnSize( NCOLS, NROWS );
    ui_showBotWin();
    ui_setMainMenu();
}

/* Hide the menu at the bottom of the screen */
void ui_hideMainMenu() {
    unpost_menu( footerMenu );
    wrefresh( wins[BOT] );
}

/* Resize the terminal screen */
void ui_resizeScreen() {
    getScrnSize();
    resizeterm( NROWS, NCOLS );
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

/* Scrolls the page up nScroll lines */
void lineData_scrollUpPage( MESSAGE *msg, WINDOW *win, int nScroll ) {

    /* If we are already at the top then print the page and return */
    if ( msg->first->lNum >= msg->pageTop->lNum ) {
        ui_printPage( msg, nScroll );
        return;
    }

    /* Start at the top of the page */
    LINE *tmp = msg->pageTop;

    /* Rewind nScroll lines */
    for ( int i = 1; i < nScroll && tmp->prev; tmp = tmp->prev, i++ )
        ;

    /* Print the lines to the screen */
    for ( int i = 0; i < nScroll && tmp->next; tmp = tmp->next, i++ )
        mvwprintw( wins[MID], i, 0, tmp->currLine );

    /* Update the pageBot pointer */
    msg->pageBot = tmp;

    /* Rewind again so we can set the pageTop pointer to the right place */
    for ( int i = 1; i < nScroll && tmp->prev; tmp = tmp->prev, i++ )
        ;

    msg->pageTop = tmp;
    wrefresh( win );
}

/* Scrolls the page down nScroll lines */
void lineData_scrollDownPage( MESSAGE *msg, WINDOW *win, int nScroll ) {

    /* If the entire message will fit on the screen without scrolling, print it */
    if ( msg->numLines <= nScroll ) {
        ui_printPage( msg, nScroll );
        return;
    }

    if ( !msg->pageBot ) {
        msg->pageBot = msg->pageTop + nScroll;
    }

    /* Start at the bottom of the page */
    LINE *tmp = msg->pageBot;

    /* Update the pageTop pointer */
    msg->pageTop = tmp;

    /* Print nScroll lines to the screen */
    for ( int i = 0; i < nScroll && tmp->next; tmp = tmp->next, i++ ) {
        mvwprintw( wins[MID], i, 0, tmp->currLine );
    }

    /* If we are not going to scroll off the page on the next call, update the pageBot pointer */
    if ( msg->pageBot->lNum + nScroll <= msg->numLines )
        msg->pageBot = tmp;

    wrefresh( win );
}

/* Free all memory and quit */
void ui_quit( MESSAGE *msg ) {
    int n = ARRAY_SIZE(mainMenuStrings);
    for ( int i = 0; i < n; ++i )
        free_item( mainMenuItems[i] );
    free_menu( footerMenu );
    endwin();
    list_save( msg );
    list_destroy( &msg );
    exit( 0 );
}

/* Select and execute options from the menu */
void ui_doMenu( MESSAGE *msg ) {
    /* Show the menu along the bottom of the screen */
    ui_showBotWin();
    ui_showMainMenu();
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
                ui_quit( msg );
            } else if ( !strcmp( item_name( currItem ), "Browse" ) ) {
                ui_hideMainMenu();
                list_firstNode( &msg );
                ui_showWins( msg );
                keepGoing = false;
                break;
            } else if ( !strcmp( item_name( currItem ), "Help" ) ) {
                keepGoing = false;
                break;
            } else if ( !strcmp( item_name( currItem ), "Delete" ) ) {
                list_deleteNode( msg, msg->messageNum );
                list_firstNode( &msg );
                ui_hideMainMenu();
                ui_showWins( msg );
                keepGoing = false;
            }

            keepGoing = false;
            break;

        default:
            /* Hide the menu and break out of the loop */
            ui_hideMainMenu();
            keepGoing = false;
            return;
            break;
        }
    }
}

/* run main GUI loop */
void guiLoop( MESSAGE *msg ) {
    unpost_menu( footerMenu );
    LINE *currLine;
    ui_showWins( msg );
    int ch;
    keypad( wins[MID], true );
    while ( ( ch = wgetch( wins[MID] ) ) ) {

        sigaction( SIGWINCH, &sa, NULL );

        if ( RECIEVED_SIGWINCH ) {
            ui_resizeScreen();
            ui_initWins();
            ui_showWins( msg );
            RECIEVED_SIGWINCH = false;
        }

        switch ( ch ) {

        /* Change to next note in list struct */
        case 'd':
            cursorRow = 0;
            cursorCol = 0;
            nlines = 0;
            if( msg->root->totalMessages > 0 )
                list_next( &msg );

            endwin();
            printf("%d\n", msg->root->totalMessages);
            inScrollMessage = false;
            needsRefresh = true;
            break;

            /* Change to previous note in list struct */
        case 'a':
            cursorRow = 0;
            cursorCol = 0;
            nlines = 0;
            inScrollMessage = false;
            if( msg->root->totalMessages > 0 )
                list_previous( &msg );
            needsRefresh = true;
            break;

            /* Show the menu along the bottom of the screen */
        case 6:
            ui_doMenu( msg );
            break;

            /* Scroll up in the message */
        case KEY_UP:

            /* If we are in the root node, break to avoid a segfault */
            if ( msg->messageNum == 0 )
                break;

            if ( cursorRow > 0 ) {
                cursorRow--;
                wmove( wins[MID], cursorRow, cursorCol );
                wrefresh( wins[MID] );
                break;
            }

            wins[MID] = newwin( NROWS - 2, NCOLS, 1, 0 );
            keypad( wins[MID], true );
            lineData_scrollUpPage( msg, wins[MID], NROWS - 2 );
            cursorRow = ( NROWS / 2 ) - 1;
            wmove( wins[MID], cursorRow, cursorCol );
            wrefresh( wins[MID] );
            break;

            /* Scroll down in the message */
        case KEY_DOWN:

            /* If we are in the root node, break to avoid a segfault */
            if ( msg->messageNum == 0 )
                break;

            if ( cursorRow <= NROWS - 3 ) {
                cursorRow++;
                wmove( wins[MID], cursorRow, cursorCol );
                wrefresh( wins[MID] );
                break;
            } else {
                cursorRow = ( NROWS / 2 ) - 1;
                keypad( wins[MID], true );
                werase( wins[MID] );
                lineData_scrollDownPage( msg, wins[MID], NROWS - 2 );
                wmove( wins[MID], cursorRow, cursorCol );
                wrefresh( wins[MID] );
                break;
            }
            break;

        case KEY_LEFT:
            if ( cursorCol <= 0 ) {
                cursorRow--;
                cursorCol = NCOLS;
            } else {
                cursorCol--;
            }
            wmove( wins[MID], cursorRow, cursorCol );
            wrefresh( wins[MID] );
            keypad( wins[MID], true );
            break;

        case KEY_RIGHT:
            if ( cursorCol >= NCOLS ) {
                cursorRow++;
                cursorCol = 0;
            } else {
                cursorCol++;
            }
            wmove( wins[MID], cursorRow, cursorCol );
            wrefresh( wins[MID] );
            keypad( wins[MID], true );
            break;

            /* Free all memory and exit */
        case 'q':
            ui_quit( msg );
            break;

        default:
            break;
        }

        if ( needsRefresh ) {
            ui_showWins( msg );
            wmove( wins[MID], cursorRow, cursorCol );
            wrefresh( wins[MID] );
            keypad( wins[MID], true );
            needsRefresh = false;
        }

    }

}

/* Run interactive mode */
void ui_run( MESSAGE *msg ) {
    list_init( &msg );
    list_load( msg );
    ui_initNcurses();
    ui_initMainMenu();
    initSigaction();
    ui_initWins();
    guiLoop( msg );
}



MESSAGE *msg;


int main( int argc, char **argv ) {

    if ( isatty( STDIN_FILENO ) && argc == 1 ) {
        ui_run( msg );
    } else {
        opts = options_new();
        nonInteractive_run( opts, argc, argv );
        free( opts );
    }

    return EXIT_SUCCESS;
}

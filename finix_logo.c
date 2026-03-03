#include "finix.h"

// Clear screen helper
void clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

// For the "Welcome To" typing effect
void type_out(const char *text, int delay_us) {
    for (const char *p = text; *p != '\0'; p++) {
        putchar(*p);
        fflush(stdout);
        usleep(delay_us);
    }
}

// Helper function for FINIX logo to fade it in/out
void fade_in_text(const char *text, int delay_us) {
    for (int i = 0; i <= 9; i++) {
        printf("\033[2J\033[H");
        printf("\033[38;5;%dm%s\033[0m\n", 232 + i, text);
        fflush(stdout);
        usleep(delay_us);
    }
}

void fade_out_text(const char *text, int delay_us) {
    for (int i = 9; i >= 0; i--) {
        printf("\033[2J\033[H");
        printf("\033[38;5;%dm%s\033[0m\n", 232 + i, text);
        fflush(stdout);
        usleep(delay_us);
    }
}

void animate_finix() {
    // Fish ASCII art I created. Took an hour after following youtube video :)
    clear_screen();
    printf("\n\n");
    printf("\033[36m");
    printf("       o                 o\n");
    printf("                  o\n");
    printf("         o   ______      o\n");
    printf("           _/  (   \\_ \n");
    printf(" _       _/  (       \\_  O\n");
    printf("| \\_   _/  (   (    0  \\\n");
    printf("|== \\_/  (   (          |\n");
    printf("|=== _ (   (   (        |\n");
    printf("|==_/ \\_ (   (          |\n");
    printf("|_/     \\_ (   (    \\__/\n");
    printf("          \\_ (      _/\n");
    printf("            |  |___/\n");
    printf("           /__/ \n");
    printf("\033[0m\n\n");

    // This is my favorite part. Type "Welcome To" as if someone is typing!!!
    printf("\033[1m\033[38;5;123m                   ");
    type_out("Welcome To", 100000);  
    printf("\033[0m\n");
    fflush(stdout);

    // Keep fish on screen for a bit to give authentic vibe
    usleep(2000000); 

    // We need to get screen ready for finix logo
    clear_screen();

    const char *finix_block =
"                 ███████╗██╗███╗  ██╗██╗██╗  ██╗\n"
"                 ██╔════╝██║████╗ ██║██║╚██╗██╔╝\n"
"                 █████╗  ██║██╔██╗██║██║ ╚███╔╝ \n"
"                 ██╔══╝  ██║██║╚████║██║ ██╔██╗ \n"
"                 ██║     ██║██║ ╚███║██║██╔╝ ██╗\n"
"                 ╚═╝     ╚═╝╚═╝  ╚══╝╚═╝╚═╝  ╚═╝\n\n"
"  ||   |||    | |    ||    |||   | |   ||||     | |\n"
"   ||  | |    |||    |||   | |   ||     |||    |||\n";

    // Fade FINIX in
    fade_in_text(finix_block, 80000);

    // Stay on screen 3 seconds
    usleep(3000000);

    // Fade FINIX out
    fade_out_text(finix_block, 80000);

    // Clear everything to prepare for interactive/batch terminal use
    clear_screen();
}

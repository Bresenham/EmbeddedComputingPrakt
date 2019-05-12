#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>


/*
pid_t spawn( const char * path,
             int fd_count, 
             const int fd_map[ ], 
             const struct inheritance * inherit, 
             char * const argv[ ], 
             char * const envp[ ] );
*/

void spawnTaskTwo() {
    /* Ich vermute, dass ist einfach stumpf der Pfad zum Code, der ausgefuehrt werden soll */
    const char* task_one_path = "Task2src.c";
    /* Anzahl an Filedeskriptoren fuer den Child-Prozess.
       0 bedeutet, dass alle vererbt werden.
       Macht Sinn meiner Meinung nach, anscheinend werden stdin, stdout und stderr dadurch gesetzt.
       Wir wollen auch die Ausgaben der Tasks auf der Konsole sehen (d.h. stdout nicht aendern).
     */
    const int fd_count = 0;
    /* Diese Struktur gibt an, was der Child-Prozess vererbt bekommen soll. Keine Ahnung bis jetzt.
       Sieht allerdings so aus, als könnte man die auch leer lassen ("Zero or more of the following bits")
    */
    struct inheritance inherit;
    /* Wuesste nicht, welche Argumente oder Umgebungsvariablen wir hier uebergeben sollten.
       Liefert die PID des neuen Tasks oder -1 wenn ein Fehler aufgetreten ist (mit errno).
     */
    const pid_t task_pid = spawn(task_one_path, fd_count, NULL, &inherit, NULL, NULL);
    if(task_pid == -1) {
        if(errno != EOK) {
            printf("Error, couldn't spawn task: %s\r\n", strerror(errno));
            exit(-1);
	    }
    }
}

void spawnTaskOne() {
    /* Ich vermute, dass ist einfach stumpf der Pfad zum Code, der ausgefuehrt werden soll */
    const char* task_one_path = "Task1src.c";
    /* Anzahl an Filedeskriptoren fuer den Child-Prozess.
       0 bedeutet, dass alle vererbt werden.
       Macht Sinn meiner Meinung nach, anscheinend werden stdin, stdout und stderr dadurch gesetzt.
       Wir wollen auch die Ausgaben der Tasks auf der Konsole sehen (d.h. stdout nicht aendern).
     */
    const int fd_count = 0;
    /* Diese Struktur gibt an, was der Child-Prozess vererbt bekommen soll. Keine Ahnung bis jetzt.
       Sieht allerdings so aus, als könnte man die auch leer lassen ("Zero or more of the following bits")
    */
    struct inheritance inherit;
    /* Wuesste nicht, welche Argumente oder Umgebungsvariablen wir hier uebergeben sollten.
       Liefert die PID des neuen Tasks oder -1 wenn ein Fehler aufgetreten ist (mit errno).
     */
    const pid_t task_pid = spawn(task_one_path, fd_count, NULL, &inherit, NULL, NULL);
    if(task_pid == -1) {
        if(errno != EOK) {
            printf("Error, couldn't spawn task: %s\r\n", strerror(errno));
            exit(-1);
	    }
    }
}

int main(int argc, char *argv[]) {
    spawnTaskOne();
    spawnTaskTwo();
	return EXIT_SUCCESS;
}
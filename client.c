/* cliTCPIt.c - Exemplu de client TCP
   Trimite un numar la server; primeste de la server numarul incrementat.
         
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

void afisare_meniu_basic() {
    printf("* Pentru adaugarea unei melodii la top introduceti `1`\n");       fflush(stdout);
    printf("* Pentru votarea unei melodii introduceti `2`\n");                fflush(stdout);
    printf("* Pentru a adauga un comentariu la o melodie introduceti `3`\n"); fflush(stdout);
    printf("* Pentru afisarea topului general introduceti `4`\n");            fflush(stdout);
    printf("* Pentru afisarea topului pe genuri introduceti `5`\n");          fflush(stdout);
}

void afisare_meniu_admin() {
    afisare_meniu_basic();
    printf("* Pentru stergerea unei melodii introduceti `6`\n");                         fflush(stdout);
    printf("* Pentru restrictionarea la vot a unui utilizator introduceti `7`\n");       fflush(stdout);
    printf("* Pentru restrictionarea de a comenta a unui utilizator introduceti `8`\n"); fflush(stdout);
}

void trimite_melodie_la_server(int sd) {
    char date_melodie[6][512];

    fgetc(stdin);
    printf("Introduceti titlul melodiei: "); 
    fgets(date_melodie[0], 128, stdin);
    date_melodie[0][strlen(date_melodie[0]) - 1] = '\0'; // elimin de la final \n (fgets adauga \n la final) 

    printf("Introduceti autorul melodiei: ");
    fgets(date_melodie[1], 128, stdin);
    date_melodie[1][strlen(date_melodie[1]) - 1] = '\0';

    printf("Introduceti anul aparitiei melodiei: ");
    fgets(date_melodie[2], 6, stdin);
    date_melodie[2][strlen(date_melodie[2]) - 1] = '\0';

    printf("Introduceti genul melodiei: ");
    fgets(date_melodie[3], 512, stdin); 
    date_melodie[3][strlen(date_melodie[3]) - 1] = '\0';

    printf("Introduceti o descriere a melodiei: ");
    fgets(date_melodie[4], 512, stdin);
    date_melodie[4][strlen(date_melodie[4]) - 1] = '\0';

    printf("Introduceti un url catre melodie: ");
    fgets(date_melodie[5], 128, stdin);
    date_melodie[5][strlen(date_melodie[5]) - 1] = '\0';

    // trimit melodia catre server
    if (write(sd, date_melodie, sizeof(date_melodie)) <= 0) {
        perror("Eroare la trimiterea melodiei catre server!");
    }
}

int main (int argc, char *argv[]) {
    int sd;			            // descriptorul de socket
    struct sockaddr_in server;	// structura folosita pentru conectare 
  	
    // mesajul trimis
    /* exista toate argumentele in linia de comanda? */
    if (argc != 3) {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi (argv[2]);

    /* cream socketul */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
  
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
  
    /* portul de conectare */
    server.sin_port = htons (port);
  
    /* ne conectam la server */
    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1) {
        perror ("[client]Eroare la connect().\n");
        return errno;
    }

    char nume_utilizator[64];
    char parola[64];

    int user_id;
    int admin_status;

    printf("Introduceti nume utilizator: ");
    scanf("%s", nume_utilizator);

    printf("Introduceti parola: ");
    scanf("%s", parola);

    // trimit numele de utilizator catre server
    if (write(sd, nume_utilizator, sizeof(nume_utilizator)) <= 0) {
		perror("Eroare la trimitere nume_utilizator catre server!\n");
	}

    // trimit parola catre server
    if (write(sd, parola, sizeof(parola)) <= 0) {
        perror("Eroare la trimitere parola catre server!\n");
    }

    // primesc user_id de la server
    if (read(sd, &user_id, sizeof(int)) <= 0) {
        perror("Eroare la primire user_id de la server!\n");
    }

    // primesc admin_status de la server
    if (read(sd, &admin_status, sizeof(int)) <= 0) {
        perror("Eroare primire admin_status de la client!\n");
    }

    if (user_id == -1 && admin_status == -1) {
        printf("Utilizator inexistent sau parola incorecta!\n");
    }
    else {
        int optiune;
        if (admin_status != 1) {
            afisare_meniu_basic();
        
            printf("Introduceti optiunea: ");
            fflush(stdout);

            scanf("%d", &optiune);
            while (optiune < 1 || optiune > 5) {
                printf("Optiune invalida! Incercati din nou: ");
                fflush(stdout);
                
                scanf("%d", &optiune);
            }        
        }
        else {
            afisare_meniu_admin();

            printf("Introduceti optiunea: ");
            fflush(stdout);

            scanf("%d", &optiune);
            while (optiune < 1 || optiune > 8) {
                printf("Optiune invalida! Incercati din nou: ");
                fflush(stdout);

                scanf("%d", &optiune);
            }

        }
        // trimit servarului optiunea aleasa
        if (write(sd, &optiune, sizeof(int)) <= 0) {
            perror("Eroare la trimiterea optiunii catre server!\n");
        }

        switch (optiune) {
            case 1:
                trimite_melodie_la_server(sd);
                break;
            default:
                printf("Optiune invalida!");
                break;
        }
    }

    /* inchidem conexiunea, am terminat */
    close (sd);
}

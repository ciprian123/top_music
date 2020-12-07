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
    printf("* Pentru adaugarea unei melodii la top introduceti `1`\n");
    printf("* Pentru votarea unei melodii introduceti `2`\n");
    printf("* Pentru afisarea topului general introduceti `3`\n");
    printf("* Pentru afisarea topului pe genuri introduceti `4`\n");
    printf("* Pentru adaugarea unui comentariu introduceti `5`\n");
}

void afisare_meniu_admin() {
    afisare_meniu_basic();
    printf("* Pentru stergerea unei melodii introduceti `6`\n");                  
    printf("* Pentru restrictionarea la vot a unui utilizator introduceti `7`\n"); 
    printf("* Pentru restrictionarea de a comenta a unui utilizator introduceti `8`\n");
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
        
        int nr_melodii;
        char lista_melodii[2048][128];

        int nr_genuri;
        char lista_genuri[128][128];

        int optiune_votare;
        int vote_status;
        int comment_status;
        char comentariu[2047];
        switch (optiune) {
            case 1:
                trimite_melodie_la_server(sd);
                break;
            case 2:
                // primesc statusul de vot al clientului
                if (read(sd, &vote_status, sizeof(int)) <= 0) {
                    perror("Eroare la primirea vote_status de la server!");
                }

                if (vote_status == 1) {
                    // primesc numarul de melodii de la server
                    if (read(sd, &nr_melodii, sizeof(int)) <= 0) {
                        perror("Eroare la primirea numarului de melodii de la server!");
                    }

                    // primesc melodiile de la server
                    if (read(sd, lista_melodii, sizeof(lista_melodii)) <= 0) {
                        perror("Eroare la primirea melodiilor de la server!");
                    }

                    for (int i = 0; i < nr_melodii; ++i) {
                        printf("%s\n", lista_melodii[i]);
                    }
                    printf("Introduceti id-ul melodiei votate: ");
                    scanf("%d", &optiune_votare);

                    // trimit la server id-ul melodiei pe care doresc sa o votez
                    if (write(sd, &optiune_votare, sizeof(int)) <= 0) {
                        perror("Eroare la trimiterea id-ului catre server");
                    }

                    printf("Melodia a fost votata cu succes!");
                } else {
                    printf("Nu aveti drepturi de vot! Contactati un admin!\n");
                }
                break;
            case 3:
                // primesc numarul de melodii de la server
                if (read(sd, &nr_melodii, sizeof(int)) <= 0) {
                    perror("Eroare la primirea numarului de melodii de la sever!");
                }

                // primesc melodiile topului general
                if (read(sd, &lista_melodii, sizeof(lista_melodii)) <= 0) {
                    perror("Eroare la primirea topului general de la server!");
                }

                printf("Topul general al melodiilor:\n");
                for (int i = 0; i < nr_melodii; ++i) {
                    printf("%s\n", lista_melodii[i]);
                }

                break;
            case 4:
                // primesc numarul de genuri de la server
                if (read(sd, &nr_genuri, sizeof(int)) <= 0) {
                    perror("Eroare la primirea numarului de genuri de la server!");
                }

                // primesc lista de genuri de la server
                if (read(sd, lista_genuri, sizeof(lista_genuri)) <= 0) {
                    perror("Eroare la primirea numarului de genuri de la server!");
                }

                printf("Genurile topului muzical:");
                for (int i = 0; i < nr_genuri; ++i) {
                    printf("%d. %s\n", i, lista_genuri[i]);
                }

                printf("Introduceti optiunea genului dorit: ");
                scanf("%d", &optiune_votare);

                while (optiune_votare < 0 || optiune_votare >= nr_genuri) {
                    printf("Optiune gresita! Incercati din nou: ");
                    scanf("%d", &optiune_votare);
                }

                // trimit serverului genul selectat pentru vizualizare la top
                if (write(sd, &optiune_votare, sizeof(int)) <= 0) {
                    perror("Eroare la trimiterea genului selectat catre server!");
                }

                // primesc numarul de melodii de la server
                if (read(sd, &nr_melodii, sizeof(int)) <= 0) {
                    perror("Eroare la primirea numarului de melodii de la server!");
                }

                // primesc melodiile topului pe gen de la server
                if (read(sd, lista_melodii, sizeof(lista_melodii)) <= 0) {
                    perror("Eroare la primirea topului de melodii pe gen de la server!");
                }

                printf("Topul melodiilor in genul %s:\n", lista_genuri[optiune_votare]);
                for (int i = 0; i < nr_melodii; ++i) {
                    printf("%s\n", lista_melodii[i]);
                }
                break;
            case 5:
                // primesc statusul de a comenta
                if (read(sd, &comment_status, sizeof(int)) <= 0) {
                    perror("Eroare la primirea statusului de a comenta de la server!");
                }

                // primesc numarul de melodii de la server
                if (read(sd, &nr_melodii, sizeof(int)) <= 0) {
                    perror("Eroarea la primirea numarului de melodii de la server!");
                }

                // primesc lista de melodii de la server
                if (read(sd, lista_melodii, sizeof(lista_melodii)) <= 0) {
                    perror("Eroare la primirea listei de melodii de la server!");
                }

                if (comment_status == 1) {
                    for (int i = 0; i < nr_melodii; ++i) {
                        printf("Id: %d     %s\n", i, lista_melodii[i]);
                    }
            
                    printf("Introduceti id-ul melodiei pentru care adaugati comentariu: ");
                    scanf("%d", &optiune_votare);

                    while (optiune_votare < 0 || optiune_votare >= nr_melodii) {
                        printf("Id incorect, incercati din nou: ");
                        scanf("%d", &optiune_votare);
                    }
                
                    nr_melodii = 0;
                    fgetc(stdin);
                    printf("Introduceti comentariul pentru piesa %d: ", optiune_votare);
                    fgets(comentariu, 2047, stdin);
                    comentariu[strlen(comentariu) - 1] = '\0';

                    // trimit la server id-ul melodiei votate
                    if (write(sd, &optiune_votare, sizeof(int)) <= 0) {
                        perror("Eroare la trimiterea id-ului piesei votate catre server!");
                    }

                    // trimit la server comentariul piesei
                    if (write(sd, comentariu, sizeof(comentariu)) <= 0) {
                        perror("Eroare la trimiterea comentariului catre server!");
                    }
                
                    printf("Comentariu inserat cu succes!\n");
                } else {
                    printf("Nu aveti dreptul de a comenta! Contactati un admin!");
                }
                break;
            default:
                printf("Optiune invalida!");
                break;
        }
    }

    /* inchidem conexiunea, am terminat */
    close (sd);
}

/* servTCPConcTh2.c - Exemplu de server TCP concurent care deserveste clientii
   prin crearea unui thread pentru fiecare client.
   Asteapta un numar de la clienti si intoarce clientilor numarul incrementat.
   Intoarce corect identificatorul din program al thread-ului.
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sqlite3.h>


/* portul folosit */
#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
} thData;


sqlite3* db;
int user_id = -1;
int admin_status = -1;

int melodii_de_votat_index = 0;
char melodii_de_votat[2048][128];

int top_melodii_index = 0;
char top_melodii[2048][128];

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

static int login_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    user_id = atoi(argv[0]);
    admin_status = atoi(argv[1]);
    return 0;
}

static int votare_melodie_callback(void *NotUsed, int argc, char** argv, char **azColName) {
    char formatare_piesa[128];
    strcpy(formatare_piesa, "Id: ");
    strcat(formatare_piesa, argv[0]);
    strcat(formatare_piesa, "     Title: ");
    strcat(formatare_piesa, argv[1]);
    strcat(formatare_piesa, "     No of votes: ");
    strcat(formatare_piesa, argv[2]);

    strcpy(melodii_de_votat[melodii_de_votat_index++],  formatare_piesa);
    return 0;
}

static int proceseaza_topul_general_callback(void *NotUsed, int argc, char** argv, char **azColName) {
    char formatare_piesa[128];
    strcpy(formatare_piesa, "Titlu: ");
    strcat(formatare_piesa, argv[0]);
    strcat(formatare_piesa, "    Nr voturi: ");
    strcat(formatare_piesa, argv[1]);

    strcpy(top_melodii[top_melodii_index++], formatare_piesa);
    return 0;
}

void autentificare_utilizator(sqlite3* db, char* nume_utilizator, char* parola) {
   char sql_query[512] = "SELECT user_id, admin_status FROM users WHERE username = '";
   char* mesaj_eroare;

   strcat(sql_query, nume_utilizator);
   strcat(sql_query, "'");
   strcat(sql_query, " AND password = '");
   strcat(sql_query, parola);
   strcat(sql_query, "'");

   int db_descriptor = sqlite3_exec(db, sql_query, login_callback, 0, &mesaj_eroare);
}

char* itoa(int number) {
    char* sir = (char*) malloc(8 * sizeof(char));
    int idx = 0;
    do {
        sir[idx++] = '0' + (number % 10);
        number /= 10;
    } while (number > 0);
    
    sir[idx] = '\0';
    for (int i = 0; i < idx / 2; ++i) {
        char tmp = sir[i];
        sir[i] = sir[idx - i - 1];
        sir[idx - i - 1] = tmp;
    }

    return (char*)sir;
}

void inserare_melodie(char date_melodie[6][512], int user_id) {
    char sql_query[512 * 7] = "INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes,      created_at) VALUES (";
    strcat(sql_query, itoa(user_id));
    strcat(sql_query, ", '");
    strcat(sql_query, date_melodie[0]);
    strcat(sql_query, "' ,'");
    strcat(sql_query, date_melodie[1]);
    strcat(sql_query, "' ,");
    strcat(sql_query, date_melodie[2]);
    strcat(sql_query, " ,'");
    strcat(sql_query, date_melodie[3]);
    strcat(sql_query, "' ,'");
    strcat(sql_query, date_melodie[4]);
    strcat(sql_query, "' ,'");
    strcat(sql_query, date_melodie[5]);
    strcat(sql_query, "', 0, date('now'))");
    
    //printf("%s", sql_query);
    //fflush(stdout);
    
    char* mesaj_eroare;
    int insert_status = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);

    if (insert_status != SQLITE_OK) {
        printf("Eroare la inserarea melodiei - %s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    } else {
        printf("Melodie inserata cu succes!");
        fflush(stdout);
    }
}

void marcare_votare_melodie(int user_id, int song_id) {
    char sql_query[128] = "UPDATE songs SET no_of_votes = no_of_votes + 1 WHERE song_id = ";
    char* mesaj_eroare;
    strcat(sql_query, itoa(song_id));
    printf("%s\n", sql_query);
    fflush(stdout);

    int status_interogare = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);
    if (status_interogare != SQLITE_OK) {
        printf("Eroare la actualizarea numarului de voturi - %s\n", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }

    strcpy(sql_query, "INSERT INTO votes (user_id, song_id, created_at) VALUES (");
    strcat(sql_query, itoa(user_id));
    strcat(sql_query, ", ");
    strcat(sql_query, itoa(song_id));
    strcat(sql_query, ", date('now'))");
    
    status_interogare = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);
    if (status_interogare != SQLITE_OK) {
        printf("Eroare la inserarea in tabelul votes - %s\n", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }

    printf("Melodie votata cu succes!");
    fflush(stdout);
}

void votare_melodie() {
    char sql_query[128] = "SELECT song_id, title, no_of_votes FROM songs";
    char* mesaj_eroare;

    int select_status = sqlite3_exec(db, sql_query, votare_melodie_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la selectarea melodiilor - %s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
}

void afisare_top_general() {
    char sql_query[128] = "SELECT title, no_of_votes FROM songs ORDER BY no_of_votes DESC";
    char* mesaj_eroare;

    int select_status = sqlite3_exec(db, sql_query, proceseaza_topul_general_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la selectarea melodiilor in topul general - %s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
}

int main () {
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;	
    int sd;		//descriptorul de socket 
    int pid;
    pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	int i = 0;

    int db_descriptor = sqlite3_open("top_music.db", &db); 
    if (db_descriptor) {
        perror("Eroare la conectarea cu baza de date!");
        return 0;
    }
  
    /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }

    /* utilizarea optiunii SO_REUSEADDR */
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  
    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));
  
    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
    /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1) {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 2) == -1) {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }

    /* servim in mod concurent clientii...folosind thread-uri */
    while (1) {
        int client;
        thData * td; //parametru functia executata de thread     
        int length = sizeof (from);

        printf ("[server]Asteptam la portul %d...\n",PORT);
        fflush (stdout);

        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0) {
	        perror ("[server]Eroare la accept().\n");
	        continue;
	    }
	
        /* s-a realizat conexiunea, se astepta mesajul */
    
	    // int idThread; //id-ul threadului
	    // int cl; //descriptorul intors de accept
        td=(struct thData*)malloc(sizeof(struct thData));	
        td->idThread=i++;
        td->cl=client;

        pthread_create(&th[i], NULL, &treat, td);	      
	}
};			

static void *treat(void * arg) {		
	struct thData tdL; 
	tdL= *((struct thData*)arg);	
	printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
	fflush (stdout);		 
	pthread_detach(pthread_self());		
	raspunde((struct thData*)arg);
	/* am terminat cu acest client, inchidem conexiunea */
	close ((intptr_t)arg);
	return(NULL);		
};

void raspunde(void *arg) {
    int nr, i = 0;
	struct thData tdL; 
	tdL= *((struct thData*)arg);

    char nume_utilizator[64];
    char parola[64];

    // citesc numele de utilizator de la client
    if (read(tdL.cl, nume_utilizator, sizeof(nume_utilizator)) <= 0) {
        perror("Eroare la primire nume_utilizator de la client!");
    }

    // citesc parola de la client
    if (read(tdL.cl, parola, sizeof(parola)) <= 0) {
        perror("Eroare la primire parola de la client!");
    }

    autentificare_utilizator(db, nume_utilizator, parola);
	
    // trimit user_id catre client
    if (write(tdL.cl, &user_id, sizeof(int)) <= 0) {
        perror("Eroare la trimitere user_id catre client!");
    }

    // trimit admin_statis catre client
    if (write(tdL.cl, &admin_status, sizeof(int)) <= 0) {
        perror("Eroare la trimitere admin_status catre client!");
    }
    
    // primesc optiunea alease de client
    int optiune;
    if (read(tdL.cl, &optiune, sizeof(int)) <= 0) {
        perror("Eroare la primirea optiunii de la client!");
    }

    printf("Optiunea este: %d\n", optiune);
    fflush(stdout);
    

    switch (optiune) {
        case 1:
            printf("Adaugarea unei melodii la top.\n");
            fflush(stdout);

            char date_melodie[6][512];
            // primesc de la client datele melodiei de introdus
            if (read(tdL.cl, date_melodie, sizeof(date_melodie)) <= 0) {
                perror("Eroare la primirea melodiei de la client!");
            }
            inserare_melodie(date_melodie, user_id);
            break;
        case 2:
            printf("Votarea unei melodii.\n");
            votare_melodie();

            // trimit la client numarul de melodii din array-ul de melodii disponibile
            if (write(tdL.cl, &melodii_de_votat_index, sizeof(int)) <= 0) {
                perror("Eroare la trimiterea numarului de melodii catre client!");
            }
            melodii_de_votat_index = 0;
            // trimit la client 'view-ul' de melodii pentru a fi votate
            if (write(tdL.cl, &melodii_de_votat, sizeof(melodii_de_votat)) <= 0) {
                perror("Eroare la trimiterea melodiilor de votat!");
            }

            // primesc id-ul melodiei votate 
            int optiune_votare;
            if (read(tdL.cl, &optiune_votare, sizeof(int)) <= 0) {
                perror("Eroare la primirea optiunii de votat!");
            }
            printf("Optiune votare: %d\n", optiune_votare);
            marcare_votare_melodie(user_id, optiune_votare);
            fflush(stdout);
            break;
        case 3:
            printf("Afisarea topului general.\n");

            afisare_top_general();

            // trimit la client numarul de melodii din topul general
            if (write(tdL.cl, &top_melodii_index, sizeof(int)) <= 0) {
                perror("Eroare la trimiterea numarul de melodii din top catre client!");
            }

            // trimit la client melodiile din topul general
            if (write(tdL.cl, top_melodii, sizeof(top_melodii)) <= 0) {
                perror("Eroare la trimiterea melodiilor din topul general!");
            }

            top_melodii_index = 0;
            for (int i = 0; i < top_melodii_index; ++i) {
                printf("%s\n", top_melodii[i]);
                fflush(stdout);
            }
            fflush(stdout);
            break;
        case 4:
            printf("Afisarea topului general.\n");
            fflush(stdout);
            break;
        case 5:
            printf("Afisarea topului pe genuri.\n");
            fflush(stdout);
            break;
        case 6:
            printf("Stergerea unei melodii.\n");
            fflush(stdout);
            break;
        case 7:
            printf("Restrictionarea la vot a unui utilizator.\n");
            fflush(stdout);
            break;
        case 8:
            printf("Restrictionarea de a comenta a unui utulizator.\n");
            fflush(stdout);
            break;
        default:
            printf("Optiune invalida!\n");
            fflush(stdout);
            break;
    }


    user_id = admin_status = -1; // resetam valorile atributelor
}

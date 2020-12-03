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


static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

static int login_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    user_id = atoi(argv[0]);
    admin_status = atoi(argv[1]);
    return 0;
}

int autentificare_utilizator(sqlite3* db, char* nume_utilizator, char* parola) {
   char sql_query[512] = "SELECT user_id, admin_status FROM users WHERE username = '";
   char* mesaj_eroare;

   strcat(sql_query, nume_utilizator);
   strcat(sql_query, "'");
   strcat(sql_query, " AND password = '");
   strcat(sql_query, parola);
   strcat(sql_query, "'");

   int db_descriptor = sqlite3_exec(db, sql_query, login_callback, 0, &mesaj_eroare);
}

int main () {
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;	
    int nr;		//mesajul primit de trimis la client 
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

    user_id = admin_status = -1; // resetam valorile atributelor
}

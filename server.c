/* 
    Server concurent TCP realizat de Ciprian Ursulean - adaptat dupa cel creat de catre Lenuta Alboaie de pe siteul de retele al facultatii.
    La realizarea acestui server s-au utilizat si informatii de la urmatoarele site-uri:
        https://profs.info.uaic.ro/~computernetworks/cursullaboratorul.php
        https://www.youtube.com/?hl=ro&gl=RO
        https://www.sqlite.org/cintro.html
        https://www.geeksforgeeks.org/sql-using-c-c-and-sqlite/
        http://zetcode.com/db/sqlitec/
        https://stackoverflow.com/questions/28969543/fatal-error-sqlite3-h-no-such-file-or-directory/31764947
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


#define PORT 2908
extern int errno;

typedef struct thData{
	int idThread;
	int cl;
} thData;


sqlite3* db;
int user_id = -1;
int admin_status = -1;
int comment_status = -1;
int vote_status = -1;

int nr_melodii = 0;
char lista_melodii[2048][512];

int top_melodii_index = 0;
char top_melodii[2048][512];

int nr_genuri = 0;
char lista_genuri[128][128];

int nr_utilizatori = 0;
char lista_utilizatori[128][256];

int nr_comentarii = 0;
char lista_comentarii[128][3048];

char id_melodie[10];

int already_exists_user = 0;

int piesa_votata_deja = 0;

static void *treat(void *);
void gestioneaza_clientul(void *);

static int utilizator_deja_existent_callback(void *not_used_prt, int argc, char **argv, char **azColName) {
    already_exists_user = 1;
    return 0;
}

static int piesa_votata_deja_callback(void *not_used_prt, int argc, char **argv, char **azColName) {
    piesa_votata_deja = 1;
    return 0;
}

static int login_callback(void *not_used_prt, int argc, char **argv, char **azColName) {
    user_id = atoi(argv[0]);
    admin_status = atoi(argv[1]);
    comment_status = atoi(argv[2]);
    vote_status = atoi(argv[3]);
    return 0;
}

static int votare_melodie_callback(void *not_used_prt, int argc, char** argv, char **azColName) {
    char formatare_piesa[512];
    strcpy(formatare_piesa, "Id: ");
    strcat(formatare_piesa, argv[0]);
    strcat(formatare_piesa, "     Title: ");
    strcat(formatare_piesa, argv[1]);
    strcat(formatare_piesa, "     No of votes: ");
    strcat(formatare_piesa, argv[2]);
    strcat(formatare_piesa, "\nURL: ");
    strcat(formatare_piesa, argv[3]);

    strcpy(lista_melodii[nr_melodii++],  formatare_piesa);
    return 0;
}

static int proceseaza_topul_general_callback(void *not_used_prt, int argc, char** argv, char **azColName) {
    char formatare_piesa[512];
    strcpy(formatare_piesa, "Titlu: ");
    strcat(formatare_piesa, argv[0]);
    strcat(formatare_piesa, "    Nr voturi: ");
    strcat(formatare_piesa, argv[1]);
    strcat(formatare_piesa, "\nURL: ");
    strcat(formatare_piesa, argv[2]);

    strcpy(top_melodii[top_melodii_index++], formatare_piesa);
    return 0;
}

static int proceseaza_genuri_melodii_callback(void *not_used_prt, int argc, char** argv, char **azColName) {
    strcpy(lista_genuri[nr_genuri++], argv[0]);
    return 0;
}

static int filtreaza_top_dupa_gen_callback(void* not_used_prt, int argc, char** argv, char **azColName) {
    char formatare_piesa[512];
    strcpy(formatare_piesa, "Titlu: ");
    strcat(formatare_piesa, argv[0]);
    strcat(formatare_piesa, "     Nr voturi: ");
    strcat(formatare_piesa, argv[1]);
    strcat(formatare_piesa, "\nURL: ");
    strcat(formatare_piesa, argv[2]);

    strcpy(top_melodii[top_melodii_index++], formatare_piesa);
    return 0;
}

static int afisare_lista_melodii_callback(void *not_used_prt, int argc, char** argv, char** azColName) {
    char formatare_piesa[512];
    strcpy(formatare_piesa, "Titlu: ");
    strcat(formatare_piesa, argv[0]);
    strcat(formatare_piesa, "\nURL: ");
    strcat(formatare_piesa, argv[1]);

    strcpy(lista_melodii[nr_melodii++], formatare_piesa);
    return 0;
}

static int afisare_lista_utilizatori_callback(void* not_used_prt, int argc, char** argv, char** azColName) {
    char formatare_utilizator[128];
    strcpy(formatare_utilizator, "Id: ");
    strcat(formatare_utilizator, argv[0]);
    strcat(formatare_utilizator, "  Username: ");
    strcat(formatare_utilizator, argv[1]);
    strcat(formatare_utilizator, "  Admin status:");
    strcat(formatare_utilizator, argv[2]);
    strcat(formatare_utilizator, "  Vote status:");
    strcat(formatare_utilizator, argv[3]);
    strcat(formatare_utilizator, "  Comment status:");
    strcat(formatare_utilizator, argv[4]);
    strcpy(lista_utilizatori[nr_utilizatori++], formatare_utilizator);
    return 0;
}

static int identificare_id_melodie_callback(void *not_used_prt, int argc, char** argv, char** azColName) {
    strcpy(id_melodie, argv[0]);
    return 0;
}

static int afisare_comentarii_callback(void *not_used_prt, int argc, char** argv, char** azColName) {
    char formatare_piesa[3047];
    strcpy(formatare_piesa, "Autor: ");
    strcat(formatare_piesa, argv[0]);
    strcat(formatare_piesa, "\nData: ");
    strcat(formatare_piesa, argv[1]);
    strcat(formatare_piesa, "\nContinut: ");
    strcat(formatare_piesa, argv[2]);
    strcpy(lista_comentarii[nr_comentarii++], formatare_piesa);
    return 0;
}

int creare_cont(sqlite3* db, char* nume_utilizator, char* parola) {
    char sql_query[512] = "SELECT * FROM users WHERE username = '";
    char* mesaj_eroare;

    strcat(sql_query, nume_utilizator);
    strcat(sql_query, "'");
    strcat(sql_query, " AND password = '");
    strcat(sql_query, parola);
    strcat(sql_query, "'");

    int select_status = sqlite3_exec(db, sql_query, utilizator_deja_existent_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la cautarea utilizatorului - %s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
    if (already_exists_user == 1) {
        already_exists_user = -1;
        return -1; // utilizatorul exista deja
    }
    else {
        strcpy(sql_query, "INSERT INTO users (username, password, admin_status, comment_status, vote_status, created_at) VALUES ('");
        strcat(sql_query, nume_utilizator);
        strcat(sql_query, "',");
        strcat(sql_query, "'");
        strcat(sql_query, parola);
        strcat(sql_query, "',");
        strcat(sql_query, " 0, 1, 1, DATE('NOW'))");
        select_status = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);
        if (select_status != SQLITE_OK) {
            printf("Eroare la cautarea utilizatorului - %s!", mesaj_eroare);
            fflush(stdout);
            sqlite3_free(mesaj_eroare);
        }
    }
    return 1;
}

void autentificare_utilizator(sqlite3* db, char* nume_utilizator, char* parola) {
   char sql_query[512] = "SELECT user_id, admin_status, comment_status, vote_status FROM users WHERE username = '";
   char* mesaj_eroare;

   strcat(sql_query, nume_utilizator);
   strcat(sql_query, "'");
   strcat(sql_query, " AND password = '");
   strcat(sql_query, parola);
   strcat(sql_query, "'");

   int db_descriptor = sqlite3_exec(db, sql_query, login_callback, 0, &mesaj_eroare);
}

char* itoa(int number) {
    // deoarece in libraria standard din C nu este pusa la dispozitie o functie 
    // care converteste un numarl la un char*, am creat una
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
    char sql_query[128] = "SELECT * FROM votes WHERE user_id = ";
    strcat(sql_query, itoa(user_id));
    strcat(sql_query, " AND song_id = ");
    strcat(sql_query, itoa(song_id));
    char* mesaj_eroare;

    int status_interogare = sqlite3_exec(db, sql_query, piesa_votata_deja_callback, 0, &mesaj_eroare);
    if (status_interogare != SQLITE_OK) {
        printf("Eroare la verificarea votarii deja existente - %s\n", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }

    // fiecare utilizator voteaza o singura data
    if (piesa_votata_deja == 1) {
        piesa_votata_deja = 0;
        return;
    }

    strcpy(sql_query, "UPDATE songs SET no_of_votes = no_of_votes + 1 WHERE song_id = ");
    strcat(sql_query, itoa(song_id));
    printf("%s\n", sql_query);
    fflush(stdout);

    status_interogare = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);
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
    char sql_query[128] = "SELECT song_id, title, no_of_votes, url FROM songs";
    char* mesaj_eroare;

    int select_status = sqlite3_exec(db, sql_query, votare_melodie_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la selectarea melodiilor - %s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
}

void afisare_lista_generala_melodii() {
    char sql_query[128] = "SELECT title, url FROM songs";
    char* mesaj_eroare;

    int select_status = sqlite3_exec(db, sql_query, afisare_lista_melodii_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la selectarea melodiilor pentru comentare - %s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
}

void inserare_comentariu(int user_id, char* comentariu, int id_ordine_piesa) {
    char sql_query[128] = "SELECT song_id FROM songs WHERE title = '";
    char* mesaj_eroare;

    char titlu_tmp[128];
    int idx = 0;
    strcpy(titlu_tmp, lista_melodii[id_ordine_piesa] + 7);
    while (idx < strlen(titlu_tmp) && titlu_tmp[idx] != '\n') {
        idx++;
    }
    titlu_tmp[idx] = '\0';

    strcat(sql_query, titlu_tmp); // elimin partea de `Title: ` din lista de melodii
    strcat(sql_query, "'");

    int select_status = sqlite3_exec(db, sql_query, identificare_id_melodie_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la extragerea id ului real al melodiei -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }

    // dupa ce avem id ul real al melodiei in baza de date, vom insera in tabelul comentarii 
    strcpy(sql_query, "INSERT INTO comments (user_id, song_id, content, created_at) VALUES (");
    strcat(sql_query, itoa(user_id));
    strcat(sql_query, ", ");
    strcat(sql_query, id_melodie);
    strcat(sql_query, ", ");
    strcat(sql_query, "'");
    strcat(sql_query, comentariu);
    strcat(sql_query, "', ");
    strcat(sql_query, "date('now'))");

    int insert_status = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);
    if (insert_status != SQLITE_OK) {
        printf("Eroare la inserarea comentariului -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
    printf("Comentariu inserat cu succes!");
    fflush(stdout);
}

void afisare_top_general() {
    char sql_query[128] = "SELECT title, no_of_votes, url FROM songs ORDER BY no_of_votes DESC";
    char* mesaj_eroare;

    int select_status = sqlite3_exec(db, sql_query, proceseaza_topul_general_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la selectarea melodiilor in topul general - %s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
}

void afisare_lista_genuri() {
    char sql_query[128] = "SELECT DISTINCT UPPER(genre) FROM songs";
    char* mesaj_eroare;

    int select_status = sqlite3_exec(db, sql_query, proceseaza_genuri_melodii_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la selectarea genurilor melodiilor - %s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
}

void filtrare_top_dupa_genuri(int gen_id) {
    char sql_query[128] = "SELECT title, no_of_votes, url FROM songs WHERE UPPER(genre) = '";
    char* mesaj_eroare;
    strcat(sql_query, lista_genuri[gen_id]);
    strcat(sql_query, "' ORDER BY no_of_votes DESC");

    int select_status = sqlite3_exec(db, sql_query, filtreaza_top_dupa_gen_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la selectarea topului pe genuri - %s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
}

void stergere_melodie(int id_ordine_piesa) {
    char sql_query[128] = "SELECT song_id FROM songs WHERE title = '";
    char* mesaj_eroare;

    char titlu_tmp[128];
    int idx = 0;
    strcpy(titlu_tmp, lista_melodii[id_ordine_piesa] + 7);
    while (idx < strlen(titlu_tmp) && titlu_tmp[idx] != '\n') {
        idx++;
    }
    titlu_tmp[idx] = '\0';

    strcat(sql_query, titlu_tmp); // elimin partea de `Title: ` din lista de melodii
    strcat(sql_query, "'");

    int select_status = sqlite3_exec(db, sql_query, identificare_id_melodie_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la extragerea id ului real al melodiei -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }

    strcpy(sql_query, "DELETE FROM songs WHERE song_id = ");
    strcat(sql_query, id_melodie);
    printf("id = %s", id_melodie);
    fflush(stdout);

    int delete_status = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);
    if (delete_status != SQLITE_OK) {
        printf("Eroare la stergerea melodiei -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }

    // stergem si comentariile asociate melodiei
    strcpy(sql_query, "DELETE FROM comments WHERE song_id = ");
    strcat(sql_query, id_melodie);
    delete_status = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);
    if (delete_status != SQLITE_OK) {
        printf("Eroare la stergerea comentariilor asociate melodiei -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }

    // stergem si voturile asociate melodiei
    strcpy(sql_query, "DELETE FROM votes WHERE song_id = ");
    strcat(sql_query, id_melodie);
    delete_status = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);
    if (delete_status != SQLITE_OK) {
        printf("Eroare la stergerea voturilor asociate melodiei -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }

    printf("Melodie stearsa cu succes!");
    fflush(stdout);
}

void afisare_utilizatori() {
    char sql_query[256] = "SELECT user_id, username, admin_status, vote_status, comment_status FROM users";
    char* mesaj_eroare;
    int select_status = sqlite3_exec(db, sql_query, afisare_lista_utilizatori_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la afisarea utilizatorilor -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
}

void administrare_drepturi_vot(int user_id, int grant_status) {
    char sql_query[128] = "UPDATE users SET vote_status = ";
    char* mesaj_eroare;
    if (grant_status == 1) {
        strcat(sql_query, "1 WHERE user_id = ");
    } else {
        strcat(sql_query, "0 WHERE user_id = ");
    }
    strcat(sql_query, itoa(user_id));
    int update_status = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);
    if (update_status != SQLITE_OK) {
        printf("Eroare eroare la actualizarea drepturilor de vot -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
    printf("Drepturi actualizate cu succes!\n");
    fflush(stdout);
}

void administrare_drepturi_comentare(int user_id, int grant_status) {
    char sql_query[128] = "UPDATE users SET comment_status = ";
    char* mesaj_eroare;
    if (grant_status == 1) {
        strcat(sql_query, "1 WHERE user_id = ");
    } else {
        strcat(sql_query, "0 WHERE user_id = ");
    }
    strcat(sql_query, itoa(user_id));
    int update_status = sqlite3_exec(db, sql_query, 0, 0, &mesaj_eroare);
    if (update_status != SQLITE_OK) {
        printf("Eroare eroare la actualizarea drepturilor de vot -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }
    printf("Drepturi actualizate cu succes!\n");
    fflush(stdout);
}

void afisare_comentarii_melodie(int id_ordine_piesa) {
    char sql_query[512] = "SELECT song_id FROM songs WHERE title = '";
    char* mesaj_eroare;

    char titlu_tmp[128];
    int idx = 0;
    strcpy(titlu_tmp, lista_melodii[id_ordine_piesa] + 7);
    while (idx < strlen(titlu_tmp) && titlu_tmp[idx] != '\n') {
        idx++;
    }
    titlu_tmp[idx] = '\0';
    strcat(sql_query, titlu_tmp); // elimin partea de `Title: ` din lista de melodii
    strcat(sql_query, "'");

    int select_status = sqlite3_exec(db, sql_query, identificare_id_melodie_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare la extragerea id ului real al melodiei -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }

    strcpy(sql_query, "select u.username, c.created_at, c.content from users u join comments c on u.user_id = c.user_id join songs s on c.song_id = s.song_id where s.song_id = ");
    strcat(sql_query, id_melodie);

    printf("%s\n", sql_query);
    fflush(stdout);

    select_status = sqlite3_exec(db, sql_query, afisare_comentarii_callback, 0, &mesaj_eroare);
    if (select_status != SQLITE_OK) {
        printf("Eroare afisarea comentariilor asociate melodiei -%s!", mesaj_eroare);
        fflush(stdout);
        sqlite3_free(mesaj_eroare);
    }

    printf("Comentarii afisate cu succes!\n");
    fflush(stdout);
}

int main () {
    struct sockaddr_in server;
    struct sockaddr_in from;

    pthread_t th[100];
	int i = 0;

    int sd;
    int pid;
    
    int db_descriptor = sqlite3_open("top_music.db", &db); 
    if (db_descriptor) {
        perror("Eroare la conectarea cu baza de date!");
        return 0;
    }
  
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("Eroare la socket().\n");
        return errno;
    }

    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));
  

    server.sin_family = AF_INET;	
    server.sin_addr.s_addr = htonl (INADDR_ANY);  
    server.sin_port = htons (PORT);
  
    /* atasam adresa la socket */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1) {
        perror ("Eroare la bind().\n");
        return errno;
    }

    if (listen (sd, 2) == -1) {
        perror ("Eroare la listen().\n");
        return errno;
    }

    while (1) {
        int client;
        thData * td; // parametru functia executata de thread     
        int length = sizeof (from);

        printf ("Asteptam la portul %d...\n", PORT);
        fflush (stdout);

        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0) {
	        perror ("Eroare la accept().\n");
	        continue;
	    }
	
        /* s-a realizat conexiunea, se astepta mesajul */
        td=(struct thData*)malloc(sizeof(struct thData));	
        td->idThread = i++;
        td->cl = client;

        pthread_create(&th[i], NULL, &treat, td);	      
	}
};			

static void *treat(void * arg) {		
	struct thData tdL; 
	tdL= *((struct thData*)arg);	
	printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
	fflush (stdout);		 
	pthread_detach(pthread_self());		
	gestioneaza_clientul((struct thData*)arg);
	/* am terminat cu acest client, inchidem conexiunea */
	close ((intptr_t)arg);
	return(NULL);		
};

void gestioneaza_clientul(void *arg) {
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

    int optiune;
    // citesc optiunea de autentificare/creare cont de la client
    if (read(tdL.cl, &optiune, sizeof(int)) <= 0) {
        perror("Eroare la primire parola de la client!");
    }   
	
    if (optiune == 1) {
        int creare_cont_status = creare_cont(db, nume_utilizator, parola);
        
        // trimit statusul de creare cont la client
        if (write(tdL.cl, &creare_cont_status, sizeof(int)) <= 0) {
            perror("Eroare la trimiterea statusului crearii contului catre client!");
        } 
    }
    else  {
    
    autentificare_utilizator(db, nume_utilizator, parola);
    // trimit user_id catre client
    if (write(tdL.cl, &user_id, sizeof(int)) <= 0) {
        perror("Eroare la trimitere user_id catre client!");
    }

    // trimit admin_statis catre client
    if (write(tdL.cl, &admin_status, sizeof(int)) <= 0) {
        perror("Eroare la trimitere admin_status catre client!");
    }
    
    // trimit vote_status catre client
    if (write(tdL.cl, &vote_status, sizeof(int)) <= 0) {
        perror("Eroare la trimitere vote_status catre client!");
    }

    // trimit comment_status catre client
    if (write(tdL.cl, &comment_status, sizeof(int)) <= 0) {
        perror("Eroare la trimitere comment_status catre client!");
    }

    // special pentru a vizualiza comentariile specifice ale unei melodii (case 6)
    afisare_lista_generala_melodii();
    
    // trimit numarul de melodii catre server
    if (write(tdL.cl, &nr_melodii, sizeof(int)) <= 0) {
        perror("Eroare la trimiterea numarului de melodii catre client!");
    }

    nr_melodii = 0;
    if (write(tdL.cl, lista_melodii, sizeof(lista_utilizatori)) <= 0) {
        perror("Eroare la trimiterea listei de melodii catre client!");
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
            if (vote_status == 1) {
                 votare_melodie();

                // trimit la client numarul de melodii din array-ul de melodii disponibile
                if (write(tdL.cl, &nr_melodii, sizeof(int)) <= 0) {
                    perror("Eroare la trimiterea numarului de melodii catre client!");
                }

                // trimit la client 'view-ul' de melodii pentru a fi votate
                if (write(tdL.cl, &lista_melodii, sizeof(lista_melodii)) <= 0) {
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

                nr_melodii = 0;
            }
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
            nr_melodii = 0;
            break;
        case 4:
            printf("Afisarea topului pe genuri.\n");

            afisare_lista_genuri();

            // trimit la client numarul de genuri
            if (write(tdL.cl, &nr_genuri, sizeof(int)) <= 0) {
                perror("Eroare la trimiterea numarului de genuri catre client!");
            }

            // trimit la client lista de genuri
            if (write(tdL.cl, lista_genuri, sizeof(lista_genuri)) <= 0) {
                perror("Eroare la trimiterea listei de genuri catre client!");
            }
            
            int optiune_gen;
            // primesc optiunea genului selectat de client
            if (read(tdL.cl, &optiune_gen, sizeof(int)) <= 0) {
                perror("Eroare la primirea optiunii genului de la client!");
            }

            filtrare_top_dupa_genuri(optiune_gen);

            // trimit numarul de melodii din top catre client
            if (write(tdL.cl, &top_melodii_index, sizeof(int)) <= 0) {
                perror("Eroare la trimiterea optiunii numarului de melodii la client!");
            }

            // trimit topul filtrat dupa gen catre client
            if (write(tdL.cl, &top_melodii, sizeof(top_melodii)) <= 0) {
                perror("Eroare la trimiterea topului de melodii filtrat la client!");
            }

            top_melodii_index = nr_genuri = nr_melodii = 0;
            break;
        case 5:
            printf("Comentarea unei melodii.\n");
            afisare_lista_generala_melodii();
           
            // trimit numarul de melodii la client
            if (write(tdL.cl, &nr_melodii, sizeof(int)) <= 0) {
                perror("Eroare la trimiterea numarului de melodii la client!");
            }

            // trimit lista melodiilor la client
            if (write(tdL.cl, lista_melodii, sizeof(lista_melodii)) <= 0) {
                perror("Eroare la trimiterea listei de melodii la client!");
            }

            if (comment_status == 1) {
                // primesc id-ul melodiei pe care doresc sa o votez
                int id_ordine_melodie;
                if (read(tdL.cl, &id_ordine_melodie, sizeof(int)) <= 0) {
                    perror("Eroare la citirea numarului de ordine al melodiei de la client!");
                }

                // primesc comentariul asociat melodiei
                char comentariu[2048];
                if (read(tdL.cl, &comentariu, sizeof(comentariu)) <= 0) {
                    perror("Eroare la citirea comentariului de la client!");
                }

                printf("%d - %s\n", id_ordine_melodie, comentariu);
                fflush(stdout);

                inserare_comentariu(user_id, comentariu, id_ordine_melodie);
            } else {
                printf("Nu aveti drep de a comenta! Contactati un admin!\n");
                fflush(stdout);
            }

            nr_melodii = 0;
            break;
        case 6:
            printf("Vizualizarea comentariilor unei melodii!\n");
            afisare_lista_generala_melodii();

            // primesc id-ul melodiei dorite de client
            int id_optiune_melodie;
            if (read(tdL.cl, &id_optiune_melodie, sizeof(int)) <= 0) {
                perror("Eroare la primirea id-ului melodiei dorite de la client!");
            }

            afisare_comentarii_melodie(id_optiune_melodie);

            // trimit numarul de comentarii catre client
            if (write(tdL.cl, &nr_comentarii, sizeof(int)) <= 0) {
                perror("Eroare la trimiterea numarului de comentarii la client!\n");
            }

            //trimit lista de comentarii la client
            if (write(tdL.cl, lista_comentarii, sizeof(lista_comentarii)) <= 0) {
                perror("Eroare la trimiterea listei de comentarii la client!");
            }

            nr_comentarii = nr_melodii = 0;
            break;
        case 7:
            printf("Stergerea unei melodii.\n");
            afisare_lista_generala_melodii();
            // trimit numarul de melodii la client
            if (write(tdL.cl, &nr_melodii, sizeof(int)) <= 0) {
                perror("Eroare la trimiterea numarului de melodii la client!");
            }
            // trimit lista melodiilor la client
            if (write(tdL.cl, lista_melodii, sizeof(lista_melodii)) <= 0) {
                perror("Eroare la trimiterea listei de melodii la client!");
            }
            nr_melodii = 0;

            // primesc id-ul melodiei pe care sa o stergem de la client
            int id_ordine_melodie;
            if (read(tdL.cl, &id_ordine_melodie, sizeof(int)) <= 0) {
                perror("Eroare la primirea id-ului melodiei de sters de la client!");
            }
            stergere_melodie(id_ordine_melodie);
            break;
        case 8:
            printf("Restrictionarea la vot a unui utilizator.\n");
            afisare_utilizatori();

            // trimit numarul de utulizatori catre client
            if (write(tdL.cl, &nr_utilizatori, sizeof(int)) <= 0) {
                perror("Eroare la trimiterea numarului de utilizatori catre client!");
            }

            // trimit lista de utulizatori la client
            if (write(tdL.cl, lista_utilizatori, sizeof(lista_utilizatori)) <= 0) {
                perror("Eroare la trimiterea listei de utilizatori catre client!");
            }

            int id_utilizator_restrctionat;
            if (read(tdL.cl, &id_utilizator_restrctionat, sizeof(int)) <= 0) {
                perror("Eroare la primirea id-ului utilizatorului restrictionat de la client!");
            }

            int grant_status;
            if (read(tdL.cl, &grant_status, sizeof(int)) <= 0) {
                perror("Eroare la primirea optiunii de restictie de la client!");
            }

            administrare_drepturi_vot(id_utilizator_restrctionat, grant_status);
            nr_utilizatori = 0;
            break;
        case 9:
            printf("Restrictionarea de a comenta a unui utulizator.\n");
            fflush(stdout);

            afisare_utilizatori();

            // trimit numarul de utulizatori catre client
            if (write(tdL.cl, &nr_utilizatori, sizeof(int)) <= 0) {
                perror("Eroare la trimiterea numarului de utilizatori catre client!");
            }

            // trimit lista de utulizatori la client
            if (write(tdL.cl, lista_utilizatori, sizeof(lista_utilizatori)) <= 0) {
                perror("Eroare la trimiterea listei de utilizatori catre client!");
            }

            if (read(tdL.cl, &id_utilizator_restrctionat, sizeof(int)) <= 0) {
                perror("Eroare la primirea id-ului utilizatorului restrictionat de la client!");
            }

            if (read(tdL.cl, &grant_status, sizeof(int)) <= 0) {
                perror("Eroare la primirea optiunii de restictie de la client!");
            }

            administrare_drepturi_comentare(id_utilizator_restrctionat, grant_status);
            nr_utilizatori = 0;
            break;
        default:
            printf("Optiune invalida!\n");
            fflush(stdout);
            break;
    }

    nr_melodii = 0;
    already_exists_user = user_id = admin_status = comment_status = vote_status = -1; // resetam valorile atributelor
    }
}

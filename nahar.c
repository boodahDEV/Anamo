#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <regex.h>        

#define THIS_OK() (printf("\x1B[32m O K "))
#define THIS_ERROR() (printf("\x1B[31mERROR"))
#define THIS_NRM(a) ((a == 0) ? printf("\x1B[0m [") : printf("\x1B[0m] "))
#define MAX_COLUM 2


/*PROTOTIPOS*/
void *carga(void *);
void *verifica(void *);
int match(char *, char *);
/*PROTOTIPOS*/

        struct TOKEN_LEX
        {
            char *linea;
            int indice;
            //char cod_asc_words[8];  //CODIGO ASCII DE LAS PALABRAS RESERVADAS
        };

        /*MATRIZ DE COMANDOS O ALFABETOS GLOBAL*/
        char  **MCOM; char *_INIT_TEXT;
        /*MATRIZ DE COMANDOS O ALFABETOS GLOBAL*/
int main(int argc, char const *argv[])
{
    struct TOKEN_LEX TOK[20];
    int STATUS;
    static pid_t RESULT_FORK, PID_SON, PID_DAD;
   

    PID_DAD = getpid();
    _INIT_TEXT = (char *)mmap(NULL, (20 * sizeof(char)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);


    //======= TODO APARTIR DE AQUI SE DUPLICARA EN EL PROCESO HIJO, TOMAR EN CUENTA AL TRATAR DE MAXIMIZAR EL CODIGO =======//
    switch (RESULT_FORK = fork())
    {
    case -1: /*PREMATURO*/
    {
        exit(EXIT_FAILURE); //Finalizo de manera erronea
    }
    case 0: /*HIJO*/
    {
       
        pthread_t LEX_THREAD_LOAD; // HILO QUE EJECUTA LA CARGA DE LOS ALFABETOS!.
        pthread_t LEX_THREAD_ANALICE; // HILO QUE EJECUTA EL ANALISIS LEXIC.
        PID_SON = getpid();

        scanf("%[^\n]", _INIT_TEXT); // COMANDO DE ENTRADA
        pthread_create(&LEX_THREAD_LOAD, NULL, carga,NULL);
        pthread_create(&LEX_THREAD_ANALICE, NULL,verifica,(void *)&TOK);
        //printf("MCON = %s\n", *(MCOM+4));
            // int a; char x[] = "_99", *y="_[a-zA-Z]+[1-9]|[a-zA-Z]+[1-9]";
            // a = match(x,y);
            // printf("a = %d",a);
       
        //pthread_cancel(LEX_THREAD_LOAD); //mata el hilo hp
        pthread_join(LEX_THREAD_ANALICE, NULL); //ESTE PROCESOS ESPERA A QUE EL HILO CULMINE
        pthread_join(LEX_THREAD_LOAD, NULL); //ESTE PROCESOS ESPERA A QUE EL HILO CULMINE
        printf("MAIN TOK: %s\n", TOK[0].linea);
        exit(getpid() > PID_DAD);
    }
    default: /*PADRE*/
    {
        waitpid(PID_SON, &STATUS, 0); //ESPERA A LA FINALIZACION DEL PROCESO HIJO!

        /* more code here for process father */

        exit(EXIT_SUCCESS);
    }
    } //end switch
    //======= NO COLOCAR NADA DESPUES DE AQUI, SI CONSIDERAS QUE EL PROCESO PADRE EJECUTE ALGO INDEPENDIENTE, DEBE SER DENTRO DEL IF, Y LO MISMO EN EL HIJO. ======/
    return 0;
} // FIN DEL MAIN alv

void *carga(void *args){ 
    FILE *archivo;
    int k=0; char temp[10];
    int x = 0; //CONTADOR POR ARCHIVO
    
    /*DEFINO A MCOM */
    MCOM = (char **)malloc(5*sizeof(char *));
        for (int i = 0; i < 5;i++)
            MCOM[i] = (char *)malloc(2*sizeof(char));
    /*DEFINO A MCOM */  

     char *ALPHABET_M = "dll/data_op.dll"; //THIS_NRM(0);THIS_ERROR();THIS_NRM(1); printf(" Correcta carga del pr.\n");

    while( x != 1) { // EL 1 INDICA LA CANTIDAD DE ARCHIVOS A recorrer 
        archivo = fopen((ALPHABET_M + x), "r"); 
        if(archivo == NULL){
            THIS_NRM(0);THIS_ERROR();THIS_NRM(1); printf("Archivo '%s' en conflicto.\n", ALPHABET_M);
            exit(EXIT_FAILURE);
        } else {
            while (!feof(archivo)){
                fscanf(archivo, "%s" ,temp);
                strcpy(*(MCOM+k),temp);
                k++;
            }//fin while que manipula los alfabetos
        } //end else
        x++;
    }//fin del whihle encargado de tomar cada archivo
     fclose(archivo);

   // THIS_NRM(0);THIS_OK();THIS_NRM(1);printf("Loading components: %d \n",k);
    THIS_NRM(0);THIS_OK();THIS_NRM(1);printf("Successfully\n");
    return EXIT_SUCCESS;
}//fin analicis de carga

void *verifica(void *args){
    struct TOKEN_LEX *TOK; 
    TOK = (struct TOKEN_LEX *)args;
    TOK->linea = (char *)malloc(20*sizeof(char));

    FILE *GEN;char z[20]; int k=0;

     GEN = fopen(_INIT_TEXT, "r"); 
        if(GEN == NULL){
            THIS_NRM(0);THIS_ERROR();THIS_NRM(1); printf("Archivo '%s' en conflicto.\n", _INIT_TEXT);
            exit(EXIT_FAILURE);
        } else {
            while (!feof(GEN)){
                fscanf(GEN, "%s" ,z);
                (TOK+k)->linea  = z;
                (TOK+k)->indice = k;
                k++;
            }//fin while que manipula los alfabetos
        }
        printf("k: %d\n",k);
        printf("linea: %s\n",TOK[4].linea);
        printf("ok3 = %d\n",TOK[4].indice);
        fclose(GEN);
        
    return EXIT_SUCCESS;
}//fin analicis

int match(char *string, char *pattern){
    int    status;
    regex_t    re;

    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
        return(0);      /* Report error. */
    }
    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);
    if (status != 0) {
        return(0);      /* Report error. */
    }
    return(1);
}
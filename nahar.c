#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <regex.h>        

#define TAMANO_LINEAS 15 // ES EL TAMANO TANTO DE LINEAS LEGIBLES COMO LONGITUD DE LINEAS, TOMAR EN CUENTA A FUTURO
#define THIS_OK() (printf("\x1B[32m O K "))
#define THIS_ERROR() (printf("\x1B[31mERROR"))
#define THIS_NRM(a) ((a == 0) ? printf("\x1B[0m [") : printf("\x1B[0m] "))
#define MAX_COLUM 2


/*PROTOTIPOS*/
void *carga(void *);
void *verifica(void *);
int match(char *, char *);
void tauto(char *); 
/*PROTOTIPOS*/

        struct TOKEN_LEX
        {
            char linea[256];
            int indice;
            //char cod_asc_words[8];  //CODIGO ASCII DE LAS PALABRAS RESERVADAS
        };


        /*MATRIZ DE COMANDOS O ALFABETOS GLOBAL*/
        char  **MCOM; char *_INIT_TEXT;
        /*MATRIZ DE COMANDOS O ALFABETOS GLOBAL*/
int main(int argc, char const *argv[])
{
    struct TOKEN_LEX TOK[30];
       char **ASIG = (char **)malloc(2*sizeof(char *));
        for (int i = 0; i < 2;i++)
            ASIG[i] = (char *)malloc(256*sizeof(char));

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
        int bandera = 0, asignacion = 0;char *token;
        pthread_t LEX_THREAD_LOAD; // HILO QUE EJECUTA LA CARGA DE LOS ALFABETOS!.
        pthread_t LEX_THREAD_ANALICE; // HILO QUE EJECUTA EL ANALISIS LEXIC.

        PID_SON = getpid();

        system("clear");
        scanf("%[^\n]", _INIT_TEXT); // COMANDO DE ENTRADA
        printf("\n");
        pthread_create(&LEX_THREAD_LOAD, NULL, carga,NULL);
        pthread_create(&LEX_THREAD_ANALICE, NULL,verifica,(void *)TOK);
        // printf("MCON = %s\n", *(MCOM+4));
            // int a; char x[] = "_99", *y="_[a-zA-Z]+[1-9]|[a-zA-Z]+[1-9]";
            // a = match(x,y);
            // printf("a = %d",a);
       
        //pthread_cancel(LEX_THREAD_LOAD); //mata el hilo hp
        pthread_join(LEX_THREAD_LOAD, NULL); //ESTE PROCESOS ESPERA A QUE EL HILO CULMINE
        pthread_join(LEX_THREAD_ANALICE, NULL); //ESTE PROCESOS ESPERA A QUE EL HILO CULMINE


        // LUEGO DE TERMINADA LAS CARGAS PROCEDE A ANALIZAR LA ENTRADA
        for (int i = 0; i < 1; i++)
        {
            for (int j = 0; j < strlen(TOK[i].linea); j++)
            {   int cant_tokens=0;
                // printf("%c", TOK[i].linea[j]);
                // ###############################################

                /* VERIFICA LAS ASIGNACIONES */
                if(match(&TOK[i].linea[j],"=")){ 
                    for(char *token = strtok(&TOK[i].linea[j],"="); token != NULL; token = strtok(NULL, "=")){
                        strcpy(*(ASIG+cant_tokens), token);
                        cant_tokens++;
                    }         
                }


                // ###############################################
            }
                  
        }
        


        printf("\n\tASIG: {%s}",ASIG[0]);
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
    FILE *GEN;
    GEN = fopen(_INIT_TEXT, "r"); 

    char z;
    int res_pal = 0; 
    int cant_pal = 0; 

    char **m_temporal = (char **)malloc(TAMANO_LINEAS*sizeof(char *));
        for (int i = 0; i < TAMANO_LINEAS;i++)
            m_temporal[i] = (char *)malloc(256*sizeof(char));; 

        if(GEN == NULL){
            THIS_NRM(0);THIS_ERROR();THIS_NRM(1); printf("Archivo '%s' en conflicto.\n", _INIT_TEXT);
            exit(EXIT_FAILURE);
        } else {
            while((z = fgetc(GEN)) != EOF)
	        {
                if(z != '\n'){
                    if(z!= 32){
                        m_temporal[cant_pal][res_pal++] = z;
                        strcpy((TOK+cant_pal)->linea, m_temporal[cant_pal]);
                        (TOK+cant_pal)->indice = cant_pal;
                    }
                }
                else{
                    // printf("[%d]\n", cant_pal);
                    // printf("m_temporal: %s\n", m_temporal[3]);
                    m_temporal[cant_pal][res_pal]='\0'; 
                    res_pal = 0;
                    cant_pal ++;    
                }
	        }
            THIS_NRM(0);THIS_OK();THIS_NRM(1); printf("El archivo '%s' esta cargado en memoria.\n", _INIT_TEXT);
         }//end else
       
        fclose(GEN);

        free(m_temporal);
        for (int i = 0; i < TAMANO_LINEAS; i++)
            free(m_temporal[i]);
        m_temporal = NULL;

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


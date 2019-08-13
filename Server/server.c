#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include "command.h"
#include "parser.h"

#define TAM 1024
/*Definicion de funciones a utilizar*/
	void login();
	void start();
	void disconnect();
	void set_pipes(char*);
/*Definicion de variables para las conexiones tanto UDP, como TCP*/
	struct sockaddr_in cli_addr;
/*Utilizo estructura data para almacenar los datos relevantes del programa*/
    struct data {
    int flag;
    int p;
    char port_udp[10];
    char port[TAM];
	char username[TAM];
	char update_date[TAM];
	char message[TAM];
	char buffer[TAM];
	char file[TAM];
	} data;

int main() 
{
	start();
	login();

	return EXIT_SUCCESS; 
} 

void login()
{
	/*Definicion de variables para el servidor TCP*/
	int sockfd, newsockfd,  puerto, pid, n,i,socket_udp;
	char buffer[TAM],buf[TAM],currentdir[TAM],msj[TAM],comando[TAM];
	socklen_t clilen;
	socklen_t slen;
	struct sockaddr_in si_other;
	char* pass = NULL;
	struct sockaddr_in serv_addr;
	/*Se inicicializa el servidor TCP*/
	sockfd = socket( AF_INET, SOCK_STREAM, 0);

	if ( sockfd < 0 ) { 
		perror( " apertura de socket ");
		exit(EXIT_FAILURE);
	}

	memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
	puerto = atoi( data.port);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( puerto );

	/*Se conecta el servidor a la direccion dada*/

	if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {
		perror( "ligadura" );
		exit(EXIT_FAILURE);
	}
	listen( sockfd, 5 );
	clilen = sizeof( cli_addr );

 	/*Se acepta un nuevo cliente en el servidor*/

	while(1)
	{
		newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
		if ( newsockfd < 0 ) {
			perror( "accept" );
			exit(EXIT_FAILURE);
		}
		pid = fork(); 
		if ( pid < 0 ) {
			perror( "fork" );
			exit(EXIT_FAILURE);
		}

		if ( pid == 0 ) 
		{  
			close( sockfd );

			/*Se pregunta al cliente y se envia si es correcto o no el nombre de usuario*/

			printf("Expecting username\n");
			memset( buffer, 0, TAM );

			n = read( newsockfd, buffer, TAM-1 );
			error_socket(n,"reading to socket" );
			buffer[strlen(buffer) - 1] = '\0';
			i = strlen(buffer);
			printf("User received %s of length %d\n", buffer, i);
			if (!strcmp("franco", buffer) ) {
		        strcpy(data.username,buffer);
		        printf("Username entered is correct\n");
		        snprintf(buffer, sizeof(buffer), "CORRECT");         		
		    }
		    else {
		        printf("Incorrect Username %s\n", buffer);
		        snprintf(buffer, sizeof(buffer), "I");
		        exit(EXIT_FAILURE);
		    }
		    printf("SENDING : %s\n",buffer);
			n = write( newsockfd, buffer, TAM-1 );
			error_socket(n,"writing to socket");
			/*Se pregunta al cliente y se envia si es correcto o no, la contraseña*/
			printf("Expecting the password\n");
			memset( buffer, 0, TAM );
			n = read( newsockfd, buffer, TAM-1 );
			error_socket(n,"reading to socket");
			buffer[strlen(buffer) - 1] = '\0';
			i = strlen(buffer);
			printf("Password received %s of length %d\n", buffer, i);
			if (!strcmp("rivero", buffer) ) {
		        printf("Password entered is correct\n");
		        snprintf(buffer, sizeof(buffer), "CORRECT");
		    }
		    else {
		        printf("Incorrect Password %s\n", pass);
		        snprintf(buffer, sizeof(buffer), "I");
		        exit(EXIT_FAILURE);
		    }
		    printf("SENDING : %s\n",buffer);
			n = write( newsockfd, buffer, TAM-1 );
			error_socket(n,"ERROR: sending messge" );
			printf("connection accepted %s\n", data.port);

			/*Se inicia la conexion TCP*/

			while(1)
			{
				/*
				* Muestra el hostname y el directorio actual
				* Queda espectante al input como el baash
				* Y ejeucta funciones para primero interpretar el comando y despues ejecutarlo
				*/
				/**/
				/*Se recibe el comando desde el cliente y se decide que hacer con ese comando*/
				do
				{
					memset( buf, 0, TAM );
					n = read( newsockfd, buf, TAM-1 );
					error_socket(n,"reading to socket");
					buf[strlen(buf)] = '\0';
					if(buf[0] == 'c'){
						if (getcwd(currentdir, sizeof(currentdir)) == NULL){//El directorio de trabajo
									printf("getcwd() error");
						}
						n = write( newsockfd, currentdir, TAM-1 );
						error_socket(n,"writing to socket" );
					}
					memset( buf, 0, TAM );
					printf("%s\n",buf );
					n = read( newsockfd, buf, TAM-1 );
					error_socket(n,"reading to socket" );
					buf[strlen(buf)] = '\0';
					if(!strcmp(buf,"exit")){
						disconnect();
						exit(0);
					}
					else if(!strcmp( "download",strtok(buf,"/"))){
						strcpy(data.file,strtok(NULL,"/"));
					    strcpy(data.port_udp,strtok(NULL,"\0"));
						strcpy(data.message, "Recibiendo archivo.\n");
					    data.flag = 1;
					    n = write( newsockfd, data.message, TAM );
						error_socket(n,"writing to socket" );
						break;
					}
					else{
						dup2( newsockfd, STDOUT_FILENO );  // duplicate socket on stdout 
						set_pipes(buf);
						fflush(stdout);
					}
				}while (!feof(stdin) && strstr(buf, "exit") == NULL); //Termina el programa en el comando exit o al hacer Ctrl+D

				if(data.flag == 1){
					char linea[TAM];
					char b_aux[TAM];
					FILE * f; /*File enviado en UDP*/
					
		   			if ((f = fopen(data.file,"rb")) == NULL){
						n = write( newsockfd, "ErrorOpenFile", TAM );
						close(f);
						error_socket(n,"writing to socket" );
						perror("No se puede abrir fichero.dat");
						exit(EXIT_FAILURE);
					}
					n = write( newsockfd,"OK", TAM );
					error_socket(n,"writing to socket" );
					close(f);
					//Enviar informacion por UDP
		    		data.p=atoi(data.port_udp);
					socket_udp = socket (AF_INET, SOCK_DGRAM, 0);
		    		error_socket(socket_udp,"socket creation UDP");

					memset (&si_other, 0, sizeof (si_other));
		    		si_other.sin_family = AF_INET;
					si_other.sin_addr.s_addr = INADDR_ANY;
		    		si_other.sin_port = htons(data.p);
		    		memset(&(si_other.sin_zero), '\0', 8);

		    		n = bind(socket_udp,(struct sockaddr *)&si_other,sizeof(si_other));
					error_socket(socket_udp,"Binding socket UDP");
					n = read( sockfd, buf, sizeof(buf));
					error_socket(n,"reading to socket" );
					//Enviar la señal que se va a comenzar la transmision de informacion UDP
					n = write( newsockfd, "start", TAM );
					error_socket(n,"writing to socket" );
					n = 0;
					int contador = 0;
		    		slen=sizeof(struct sockaddr);
					FILE * fp=fopen(data.file,"rb");
					/* Se envia por lineas del archivo, los datos al cliente  */
					while(!feof(fp)){
						//Se lee un paquete 
						if((fread(b_aux,1,TAM,fp)) != TAM){
							if(ferror(fp) != 0){
								fprintf(stderr,"Error reading file.\n");
								exit(EXIT_FAILURE);
							}
							else if(feof(fp) != 0);
						}
						//se recibe mensaje de sincronización
						n = recvfrom(socket_udp,linea,TAM-1,0,(struct sockaddr *)&si_other,&slen);
						error_socket(n,"reading to socket" );

						n = sendto(socket_udp,(void *)b_aux,TAM,0,(struct sockaddr *) &si_other,slen);
						error_socket(n,"writing to socket" );
						contador++;
					}
	      			/** Luego se envia el mensaje de finalizacion de transferencia  */
					delay(10);
		   			printf("Se envio el archivo correctamente\n");
					n = sendto(socket_udp,(void *)"finish",18,0,(struct sockaddr *) &si_other,slen);
		 			error_socket(n,"writing to socket" );
					fclose(fp);
		   			close(socket_udp);
					exit(EXIT_SUCCESS);
				}
			}
		}
		else 
		{
			printf( "SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid );
			close( newsockfd );
		}
	}
}

/*funciona como inicio de la conexion tcp, consiguiendo los parametros necesarios para que se realice la conexion*/
void start(){
	/*se declaran las variables a utilizar para la conexion al servidor*/

	char comando_inicial[TAM];    
    /*se da un mensaje de bienvenida al servidor*/

    printf("welcome to server\n");
    printf("insert connect to start\n");
    printf(">");
	memset( comando_inicial, '\0', TAM );
	fgets (comando_inicial, TAM-1, stdin);
	comando_inicial[strlen(comando_inicial)-1] = '\0';

	/*Si se recibe la cadena connect, y se toma un puerto por defecto el puerto 6020 */

	if(!strcmp( "connect", comando_inicial ))
	{
		strcpy(data.port,"6020");
		fprintf( stderr, "Uso: %s <port>", data.port );
		printf("\n\n");
	}

	else{
		perror( "INVALID FORMAT ERROR: expect connect " );
		exit(EXIT_FAILURE);    
	}
}

/*Desconexion del servidor*/

void disconnect(){
	printf("client %s disconnected \n",data.username);
	exit(EXIT_SUCCESS);
}
/**
  Detecta si hubo algun tipo de error al inicializar parametros

  @param n Valor de control
  @param leyenda Frase indicando el tipo de error
  @param newsockfd Descriptor del socket
*/
void error_socket(int n,char message[]){
	if (n < 0) {
		perror (message);
		exit (EXIT_FAILURE);
	}
}
/*
 * Separa los comandos segun pipes, crea los pipes y ejecuta los comandos
 */
void set_pipes(char *input_string)
{
	struct parser_element commands[10];// Obtiene la secuencia de comandos en pipes
	int num_commands=0;
	parse_pipes(input_string, &num_commands, &commands[0]);
	int pipes[num_commands][2];
	//creo todos los pipes
	int i;
	for(i=0; i < num_commands-1; i++){
		if(pipe(pipes[i])<0) {
			perror("Error creating pipe!");
			exit(EXIT_FAILURE);
		}
	}

	int counter=0;
	int pid;
	while(counter < num_commands){
		//Checkeo que no sea builtin
		if(!built_in_command(&commands[counter])){
			pid = fork();
			if (pid==0){
				if(!counter==0){//Si no es el primer comando
					if(dup2(pipes[counter-1][0],0)< 0){
						perror("Error with dup!");
					}
				}
				if(!(counter==(num_commands-1))){ //Si no es el ultimo comando
					if(dup2(pipes[counter][1],1) < 0){
							perror("Error with dup!");
					}
				}
				command_search_logic(&commands[counter]);
				exit(0);
			}
			if(!commands[counter].background_process)
				wait(0);
			if(counter >0)
				close(pipes[counter-1][0]);
			if(counter<(num_commands-1))
				close(pipes[counter][1]);
		}
		counter++;
	}
}

/**
  @brief Permite obtener un delay

  @param milliseconds Valor del delay en milisegundos
*/
void delay (int milliseconds){
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock ();
    while ((now-then) < pause)
        now = clock ();
}
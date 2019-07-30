#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
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
	long int findSize(const char*);
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
	char size_file[TAM];
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
	int sockfd, newsockfd,  puerto, pid, n,i;
	char buffer[TAM],buf[TAM],currentdir[TAM],msj[TAM],comando[TAM],socket_udp,buf2[30];
	socklen_t clilen;
	socklen_t slen;
	char buffer2[1024000];
    int leidos;
	struct sockaddr_in si_other;
	char* pass = NULL;
	struct sockaddr_in serv_addr;
	FILE *f; /*File enviado en UDP*/
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
			printf("%s\n",buffer );
			if ( n < 0 ) {
				perror( "lectura de socket" );
				exit(1);
			}
			else{
				buffer[strlen(buffer) - 1] = '\0';
				printf("%s\n",buffer );
				i = strlen(buffer);
		        printf("User received %s of length %d\n", buffer, i);
			}
			printf("Received\n");
			if (!strcmp("franco", buffer) ) {
		        strcpy(data.username,buffer);
		        printf("Username entered is correct\n");
		        snprintf(buffer, sizeof(buffer), "CORRECT");         		
		    }
		    else {
		        printf("Incorrect Username %s\n", buffer);
		        snprintf(buffer, sizeof(buffer), "I");
		        exit(1);
		    }
		    printf("SENDING : %s\n",buffer);
			n = write( newsockfd, buffer, TAM-1 );
			if ( n < 0 ) {
				perror( "ERROR: sending messge" );
				exit(EXIT_FAILURE);
			}
			/*Se pregunta al cliente y se envia si es correcto o no, la contraseña*/

			printf("Expecting the password\n");
			memset( buffer, 0, TAM );
			n = read( newsockfd, buffer, TAM-1 );
			printf("%s\n",buffer );
			if ( n < 0 ) {
				perror( "lectura de socket" );
				exit(1);
			}
			else{
				buffer[strlen(buffer) - 1] = '\0';
				printf("%s\n",buffer );
				i = strlen(buffer);
		        printf("Password received %s of length %d\n", buffer, i);
			}
			printf("Received\n");
			if (!strcmp("rivero", buffer) ) {
		        printf("Password entered is correct\n");
		        snprintf(buffer, sizeof(buffer), "CORRECT");
		    }
		    else {
		        printf("Incorrect Password %s\n", pass);
		        snprintf(buffer, sizeof(buffer), "I");
		        exit(1);
		    }
		    printf("SENDING : %s\n",buffer);
			n = write( newsockfd, buffer, TAM-1 );
			if ( n < 0 ) {
				perror( "ERROR: sending messge" );
				exit(EXIT_FAILURE);
			}
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
					if ( n < 0 ) 
					{
						perror( "reading to socket" );
						exit(1);
					}
					else{
						buf[strlen(buf)] = '\0';
						if(buf[0] == 'c'){
							if (getcwd(currentdir, sizeof(currentdir)) == NULL){//El directorio de trabajo
								       printf("getcwd() error");
							}
							n = write( newsockfd, currentdir, TAM-1 );
							if ( n < 0 ) 
							{
								perror( "writing to socket" );
								exit(EXIT_FAILURE);
							}
						}

					}
					memset( buf, 0, TAM );
					printf("%s\n",buf );
					n = read( newsockfd, buf, TAM-1 );
					if ( n < 0 ) 
					{
						perror( "reading to socket" );
						exit(1);
					}

					
					buf[strlen(buf)] = '\0';

					if(!strcmp(buf,"exit"))
					{
						disconnect();
						exit(0);
					}
					else if(!strcmp( "download",strtok(buf,"/")))
					{

						strcpy(data.file,strtok(NULL,"/"));
					    strcpy(data.port_udp,strtok(NULL,"\0"));
						sprintf(data.size_file,"%ld",findSize(data.file));
						strcpy(data.message, "Recibiendo archivo.\n");
					    data.flag = 1;
					    n = write( newsockfd, data.message, TAM-1 );
						if ( n < 0 ) 
						{
							perror( "writing to socket" );
							exit(EXIT_FAILURE);
						}
						n = write( newsockfd, data.size_file, TAM );
						if ( n < 0 ) 
						{
							perror( "writing to socket" );
							exit(EXIT_FAILURE);
						}
						break;
					}
					else
					{
						dup2( newsockfd, STDOUT_FILENO );  /* duplicate socket on stdout */
						set_pipes(buf);
						fflush(stdout);
					}
					
					
				}while (!feof(stdin) && strstr(buf, "exit") == NULL); //Termina el programa en el comando exit o al hacer Ctrl+D

				if(data.flag == 1)
				{
					data.flag = 0;
					/*Enviar informacion por UDP*/
		    		data.p=atoi(data.port_udp);
		    		slen=sizeof(si_other);
		    		if ( (socket_udp=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		    		{
		        		perror("socket");
		        		exit(1);
		    		}
		    		si_other.sin_family = AF_INET;
		    		si_other.sin_port = htons(data.p);
		    		memset(&(si_other.sin_zero), '\0', 8);
		    		si_other.sin_addr = cli_addr.sin_addr;
		    		strcpy(buf, "start");
		    		/*Enviar la señal que se va a comenzar la transmision de informacion UDP*/
		   			 if (sendto(socket_udp, buf, strlen(buf) , 0 , (struct sockaddr *) &si_other, slen)==-1)
		        	{
		            	perror("sendto()");
		            	exit(1);
		       		}
		        	
		   			memset(buf, 0, sizeof(buf));

		   			f = fopen(data.file,"rb");
		   			if (f == NULL)
					{
						perror("No se puede abrir fichero.dat");
						return -1;
					}
		   			
					leidos = fread( buffer2,1,1024000,f);
					sprintf(buf2,"%d",leidos);
		   			/*Enviar la linea de informacion*/
		   			if (sendto(socket_udp, buffer2, strlen(buffer2)+1 , 0 , (struct sockaddr *) &si_other, slen)==-1)
	        		{
	            		perror("sendto()");
	            		exit(1);
	       			}
	       			memset(buffer2, 0, sizeof(buffer2));
		   			
		   			/*Enviar la linea de informacion*/
		   			if (sendto(socket_udp, buf2, strlen(buf2)+1 , 0 , (struct sockaddr *) &si_other, slen)==-1)
	        		{
	            		perror("sendto()");
	            		exit(1);
	       			}
		   			memset(buf2, 0, sizeof(buf2));
		   			
		   			/*Enviar la señal que se finalizo la transmision de informacion UDP*/
		   			strcpy(buf, "finish");
		   			if (sendto(socket_udp, buf, strlen(buf), 0 , (struct sockaddr *) &si_other, slen)==-1)
		        	{
		            	perror("sendto()");
		            	exit(1);
		       		}
		   			memset(buf, 0, sizeof(buf));
		   			printf("Se envio el archivo correctamente\n");

		 			fclose(f);
		   			close(data.file);
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

/*function to get size of the file.*/
long int findSize(const char *file_name)
{
    struct stat st; /*declare stat variable*/
     
    /*get the size using stat()*/
     
    if(stat(file_name,&st)==0)
        return (st.st_size);
    else
        return -1;
}

/*funciona como inicio de la conexion tcp, consiguiendo los parametros necesarios para que se realice la conexion*/
void start()
{

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

void disconnect()
{
	printf("client %s disconnected \n",data.username);
	exit(0);
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
			exit(1);
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
void udp_communication(){
	 /*Definicion de variables para el cliente UDP*/
   int socket_file_udp;
   socklen_t cliente_len_udp;
   struct sockaddr_in cliente_addr_udp;
   char buff[TAM];
   
   FILE *f; 
  sprintf(data.port_udp, "%d", data.p);	
	
   /*Apertura del socket cliente UDP*/
   socket_file_udp = socket(AF_INET, SOCK_DGRAM, 0);
   if (socket_file_udp < 0){
		perror("apertura de socket");
		exit(1);
   }

   cliente_addr_udp.sin_family = AF_INET;
   cliente_addr_udp.sin_port = htons(data.p);
   cliente_addr_udp.sin_addr = cli_addr.sin_addr;
   memset(&(cliente_addr_udp.sin_zero), '\0', 8);
   

   cliente_len_udp = sizeof(cliente_addr_udp);

   strcpy(buff, "start");
   /*Enviar la señal que se va a comenzar la transmision de informacion UDP*/
   if(sendto(socket_file_udp, (void *)buff, TAM, 0, (struct sockaddr *)&cliente_addr_udp, cliente_len_udp) < 0){
   		
   		perror("Escritura en socket");
		exit(1);
   } 	
   memset(buff, 0, sizeof(buff));

   /*ACK del servidor UDP*/
   if (recvfrom(socket_file_udp, buff, TAM-1, 0, (struct sockaddr *)&cliente_addr_udp, &cliente_len_udp) < 0){
		perror("lectura de socket");
		exit(1);
   }
   memset(buff, 0, sizeof(buff));
   f = fopen("data.csv","r");

   while(fgets(buff, TAM, f) != NULL){
   	   /*Enviar la linea de informacion*/
	   if(sendto(socket_file_udp, (void *)buff, TAM, 0, (struct sockaddr *)&cliente_addr_udp, cliente_len_udp) < 0){
	   		perror("Escritura en socket");
			exit(1);
	   } 	
	   memset(buff, 0, sizeof(buff));

	   /*ACK del servidor UDP*/
	   if (recvfrom(socket_file_udp, buff, TAM-1, 0, (struct sockaddr *)&cliente_addr_udp, &cliente_len_udp) < 0){
			perror("lectura de socket");
			exit(1);
	   }
	   memset(buff, 0, sizeof(buff));
   }

   fclose(f);

   remove("data.csv");

   /*Enviar la señal que se finalizo la transmision de informacion UDP*/
   strcpy(buff, "finish");
   if(sendto(socket_file_udp, (void *)buff, TAM, 0, (struct sockaddr *)&cliente_addr_udp, cliente_len_udp) < 0) {
		perror("Escritura en socket");
		exit(1);
   } 	

   memset(buff, 0, sizeof(buff));
   /*ACK del servidor UDP*/
   if (recvfrom(socket_file_udp, buff, TAM-1, 0, (struct sockaddr *)&cliente_addr_udp, &cliente_len_udp) < 0){
		perror("lectura de socket");
		exit(1);
   }
   memset(buff, 0, sizeof(buff));
}

/*Se escribe en un archivo la informacion pedida por el cliente y se envia en formato UDP*/
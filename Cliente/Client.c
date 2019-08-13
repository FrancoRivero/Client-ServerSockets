#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <math.h>
#define TAM 1024
void set_data(char*);
void check_ip(char*);
void start();
void login();  

int socket_file_udp;
struct data {
   char username[TAM];
   char number_ip[TAM];
   int number_port;
} data;

struct udp{
	int port_udp;
	int flag;
	char port[10];
	char file[TAM];
}udp;
int main() 
{
   start();
   login();
   
	return 0;
} 
void login(){
	int sockfd, puerto, n,m;
	struct sockaddr_in serv_addr; //variables de conexion tcp-ip
	struct hostent *server;
	char buffer[TAM],user[TAM],command[TAM],received[TAM],currentdir[TAM],buff[TAM];//,buf2[30],buffer2[1024000]; //variables de uso auxiliar
	    
	

	puerto = data.number_port;
	fprintf( stderr, "Using port %i host\n", puerto); 

	
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd < 0 ) {
		perror( "ERROR apertura de socket" );
		exit(EXIT_FAILURE);
	}

	server = gethostbyname(data.number_ip);
	fprintf( stderr, "Using ip address %s\n", data.number_ip); 
	printf("connected as user %s\n",data.username );
	if (server == NULL) {
		fprintf( stderr,"Error, no existe el host\n" );
		exit( 0 );
	}
	memset( (char *) &serv_addr, '0', sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	bcopy( (char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length );
	serv_addr.sin_port = htons( puerto );
	if ( connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {
		perror( "conexion" );
		exit(EXIT_FAILURE);
	}
		printf("Enter the username\n");
		printf(">");
		memset( user, '\0', TAM );
		fgets( user, TAM-1, stdin );
		
        m = write(sockfd, user, strlen(user));
        if (m < 0) {
            perror("Error sending message");
            exit(EXIT_FAILURE);
        } 
        user[strlen(user)-1] = '\0';
        memset( user, '\0', TAM );
        m = read(sockfd, user, sizeof(user));
       if (m < 0) {
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        }
        else {
        	if(!strcmp("CORRECT",user)){
        		printf("User correct\n");
        	}
        	else {
        		printf("User incorrect \n");
        		exit(EXIT_FAILURE);
        	}
        }
        printf("Enter the password :\n");
        printf(">");
        memset( buffer, '\0', TAM );
		fgets( buffer, TAM-1, stdin );
		
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Error sending message");
            exit(EXIT_FAILURE);
        } 
        buffer[strlen(buffer)-1] = '\0';
        memset( buffer, '\0', TAM );
        n = read(sockfd, buffer, sizeof(buffer));
       	if (n < 0) {
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        }
        else {
        	if(!strcmp("CORRECT",buffer)){
        		printf("Password correct\n");
        	}
        	else {
        		printf("password incorrect \n");
        		exit(0);
        	}
        }
   while(1) 
   {
   		n = write(sockfd, "c", 1);
     	if ( n < 0 ) 
     	{
			perror( "writing to socket" );
			exit(EXIT_FAILURE);
		}
   		n = read( sockfd, currentdir, TAM );
		if ( n < 0 ) {
			perror( "reading to socket" );
			exit(EXIT_FAILURE);
		}
		if(strlen(currentdir) == 0)
			printf("%s@%s:~$ ", data.username, data.number_ip);
   		else
   			printf("%s@%s:~%s$ ", data.username, data.number_ip,currentdir);
   		memset( command, '\0', TAM );
   		fgets (command, TAM-1, stdin);
   		command[strlen(command)-1] = '\0';
   		if(!strcmp(command,"exit")){
   			n = write(sockfd, command, strlen(command));
   			exit(EXIT_SUCCESS);
   		}
   		else if(!strcmp(command,"download" )){
			printf("Agregar el archivo a descargar:\n >");
			memset( buff, '\0', TAM );
	   		fgets (buff, TAM-1, stdin);
	   		buff[strlen(buff)-1] = '\0';
	   		strcpy(udp.file,buff);
	   		strcat(command,"/");
	    	strcat(command,buff);
	    	udp.flag = 1;
	    	udp.port_udp = 1000 + (rand() % 1000); //se toma un puerto variable entre 1000 y 2000
	    	sprintf(udp.port, "%d", udp.port_udp); 
	    	strcat(command,"/");
	    	strcat(command,udp.port);
	    	n = write(sockfd, command, strlen(command));
	    	if ( n < 0 ) 
	     	{
				perror( "writing to socket" );
				exit(EXIT_FAILURE);
			}
		}
   		else{
   		
    		n = write(sockfd, command, strlen(command));
   		}
     	if ( n < 0 ) 
     	{
			perror( "writing to socket" );
			exit(EXIT_FAILURE);
		}

		n = read( sockfd, received, TAM );
		if ( n < 0 ) {
			perror( "reading to socket" );
			exit(EXIT_FAILURE);
		}
		printf("\n\n");
		printf("%s\n",received );
		
		if (udp.flag){	
			FILE *f;
			struct sockaddr_in si_me;
			//int rec = 0;
			f = fopen(udp.file,"wb");
				
			if (f==NULL){
				perror("No se puede abrir fichero.dat \n");
				exit(EXIT_FAILURE);
			}
			char buffAuxiliar[TAM];
        	n = read( sockfd, buffAuxiliar, TAM*sizeof(char));
			if ( n < 0 ) {
				perror( "reading to socket" );
				exit(EXIT_FAILURE);
			}
			if(strcmp("OK",buffAuxiliar)){
				printf("El archivo no existe en el servidor \n");
				fclose(f);
				exit(EXIT_FAILURE);
			}
			memset(buffAuxiliar,'\0',TAM);
			printf("El archivo existe en el servidor \n");
		    //se crea el socket UDP
		    if ((socket_file_udp=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		    {
		        perror("socket creation failed \n");
		        exit(EXIT_FAILURE);
		    }
		    memset((char *) &si_me, 0, sizeof(si_me));
		     
		    si_me.sin_family = AF_INET;
		    si_me.sin_port = htons(6021);
		    si_me.sin_addr = *((struct in_addr *) server->h_addr_list[0]);
		    memset(&(si_me.sin_zero), '\0', 8);
		     
		    socklen_t slen = sizeof(si_me);
		    char buffer_udp[TAM];
			n = write( sockfd, "isAlive", TAM);
			if ( n < 0 ) {
				perror( "reading to socket" );
				exit(EXIT_FAILURE);
			}
			memset( buffAuxiliar, '\0', TAM );
			n = read( sockfd, buffAuxiliar, TAM);
			if ( n < 0 ) {
				perror( "reading to socket" );
				exit(EXIT_FAILURE);
			}
			if(strcmp("start",buffAuxiliar)){
				printf("No se pudo comenzar con la descarga \n");
				fclose(f);
				exit(EXIT_FAILURE);
			}
			printf("Se comenzo con la descarga \n");
				
			/** Mientras no se reciba la cadena "finish" por parte del servidor, se guardaran
				en el archivo todas las lineas provenientes del mismo  */
			int contador = 0;
			while(udp.flag)
			{   
				memset(buffer_udp, 0, TAM);
				printf("%s",buffer_udp);
				
				/** Esto se envía solo para mantener sincronizados el servidor 
				con el cliente  */
				n = sendto(socket_file_udp,(void *)"cla",TAM,0,(struct sockaddr *)&si_me,slen);
	            if ( n < 0 ) {
					perror( "writing to socket UDP" );
					exit(EXIT_FAILURE);
				}

				n = recvfrom(socket_file_udp,(void *)buffer_udp,TAM,0,(struct sockaddr *)&si_me,&slen);
				if ( n < 0 ) {
					perror( "reading to socket UDP" );
					exit(EXIT_FAILURE);
				}
				contador++;
				/** Error en el servidor por perror  */
				/*if (!strcmp (buffer_udp,"error")) { 
					printf("Hubo un error en el servidor, intentelo nuevamente\n");
				}*/

				
				if (!strcmp ("finish", buffer_udp)) {
					udp.flag = 0;
					printf("llegaron: %u paquetes\n",contador-1);
				}
				else{
					fwrite(buffer_udp, 1, TAM, f);
					/*if((rec = fwrite(buffer_udp, 1, TAM, f))== TAM){
						printf("Error en la escritura del archivo \n");
						exit(EXIT_FAILURE);
					}*/
				}
				//fclose(f);
			}
			fclose(f);
			char cwd[TAM];
			if (getcwd (cwd, sizeof (cwd)) != NULL) 
				printf ("Archivo descargado en %s\n\n", cwd);
			close(socket_file_udp);		 
			exit(EXIT_SUCCESS);
		}
   }
}
/*Se analiza la informacion sobre la conexion y se guarda en la estructura data*/
void set_data(char* s ){
	char aux[TAM];
    strcpy( data.username, strtok(s,"@"));
    strcpy(aux ,strtok(NULL,":"));
    strcpy( data.number_ip, aux);
    strcpy(aux ,strtok(NULL,"\0"));
   	data.number_port = atoi(aux);
    check_ip(data.number_ip);
    
}

/*Se chequea la direccion de ip para aceptar solo un formato xxx.xxx.xxx.xxx*/

void check_ip(char* ip){
	char aux[TAM];
	strcpy(aux,ip);
if(strcmp("localhost",aux)){
	strcpy(aux,strtok(aux,"."));
	if(strlen(aux)>3 || atoi(aux)>255 || atoi(aux)<0){
		perror("INVALID FORMAT ERROR: expect xxx.xxx.xxx.xxx or localhost, or doesn't belong to an ip address" );
		printf("%s\n",aux );
		exit(EXIT_FAILURE);
	}
	strcpy(aux,strtok(NULL,"."));
	if(strlen(aux)>3 || atoi(aux)>255 || atoi(aux)<0){
		perror("INVALID FORMAT ERROR: expect xxx.xxx.xxx.xxx or localhost, or doesn't belong to an ip address" );
			printf("%s\n",aux );
			exit(EXIT_FAILURE);
	}
	strcpy(aux,strtok(NULL,"."));
	if(strlen(aux)>3 || atoi(aux)>255 || atoi(aux)<0){
		perror("INVALID FORMAT ERROR: expect xxx.xxx.xxx.xxx or localhost, or doesn't belong to an ip address" );
			printf("%s\n",aux );
			exit(EXIT_FAILURE);
	}
	
	strcpy(aux,strtok(NULL,"\0"));
	if(strlen(aux)>3 || atoi(aux)>255 || atoi(aux)<0){
		perror("INVALID FORMAT ERROR: expect xxx.xxx.xxx.xxx or localhost, or doesn't belong to an ip address" );
			printf("%s\n",aux );
			exit(EXIT_FAILURE);
	}
}
else if (!strcmp("localhost",aux)){
 strcpy(ip,"127.0.0.1");
}
else
{
	perror("INVALID FORMAT ERROR: expect xxx.xxx.xxx.xxx or localhost" );
	exit(EXIT_FAILURE);
}

}

/*Se pide la informacion del cliente y se da un mensaje de bienvenida*/

void start(){
	 char comando_inicial[TAM];
    char connection[TAM]; 
    char aux[TAM],aux2[TAM]; 
    printf("insert connect to start\n");
    printf(">");
	memset( comando_inicial, '\0', TAM );
	fgets (comando_inicial, TAM-1, stdin);
	comando_inicial[strlen(comando_inicial)-1] = '\0';

	if(!strcmp( "connect", comando_inicial )){
			printf("insert username with the following format (user@number_ip:port)\n");
			printf(">");
			memset( connection, '\0', TAM );
	        fgets (connection, TAM-1, stdin);
	        connection[strlen(connection)-1] = '\0';
	        strcpy( aux,connection );
	        strcpy( aux2,connection );
	        if(strchr(aux2,'@')== NULL || strchr(aux2,':') == NULL){
	        	perror( "INVALID FORMAT ERROR: expect format *username@ip_number:port* " );
		        exit(EXIT_FAILURE);
	        }
	        else{
	        	set_data(aux);
	        }

		}
		else{
				perror( "INVALID FORMAT ERROR: expect connect " );
		        exit(EXIT_FAILURE);
	        
		}
}
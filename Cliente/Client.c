#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
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
	char buffer[TAM],user[TAM],command[TAM],received[TAM],currentdir[TAM],buff[TAM],buf2[30],buffer2[1024000]; //variables de uso auxiliar
	    
	

	puerto = data.number_port;
	fprintf( stderr, "Using port %i host\n", puerto); 

	
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd < 0 ) {
		perror( "ERROR apertura de socket" );
		exit( 1 );
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
	bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length );
	serv_addr.sin_port = htons( puerto );
	if ( connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {
		perror( "conexion" );
		exit( 1 );
	}
		printf("Enter the username\n");
		printf(">");
		memset( user, '\0', TAM );
		fgets( user, TAM-1, stdin );
		
        m = write(sockfd, user, strlen(user));
        if (m < 0) {
            perror("Error sending message");
            exit(1);
        } 
        user[strlen(user)-1] = '\0';
        memset( user, '\0', TAM );
        m = read(sockfd, user, sizeof(user));
       if (m < 0) {
            perror("Error receiving message");
            exit(1);
        }
        else {
        	if(!strcmp("CORRECT",user)){
        		printf("User correct\n");
        	}
        	else {
        		printf("User incorrect \n");
        		exit(0);
        	}
        }
        printf("Enter the password :\n");
        printf(">");
        memset( buffer, '\0', TAM );
		fgets( buffer, TAM-1, stdin );
		
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Error sending message");
            exit(1);
        } 
        buffer[strlen(buffer)-1] = '\0';
        memset( buffer, '\0', TAM );
        n = read(sockfd, buffer, sizeof(buffer));
       	if (n < 0) {
            perror("Error receiving message");
            exit(1);
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
			exit( 1 );
		}
   		n = read( sockfd, currentdir, TAM );
		if ( n < 0 ) {
			perror( "reading to socket" );
			exit( 1 );
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
   			exit(0);
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
				exit( 1 );
			}
		}
   		else{
   		
    		n = write(sockfd, command, strlen(command));
   		}
     	if ( n < 0 ) 
     	{
			perror( "writing to socket" );
			exit( 1 );
		}

		n = read( sockfd, received, TAM );
		if ( n < 0 ) {
			perror( "reading to socket" );
			exit( 1 );
		}
		printf("\n\n");
		printf("%s\n",received );
		if (udp.flag)
		{	
			struct sockaddr_in si_me;
		     
		    //se crea el socket UDP
		    if ((socket_file_udp=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		    {
		        perror("socket");
		        exit(1);
		    }
		    memset((char *) &si_me, 0, sizeof(si_me));
		     
		    si_me.sin_family = AF_INET;
		    si_me.sin_port = htons(udp.port_udp);
		    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
		    memset(&(si_me.sin_zero), '\0', 8);
		     
		    //se crea el enlace con el puerto
		    if( bind(socket_file_udp , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
		    {
		        perror("bind");
		        exit(1);
		    }

		    socklen_t slen = sizeof(si_me);
		    char buffer_udp[TAM];
			int  recv_len,leidos;
			FILE *f;

			while(udp.flag)
			{

			    f = fopen(udp.file,"wb");
				
				if (f==NULL)
			    {
			       perror("No se puede abrir fichero.dat");
			       exit(1);
			    }
				memset(buffer_udp, 0, TAM);
				if ((recv_len = recvfrom(socket_file_udp, buf2, 30, 0, (struct sockaddr *) &si_me, &slen)) == -1)
	            {
	            	perror("recvfrom()");
	            	exit(1);
	            }

				leidos = sizeof(buf2) / sizeof(char);
				
				if ((recv_len = recvfrom(socket_file_udp, buffer2, 1024000, 0, (struct sockaddr *) &si_me, &slen)) == -1)
	            {
	            	perror("recvfrom()");
	            	exit(1);
	            }
	            fwrite (buffer2, 1, leidos, f);
	            udp.flag = 0;
				 fclose(f);
			}

			close(socket_file_udp);		
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
		exit( 1 );
	}
	strcpy(aux,strtok(NULL,"."));
	if(strlen(aux)>3 || atoi(aux)>255 || atoi(aux)<0){
		perror("INVALID FORMAT ERROR: expect xxx.xxx.xxx.xxx or localhost, or doesn't belong to an ip address" );
			printf("%s\n",aux );
			exit( 1 );
	}
	strcpy(aux,strtok(NULL,"."));
	if(strlen(aux)>3 || atoi(aux)>255 || atoi(aux)<0){
		perror("INVALID FORMAT ERROR: expect xxx.xxx.xxx.xxx or localhost, or doesn't belong to an ip address" );
			printf("%s\n",aux );
			exit( 1 );
	}
	
	strcpy(aux,strtok(NULL,"\0"));
	if(strlen(aux)>3 || atoi(aux)>255 || atoi(aux)<0){
		perror("INVALID FORMAT ERROR: expect xxx.xxx.xxx.xxx or localhost, or doesn't belong to an ip address" );
			printf("%s\n",aux );
			exit( 1 );
	}
}
else if (!strcmp("localhost",aux)){
 strcpy(ip,"127.0.0.1");
}
else
{
	perror("INVALID FORMAT ERROR: expect xxx.xxx.xxx.xxx or localhost" );
	exit( 1 );
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
		        exit( 1 );
	        }
	        else{
	        	set_data(aux);
	        }

		}
		else{
				perror( "INVALID FORMAT ERROR: expect connect " );
		        exit( 1 );
	        
		}
}
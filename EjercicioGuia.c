#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>
#include <pthread.h>

//MYSQL//
int err;
char consulta[200];//Lo usamos para preparar las conultas
MYSQL *conn;//Conector para acceder al servidor de bases de datos
MYSQL_RES *resultado;//variable para comprobar errores
MYSQL_ROW row;

//SOCKETS//
int sock_conn, sock_listen, ret;
char buff[512];
char buff2[512];




//Da respuesta al resultado del Check_Log
//Devuelve 0 si se loguea bien
//Devuelve 1 si no
void Convertir_Faren_a_Cel (int grados)
	//Comprueba el nombre del usuario
{
	int convertido = 0;
	convertido = grados - 273 ;
	char mensaje [20];
	sprintf(mensaje, "1/%d",convertido);
	write(sock_conn, mensaje, sizeof(mensaje));
	
	
}

void Convertir_Cel_a_Faren (int grados)
{
	int convertido = 0;
	convertido = grados + 273 ;
	char mensaje [20];
	sprintf(mensaje, "2/%d",convertido);
	write(sock_conn, mensaje, sizeof(mensaje));
	
}
//comprueba en que posicion esta el usuario





void *AtenderCliente(void *socket)
{
	
	
	int sock_conn;
	int *s;
	s = (int *) socket;
	sock_conn = *s;
	
	int select; //selecciona que esta haciendo el usuario
	char mail[20]; //mail que arrancamos
	char password[20]; //password que arrancamos
	char nombre[20]; //nombre que arrancamos
	char cuestionador[20];
	int cuestion;
	int terminar = 0;
	
	
	while(terminar == 0)
	{
		
		ret=read(sock_conn,buff, sizeof(buff));
		buff[ret] = '\0';
		
		char *p = strtok(buff, "/");
		
		select = atoi(p);
		
		if(select == 1) 
		{
			int grados;
			p = strtok (NULL, "/");
			grados = atoi(p);
			Convertir_Cel_a_Faren (grados);
			select = 100;
		}
		if(select == 2) //Log In
		{
			int *grados;
			p = strtok (NULL, "/");
			grados = atoi(p);
			
			Convertir_Faren_a_Cel(grados);
			select = 100;
		}
		
		
		
		
		
	}
	close (sock_conn);
}

int main(int argc, char *argv[])
{
	//MYSQL//
	//Creamos una conexion al servidor MYSQL
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	//inicializar la conexion, indicando nuestras claves de acceso
	// al servidor de bases de datos (user,pass)
	conn = mysql_real_connect (conn, "localhost","root", "mysql", NULL, 0, NULL, 0);
	if (conn==NULL)
	{
		printf ("Error al inicializar la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	//entramos en la base de datos "juego"
	err=mysql_query (conn, "use juego");
	if (err!=0)  //por si la base de datos no existe
	{
		printf("Error al seleccionar la base de datos\n");
		exit (1);
	}
	
	//SOCKETS//
	struct sockaddr_in serv_adr;
	
	
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina.
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050 o parecidos en caso de error de blind
	serv_adr.sin_port = htons(9080);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 2) < 0)
		printf("Error en el Listen");
	
	
	int i = 0;
	int sockets[100];
	pthread_t thread[100];
	
	for(;;)
	{
		printf ("Escuchando\n");
		//sock_conn es el socket que usaremos para este cliente
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("Conexion establecida\n");
		
		sockets[i] = sock_conn;
		pthread_create (&thread[i], NULL, AtenderCliente, &sockets[i]);
		
	}
	
	mysql_close(conn);
}

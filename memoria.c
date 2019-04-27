#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>


struct log{
  char IP[16];
  char PUERTO[5];
};



void *funcionServer(char *PUERTO) {
  struct sockaddr_in direccionServer;
  direccionServer.sin_family = AF_INET;
  direccionServer.sin_addr.s_addr = INADDR_ANY;
  direccionServer.sin_port = htons(atoi(PUERTO));

  int server = socket(AF_INET,SOCK_STREAM,0);

  if(bind(server, (void*) &direccionServer,sizeof(direccionServer)) != 0){
    perror("[-] Fallo en el bind. \n");
    exit(-1);
  }

  printf("[+] Server escuchando..\n" );
  listen(server,10);

  struct sockaddr_in direccionCliente;
  unsigned int lenDireccion;

  int cliente = accept(server, (void *) &direccionCliente, &lenDireccion);

  printf("[+] Se conectó el cliente %d\n", cliente );

  char paquete[] = "[+] Mensaje recibido!";
  char* buffer = malloc(5);

  int bytesRecibidos = recv(cliente,buffer,1000,0);

  printf("[+] Mensaje del cliente : %s\n",buffer );
  free(buffer);

  send(cliente,paquete,sizeof(paquete),0);

  for(;;);

}


void *funcionClient(struct log *memoria){
  struct sockaddr_in direccionServer;
  char paquete[] = "Este es un mensaje del cliente";
  char buffer[1024];

  printf("IP: %s\n",memoria -> IP );
  printf("PUERTO: %d\n",atoi(memoria->PUERTO) );

  direccionServer.sin_family = AF_INET;
  direccionServer.sin_addr.s_addr = inet_addr(memoria->IP);
  direccionServer.sin_port = htons(atoi(memoria->PUERTO));

  int cliente = socket(AF_INET,SOCK_STREAM,0);
  if(connect(cliente,(void *) &direccionServer,sizeof(direccionServer)) != 0){
    perror("[-] Error al conectarse con el servidor \n");
    exit(-1);
  }

  send(cliente,paquete,strlen(paquete),0);
  recv(cliente,buffer,1024,0);
  printf("Buffer: %s\n",buffer );

}



int main(int argc, char *argv[]) {

  pthread_t servidor;
  pthread_t cliente;

  struct log memoria1 ;

  FILE *logIps;
  FILE *logPorts;
  FILE *miLog;

//  char direccionIP[16];
  //char direccionPuerto[5];
  char miPuerto[5];


  //creamos la struct con los datos sacados del log

  //leemos la ip de otra memoria para conectarnos
  logIps = fopen("ips.txt","r");
  fscanf(logIps,"%s",memoria1.IP);
  fclose(logIps);

  //leemos el puerto para conectarnos
  logPorts = fopen("puertos.txt","r");
  fscanf(logPorts,"%s",memoria1.PUERTO);
  fclose(logPorts);

  //abrimos nuestro log, y nos fijamos que puerto tenemos asignado
  miLog = fopen("miLog.txt","r");
  fscanf(miLog,"%s",miPuerto);
  fclose(miLog);

  printf("Mi puerto: %s\n",miPuerto);


  pthread_create(&servidor,NULL, &funcionServer, miPuerto);
  //pthread_create(&cliente,NULL, &funcionClient,(void *) &memoria1); //pasamos puerto y direccion con un struct
  pthread_join(servidor,NULL);
  //pthread_join(cliente,NULL);



  return 0;
}

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 1337

int checksum(char pacote[], int size) {
  int soma = 0;

  for (int i = 0; i < size; i++) {
    soma += pacote[i];
    soma %= 128;
  }

  return soma;
}

typedef enum { false, true } bool;

int main() {
  int porta, connector;
  ssize_t ler_bytes, escrever_bytes;
  int socket_cliente, binder;
  int nPacotes = 0, tam_buffer = 4096;
  struct sockaddr_in serv_addr;
  char nome_arquivo[200], resposta[30];

  socket_cliente = socket(AF_INET, SOCK_DGRAM,
                          0);  // Cria uma conexão socket no socket_cliente

  if (socket_cliente <= 0) {
    printf("Erro no socket: %s\n", strerror(errno));
    exit(1);
  }

  bzero(&serv_addr,
        sizeof(serv_addr));  // da memset no valor 0 em todo serv_addr

  serv_addr.sin_family =
      AF_INET;  // Termo que guarda o tipo de conexão (ipv4,ipv6...)
  serv_addr.sin_port = htons(PORT);  // Guarda a porta a se checar

  // Recebe o socket, a struct com as informações do server e o tamanho da
  // struct com as informações do server Essa função conecta cliente ao servidor
  // e retorna negativo se a conexão falhou
  binder = bind(socket_cliente, (const struct sockaddr *)&serv_addr,
                sizeof(serv_addr));
  if (connector < 0) {
    fprintf(stderr, "%s", "Falha na conecao\n");
    exit(1);
  } else {
    printf("[SERVIDOR CONECTADO]\n\n");
  }

  //***************************************************************
  //			           REQUISIÇÃO DE ARQUIVO
  //***************************************************************

  printf("Qual arquivo pedir: ");
  scanf("%s", nome_arquivo);

  // fgets(nome_arquivo, sizeof(nome_arquivo), stdin); //lê mensagem do terminal
  // e
  escrever_bytes =
      sendto(socket_cliente, nome_arquivo, sizeof(nome_arquivo), 0,
             (const struct sockaddr *)&serv_addr,
             sizeof(serv_addr));  // envia pelo socket_cliente o nome_arquivo

  if (escrever_bytes == 0) {  // se vc n enviar nada
    printf("Erro no write: %s\n", strerror(errno));
    printf("Nada escrito.\n");
    exit(1);
  }
  int servlen = sizeof(serv_addr);
  escrever_bytes = recvfrom(socket_cliente, &resposta, sizeof(nome_arquivo), 0,
                            (struct sockaddr *)&serv_addr, &servlen);

  if (escrever_bytes <= 0) {  // se vc n enviar nada
    printf("%s\n", strerror(errno));
    exit(1);
  }
  //***************************************************************
  //			         FIM REQUISIÇÃO DE ARQUIVO
  //***************************************************************
  FILE *file;
  char pacote[4099];
  int ack[2];
  ssize_t ler;


  //***************************************************************
  //			           RECEBIMENTO DO ARQUIVO
  //***************************************************************

  file = fopen(nome_arquivo, "wb");

  while (1) {
    int contador = 0;

    ler = recvfrom(socket_cliente, pacote, sizeof(pacote), 0,
                   (struct sockaddr *)&serv_addr, &servlen);

      while (pacote[4096] != checksum(pacote, tam_buffer)) {
      if (contador == 3) {
        printf("Falha ao transferir arquivo checksum não bateu\n");
        return 0;
      }

      /*
        Pacotes vem fora de ordem, precisamos escrever o arquivo
        pacote a pacote sem ser sequencial, escrever em: numero de sequencia * 4096
      */


      contador++;
      printf("Pacote corrompido, tentativa %d\n", contador);
      ler = recvfrom(socket_cliente, pacote, sizeof(pacote), 0,
                     (struct sockaddr *)&serv_addr, &servlen);
    }

    nPacotes++;
    fwrite(pacote, 1, 4096, file);

    if (pacote[4097] == 1) {
      printf("Ultimo pacote = %d\n", nPacotes);
      break;
    }

    // ack[0] = 1;
    // ack[1] = pacote[];
    // write(socket_cliente,ack,sizeof(ack));
  }
  printf("Pacotes: %d\n", nPacotes);
  printf("done\n");

  fclose(file);
  close(socket_cliente);  // Fecha a conexão socket

  return 0;
}
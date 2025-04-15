// client.c
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
 

#define PORT      8080
#define SERVER_IP "127.0.0.1"
 

int main() {
   int                 sockfd;
   struct sockaddr_in  server_addr;
   SSL_CTX            *ctx;
   SSL                *ssl;
 

   // Initialize OpenSSL
   SSL_library_init();
   OpenSSL_add_all_algorithms();
   SSL_load_error_strings();
 

   // Create SSL context
   ctx = SSL_CTX_new(TLS_client_method());
   if (!ctx) {
      perror("SSL_CTX_new");
      exit(1);
   }
 

   // Create socket
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
      perror("socket");
      exit(1);
   }
 

   // Connect to server
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family                                    = AF_INET;
   server_addr.sin_port                                      = htons(PORT);
   if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
      perror("inet_pton");
      exit(1);
   }
   if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
      perror("connect");
      exit(1);
   }
 

   // Create SSL connection
   ssl = SSL_new(ctx);
   SSL_set_fd(ssl, sockfd);
 

   // Perform SSL handshake
   if (SSL_connect(ssl) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(1);
   }
 

   // Send and receive data
   SSL_write(ssl, "Hello from client!", strlen("Hello from client!"));
   char buffer[1024];
   int bytes_read        = SSL_read(ssl, buffer, sizeof(buffer) - 1);
   if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      printf("Received: %s\n", buffer);
   } else {
      ERR_print_errors_fp(stderr);
   }
 

   // Close connection
   SSL_free(ssl);
   close(sockfd);
   SSL_CTX_free(ctx);
 

   return 0;
}

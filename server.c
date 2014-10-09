#include "header.h"

void main() {
  int listenfd,client_fd; 
  socklen_t  sin_size;
  FILE  *cfp;
  int yes = 1 ,numbytes;;
  struct sockaddr_in server;
  struct sockaddr_in client;
  char buf[100] , path[100] , path_file[100];

  strcpy(path , SERVER_ROOT);

  if((listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
    perror("socket error");
    exit(1);
  }

  if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&yes , sizeof(int)) == -1) {
    perror("setsockopt error");
    exit(1);
  }

  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  server.sin_addr.s_addr = htons(INADDR_ANY);
  bzero( &(server.sin_zero),8);
  if(bind(listenfd, (struct sockaddr *)&server , sizeof(struct sockaddr)) == -1) {
    perror("bind error");
   exit(1);
  }

  if(listen(listenfd,BACKLOG) == -1) {
    perror("listen error");
    exit(1);
  }

  sin_size = sizeof(struct sockaddr_in);

  while(1) {
    if((client_fd = accept(listenfd, (struct sockaddr *) &client, &sin_size)) == -1) {
    perror("accept error");
    continue;
    }

    if(!fork()) {

        if ((numbytes=recv(client_fd, buf, 99, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        get_path(buf , path_file);
        strcat(path , path_file);

        cfp = fdopen(client_fd , "w");

        if(access(path , 0) != -1) {
            write_path(cfp , path);
        } else {
            error_path(cfp);
        }

        fflush(cfp);
        exit(0);
    }

    close(client_fd);
  }

}


int get_path(char *require , char *path_file) {
    char command[BUFSIZ];     
    char arguments[BUFSIZ]; 

    if (sscanf(require, "%s%s", command, arguments) != 2) {
         return;  
    }

    if(strlen(arguments) == 1) {
        strcpy(arguments , "/index.html");
    }

    strcpy(path_file , arguments);
}

int write_path(FILE *cfp , char *path) {
    FILE *fp;
    char str[MAX_FLUSH];

    fp =fopen(path , "r");

    write_header(cfp , path);
    while(!feof(fp)) {
        if(fgets(str,128,fp)!=NULL) write_page(cfp , str);
    }

   fclose(fp);
}

int error_path(FILE *cfp) {
    fprintf(cfp , "Http/1.1 404 NOT FOUND\r\n");
    fprintf(cfp , "Server: Shine/1.0\r\n");
    fprintf(cfp , "Date: Wed, 18 Apr 2012 13:50:08 GMT\r\n");
    fprintf(cfp , "Content-Type: text/html; charset=utf-8\r\n");
    fprintf(cfp , "\n");

    write_page(cfp , "<!DOCTYPE HTML PUBLIC '-//IETF//DTD HTML 2.0//EN'>\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\n<p>The requested URL found
on this server.</p>\n</body></html>");

    return 0;
}

int write_header(FILE *cfp , char *path) {
    char *type;
    int n = strlen(path);

    if(strcasecmp(&path[n-3] , "jpg") == 0 || strcasecmp(&path[n-4] , "jpeg") == 0) {
        type = "image/jpeg";
    }else if(strcasecmp(&path[n-3] , "gif") == 0) {
        type = "image/gif";
    }else if(strcasecmp(&path[n-3] , "png") == 0) {
        type = "image/png";
    }else{
        type = "text/html";
    }

    fprintf(cfp , "Http/1.1 200 OK\r\n");
    fprintf(cfp , "Server: Shine/1.0\r\n");
    fprintf(cfp , "Date: Wed, 18 Apr 2012 13:50:08 GMT\r\n");
    fprintf(cfp , "Content-Type: %s; charset=utf-8\r\n" , type);
    fprintf(cfp , "\n");

    return 0;
}

int write_page(FILE *cfp , char *str) {
    fprintf(cfp , "%s" , str);

    return 0;
}



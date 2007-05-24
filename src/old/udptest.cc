//#include <gtkmm.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <gtkmm.h>
#include <gdkmm.h>


using namespace std; 


int main(int argc, char *argv[])
{
  int sockfd, portno (2000), n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  serv_addr.sin_port = htons(portno); 


  sockfd=socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);

  if(bind(sockfd,(struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    {
      cerr << "can't bind to socket "  << endl;
    }
  
  char test[1000]; 
  int result = recv(sockfd, test, 1000, 0); 

  Gtk::Main::run(win);
 
  cout << "Read " << result << " bytes! " << endl; 
  

  close (sockfd); 

}

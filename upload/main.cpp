#include <stdio.h>
#include <winsock2.h>

#include <string>

#include "ftplib.h"

#include "mtlog.h"


const char* IN_FNAME = "upload_template.html";
const char* OUT_FNAME = "index.html";
const char MODE = 'I';


Logging g_logFile("ftp_upload");

class WSAInit
{
public:
  WSAInit()
  {
    LOGL(g_logFile, "invoked");

    WORD ver = MAKEWORD(2, 2);
    WSADATA ws;
    WSAStartup(ver, &ws);
  }

  ~WSAInit()
  {
    WSACleanup();
  }
};

int main(int argc, char** argv)
{
  try
  {
    WSAInit init;

    char host_name[1024];
    gethostname(host_name, sizeof(host_name));
    //LOGL(g_logFile, "hostname: " << host_name);

    struct hostent* he = gethostbyname(host_name);
    if( !he )
      throw std::string("Failed to gethostbyname, error: ") + itoa(WSAGetLastError(), host_name, 10);

    LOGL(g_logFile, "hostname: " << he->h_name);

    struct in_addr addr;
    memcpy(&addr, he->h_addr_list[0], sizeof(addr));
    const char* ip = inet_ntoa(addr);
    LOGL(g_logFile, "IP: " << ip);

    FILE* fs = fopen(IN_FNAME, "r");
    if( !fs )
      throw std::string("Failed to open file: ") + IN_FNAME + ", error: " + strerror(errno);

    char buf_in[10240];
    size_t br = fread(buf_in, 1, sizeof(buf_in), fs);
    buf_in[br] = 0;
    fclose(fs);

    char buf_out[10240];
    sprintf(buf_out, buf_in, ip);

    fs = fopen(OUT_FNAME, "w");
    if( !fs )
      throw std::string("Failed to open file: ") + OUT_FNAME + ", error: " + strerror(errno);

    fwrite(buf_out, 1, strlen(buf_out), fs);

    fclose(fs);
  }
  catch( const std::string& ex )
  {
    LOGE(g_logFile, "exception: " << ex.data());
  }

  try
  {
    FtpInit();
    netbuf* conn = NULL;

    const char* host = "upload.attbi.com";
    const char* user = "andrewt";
    const char* pass = "Natalie";

    if( !FtpConnect(host, &conn) )
    {
      const char* lr = ftplib_lastresp;
      throw std::string("Failed to connect to host: ") + host + ", last response: " + (lr ? lr : "");
    }

    if( !FtpLogin(user, pass, conn) )
      throw std::string("Failed to login, last response: ") + FtpLastResponse(conn);

    FtpOptions(FTPLIB_CALLBACK, (long) NULL, conn);

    if( !FtpPut(OUT_FNAME, OUT_FNAME, MODE, conn) )
      throw std::string("Failed to send file: ") + OUT_FNAME + ", last response: " + FtpLastResponse(conn);

    FtpClose(conn);

    LOGL(g_logFile, "success");
  }
  catch( const std::string& ex )
  {
    LOGE(g_logFile, "exception: " << ex.data());
  }
  return 0;
}

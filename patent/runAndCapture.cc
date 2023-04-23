#include <stdexcept>
#include <string>
#include <cerrno>

#include "util/Formatter.h"


int const BUF_SIZE = 1024 * 10;

/*! Runs an external command and captures its output
 */
std::string runAndCapture(std::string const& command)
{
  // redirect stderr to stdout to capture it too
  std::string redir_command = command + "  2>&1";

  //std::cout << "redir_command: " << redir_command << std::endl;

  FILE* pipe = popen(redir_command.c_str(), "r");
  if( !pipe )
    throw std::runtime_error(Formatter("Failed to popen command: ") << redir_command
      << ", error: " << strerror(errno));

  std::string out;

  char buf[BUF_SIZE];
  size_t rb = 0;
  while( (rb = fread(buf, 1, sizeof(buf), pipe)) > 0 )
    out.append(buf, rb);

  pclose(pipe);

  //std::cout << "out: " << out << std::endl;

  return out;
}

#include "ftp_mod.hpp"
#include "ftp_server.hpp"
#include "task.hpp"

namespace ftp
{
  static std::unique_ptr<espp::FtpServer> ftp_server;

  void start_server_task()
  {
    if (ftp_server)
    {
      return;
    }

    espp::Logger logger({.tag = "FTP", .level = espp::Logger::Verbosity::DEBUG});

    logger.info("Initializing FTP Server...");
    ftp_server = std::make_unique<espp::FtpServer>("192.168.1.13", 21, espp::FileSystem::get().get_root_path());
    ftp_server->start();
    logger.info("FTP Server started");
  }

  void stop_server_task()
  {
    ftp_server.reset(); // Destructor handles cleanup
  }
}

///
/// \file   Server.cxx
/// \author Adam Wegrzynek
///

// TObject2Json
#include "TObject2Json/TObject2JsonFactory.h"

using o2::quality_control::tobject_to_json::TObject2JsonFactory;

int main(int argc, char *argv[])
{
  std::string url = "mysql://root:@localhost/adam";
  std::string zeromqUrl = "tcp://localhost:5555";

  auto converter = TObject2JsonFactory::Get(url, zeromqUrl);
  converter->start();
  return 0;
}

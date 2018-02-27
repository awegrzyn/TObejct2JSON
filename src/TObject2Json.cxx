// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file   TObejct2Json.cxx
/// \author Vladimir Kosmala
/// \author Adam Wegrzynek
///

// TObject2Json
#include "TObject2Json/TObject2Json.h"
#include "Backends/MySql.h"
#include "QualityControl/QcInfoLogger.h"

// ZMQ
#include <zmq.h>

// Boost
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace o2::quality_control::core;
using namespace std::string_literals;

namespace o2 {
namespace quality_control {
namespace tobject_to_json {

TObject2Json::TObject2Json(std::unique_ptr<Backend> backend, std::string zeromqUrl)
  : mBackend(std::move(backend))
{
  void *context = zmq_ctx_new();
  mZeromqSocket = zmq_socket(context, ZMQ_REP);
  int rc = zmq_bind(mZeromqSocket, zeromqUrl.data());
  if (rc == 0) {
    throw std::runtime_error("Couldn't bind the socket "s + zmq_strerror(zmq_errno()));
  }
  QcInfoLogger::GetInstance() << "ZeroMQ server: Socket bound " << zeromqUrl << infologger::endm;
}

string TObject2Json::handleRequest(string request)
{
  // Check empty messagae
  if (request.length() == 0) {
    QcInfoLogger::GetInstance() << "Empty request received, ignoring..." << infologger::endm;
    return "";
  }

  QcInfoLogger::GetInstance() << "Received request (" << request << ")" << infologger::endm;

  // Split request into command and arguments
  vector<string> parts;
  boost::split(parts, request, boost::is_any_of(" "));
  
  if (parts.size() != 2) {
    QcInfoLogger::GetInstance() << "! Service requires 2 arguments" << infologger::endm;
  }
  
  string agentName = parts[0];
  string objectName = parts[1];
  try {
    return mBackend->getJsonObject(agentName, objectName);
  } catch (const std::exception& error) {
    return "500 unhandled error";
  }
}

void TObject2Json::start()
{
  while(1) {
    // Wait for next request from client inside a zmq message
    zmq_msg_t messageReq;
    zmq_msg_init(&messageReq);
    int size = zmq_msg_recv(&messageReq, mZeromqSocket, 0);
    if (size == -1) {
      throw std::runtime_error("Unable to receive zmq message: "s + zmq_strerror(zmq_errno()));
    }

    // Process message
    string request((const char*)zmq_msg_data(&messageReq), size);
    zmq_msg_close(&messageReq);
    string response = handleRequest(request);
    QcInfoLogger::GetInstance() << "Debug: ZMQ server: sending back " << response << infologger::endm;

    // Send back response inside a zmq message
    zmq_msg_t messageRep;
    zmq_msg_init_size(&messageRep, response.size());
    memcpy(zmq_msg_data(&messageRep), response.data(), response.size());
    size = zmq_msg_send(&messageRep, mZeromqSocket, 0);
    if (size == -1) {
      throw std::runtime_error("Unable to send zmq message: "s + zmq_strerror(zmq_errno()));
    }
    QcInfoLogger::GetInstance() << "Debug: ZMQ server: sent back " << size << " bytes" << infologger::endm;
    zmq_msg_close(&messageRep);
  }
}

} // namespace tobject_to_json
} // namespace quality_control
} // namespace o2
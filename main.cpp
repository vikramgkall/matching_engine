
#include <cstdint>
#include <iostream>
#include <iomanip>
#include "Exchange.hpp"
#include "common.hpp"
#include "exchange_impl.hpp"


namespace {
  unsigned char const enter_order[] = {
    0x4f, 0x00, 0x00, 0x00, 0x2a, 0x42, 0x00, 0x00, 0x03, 0xe8,
    0x43, 0x53, 0x43, 0x4f, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x15, 0xf4, 0x00, 0x59, 0x52, 0x59,
    0x4e, 0x4f, 0x72, 0x64, 0x65, 0x72, 0x20, 0x31, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x06, 0x05, 0x05, 0x00,
    0x00, 0x00, 0x64
  };
}
int main() {
  using ExchangeImpl = mstu::vikram::exchange::ExchangeImpl;
  using ExchangeSettings = mstu::vikram::exchange::ExchangeSettings;
  mstu::exch::Exchange<ExchangeImpl> exchange(ExchangeSettings{});

  using OuchMsg = mstu::exch::ProtocolMsg<mstu::exch::Protocol::OUCH>;
  auto order_entry = exchange.Connect<mstu::exch::Protocol::OUCH>(
    mstu::exch::Endpoint("foo:bar"), [](OuchMsg data) {
      for (std::size_t i = 0; i < data.size(); i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)data[i] << " ";
      }
      std::cout << std::endl;
    });
  order_entry.Post(OuchMsg(enter_order, sizeof(enter_order)));
  mstu::vikram::exchange::EnterOrder msg = *reinterpret_cast<const mstu::vikram::exchange::EnterOrder*>(enter_order);
  msg.user_ref_num_ = htonl(htonl(msg.user_ref_num_) + 1);
  order_entry.Post(OuchMsg(&msg, sizeof(msg) + htons(msg.appendage_length_)));
  msg.side_ = 'D';
  msg.user_ref_num_ = htonl(htonl(msg.user_ref_num_) + 1);
  order_entry.Post(OuchMsg(&msg, sizeof(msg) + htons(msg.appendage_length_)));
}
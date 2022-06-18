#pragma once

#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "common.hpp"
#include "enum.hpp"
#include "messages.hpp"

#if _MSC_VER
#include <winsock.h>
#elif defined (__GNUC__)
#include <arpa/inet.h>
#endif

namespace mstu::vikram::exchange {

template<typename PostableT>
class Gateway {
public:
  Gateway(PostableT postable) noexcept
    : postable_(postable) {}

  void Post(exch::ProtocolMsg<exch::Protocol::OUCH>const& data) {
    assert(data.size());
    switch (data[0]) {
    case 'O':
      const EnterOrder * msg = reinterpret_cast<const EnterOrder*>(data.data());
      if (ValidateUserSeqNum(htonl(msg->user_ref_num_))) {
        UpdateLastSeqNum(htonl(msg->user_ref_num_));
        RejectReason r = msg->Validate();
        if (r == RejectReason::kSuccess) {
          OrderAccepted ret_msg;
          ret_msg.user_ref_num_ = msg->user_ref_num_;
          ret_msg.timestamp_ = 0;
          ret_msg.side_ = msg->side_;
          ret_msg.quantity_ = msg->quantity_;
          std::memcpy(ret_msg.symbol_, msg->symbol_, sizeof(ret_msg.symbol_));
          ret_msg.price_ = msg->price_;
          ret_msg.time_in_force_ = msg->time_in_force_;
          ret_msg.display_ = msg->display_;
          ret_msg.order_reference_number_ = 0;
          ret_msg.capacity_ = msg->capacity_;
          ret_msg.intermarket_sweep_eligibility_ = msg->intermarket_sweep_eligibility_;
          ret_msg.cross_type_ = msg->cross_type_;
          ret_msg.order_state_ = 'L';
          std::memcpy(ret_msg.cl_ord_id_, msg->cl_ord_id_, sizeof(ret_msg.cl_ord_id_));
          ret_msg.appendage_length_ = msg->appendage_length_;
          char buffer[512];
          std::memcpy(buffer, &ret_msg, sizeof(ret_msg));
          if (msg->appendage_length_) {
            assert(data.size() == sizeof(*msg) + htons(msg->appendage_length_));
            std::memcpy(buffer + sizeof(ret_msg), data.data() + sizeof(*msg), htons(msg->appendage_length_));
          }
          exch::ProtocolMsg<exch::Protocol::OUCH> raw_ret_msg(reinterpret_cast<const void*>(buffer),
            sizeof(ret_msg) + htons(msg->appendage_length_));
          postable_(raw_ret_msg);
        } else {
          Rejected ret_msg;
          ret_msg.user_ref_num_ = msg->user_ref_num_;
          std::memcpy(ret_msg.cl_ord_id_, msg->cl_ord_id_, sizeof(msg->cl_ord_id_));
          ret_msg.reason = htons(static_cast<uint16_t>(r));
          exch::ProtocolMsg<exch::Protocol::OUCH> raw_ret_msg(reinterpret_cast<const void*>(&ret_msg),
            sizeof(ret_msg));
          postable_(raw_ret_msg);
        }
      }
    }
  }
private:
  using seq_num_type = decltype(EnterOrder::user_ref_num_);
  bool ValidateUserSeqNum(seq_num_type seq_num) noexcept {
    //assert(last_seq_num_ != std::numeric_limits<uint32_t>::max());
    return (seq_num > last_seq_num_);
  }
  void UpdateLastSeqNum(seq_num_type seq_num) noexcept {
    assert(seq_num > last_seq_num_);
    last_seq_num_ = seq_num;
  }
  PostableT postable_;
  seq_num_type last_seq_num_{0};
};

struct ExchangeSettings {
  std::vector<mstu::exch::AccountEntry> accounts{};
  std::string json_options{""};
};

class ExchangeImpl {

public:
  explicit ExchangeImpl(ExchangeSettings const & settings) noexcept
    : settings_(settings)
  {
  }

  template <typename mstu::exch::Protocol protocol, typename PostableT>
  Gateway<PostableT> Connect(mstu::exch::Endpoint const& endpoint,
                  PostableT postable)
  {
    settings_.accounts.emplace_back(mstu::exch::AccountEntry());
    settings_.accounts.back().account = exch::AccountId::Anonymous;
    settings_.accounts.back().endpoint = endpoint;
    settings_.accounts.back().protocol = protocol;
    return Gateway<PostableT>(postable);
  }

private:
  ExchangeSettings settings_;
};

}

#pragma once

#include <cstdint>
#include "enum.hpp"
#include "utils.hpp"

namespace mstu::vikram::exchange {

#if _MSC_VER
#pragma pack(push, 1)
struct EnterOrder {

  RejectReason Validate() const noexcept {
    if (auto e = Validate::Side_(static_cast<Side>(side_));
      e != RejectReason::kSuccess) { return e; }
    /*if (auto e = Validate::TIF_(static_cast<TIF>(time_in_force_));
      e != RejectReason::kSuccess) { return e; }*/
    return RejectReason::kSuccess;
  }
  char type_{'O'};
  uint32_t user_ref_num_{0};
  char side_{' '};
  uint32_t quantity_{0};
  char symbol_[8];
  uint64_t price_{0};
  char time_in_force_{' '};
  char display_{' '};
  char capacity_{' '};
  char intermarket_sweep_eligibility_{' '};
  char cross_type_{' '};
  char cl_ord_id_[14];
  uint16_t appendage_length_{0};
};
struct OrderAccepted {
  char type_{'A'};
  uint64_t timestamp_{0};
  uint32_t user_ref_num_{0};
  char side_{' '};
  uint32_t quantity_{0};
  char symbol_[8]{""};
  uint64_t price_{0};
  char time_in_force_{' '};
  char display_{' '};
  uint64_t order_reference_number_{0};
  char capacity_{' '};
  char intermarket_sweep_eligibility_{' '};
  char cross_type_{' '};
  char order_state_{ ' ' };
  char cl_ord_id_[14]{""};
  uint16_t appendage_length_{0};
};
struct Rejected {
  char type_{'J'};
  uint64_t timestamp_{0};
  uint32_t user_ref_num_{0};
  uint16_t reason{0};
  char cl_ord_id_[14]{""};
};
#pragma pack(pop)
#elif defined (__GNUC__)
struct EnterOrder {
  char type_{'O'};
  uint32_t user_ref_num_{0};
  char side_{' '};
  uint32_t quantity_{0};
  char symbol_[8];
  uint64_t price_{0};
  char time_in_force_{' '};
  char display_{' '};
  char capacity_{' '};
  char intermarket_sweep_eligibility_{' '};
  char cross_type_{' '};
  char cl_ord_id_[14];
  uint16_t appendage_length_{0};
} __attribute__((__packed__));
struct OrderAccepted {
  char type_{'A'};
  uint64_t timestamp_{0};
  uint32_t user_ref_num_{0};
  char side_{' '};
  uint32_t quantity_{0};
  char symbol_[8];
  uint64_t price_{0};
  char time_in_force_{' '};
  char display_{' '};
  uint64_t order_reference_number_{0};
  char capacity_{' '};
  char intermarket_sweep_eligibility_{' '};
  char cross_type_{' '};
  char order_state_{' '};
  char cl_ord_id_[14];
  uint16_t appendage_length_{'0'};
} __attribute__((__packed__));
struct Rejected {
  char type_{'J'};
  uint64_t timestamp_{0};
  uint32_t user_ref_num_{0};
  uint16_t reason{0};
  char cl_ord_id_[14]{""};
} __attribute__((__packed__));
#endif

}

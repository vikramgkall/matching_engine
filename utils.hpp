#pragma once

#include "enum.hpp"

namespace mstu::vikram::exchange {

class Validate {
public:
  static RejectReason Side_(const Side side) noexcept {
    switch (static_cast<Side>(side)) {
    case Side::kBuy:
    case Side::kSell:
    case Side::kSellShort:
    case Side::kSellShortExempt:
      return RejectReason::kSuccess;
    default:
      return RejectReason::kInvalidSide;
    }
  }
  static RejectReason TIF_(const TIF tif) noexcept {
    switch (static_cast<TIF>(tif)) {
    case TIF::kDay:
    case TIF::kIOC:
    case TIF::kGTX:
    case TIF::kGTT:
    case TIF::kAfterHours:
      return RejectReason::kSuccess;
    default:
      return RejectReason::kInvalidSide;
    }
  }
};

}

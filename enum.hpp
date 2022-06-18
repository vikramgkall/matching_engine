#pragma once

#include <cstdint>

namespace mstu::vikram::exchange {

enum class Side : std::uint8_t {
  /*B*/ kBuy = 'B',
  /*S*/ kSell = 'S',
  /*T*/ kSellShort = 'T',
  /*E*/ kSellShortExempt = 'E'
};
enum class TIF : std::uint8_t {
  /*0*/ kDay        = '0',
  /*3*/ kIOC        = '3',
  /*5*/ kGTX        = '5',
  /*6*/ kGTT        = '6',
  /*E*/ kAfterHours = 'E'
};
enum class RejectReason : std::uint16_t {
  /*0x0000*/ kSuccess,
  /*0x0001*/ kQuoteUnavailable,
  /*0x0002*/ kDestinationClosed,
  /*0x0003*/ kInvalidDisplay,
  /*0x0004*/ kInvalidMaxFloor,
  /*0x0005*/ kInvalidPegType,
  /*0x0006*/ kFatFinger,
  /*0x0007*/ kHalted,
  /*0x0008*/ kISONotAllowed,
  /*0x0009*/ kInvalidSide,
  /*0x000A*/ kProcessingError,
  /*0x000B*/ kCancelPending,
  /*0x000C*/ kFirmNotAuthorized,
  /*0x000D*/ kInvalidMinQuantity,
  /*0x000E*/ kNoClosingReferencePrice,
  /*0x000F*/ kOther,
  /*0x0010*/ kCancelNotAllowed,
  /*0x0011*/ kPeggingNotAllowed,
  /*0x0012*/ kCrossedMarket,
  /*0x0013*/ kInvalidQuantity,
  /*0x0014*/ kInvalidCrossOrder,
  /*0x0015*/ kReplaceNotAllowed,
  /*0x0016*/ kRoutingNotAllowed,
  /*0x0017*/ kInvalidSymbol,
  /*0x0018*/ kTest,
  /*0x0019*/ kLateLOCTooAggressive,
  /*0x001A*/ kRetailNotAllowed,
  /*0x001B*/ kInvalidMidpointPostOnlyPrice,
  /*0x001C*/ kInvalidDestination,
  /*0x001D*/ kInvalidPrice,
  /*0x001E*/ kSharesExceedThreshold,
  /*0x001F*/ kExceedsMaximumAllowedNotionalValue
};

}

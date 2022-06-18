#pragma once

#include "common.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace mstu::exch
{

/**
 * An extremely simplified, yet simultaneously overly generalized interface
 * into an exchange.
 *
 * The purpose is to provide an interface that can be used to interact with
 * the various components of an exchange, without defining so much as to
 * dictate implementation or even which features may or may not be supported.
 *
 * In general, the outside API to an exchange is composed of network
 * connections.  We don't want to require something so complicated, yet
 * we want to allow the implementations to be a bit free in how they
 * implement the APIs.
 *
 * So, this class provides generic interfaces to interact with the pseudo
 * exchange based on messages, allowing a plethora of implementation options.
 */
template <typename ImplT>
class Exchange
{
    ImplT impl;

public:
    /**
     * Construct an Exchange object, which will then provide access
     * points to create specific entries into the overall system.
     *
     * @param An object that contains various settings.  This type must
     *     provide at least the same API as basic_exchange_settings.
     */
    template <typename SettingsT,
              when<cncpt::basic_exchange_settings<SettingsT>> = true>
    explicit Exchange(SettingsT const & settings)
    : impl(settings)
    { }


    /**
     * Simulate a two-way communication connection with the exchange.
     *
     * @tparam Protocol The messaging protocol expectation for data that
     *     is sent back and forth between the @p client and the returned
     *     object.
     *
     * @param endpoint The exchange endpoint for the two-way connection.
     *
     * @param client The outbound side of the connection.  Must be postable.
     *     Data from the Exchange service will be sent to the client
     *     over the postable interface.
     *
     * @return A postable object which is to be used for sending data to the
     *     Exchange service.
     *
     * @code
     * using OuchMsg = exch::ProtocolMsg<exch::Protocol::OUCH>;
     * auto exchange = exch::Exchange<ExchImpl>(settings);
     * auto order_entry = exchange.Connect<exch::Protocol::OUCH>(
     *     exch::Endpoint("foo:bar"),
     *     [](OuchMsg data) { });
     * static_assert(exch::cncpt::postable<decltype(order_entry), OuchMsg>);
     * exch::PostMsg(
     *     order_entry,
     *     OuchMsg("Hello, world!", 13));
     * @endcode
     *
     * @note Messages sent to the Exchange are INBOUND, while
     *     messages sent from the Exchange are OUTBOUND.
     */
    template <Protocol protocol, typename PostableT,
              when<cncpt::postable<PostableT, ProtocolMsg<protocol>>> = true>
    auto Connect(Endpoint const & endpoint, PostableT client)
    -> decltype(impl.template Connect<protocol>(endpoint, client))
    {
        static_assert(cncpt::postable<
            decltype(impl.template Connect<protocol>(endpoint, client)),
            ProtocolMsg<protocol>>);
        return impl.template Connect<protocol>(endpoint, client);
    }


    /**
     * Subscribe to an outbound stream of messages.
     *
     * This is a one-way communication stream, outbound from the exchange.
     *
     * @tparam Protocol The messaging protocol expectation for data that
     *     is sent from the exchange to the @p client.
     *
     * @param endpoint The exchange endpoint for the outbound connection.
     *
     * @param client The outbound side of the connection.  Must be postable.
     *     Data from the Exchange service will be sent to the client
     *     over the postable interface.
     *
     * @code
     * using ItchMsg = exch::ProtocolMsg<exch::Protocol::ITCH>;
     * auto exchange = exch::Exchange<ExchImpl>(settings);
     * exchange.Subscribe<exch::Protocol::ITCH>(
     *     exch::Endpoint("mktdata:all"),
     *     [](ItchMsg data) { });
     * @endcode
     */
    template <Protocol protocol, typename PostableT,
              when<cncpt::postable<PostableT, ProtocolMsg<protocol>>> = true>
    auto Subscribe(Endpoint const & endpoint, PostableT client)
    -> decltype(impl.template Subscribe<protocol>(endpoint, client), void())
    {
        impl.template Subscribe<protocol>(endpoint, client);
    }
};

} // namespace mstu::exch


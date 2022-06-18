#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace mstu::exch {

template <bool Condition>
using when = std::enable_if_t<Condition, bool>;

template <unsigned N> struct PriorityTag : PriorityTag<N-1> { };
template <> struct PriorityTag<0> { };


/**
 * A special form of string_view for simple common data views.
 */
class DataView
: public std::basic_string_view<unsigned char>
{
    using Base = std::basic_string_view<unsigned char>;

public:
    using Base::Base;
    explicit DataView(void const * data, std::size_t size)
    : Base(static_cast<unsigned char const *>(data), size)
    { }
};


namespace protocol
{
struct OUCH { };
struct ITCH { };
}


enum class Protocol : std::uint8_t
{
    OUCH,
    ITCH,
};

template <Protocol P>
struct ProtocolMsg
: DataView
{
    static constexpr Protocol protocol = P;
    using DataView::DataView;
    explicit ProtocolMsg(DataView d)
    : DataView(d)
    { }
};



/**
 * This is a semi-opaque representation for a communication endpoint,
 * analogous to an IP address and port for networking endpoints.
 */
class Endpoint
{
    std::string v;

public:
    explicit Endpoint(std::string_view sv)
    : v(sv.data(), sv.size())
    { }

    template <typename... ArgTs,
              when<std::is_constructible_v<std::string, ArgTs...>> = true>
    explicit Endpoint(ArgTs && ... args)
    : v(std::forward<ArgTs>(args)...)
    { }

    std::string const & str() const
    {
        return v;
    }
};


namespace detail
{
template <typename TargetT, typename DataT>
auto
PostMsgImpl(TargetT & target, DataT data, int)
noexcept(noexcept(target.Post(data)))
-> decltype(target.Post(data), void())
{
    target.Post(data);
}
void Post();
template <typename TargetT, typename DataT>
auto
PostMsgImpl(TargetT & target, DataT data, long)
noexcept(noexcept(Post(target, data)))
-> decltype(Post(target, data), void())
{
    Post(target, data);
}
template <typename TargetT, typename DataT>
auto
PostMsgImpl(TargetT & target, DataT data, ...)
noexcept(noexcept(target(data)))
-> decltype(target(data), void())
{
    target(data);
}
} // namespace detail
/**
 * Post one complete message to @p target.
 *
 * @param target the receiver of the message.  The message is sent by
 *     calling either target.Post(data) or Post(target, data), or calling
 *     target(data) - in that order of preference.
 * @param data A view into the message to be sent.  If the bytes are to be
 *     kept beyond the function call, they must be copied.
 *
 * @note The bytes of data are assumed to contain one message, in a format
 *     acceptable to the target.  For example, if the target expects a
 *     MoldUDP message, then one MoldUDP message would be sent, which may
 *     contain multiple messages sent underneath the MOLD wrapper.  If the
 *     target expects an OUCH message, then the @p data should hold one
 *     OUCH message.
 *
 * @note It is assumed that the target will not throw while processing
 *     the data.  If it does throw, the exception will be silently dropped
 *     on the floor.
 */
inline constexpr auto PostMsg = [] (auto & target, auto data)
-> decltype(detail::PostMsgImpl(target, data, 0))
{
    if constexpr (noexcept(detail::PostMsgImpl(target, data, 0))) {
        detail::PostMsgImpl(target, data, 0);
    } else {
        try {
            detail::PostMsgImpl(target, data, 0);
        } catch (...) {
        }
    }
};

namespace cncpt
{
/**
 * An instance of type @p TargetT can receive a data message of type
 * @p DataT via exch::PostMsg.
 */
template <typename TargetT, typename DataT, typename = void>
struct Postable : std::false_type { };
template <typename TargetT, typename DataT>
struct Postable<
    TargetT,
    DataT,
    std::void_t<decltype(PostMsg(
            std::declval<TargetT &>(),
            std::declval<DataT>()))>>
: std::true_type { };
template <typename TargetT, typename DataT>
inline constexpr bool postable = Postable<TargetT, DataT>::value;
}


enum class AccountId : std::uint32_t
{
    Anonymous = 0
};

struct AccountEntry
{
    AccountId account;
    Protocol protocol;
    Endpoint endpoint;
};

namespace cncpt
{
namespace detail
{
template <typename T, typename U> struct is_vector_of : std::false_type { };
template <typename U, typename A>
struct is_vector_of<std::vector<U, A>, U> : std::true_type { };
}
/**
 * The concept BasicExchangeSettings<T> specifies that an instance of
 * type T can be used as a settings object.  More specifically, it meets
 * these general characteristics.
 *
 * @code
 * struct Settings
 * {
 *     std::vector<AccountEntry> accounts;
 *     std::string json_options;
 * };
 * @endcode
 */
template <typename T, typename = void>
struct BasicExchangeSettings : std::false_type { };
template <typename T>
struct BasicExchangeSettings<
    T,
    std::enable_if_t<
        detail::is_vector_of<
            decltype(std::declval<T &>().accounts),
            AccountEntry>::value &&
        std::is_same_v<
            std::string,
            decltype(std::declval<T &>().json_options)>>>
: std::true_type { };
template <typename T>
inline constexpr bool basic_exchange_settings = BasicExchangeSettings<T>::value;
}

} // namespace mstu::exch


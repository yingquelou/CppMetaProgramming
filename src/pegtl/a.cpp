#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/json.hpp>
#include <tao/pegtl/eol.hpp>
#include <tao/pegtl/file_input.hpp>
#include <tao/pegtl/istream_input.hpp>
#include <tao/pegtl/read_input.hpp>
#include <tao/pegtl/rules.hpp>
#include <tao/pegtl/string_input.hpp>
#include <type_traits>
namespace pegtl = tao::pegtl;

namespace hello {
// Parsing rule that matches a literal "Hello, ".

struct prefix
    : pegtl::string<'H', 'e', 'l', 'l', 'o', ',', ' '> { };

// Parsing rule that matches a non-empty sequence of
// alphabetic ascii-characters with greedy-matching.

struct name
    : pegtl::plus<pegtl::alpha> { };

// Parsing rule that matches a sequence of the 'prefix'
// rule, the 'name' rule, a literal "!", and 'eof'
// (end-of-file/input), and that throws an exception
// on failure.

struct grammar
    : pegtl::must<prefix, name, pegtl::one<'!'>, pegtl::eof> { };

// Class template for user-defined actions that does
// nothing by default.
template <typename T, typename... Ts>
static constexpr bool along = std::disjunction_v<std::is_same<T, Ts>...>;
template<typename T>
std::string trim(T&& s)
{
    auto begin = std::find_if_not(std::begin(s), std::end(s), ::isspace);
    auto end = std::find_if_not(std::rbegin(s), std::rend(s), ::isspace);
    return { begin, end.base() };
}
std::ofstream ofs("E:\\repo\\cpp\\cpp\\z.json");
template <typename Rule>
struct action {
    template <typename ParseInput, typename... rest>
    static void apply(const ParseInput& in, rest&&... v)
    {
        if constexpr (along<Rule, pegtl::json::key, pegtl::json::name_separator, pegtl::json::value_separator
            , pegtl::json::string, pegtl::json::begin_array, pegtl::json::end_array, pegtl::json::number, pegtl::json::null
            , pegtl::json::true_
            , pegtl::json::begin_object
            , pegtl::json::end_object
            , pegtl::json::false_>) {
            ofs << trim(in.string_view());
        }
    }
};

// Specialisation of the user-defined action to do
// something when the 'name' rule succeeds; is called
// with the portion of the input that matched the rule.

} // namespace hello
void parse_json(const std::string& filename){
    
}
int main(int argc, char* argv[])
{

    // Start a parsing run of argv[1] with the string
    // variable 'name' as additional argument to the
    // action; then print what the action put there.
    pegtl::file_input<pegtl::tracking_mode::eager, pegtl::eol::lf> mm("E:\\repo\\cpp\\cpp\\CMakePresets.json");
    pegtl::parse<pegtl::json::text, hello::action>(mm);
    return 0;
}
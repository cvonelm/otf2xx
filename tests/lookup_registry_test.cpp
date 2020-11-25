#include <otf2xx/registry.hpp>

#include <iostream>
#include <string>
#include <unistd.h>

enum class LocationType
{
    THREAD,
    CPU,
    SAMPLE_CPU,
    UNKOWN
};

struct Location
{
    LocationType type;
    int location;

    Location() : type(LocationType::UNKOWN), location(-1)
    {
    }

    Location(LocationType t, int l) : type(t), location(l)
    {
    }

    std::string name() const
    {
        return "name";
    }

    pid_t tid() const
    {
        return (type == LocationType::THREAD ? location : -1);
    }
    int cpuid() const
    {
        return (type != LocationType::THREAD ? location : -1);
    }

    // Needed becausse this is used as a Key in some Structures.
    // Simply order (arbitrarly) by type first, then by location
    friend bool operator<(const Location& lhs, const Location& rhs)
    {
        if (lhs.type != rhs.type)
        {
            return lhs.type < rhs.type;
        }
        else
        {
            return lhs.location < rhs.location;
        }
    }
};

struct CpuLocation : public Location
{
    CpuLocation(int cpuid) : Location(LocationType::CPU, cpuid)
    {
    }
};

struct ThreadLocation : public Location
{
    ThreadLocation(pid_t tid) : Location(LocationType::THREAD, tid)
    {
    }
};

struct SampleCpuLocation : public Location
{
    SampleCpuLocation(int cpuid) : Location(LocationType::SAMPLE_CPU, cpuid)
    {
    }
};

struct ByLocation
{
    using key_type = Location;

    key_type key;

    explicit ByLocation(key_type key) : key(key)
    {
    }
};

struct ByCPU
{
    using key_type = int;

    key_type key;

    explicit ByCPU(key_type key) : key(key)
    {
    }
};

struct ByProcess
{
    using key_type = int;

    key_type key;

    explicit ByProcess(key_type key) : key(key)
    {
    }
};

template <typename Definition>
struct MyHolder
{
    using type = typename otf2::get_default_holder<Definition>::type;
};

template <>
struct MyHolder<otf2::definition::string>
{
    using type =
        otf2::lookup_definition_holder<otf2::definition::string, ByCPU, ByProcess, ByLocation>;
};
template <>
struct MyHolder<otf2::definition::system_tree_node>
{
    using type =
        otf2::lookup_definition_holder<otf2::definition::system_tree_node, ByCPU, ByProcess>;
};
int main()
{
    otf2::registry rego;
    rego.create<otf2::definition::string>(2, std::string("horst"));
    rego.create<otf2::definition::string>(std::string("regenbogen"));

    otf2::lookup_registry<MyHolder> reg;

    // otf2::registry_view<otf2::definition::string, ByCPU> cpu_strings(rego);

    rego.emplace<otf2::definition::string>(ByLocation(CpuLocation(42)), std::string("The answer"));
    rego.emplace<otf2::definition::string>(ByLocation(ThreadLocation(666)),
                                           std::string("Number of the beast"));
    const auto& str1 = reg.create<otf2::definition::string>(ByCPU(7), std::string("cpu 7"));
    const auto& str2 =
        reg.create<otf2::definition::string>(ByProcess(7435), std::string("process 7435"));
    const auto& str3 =
        reg.create<otf2::definition::string>(ByProcess(745), 17, std::string("process 745"));
    reg.create<otf2::definition::string>(2, std::string("horst"));
    reg.create<otf2::definition::string>(std::string("regenbogen"));

    reg.create<otf2::definition::system_tree_node>(ByCPU(42), str1, str2);
    reg.emplace<otf2::definition::system_tree_node>(ByCPU(42), str1, str2);
    otf2::definition::system_tree_node& node =
        reg.get<otf2::definition::system_tree_node>(ByCPU(42));
    node.name(str3);

    {
        auto test = reg.get<otf2::definition::string>(ByCPU(7));
        std::cout << test.ref() << " - " << test.str() << std::endl;
    }
    {
        auto test = reg.get<otf2::definition::string>(ByProcess(7435));
        std::cout << test.ref() << " - " << test.str() << std::endl;
    }

    auto def = otf2::definition::string(6, "I'm alien");
    reg.register_definition(ByProcess(42), std::move(def));

    for (auto& def : reg.all<otf2::definition::string>())
    {
        std::cout << def.ref() << " - " << def.str() << std::endl;
    }

    std::cout << reg.has<otf2::definition::string>(6) << " "
              << reg.has<otf2::definition::string>(ByCPU(6)) << " "
              << reg.has<otf2::definition::string>(ByCPU(7)) << " "
              << reg.has<otf2::definition::string>(ByProcess(6)) << std::endl;

    auto& mutable_def = reg.create<otf2::definition::system_tree_node>(
        reg.get<otf2::definition::string>(ByProcess(42)),
        reg.create<otf2::definition::string>("some class"));

    std::cout << mutable_def.name() << std::endl;

    mutable_def.name(reg.create<otf2::definition::string>("New stn name"));

    std::cout << mutable_def.name() << std::endl;
}

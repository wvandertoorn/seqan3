// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/core/algorithm/parameter_pack.hpp>
#include <seqan3/core/type_list.hpp>

using namespace seqan3;

TEST(parameter_pack, for_each_value)
{
    int i = 0;
    auto fn = [&i](int arg)
    {
        EXPECT_EQ(i, arg);
        ++i;
    };

    detail::for_each_value(fn);
    EXPECT_EQ(i, 0);
    detail::for_each_value(fn, 0);
    EXPECT_EQ(i, 1);
    detail::for_each_value(fn, 1, 2);
    EXPECT_EQ(i, 3);
    detail::for_each_value(fn, 3, 4, 5);
    EXPECT_EQ(i, 6);
}

TEST(parameter_pack, for_each_value2)
{
    std::stringstream stream{};

    auto fn = [&stream](auto const & arg)
    {
        if constexpr(Alphabet<decltype(arg)>)
            stream << to_char(arg) << ";";
        else
            stream << arg << ";";
    };

    detail::for_each_value(fn);
    detail::for_each_value(fn, 0);
    detail::for_each_value(fn, 1.0, '2');
    detail::for_each_value(fn, "3;4", -5, 'C'_dna4);

    EXPECT_EQ(stream.str(), "0;1;2;3;4;-5;C;");
}

template <typename type>
void print_to_stream(std::stringstream & stream, std::type_identity<type>)
{
    if constexpr (std::is_same_v<type, bool>)
        stream << type{0} << ";";
    if constexpr (std::is_same_v<type, uint8_t>)
        stream << unsigned{1} << ";";
    if constexpr (std::is_same_v<type, int8_t>)
        stream << int{-1} << ";";
    if constexpr (std::is_same_v<type, uint16_t>)
        stream << type{2} << ";";
    if constexpr (std::is_same_v<type, int16_t>)
        stream << type{-2} << ";";
    if constexpr (std::is_same_v<type, uint32_t>)
        stream << type{3} << ";";
    if constexpr (std::is_same_v<type, int32_t>)
        stream << type{-3} << ";";
    if constexpr (std::is_same_v<type, uint64_t>)
        stream << type{4} << ";";
    if constexpr (std::is_same_v<type, int64_t>)
        stream << type{-4} << ";";
}

TEST(parameter_pack, for_each_type)
{
    std::stringstream stream{};

    auto fn = [&stream](auto id)
    {
        print_to_stream(stream, id);
    };

    detail::for_each_type<bool, uint8_t, int8_t, uint16_t, int16_t,
                          uint32_t, int32_t, uint64_t, int64_t>(fn);

    EXPECT_EQ(stream.str(), "0;1;-1;2;-2;3;-3;4;-4;");
}

TEST(type_list, for_each_type)
{
    std::stringstream stream{};

    auto fn = [&stream](auto id)
    {
        print_to_stream(stream, id);
    };

    using types = type_list<bool, uint8_t, int8_t, uint16_t, int16_t,
                            uint32_t, int32_t, uint64_t, int64_t>;
    detail::for_each_type(fn, types{});

    EXPECT_EQ(stream.str(), "0;1;-1;2;-2;3;-3;4;-4;");
}

TEST(tuple, for_each_type)
{
    std::stringstream stream{};

    auto fn = [&stream](auto id)
    {
        print_to_stream(stream, id);
    };

    using tuple = std::tuple<bool, uint8_t, int8_t, uint16_t, int16_t,
                             uint32_t, int32_t, uint64_t, int64_t>;
    detail::for_each_type(fn, tuple{});

    EXPECT_EQ(stream.str(), "0;1;-1;2;-2;3;-3;4;-4;");
}

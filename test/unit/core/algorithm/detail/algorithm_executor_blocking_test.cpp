// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <seqan3/std/algorithm>
#include <seqan3/std/ranges>
#include <string>

#include <seqan3/core/algorithm/detail/algorithm_executor_blocking.hpp>
#include <seqan3/range/views/persist.hpp>
#include <seqan3/range/views/zip.hpp>
#include <seqan3/test/pretty_printing.hpp>

// A dummy algorithm that just counts the number of equal characters in two sequences.
struct dummy_algorithm
{
    template <typename sequences_t, typename callback_t>
    constexpr void operator()(sequences_t && sequence_pairs,
                              callback_t && callback) const
    {
        auto && [first_seq, second_seq] = sequence_pairs;

        size_t count = 0;
        for (auto && [lhs, rhs] : seqan3::views::zip(first_seq, second_seq))
            if (lhs == rhs)
                ++count;

        if (count != 0)  // Simulating not to call the callback without a result.
            callback(count);
    }
};

template <typename resource_t>
struct algorithm_type_for_input
{
    using algorithm_input_t = std::ranges::range_value_t<resource_t>;
    using callback_t = std::function<void(size_t)>;
    using type = std::function<void(algorithm_input_t, callback_t)>;
};

template <typename t>
struct algorithm_executor_blocking_test : public ::testing::Test
{
    // Some globally defined test types
    using sequence_pair_t = std::pair<std::string, std::string>;
    using sequence_pairs_t = std::vector<sequence_pair_t>;

    sequence_pair_t sequence_pair{"AACGTACGT", "ATCGTCCGT"};  // Hamming distance is 7
    sequence_pairs_t sequence_pairs{5, sequence_pair};
};

using testing_types = testing::Types<seqan3::detail::execution_handler_sequential,
                                     seqan3::detail::execution_handler_parallel>;
TYPED_TEST_SUITE(algorithm_executor_blocking_test, testing_types, );

TYPED_TEST(algorithm_executor_blocking_test, construction)
{
    using algorithm_t = typename algorithm_type_for_input<typename TestFixture::sequence_pairs_t &>::type;
    using executor_t =
        seqan3::detail::algorithm_executor_blocking<typename TestFixture::sequence_pairs_t &,
                                                    algorithm_t,
                                                    size_t,
                                                    TypeParam>;

    EXPECT_FALSE(std::is_default_constructible_v<executor_t>);
    EXPECT_FALSE(std::is_copy_constructible_v<executor_t>);
    EXPECT_TRUE(std::is_move_constructible_v<executor_t>);
    EXPECT_FALSE(std::is_copy_assignable_v<executor_t>);
    EXPECT_TRUE(std::is_move_assignable_v<executor_t>);
}

TYPED_TEST(algorithm_executor_blocking_test, is_eof)
{
    using algorithm_t = typename algorithm_type_for_input<typename TestFixture::sequence_pairs_t &>::type;
    using executor_t =
        seqan3::detail::algorithm_executor_blocking<typename TestFixture::sequence_pairs_t &,
                                                    algorithm_t,
                                                    size_t,
                                                    TypeParam>;

    executor_t exec{this->sequence_pairs, algorithm_t{dummy_algorithm{}}};
    EXPECT_FALSE(exec.is_eof());
}

TYPED_TEST(algorithm_executor_blocking_test, type_deduction)
{
    using algorithm_t = typename algorithm_type_for_input<typename TestFixture::sequence_pairs_t &>::type;
    seqan3::detail::algorithm_executor_blocking exec{this->sequence_pairs, algorithm_t{dummy_algorithm{}}, 0u};
    EXPECT_FALSE(exec.is_eof());
}

TYPED_TEST(algorithm_executor_blocking_test, next_result)
{
    using algorithm_t = typename algorithm_type_for_input<typename TestFixture::sequence_pairs_t &>::type;
    using executor_t =
        seqan3::detail::algorithm_executor_blocking<typename TestFixture::sequence_pairs_t &,
                                                    algorithm_t,
                                                    size_t,
                                                    TypeParam>;

    executor_t exec{this->sequence_pairs, algorithm_t{dummy_algorithm{}}};

    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_FALSE(static_cast<bool>(exec.next_result()));
}

TYPED_TEST(algorithm_executor_blocking_test, move_assignment)
{
    using algorithm_t = typename algorithm_type_for_input<typename TestFixture::sequence_pairs_t &>::type;
    using executor_t =
        seqan3::detail::algorithm_executor_blocking<typename TestFixture::sequence_pairs_t &,
                                                    algorithm_t,
                                                    size_t,
                                                    TypeParam>;

    executor_t exec{this->sequence_pairs, algorithm_t{dummy_algorithm{}}};
    executor_t exec_move_assigned{this->sequence_pairs, algorithm_t{dummy_algorithm{}}, 2u};

    exec_move_assigned = std::move(exec);

    EXPECT_EQ(exec_move_assigned.next_result().value(), 7u);
    EXPECT_EQ(exec_move_assigned.next_result().value(), 7u);
    EXPECT_EQ(exec_move_assigned.next_result().value(), 7u);

    executor_t exec_move_constructed{std::move(exec_move_assigned)};
    EXPECT_EQ(exec_move_constructed.next_result().value(), 7u);
    EXPECT_EQ(exec_move_constructed.next_result().value(), 7u);
    EXPECT_FALSE(static_cast<bool>(exec_move_constructed.next_result()));
}

TYPED_TEST(algorithm_executor_blocking_test, lvalue_sequence_pair_view)
{
    auto v = std::views::single(this->sequence_pair);
    using algorithm_t = typename algorithm_type_for_input<decltype(v)>::type;
    using executor_t = seqan3::detail::algorithm_executor_blocking<decltype(v),
                                                                   algorithm_t,
                                                                   size_t,
                                                                   TypeParam>;

    executor_t exec{v, algorithm_t{dummy_algorithm{}}};
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_FALSE(static_cast<bool>(exec.next_result()));
}

TYPED_TEST(algorithm_executor_blocking_test, rvalue_sequence_pair_view)
{
    using single_pair_t = decltype(std::views::single(this->sequence_pair));
    using algorithm_t = typename algorithm_type_for_input<single_pair_t>::type;
    using executor_t = seqan3::detail::algorithm_executor_blocking<single_pair_t,
                                                                   algorithm_t,
                                                                   size_t,
                                                                   TypeParam>;

    executor_t exec{std::views::single(this->sequence_pair), algorithm_t{dummy_algorithm{}}};
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_FALSE(static_cast<bool>(exec.next_result()));
}

TYPED_TEST(algorithm_executor_blocking_test, lvalue_sequence_pairs)
{
    using algorithm_t = typename algorithm_type_for_input<typename TestFixture::sequence_pairs_t &>::type;
    using executor_t = seqan3::detail::algorithm_executor_blocking<typename TestFixture::sequence_pairs_t &,
                                                                   algorithm_t,
                                                                   size_t,
                                                                   TypeParam>;

    executor_t exec{this->sequence_pairs, algorithm_t{dummy_algorithm{}}};
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_FALSE(static_cast<bool>(exec.next_result()));
}

TYPED_TEST(algorithm_executor_blocking_test, rvalue_sequence_pairs_view)
{
    using persist_pairs_t = decltype(this->sequence_pairs | seqan3::views::persist);
    using algorithm_t = typename algorithm_type_for_input<persist_pairs_t>::type;
    using executor_t = seqan3::detail::algorithm_executor_blocking<persist_pairs_t,
                                                                   algorithm_t,
                                                                   size_t,
                                                                   TypeParam>;

    executor_t exec{this->sequence_pairs | seqan3::views::persist, algorithm_t{dummy_algorithm{}}};
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_FALSE(static_cast<bool>(exec.next_result()));
}

TYPED_TEST(algorithm_executor_blocking_test, empty_result_bucket)
{
    using algorithm_t = typename algorithm_type_for_input<typename TestFixture::sequence_pairs_t &>::type;
    using executor_t =
        seqan3::detail::algorithm_executor_blocking<typename TestFixture::sequence_pairs_t &,
                                                    algorithm_t,
                                                    size_t,
                                                    TypeParam>;
    this->sequence_pairs[3].first = "";
    executor_t exec{this->sequence_pairs, algorithm_t{dummy_algorithm{}}};

    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_EQ(exec.next_result().value(), 7u);
    EXPECT_FALSE(static_cast<bool>(exec.next_result()));
}

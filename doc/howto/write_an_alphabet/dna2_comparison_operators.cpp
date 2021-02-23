// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

//! [comparison]
#include <seqan3/alphabet/concept.hpp>                   // alphabet concept checks

struct dna2
{
    uint8_t rank{};

    // Equality and inequality operators

    friend bool operator==(dna2 const & lhs, dna2 const & rhs) noexcept
    {
        return lhs.rank == rhs.rank;
    }

    friend bool operator!=(dna2 const & lhs, dna2 const & rhs) noexcept
    {
        return !(lhs == rhs);
    }

    // Comparison operators

    friend bool operator<(dna2 const & lhs, dna2 const & rhs) noexcept
    {
        return lhs.rank < rhs.rank;
    }

    friend bool operator<=(dna2 const & lhs, dna2 const & rhs) noexcept
    {
        return lhs.rank <= rhs.rank;
    }

    friend bool operator>(dna2 const & lhs, dna2 const & rhs) noexcept
    {
        return lhs.rank > rhs.rank;
    }

    friend bool operator>=(dna2 const & lhs, dna2 const & rhs) noexcept
    {
        return lhs.rank >= rhs.rank;
    }
};

static_assert(std::equality_comparable<dna2>);            // ok
static_assert(std::totally_ordered<dna2>);          // ok
//! [comparison]

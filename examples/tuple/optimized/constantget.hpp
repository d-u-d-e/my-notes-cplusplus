#pragma once
#include "tuplestorage4.hpp"

/*
    The get() operation is extremely common when working with tuples, but it’s recursive implementa-
    tion requires a linear number of template instantiations that can affect compile time. Fortunately, the
    EBCO optimizations introduced in the previous section have enabled a more efficient implementation
    of get that we will describe here.
*/

template <unsigned H, typename T>
T& getHeight(TupleElt2<H, T>& te)
{
  return te.get();
}

/*
    If we can compute the height H of the element we wish to extract, we can rely on the
    conversion from the Tuple specialization to TupleElt<H, T> (where T is deduced) to extract that
    element without manually walking through all of the indices
*/

template <unsigned I, typename... Elements>
auto get(Tuple4<Elements...>& t)
{
  return getHeight<sizeof...(Elements) - I - 1>(t);
}
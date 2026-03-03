#pragma once

/*
    Our formulation for Tuple storage uses more storage than is strictly necessary. One problem is that
    the tail member will eventually be an empty tuple, because every nonempty tuple terminates with
    an empty tuple, and data members must always have at least one byte of storage.
    To improve Tuple’s storage efficiency, we can apply the empty base class optimization (EBCO).
*/

template <typename... Types>
class Tuple;

template <typename Head, typename... Tail>
class Tuple<Head, Tail...> : private Tuple<Tail...>
{
  private:
  Head head;

  public:
  Head& getHead()
  {
    return head;
  }
  Head const& getHead() const
  {
    return head;
  }
  Tuple<Tail...>& getTail()
  {
    return *this;
  }
  Tuple<Tail...> const& getTail() const
  {
    return *this;
  }

  // + others....
};

/*
    In this new formulation of Tuple storage, the tail is in a base class, so it will be
    initialized before the member head. Thus it has the practical side effect of reversing the order in which the tuple elements are initialized
    in constructors. This problem can be addressed by sinking the head member into its own base class that precedes the tail in the base class list.
    See tuplestorage2.hpp
*/
/* Copyright 2024-2025 MarcosHCK
 * This file is part of DAA-Final-Project.
 *
 * DAA-Final-Project is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DAA-Final-Project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DAA-Final-Project. If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <vector>

template<typename TClass> class BNode
{
private:

  std::set<TClass> segments;

public:

  inline BNode () : segments ()
    {
    }

  inline BNode (const BNode<TClass>& o) : segments (o.segments)
    {
    }

  inline BNode (TClass&& val, BNode<TClass>&& o) : segments (std::move (o.segments))
    {
      segments.insert (std::move (val));
    }

  inline BNode (const BNode<TClass>& a, const BNode<TClass>& b)
    {
      auto r = & segments;
      auto x = & a.segments;
      auto y = & b.segments;

      std::set_union (x->begin (), x->end (), y->begin (), y->end (), std::inserter (*r, r->end ()));
    }

  inline constexpr const std::set<TClass>& lookup () const
    {
      return segments;
    }
};

template<typename TInterval, typename TNode, typename TVal> class BTree
{
private:

  TInterval end, start;
  std::unique_ptr<BTree<TInterval, TNode, TVal>> left = nullptr;
  std::unique_ptr<BTree<TInterval, TNode, TVal>> right = nullptr;
  TNode value;

  inline BTree* get_left ()
    {
      if (left == nullptr) split ();
      return left.get ();
    }

  inline BTree* get_right ()
    {
      if (right == nullptr) split ();
      return right.get ();
    }

  inline BTree<TInterval, TNode, TVal>* inserti (const TInterval& l, const TInterval& r, TVal&& val)
    {
      if (l < start || r > end || l > end)

        std::__throw_out_of_range ("inserting segment out of range");

      if (l == start && r == end && (left == nullptr && right == nullptr))

        return (value = TNode (std::forward<TVal> (val), std::move (value)), this);
      else
        {
          auto m = mid (start, end);
          BTree<TInterval, TNode, TVal>* lt;
          BTree<TInterval, TNode, TVal>* rt;

          if (l > m)
            {
              rt = get_right ()->inserti (l, r, std::forward<TVal> (val));
              lt = left.get ();
            }
          else if (m >= r)
            {
              lt = get_left ()->inserti (l, r, std::forward<TVal> (val));
              rt = right.get ();
            }
          else
            {
              lt = get_left ()->inserti (l, m, std::forward<TVal> (val));
              rt = get_right ()->inserti (m + 1, r, std::forward<TVal> (val));
            }

          return (value = TNode (lt->value, rt->value), this);
        }
    }

  inline const TNode lookupi (TInterval l, TInterval r) const
    {
      if (l == start && r == end && (left == nullptr && right == nullptr))

        return value;
      else
        {
          auto m = mid (start, end);

          if (l > m)
            {
              return right->lookupi (l, r);
            }
          else if (m >= r)
            {
              return left->lookupi (l, r);
            }
          else
            {
              auto lt = left->lookupi (l, m);
              auto rt = right->lookupi (m + 1, r);
              return TNode (lt, rt);
            }
        }
    }

  static inline TInterval mid (TInterval l, TInterval r)
    {
      return l + (r - l) / 2;
    }

  inline void split ()
    {
      auto m = mid (start, end);
      (left = std::make_unique<BTree<TInterval, TNode, TVal>> (start, m))->value = value;
      (right = std::make_unique<BTree<TInterval, TNode, TVal>> (m + 1, end))->value = value;
      value = TNode (left->value, right->value);
    }

public:

  inline BTree (const TInterval& l, const TInterval& r) : end (r), start (l)
    {
      if (l < 0 || l > r)

        std::__throw_invalid_argument ("invalid BTree interval");
    }

  inline constexpr const auto get_left () const { return left.get (); }
  inline constexpr const auto get_right () const { return right.get (); }
  inline constexpr const TNode& get_value () const { return value; }

  inline void insert (const TInterval& l, const TInterval& r, TVal&& val)
    {
      inserti (l, r, std::move (val));
    }

  inline const auto lookup (TInterval l, TInterval r) const
    {
      return lookupi (l, r).lookup ();
    }
};

template<typename C, typename P> inline std::vector<std::tuple<C, P, P>> cleanse (std::vector<std::tuple<C, P, P>>&& vec, const BTree<P, BNode<P>, P>& tree, P* upset, P* zeros)
{
  P i = 0;
  P zerocount = 0;
  std::set<P> useless;

  for (auto iter = vec.begin (); iter != vec.end (); ++i, ++iter) if (std::get<0> (*iter) == 0)
    {
      auto e = std::get<1> (*iter);
      auto s = std::get<2> (*iter);
      bool f = true;

      for (const auto& j : tree.lookup (s, e)) if (std::get<0> (vec [j]) != 0) f = false;
      if (f == true) useless.insert (i);
      ++zerocount;
    }

  std::vector<std::tuple<C, P, P>> clean;
  i = 0;

  clean.reserve (vec.size () - useless.size ());

  for (auto iter = std::make_move_iterator (vec.begin ()); iter != std::make_move_iterator (vec.end ()); ++i, ++iter)

    if ((useless.find (i) != useless.end ()) == false)
      {
        clean.push_back (*iter);
      }

  return (*upset = useless.size (), *zeros = zerocount, std::move (clean));
}

template<typename C, typename P> inline std::map<P, std::set<C>> genavail (const std::vector<std::tuple<C, P, P>>& vec, const BTree<P, BNode<P>, P>& tree)
{
  C c;
  P i = 0;
  std::map<P, std::set<C>> avail;

  for (auto iter = vec.begin (); iter != vec.end (); ++i, ++iter) if (std::get<0> (*iter) == 0)
    {
      std::set<C> r;
      auto e = std::get<1> (*iter);
      auto s = std::get<2> (*iter);

      for (const auto& j : tree.lookup (s, e))
        {
          if ((c = std::get<0> (vec [j])) != 0)
            r.insert (c);
        }
      avail.insert (std::make_pair (i, std::move (r)));
    }

  return std::move (avail);
}

template<typename C, typename P> inline P upset_count (const std::vector<std::tuple<C, P, P>>& vec, const BTree<P, BNode<P>, P>& tree)
{
  P upset = 0;

  for (auto iter = vec.begin (); iter != vec.end (); ++iter)
    {
      auto c = std::get<0> (*iter);
      auto e = std::get<1> (*iter);
      auto s = std::get<2> (*iter);
      auto f = true;

      for (const auto& j : tree.lookup (s, e))
        {
          auto k = std::get<0> (vec [j]);
          if (c != k) { f = false; break; }
        }

      if (f == true) ++upset;
    }

  return upset;
}

template<typename C, typename P> inline P optimize (std::vector<std::tuple<C, P, P>>& vec, const BTree<P, BNode<P>, P>& tree, std::map<P, std::set<C>>&& avail)
{
  P b, best = 0;
  std::vector<std::set<C>*> sets;
  std::vector<P> idxs;
  std::vector<typename std::set<C>::const_iterator> iters;

  idxs.reserve (avail.size ());
  iters.reserve (avail.size ());
  sets.reserve (avail.size ());

  for (auto iter = avail.begin (); iter != avail.end (); ++iter)
    {
      idxs.push_back (std::get<0> (*iter));
      iters.push_back ((std::get<1> (*iter)).begin ());
      sets.push_back (& std::get<1> (*iter));
    }

  while (true)
    {
      for (int i = 0; i < iters.size (); ++i)
        {
          auto k = (idxs [i]);
          auto c = *(iters [i]);
          auto e = std::get<1> (vec [k]);
          auto s = std::get<2> (vec [k]);

          vec [k] = std::make_tuple (c, e, s);
        }

      if ((b = upset_count (vec, tree)) > best)
        best = b;

      for (int i = iters.size () - 1; i >= 0; --i)
        {
          if (++iters [i] != sets [i]->end ())
            break;
          else if (i == 0)
            return best;
        }
    }
}

template<typename C, typename P> inline P case_ ()
{
  P npeople;
  std::cin >> npeople;

  C country;
  P end, maxl = 0, start;

  std::vector<std::tuple<C, P, P>> vec;
  vec.reserve (npeople);

  for (P i = 0; i < npeople; ++i)
    {
      std::cin >> start >> end >> country;

      maxl = std::max (maxl, end);
      vec.push_back (std::make_tuple (country, end, start));
    }

  auto tree = BTree<P, BNode<P>, P> (0, maxl);

  std::sort (vec.begin (), vec.end (), [](const std::tuple<C, P, P>& a, const std::tuple<C, P, P>& b)
    {
      return std::get<2> (a) < std::get<2> (b);
    });

  P i = 0;
  P upset = 0;
  P zeros = 0;

  for (auto iter = vec.begin (); iter != vec.end (); ++iter)
    {
      auto c = std::get<0> (*iter);
      auto e = std::get<1> (*iter);
      auto s = std::get<2> (*iter);

      tree.insert (s, e, i++);
      if (c == 0) ++zeros;
    }

  if (zeros == 0)

    upset = upset_count (vec, tree);
  else
    {
      zeros = 0;
      vec = cleanse (std::move (vec), tree, &upset, &zeros);

      if (zeros == 0)

        upset += upset_count (vec, tree);
      else
        upset += optimize (vec, tree, genavail (vec, tree));
    }

  return upset;
}

template<typename C, typename P> inline int program ()
{
  P ncases;
  std::cin >> ncases;

  for (P i = 0; i < ncases; ++i) std::cout << case_<C, P> () << std::endl;
  return 0;
}

int main ()
{
  return program<unsigned, unsigned> ();
}

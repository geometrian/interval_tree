#pragma once
#ifndef _INTERVAL_TREE_
#define _INTERVAL_TREE_

#include <algorithm>
#include <limits>
#include <vector>


#define _new new //You can override this to track allocations if you like.


namespace std {


template <size_t dimension, typename T> class interval_tree;
template <                  typename T> class interval_tree<1,T> final {
	public:
		class interval final { public:
			T left, right;
			void* user_ptr;
			inline bool not_lt(T val) const { return left <=val; } //`!(val<left )`
			inline bool not_gt(T val) const { return val<=right; } //`!(val>right)`
			inline bool intersects(T val) const { return left<=val && val<=right; }
		};

	private:
		template <class interval_tree_t> class _iterator final : public ::std::iterator<random_access_iterator_tag,interval const> {
			private:
				typedef ::std::iterator<random_access_iterator_tag,interval const> _parent_t;
				typedef _iterator<interval_tree_t> _self_t;
				interval_tree_t* _parent;
				size_t _i;
			public:
				using typename _parent_t::iterator_category;
				using typename _parent_t::value_type;
				using typename _parent_t::difference_type;
				using typename _parent_t::pointer;
				using typename _parent_t::reference;
			public:
				inline _iterator(interval_tree_t* parent, size_t i) : _parent(parent), _i(i) {} //TODO: private!
				inline reference operator*(void) const { return _parent->_search_tree[_i]; }
				inline pointer operator->(void) const { return _parent->_search_tree.data()+_i; }
				inline _self_t& operator++(void) { ++_i; return *this; }
				inline _self_t  operator++(int ) { ++_i; return *this; }
				inline _self_t& operator--(void) { --_i; return *this; }
				inline _self_t  operator--(int ) { --_i; return *this; }
				inline _self_t& operator+=(difference_type offset)       { _i+=offset; return *this; }
				inline _self_t  operator+ (difference_type offset) const { return {_parent,_i+offset}; }
				inline _self_t& operator-=(difference_type offset)       { _i-=offset; return *this; }
				inline _self_t  operator- (difference_type offset) const { return {_parent,_i-offset}; }
				inline difference_type operator-(_self_t const& other) const { return _i-other._i; }
				inline reference operator[](difference_type offset) const { _parent->_search_tree[_i+offset]; }
				inline bool operator< (_self_t const& other) const { return _i< other._i; }
				inline bool operator<=(_self_t const& other) const { return _i<=other._i; }
				inline bool operator> (_self_t const& other) const { return _i> other._i; }
				inline bool operator>=(_self_t const& other) const { return _i>=other._i; }
				inline bool operator==(_self_t const& other) const { return _i==other._i; }
				inline bool operator!=(_self_t const& other) const { return _i!=other._i; }
		};
	public:
		typedef _iterator<interval_tree      >       iterator;
		typedef _iterator<interval_tree const> const_iterator;

	private:
		class _search_interval final { public:
			interval i;
			bool mutable selected;
			inline void add_to(vector<interval>* result) const { result->emplace_back(i); selected=true; }
		};
		class _search_key final { public: T key; _search_interval const* si; };
		class _Node final {
			private:
				T _val_center;
				_Node const *_left, *_right;
				vector<_search_interval> _center;
				vector<_search_interval const*> _center_by_l, _center_by_r;
			public:
				_Node(interval_tree* parent, vector<interval> const& intervals) {
					T left  = numeric_limits<T>::   max();
					T right = numeric_limits<T>::lowest();
					for (interval const& i : intervals) {
						//assert_term(i.left<=i.right,"Invalid interval!");
						left  = min(left, i. left);
						right = max(right,i.right);
					}
					_val_center = (left+right)/2;

					vector<interval> S_left, S_right;
					for (interval const& i : intervals) {
						if      (i.right<_val_center) S_left .emplace_back(i);
						else if (i. left>_val_center) S_right.emplace_back(i);
						else _center.push_back({i,false});
					}
					_center_by_l.reserve(_center.size()); _center_by_r.reserve(_center.size());
					for (_search_interval const& si : _center) {
						_center_by_l.emplace_back(&si);
						_center_by_r.emplace_back(&si);
					}
					sort(_center_by_l.begin(),_center_by_l.end(), [](_search_interval const* a,_search_interval const* b)->bool{return a->i. left<b->i. left;});
					sort(_center_by_r.begin(),_center_by_r.end(), [](_search_interval const* a,_search_interval const* b)->bool{return a->i.right>b->i.right;});

					if (S_left .empty()) _left =nullptr; else _left =_new _Node(parent, S_left );
					if (S_right.empty()) _right=nullptr; else _right=_new _Node(parent, S_right);

					//Note: must be built once pointers are stable; i.e., after construction.
					for (_search_interval const* si : _center_by_l) {
						parent->_search_tree.push_back({si->i.left, si});
						parent->_search_tree.push_back({si->i.right,si});
					}
				}
				inline ~_Node(void) { delete _left; delete _right; }

				template <bool not_checked> void intersect(T point, vector<interval>* result) const {
					if        (point<_val_center) {
						for (_search_interval const* si : _center_by_l) {
							if (si->i.not_lt(point) && (not_checked||!si->selected)) si->add_to(result);
							else break;
						}
						if (_left !=nullptr) _left->intersect<not_checked>(point,result);
					} else if (point>_val_center) {
						for (_search_interval const* si : _center_by_r) {
							if (si->i.not_gt(point) && (not_checked||!si->selected)) si->add_to(result);
							else break;
						}
						if (_right!=nullptr) _right->intersect<not_checked>(point,result);
					} else {
						for (_search_interval const* si : _center_by_l) {
							if (not_checked||!si->selected) si->add_to(result);
						}
					}
				}
		};
		_Node const* _interval_tree;
		vector<_search_key> _search_tree;

	public:
		explicit inline interval_tree(vector<interval> const& intervals) {
			_interval_tree = _new _Node(this,intervals); //Note: must come after `._search_tree` exists.
			sort(_search_tree.begin(),_search_tree.end(), [](_search_key const& a,_search_key const& b)->bool{return a.key<b.key;});
		}
		explicit inline interval_tree(interval_tree const& other) = delete;
		explicit inline interval_tree(interval_tree&& other) = delete;
		inline ~interval_tree(void) { delete _interval_tree; }

		inline void intersect(       T point, vector<interval>* result) const { _interval_tree->template intersect<true>(point,result); }
		       void intersect(interval     i, vector<interval>* result) const {
			//Find all intervals with `.left` or `.right` inside `i`.
			auto iter = lower_bound(_search_tree.cbegin(),_search_tree.cend(), i, [](_search_key const& a,interval const& i)->bool{ return a.key<=i.left; });
			if (iter<_search_tree.cend()) {
				LOOP:
					_search_key const& sk = *iter;
					if (!sk.si->selected) sk.si->add_to(result);
					if (++iter<_search_tree.cend() && i.intersects(iter->key)) goto LOOP;
			}

			//Find all intervals that enclose `i`.
			_interval_tree->template intersect<false>(i.left,result);

			//Reset
			for (_search_key const& sk : _search_tree) sk.si->selected=false;
		}

		inline       iterator  begin(void)       noexcept { return { this,                   0 }; }
		inline const_iterator  begin(void) const noexcept { return { this,                   0 }; }
		inline const_iterator cbegin(void) const noexcept { return { this,                   0 }; }
		inline       iterator    end(void)       noexcept { return { this, _search_tree.size() }; }
		inline const_iterator    end(void) const noexcept { return { this, _search_tree.size() }; }
		inline const_iterator   cend(void) const noexcept { return { this, _search_tree.size() }; }
};


}


#endif

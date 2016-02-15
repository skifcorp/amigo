#ifndef __AMTREE_H_
#define __AMTREE_H_

#include <type_traits>
#include <vector>

using std::vector;

template <class SubType>
class TreeNode
{
public:
  TreeNode (TreeNode * p = 0) : parent_(p), next_sibling_(nullptr), first_child_(nullptr), last_child_(nullptr)
  {
    static_assert ( std::is_base_of<TreeNode, SubType>::value, "Template class must have base of TreeNode" );
  }
  
  virtual ~TreeNode ()
  {
    for ( TreeNode * t = first_child_; t; t = t->next_sibling_) {
      delete t;
    }
    first_child_ = last_child_ = nullptr;
  }
  
  TreeNode (const TreeNode& ) = delete;

  void appendChild (TreeNode * t)
  {
    t->parent_ = this;
    if ( isEmpty() ) {
      first_child_ = last_child_  = t;
    }
    else { 
      last_child_->next_sibling_ = t;
      last_child_ = t;
    }
  }
  
  bool isEmpty() const 
  {
    return first_child_ == nullptr;
  }
  

  SubType * parent() {return static_cast<SubType *>(parent_);}
  const SubType * parent () const {return static_cast<SubType*> (parent_);}
  SubType * firstChild () {return static_cast<SubType *>(first_child_);}
  const SubType * firstChild () const {return static_cast<SubType*>(first_child_);}
  SubType * lastChild () {return static_cast<SubType *>(last_child_);}
  const SubType * lastChild () const {return static_cast<SubType*>(last_child_);}
  SubType * nextSibling () {return static_cast<SubType*> (next_sibling_);}
  const SubType * nextSibling () const {return static_cast<SubType*>(next_sibling_);}
private:
  TreeNode * parent_;
  TreeNode *next_sibling_, *first_child_, *last_child_;
};

template <class SubType>
class TreeRoot : public vector<TreeNode<SubType>*>
{
public:
  typedef typename vector<TreeNode<SubType>>::size_type size_type;
  
  TreeRoot(size_type sz)
  {
    this->reserve(sz);
  }

  TreeRoot()
  {
  }
  
  ~TreeRoot()
  {
    for (auto n : *this) {
      delete n;
    }
    this->clear();
  }
};

#endif

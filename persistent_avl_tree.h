#pragma once

#include "linked_list.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


enum InsertOrReplaceMode {
    INSERT_LEFT_IF_FOUND = -1,
    THROW_IF_FOUND = 0,
    INSERT_RIGHT_IF_FOUND = 1,
    REPLACE_IF_FOUND = 2,
    REPLACE_ONLY = 3
};


/*
 * A self-balancing, persistent, immutable, binary search tree.
 */
template<typename NodeContent, typename DerivedTree>
class AvlTree {
    public:
        typedef std::shared_ptr<DerivedTree> TreePtr;
        typedef NodeContent NodeContentT;

        AvlTree(
            const NodeContent& content,
            const TreePtr& left,
            const TreePtr& right
        );

        virtual ~AvlTree() {}

        const NodeContent& get_content() { return content; }
        const TreePtr& get_left() { return left; }
        const TreePtr& get_right() { return right; }
        int get_size() { return size; } // Number of nodes in this tree.
        int get_height() { return height; } // Number of levels in this tree, i.e. length of the longest path from the root.

        const TreePtr& get_child(int left_or_right) {
            assert(left_or_right != 0);
            if (left_or_right < 0) { return left; }
            else                   { return right; }
        }

        int get_num_children() { return int(bool(left)) + int(bool(right)); }

        DerivedTree* get_derived() {
            DerivedTree* derived_tree = dynamic_cast<DerivedTree*>(this);
            assert(derived_tree);
            return derived_tree;
        }

        virtual std::string get_label() { return "x"; }

        std::string draw_as_text();

        static TreePtr construct_from_vector(
            const std::vector<NodeContent>& vec,
            int start_index = 0,
            int end_index = -1 // Exclusive of end_index.
        );

        /*
         * A FinderFunc defines a path to a node or an empty spot within a tree.
         * A FinderFunc returns
         *     -1 if we should go to current_node->get_left(),
         *     1 if we should go to current_node->get_right(),
         *     or 0 if we should stop at current_node.
         */
        typedef std::function<int (const TreePtr& current_node)> FinderFunc;

        /*
         * Returns the node of the tree found by finder_func,
         * or nullptr if finder_func ended at an empty spot.
         */
        static TreePtr find(
            const TreePtr& self,
            FinderFunc&& finder_func,
            int* num_to_left = nullptr // Make sure to initialize num_to_left to 0 before passing.
        );

        static FinderFunc index_finder(int index, int from_left_or_right = -1);
        static FinderFunc furthest_inserter(int left_or_right);
        static FinderFunc furthest_finder(int left_or_right);

        static std::shared_ptr<DerivedTree> null() { return nullptr; }

        TreePtr rotate(int left_or_right);
        TreePtr double_rotate(int left_or_right);
        static TreePtr balance(const TreePtr& self);

        static TreePtr insert_or_replace(
            const TreePtr& self,
            FinderFunc&& finder_func,
            const NodeContent& new_content,
            InsertOrReplaceMode mode = REPLACE_IF_FOUND
        );

        /*
         * Throws if item does not exist.
         */
        static TreePtr remove(
            const TreePtr& self,
            FinderFunc&& finder_func,
            TreePtr* removed_node = nullptr // If non-null, will be set to the node that was found and removed.
        );

    private:
        struct DrawDimensions {
            int width                  = 0;
            int height                 = 0;
            int right_width            = 0;
            int root_x                 = 0; // Relative; could be shifted by start_x
            int left_child_x           = 0; // Relative; could be shifted by start_x
            int right_child_x          = 0; // Relative; could be shifted by start_x
        };

        typedef std::map<DerivedTree*, DrawDimensions> DrawMemo;

        static DrawDimensions get_draw_dimensions(DerivedTree* self, DrawMemo* memo);
        static void draw_to_text(
            DerivedTree* self,
            std::vector<std::string>* text_lines,
            int start_x,
            int start_y,
            DrawMemo* memo,
            bool is_left_subtree = false
        );

    private:
        const NodeContent content;
        const TreePtr left;
        const TreePtr right;
        const int size;
        const int height;
};


// Standalone functions declared/defined here:
// --------------------------------------------------

namespace TreeOps {

    template<typename TreeType>
    std::shared_ptr<TreeType> make_tree(
        const typename TreeType::NodeContentT& content,
        std::shared_ptr<TreeType> child1,
        std::shared_ptr<TreeType> child2,
        int child2_left_or_right = 1
    ) {
        assert(child2_left_or_right != 0);
        if (child2_left_or_right < 0) {
            return std::make_shared<TreeType>(
                content,
                child2,
                child1
            );
        } else {
            return std::make_shared<TreeType>(
                content,
                child1,
                child2
            );
        }
    }

    template<typename TreeType>
    std::string draw_as_text(const std::shared_ptr<TreeType>& tree) {
        if (tree == nullptr) {
            return "NULL TREE";
        }
        return tree->get_derived()->draw_as_text();
    }

    template<typename TreeType>
    std::string get_label(const std::shared_ptr<TreeType>& tree) {
        if (tree == nullptr) {
            return "NULL TREE";
        }
        return tree->get_derived()->get_label();
    }

    template<typename TreeType>
    int get_size(const std::shared_ptr<TreeType>& tree) {
        if (tree == nullptr) {
            return 0;
        }
        return tree->get_derived()->get_size();
    }

    template<typename TreeType>
    int get_height(const std::shared_ptr<TreeType>& tree) {
        if (tree == nullptr) {
            return 0;
        }
        return tree->get_derived()->get_height();
    }

    template<typename TreeType>
    std::shared_ptr<TreeType> find(
        const std::shared_ptr<TreeType>& self,
        typename TreeType::FinderFunc&& finder_func,
        int* num_to_left = nullptr // Make sure to initialize num_to_left to 0 before passing.
    ) {
        return TreeType::find(self, std::move(finder_func), num_to_left);
    }

    template<typename TreeType>
    int get_balance_factor(const std::shared_ptr<TreeType>& tree) {
        if (tree == nullptr) {
            return 0;
        }
        int lh = TreeOps::get_height(tree->get_left());
        int rh = TreeOps::get_height(tree->get_right());
        return rh - lh;
    }

    template<typename TreeType>
    bool is_balanced(const std::shared_ptr<TreeType>& tree) {
        return abs(TreeOps::get_balance_factor(tree)) <= 1;
    }

    template<typename TreeType>
    bool is_balanced_recursively(const std::shared_ptr<TreeType>& tree) {
        if (tree == nullptr) {
            return true;
        }
        return TreeOps::is_balanced(tree)
            && TreeOps::is_balanced_recursively(tree->get_left())
            && TreeOps::is_balanced_recursively(tree->get_right());
    }

    template<typename TreeType>
    std::shared_ptr<TreeType> insert_or_replace(
        const std::shared_ptr<TreeType>& self,
        typename TreeType::FinderFunc&& finder_func,
        const typename TreeType::NodeContentT& new_content,
        InsertOrReplaceMode mode = REPLACE_IF_FOUND
    ) {
        return TreeType::insert_or_replace(self, std::move(finder_func), new_content, mode);
    }

    // Throws if item does not exist.
    template<typename TreeType>
    std::shared_ptr<TreeType> remove(
        const std::shared_ptr<TreeType>& self,
        typename TreeType::FinderFunc&& finder_func,
        std::shared_ptr<TreeType>* removed_node = nullptr // If non-null, will be set to the node that was found and removed.
    ) {
        return TreeType::remove(self, std::move(finder_func), removed_node);
    }

}


// Class method implementations defined here:
// --------------------------------------------------

#define AvlTreeX AvlTree<NodeContent, DerivedTree>

// (constructor)
template<typename NodeContent, typename DerivedTree>
AvlTreeX::AvlTree(
    const NodeContent& content,
    const TreePtr& left,
    const TreePtr& right
):
    content(content),
    left(left),
    right(right),
    size(TreeOps::get_size(left) + 1 + TreeOps::get_size(right)),
    height(1 + std::max(TreeOps::get_height(left), TreeOps::get_height(right)))
{}

// (static method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::DrawDimensions
AvlTreeX::get_draw_dimensions(DerivedTree* self, DrawMemo* memo) {
    constexpr int MIN_SPACE_BETWEEN_SUBTREES = 2; // Should be greater than zero.

    if (self == nullptr) {
        return DrawDimensions();
    }

    assert(memo);

    if (memo->count(self)) {
        return memo->at(self);
    }

    DrawDimensions left  = get_draw_dimensions(self->get_left() .get(), memo);
    DrawDimensions right = get_draw_dimensions(self->get_right().get(), memo);

    DrawDimensions ret;

    // Compute width
    const int label_width = int(self->get_label().size());
    ret.width = std::max({
        label_width + (label_width % 2), // Make the label width divisible by 2.
        left.width + MIN_SPACE_BETWEEN_SUBTREES + right.width,
        1
    });

    // Compute height
    if (self->get_left() || self->get_right()) {
        ret.height = 2 + std::max(left.height, right.height);
    } else {
        ret.height = 1;
    }

    // Compute right_width
    ret.right_width = right.width;

    // Compute root_x
    const int space_between_subtrees = ret.width - left.width - right.width;
    ret.root_x = left.width + space_between_subtrees / 2;

    // Compute left_child_x
    if (self->get_left()) {
        ret.left_child_x = left.root_x;
    } else {
        ret.left_child_x = ret.root_x;
    }

    // Compute right_child_x
    if (self->get_right()) {
        ret.right_child_x = left.width + space_between_subtrees + right.root_x;
    } else {
        ret.right_child_x = ret.root_x;
    }

    (*memo)[self] = ret;
    return ret;
}

// (static method)
template<typename NodeContent, typename DerivedTree>
void AvlTreeX::draw_to_text(
    DerivedTree* self,
    std::vector<std::string>* text,
    int start_x,
    int start_y,
    DrawMemo* memo,
    bool is_left_subtree /* = false */
) {
    if (self == nullptr) {
        return;
    }

    DrawDimensions dims = get_draw_dimensions(self, memo);

    assert(text);
    assert(text->size() >= start_y + dims.height);
    assert(text->at(start_y).size() >= start_x + dims.width);

    const int left_start_x  = start_x;
    const int right_start_x = start_x + dims.width - dims.right_width;
    const int left_edge_x  = start_x + dims.left_child_x;
    const int right_edge_x = start_x + dims.right_child_x - 1;

    // Draw the right.
    // (Before the left, in case we want to insert multi-byte unicode
    // (characters, which would mess with the string indexes otherwise.)
    if (self->get_right()) {
        text->at(start_y + 1).replace(right_edge_x, 1, "\\");
        draw_to_text(
            self->get_right().get(),
            text,
            right_start_x,
            start_y + 2,
            memo,
            false
        );
    }

    // Draw the left.
    if (self->get_left()) {
        text->at(start_y + 1).replace(left_edge_x, 1, "/");
        draw_to_text(
            self->get_left().get(),
            text,
            left_start_x,
            start_y + 2,
            memo,
            true
        );
    }

    // Draw the label.
    const int label_len = self->get_label().size();
    int label_start_x = start_x + dims.root_x - (label_len / 2) - ((label_len % 2) * int(is_left_subtree));
    if (label_start_x < start_x) {
        label_start_x = start_x;
    }
    if (label_start_x + label_len > start_x + dims.width) {
        label_start_x = start_x + dims.width - label_len;
    }
    text->at(start_y).replace(label_start_x, label_len, self->get_label());

    // Draw the overscores.
    // Note: Here we are inserting multi-byte unicode characters, which messes with the string indexes.
    const int overscores_start_x = left_edge_x + 1;
    const int overscores_end_x   = right_edge_x;
    const int overscores_width   = overscores_end_x - overscores_start_x;
    if (overscores_width > 0) {
        // text->at(start_y).replace(overscores_start_x, overscores_width, std::string(overscores_width, '_'));
        std::string replacement;
        for (int i = 0; i < overscores_width; i++) {
            if (label_start_x <= (overscores_start_x + i) &&
                                 (overscores_start_x + i) < label_start_x + label_len) {
                replacement += " ";
            } else {
                replacement += "‾";
            }
        }
        text->at(start_y + 1).replace(overscores_start_x, overscores_width, replacement);
    }
}

// (instance method)
template<typename NodeContent, typename DerivedTree>
std::string AvlTreeX::draw_as_text() {

    DerivedTree* derived_this = this->get_derived();

    DrawMemo memo;
    std::vector<std::string> text_lines;

    DrawDimensions dims = get_draw_dimensions(derived_this, &memo);

    for (int r = 0; r < dims.height; r++) {
        text_lines.push_back(
            std::string("|")
            + std::string(dims.width, ' ')
            + std::string("|")
        );
    }

    draw_to_text(
        derived_this,
        &text_lines,
        1,
        0,
        &memo
    );

    std::string ret;
    for (const std::string& line : text_lines) {
        ret += line;
        ret += '\n';
    }
    return ret;
}

// (static method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::TreePtr
AvlTreeX::construct_from_vector(
    const std::vector<NodeContent>& vec,
    int start_index /* = 0 */,
    int end_index /* = -1 */
) {
    if (end_index == -1) {
        end_index = vec.size();
    }
    if (end_index <= start_index) {
        return nullptr;
    }
    const int mid_index = (start_index + end_index) / 2;
    return std::make_shared<DerivedTree>(
        vec[mid_index],
        construct_from_vector(vec, start_index, mid_index),
        construct_from_vector(vec, mid_index + 1, end_index)
    );
}

// (static method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::TreePtr
AvlTreeX::find(
    const TreePtr& self,
    FinderFunc&& finder_func,
    int* num_to_left /* = nullptr */ // Make sure to initialize num_to_left to 0 before passing.
) {
    if (self == nullptr) {
        return nullptr;
    }
    const int direction = finder_func(self);
    if (direction < 0) {
        return find(self->get_left(), std::move(finder_func), num_to_left);
    }
    else if (direction == 0) {
        if (num_to_left) {
            *num_to_left += TreeOps::get_size(self->get_left());
        }
        return self;
    }
    else { // direction > 0
        if (num_to_left) {
            *num_to_left += TreeOps::get_size(self->get_left()) + 1;
        }
        return find(self->get_right(), std::move(finder_func), num_to_left);
    }
}

// (static method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::FinderFunc
AvlTreeX::index_finder(int index, int from_left_or_right /* = -1 */) {
    assert(from_left_or_right != 0);
    return [index, from_left_or_right](const TreePtr& current_node) mutable {
        assert(current_node != nullptr);
        const int left_size = TreeOps::get_size(current_node->get_child(from_left_or_right));
        if (index < left_size) {
            return from_left_or_right;
        }
        else if (index == left_size) {
            return 0;
        }
        else { // index > left_size
            index -= (left_size + 1);
            return -from_left_or_right;
        }
    };
}

// (static method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::FinderFunc
AvlTreeX::furthest_inserter(int left_or_right) {
    assert(left_or_right != 0);
    return [left_or_right](const TreePtr& current_node) {
        assert(current_node != nullptr);
        return left_or_right;
    };
}

// (static method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::FinderFunc
AvlTreeX::furthest_finder(int left_or_right) {
    assert(left_or_right != 0);
    return [left_or_right](const TreePtr& current_node) {
        assert(current_node != nullptr);
        if (current_node->get_child(left_or_right) == nullptr) {
            return 0;
        } else {
            return left_or_right;
        }
    };
}

// (instance method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::TreePtr
AvlTreeX::rotate(int left_or_right) {
    assert(left_or_right != 0);

    // Written assuming RIGHT rotation, i.e. that left_or_right == 1.
    // However, works for both cases.

    const int left = -left_or_right;
    const int right = left_or_right;

    assert(this->get_child(left) != nullptr);

    const TreePtr&  subtree1 = this->get_child(left)->get_child(left);
    const NodeContent& node2 = this->get_child(left)->get_content();
    const TreePtr&  subtree3 = this->get_child(left)->get_child(right);
    const NodeContent& node4 = this->get_content();
    const TreePtr&  subtree5 = this->get_child(right);
    TreePtr new_right_subtree = TreeOps::make_tree(node4, subtree3, subtree5, right);
    return TreeOps::make_tree(node2, subtree1, new_right_subtree, right);
}

// (instance method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::TreePtr
AvlTreeX::double_rotate(int left_or_right) {
    assert(left_or_right != 0);

    // Written assuming RIGHT rotation, i.e. that left_or_right == 1.
    // However, works for both cases.

    const int left = -left_or_right;
    const int right = left_or_right;

    assert(this->get_child(left) != nullptr);
    assert(this->get_child(left)->get_child(right) != nullptr);

    TreePtr new_left_subtree = this->get_child(left)->rotate(left);
    TreePtr new_self = TreeOps::make_tree(this->get_content(), new_left_subtree, this->get_child(right), right);
    return new_self->rotate(right);
}

// (static method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::TreePtr
AvlTreeX::balance(const TreePtr& self) {
    if (self == nullptr) {
        return nullptr;
    }

    if (TreeOps::is_balanced(self)) {
        return self;
    }

    const int lh = TreeOps::get_height(self->get_left());
    const int rh = TreeOps::get_height(self->get_right());

    const int direction = (lh > rh) ? 1 : -1; // Direction of rotation.
    const TreePtr& taller_child = self->get_child(-direction);
    assert(taller_child != nullptr);

    const int inner_h = TreeOps::get_height(taller_child->get_child(direction));
    const int outer_h = TreeOps::get_height(taller_child->get_child(-direction));

    TreePtr result;

    if (inner_h > outer_h) {
        result = self->double_rotate(direction);
    } else {
        result = self->rotate(direction);
    }

    // Assert that the tree is now balanced (unless the original tree was abnormally imbalanced).
    assert(TreeOps::is_balanced(result) || abs(lh - rh) > 2);

    return result;
}

// (static method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::TreePtr
AvlTreeX::insert_or_replace(
    const TreePtr& self,
    FinderFunc&& finder_func,
    const NodeContent& new_content,
    InsertOrReplaceMode mode /* = REPLACE_IF_FOUND */
) {
    assert(mode == INSERT_LEFT_IF_FOUND
        || mode == THROW_IF_FOUND
        || mode == INSERT_RIGHT_IF_FOUND
        || mode == REPLACE_IF_FOUND
        || mode == REPLACE_ONLY
    );

    if (self == nullptr) {
        // Return new node, unless mode == REPLACE_ONLY.
        if (mode == REPLACE_ONLY) {
            throw std::runtime_error("insert_or_replace(): Node not found (and mode is REPLACE_ONLY).");
        } else {
            return std::make_shared<DerivedTree>(new_content, nullptr, nullptr);
        }
    }

    int direction = finder_func(self); // Note: We don't have a shared_ptr to `this`, so this function cannot be an instance method.

    if (direction == 0) {
        // Node found.
        if (mode == THROW_IF_FOUND) {
            throw std::runtime_error("insert_or_replace(): Node found (and mode is THROW_IF_FOUND).");
        } else if (mode == INSERT_LEFT_IF_FOUND) {
            direction = -1;
            finder_func = furthest_inserter(1);
        } else if (mode == INSERT_RIGHT_IF_FOUND) {
            direction = 1;
            finder_func = furthest_inserter(-1);
        } else if (mode == REPLACE_IF_FOUND || mode == REPLACE_ONLY) {
            return std::make_shared<DerivedTree>(new_content, self->get_left(), self->get_right());
        } else {
            assert(false); // Should not get here.
        }
    }

    // Keep searching.
    TreePtr new_child = insert_or_replace(
        self->get_child(direction),
        std::move(finder_func),
        new_content,
        mode
    );
    TreePtr new_self = TreeOps::make_tree(self->get_content(), self->get_child(-direction), new_child, direction);
    return balance(new_self);
}

// (static method)
template<typename NodeContent, typename DerivedTree>
typename AvlTreeX::TreePtr
AvlTreeX::remove(
    const TreePtr& self,
    FinderFunc&& finder_func,
    TreePtr* removed_node /* = nullptr */ // If non-null, will be set to the node that was found and removed.
) {
    if (self == nullptr) {
        throw std::runtime_error("remove(): Node not found.");
    }

    const int direction = finder_func(self);

    if (direction == 0) {
        // Node found.
        if (removed_node != nullptr) {
            *removed_node = self;
        }

        if (self->get_left() == nullptr) {
            return self->get_right();
        } else if (self->get_right() == nullptr) {
            return self->get_left();
        } else {
            // Remove the rightmost node on the left or the leftmost node on the right. Then replace the current node content with that node content.
            const int sub_direction = (
                TreeOps::get_size(self->get_right()) > TreeOps::get_size(self->get_left())
            ) ? 1 : -1;
            TreePtr sub_removed_node;

            TreePtr new_child = remove(
                self->get_child(sub_direction),
                furthest_finder(-sub_direction),
                &sub_removed_node
            );
            assert(sub_removed_node != nullptr);
            TreePtr new_self = TreeOps::make_tree(sub_removed_node->get_content(), self->get_child(-sub_direction), new_child, sub_direction);
            return balance(new_self);
        }
    }

    // Keep searching.
    TreePtr new_child = remove(
        self->get_child(direction),
        std::move(finder_func),
        removed_node
    );
    TreePtr new_self = TreeOps::make_tree(self->get_content(), self->get_child(-direction), new_child, direction);
    return balance(new_self);
}

#undef AvlTreeX

// DONE
// ----------
    // DONE: Refactor so that mid (and other values) are returned from get_draw_dimensions()
    // DONE: Compute mid as the midpoint of the space between subtrees, which can expand to fill the label width
    // DONE: Connect left_mid_x and right_mid_x with underscores
    // DONE: Handle off-by-1? (Add 1 to label_len to make it divisible by 2)
    // DONE: Implement class LinkedList<T>
    // DONE: Move main() and the custom tree classes to another file. Implementations probably need to move to header file.
    // DONE: Implement static TreePtr find(const TreePtr& self, FinderFunc&& finder_func, int* num_to_left = nullptr) // Make sure to initialize num_to_left to 0 before passing.
    // DONE: Implement index_finder(int index, int from_left_or_right = -1)
    // DONE: Implement const TreePtr& get_child(int left_or_right)

    // DONE: Implement  make_tree<DerivedTree, NodeContent>(
    //                      const NodeContent& content,
    //                      std::shared_ptr<DerivedTree> child1,
    //                      std::shared_ptr<DerivedTree> child2,
    //                      int left_or_right = 1
    //                  )

    // DONE: Implement TreePtr rotate(int left_or_right)
    // DONE: Implement TreePtr double_rotate(int left_or_right)

    // DONE: Implement static TreePtr balance(const TreePtr& self)

    // DONE: Implement furthest_finder(int left_or_right)
    // DONE: Implement furthest_inserter(int left_or_right)

    // DONE: Define this (in file scope):
        // enum InsertOrReplaceMode {
        //     INSERT_LEFT_IF_FOUND = -1,
        //     THROW_IF_FOUND = 0,
        //     INSERT_RIGHT_IF_FOUND = 1,
        //     REPLACE_IF_FOUND = 2,
        //     REPLACE_ONLY = 3
        // };

    // DONE: Implement static TreePtr insert_or_replace(const TreePtr& self, FinderFunc&& finder_func, const NodeContent& new_content, InsertOrReplaceMode mode = REPLACE_IF_FOUND)

    // DONE: Implement static TreePtr remove(const TreePtr& self, FinderFunc&& finder_func, TreePtr* removed_node = nullptr) // Throws if item does not exist


// TODO
// ----------
    // TODO: Can I re-balance a merged tree by just calling balance() repeatedly?

    // TODO: Implement static LinkedList<TreePtr>::Ptr get_path(const TreePtr& self, FinderFunc&& finder_func, bool prefer_left_if_not_found = false, const LinkedList<TreePtr>::Ptr& base = nullptr)
    // TODO: Implement static LinkedList<TreePtr>::Ptr get_next_path(const LinkedList<TreePtr>::Ptr& path, int shift_amount = 1)

    // TODO: Implement some finder functions
    // TODO: Implement cmp_finder(const NodeContent& to_find, std::function<int (const NodeContent& c1, const NodeContent& c2)> cmp)
    // TODO: Implement cmp_finder(const NodeContent& to_find)

    // TODO: Implement static TreePtr insert(const TreePtr& self, FinderFunc&& finder_func, const NodeContent& new_content, int mode_if_found = 0) // mode is one of {-1 = insert_to_left, 1 = insert_to_right, 0 = throw_if_found}
    // TODO: Implement static TreePtr replace(const TreePtr& self, FinderFunc&& finder_func, const NodeContent& new_content) // Throws if item does not exist. Uses REPLACE_ONLY.

    // TODO: Add sum_of_node_heights and get_average_node_height()
    // TODO: Test the average height after a bunch of insertions

    // TODO: Implement a standard iterator?

    // TODO: Test that the destructor is called the right number of times

    // TODO: Add a base class for a K,V ordered map. Take DerivedTree as a template param.
    // TODO: Add usable derived classes
    // TODO: Add examples

    // TODO: Add a readme


// CANCELLED
// ----------
    // CANCELLED: Implement get_shared()
    // CANCELLED: Implement set_shared() (private?)
    // CANCELLED: Implement make_tree<>()
    //   See https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-api-4.6/a01033_source.html


// g++ -o run_tests run_tests.cpp -std=c++11 && echo && ./run_tests

#include "persistent_avl_tree.h"

using namespace std;
using namespace TreeOps;


template<typename NodeContent>
class UsableTree : public AvlTree<NodeContent, UsableTree<NodeContent>> {
    public:
        typedef AvlTree<NodeContent, UsableTree> Base;
        typedef typename Base::TreePtr TreePtr2;

        using AvlTree<NodeContent, UsableTree>::AvlTree;

        // UsableTree(
        //     const NodeContent& content,
        //     const TreePtr2& left,
        //     const TreePtr2& right
        // ):
        //     Base(content, left, right)
        // {}

        virtual std::string get_label() {
            std::ostringstream oss;
            oss << this->get_content();
            return oss.str();
        }
};

class CustomTree : public AvlTree<int, CustomTree> {
    public:
        using AvlTree::AvlTree;

        // CustomTree(
        //     int content,
        //     const TreePtr& left,
        //     const TreePtr& right
        // ):
        //     AvlTree(content, left, right)
        // {}
};


static string strip_prefix(const string& s, char prefix_char) {
    // Find the first non-prefix character.
    int i;
    for (i = 0; i < s.size(); i++) {
        if (s[i] != prefix_char) {
            break;
        }
    }
    return s.substr(i);
}

static string strip_margin(const string& s) {
    string ret;
    istringstream iss(s);

    for (string line; getline(iss, line); ) {
        ret += "\n" + strip_prefix(line, ' ');
    }

    return strip_prefix(ret, '\n');
}


int main() {
    cout << "Hello world" << endl;
    cout << endl;

    cout << "sizeof(bool)  = " << sizeof(bool) << endl;
    cout << "sizeof(short) = " << sizeof(short) << endl;
    cout << "sizeof(int)   = " << sizeof(int) << endl;
    cout << "sizeof(long)  = " << sizeof(long) << endl;
    cout << "sizeof(void*) = " << sizeof(void*) << endl;
    cout << endl;

    assert(1 == 1);


    const auto tree1 = make_tree<CustomTree>(100, nullptr, nullptr);
    cout << "tree1:" << endl;
    cout << draw_as_text(tree1) << endl;
    cout << endl;


    const auto tree2 =
        make_tree<CustomTree>(
            100,
            nullptr,
            make_tree<CustomTree>(
                100,
                nullptr,
                nullptr
            )
        );
    cout << "tree2:" << endl;
    cout << draw_as_text(tree2) << endl;
    cout << endl;


    const auto tree3 =
        make_tree(
            100,
            make_tree<CustomTree>(
                100,
                nullptr,
                nullptr
            ),
            make_tree<CustomTree>(
                100,
                nullptr,
                make_tree<CustomTree>(
                    100,
                    nullptr,
                    nullptr
                )
            )
        );
    cout << "tree3:" << endl;
    cout << draw_as_text(tree3) << endl;
    cout << endl;


    const auto tree4 =
        make_tree(
            100,
            make_tree(
                100,
                CustomTree::null(),
                CustomTree::null()
            ),
            make_tree(
                100,
                make_tree<CustomTree>(
                    100,
                    nullptr,
                    nullptr
                ),
                make_tree<CustomTree>(
                    100,
                    nullptr,
                    nullptr
                )
            )
        );
    cout << "tree4:" << endl;
    cout << draw_as_text(tree4) << endl;
    cout << endl;


    const auto tree5 = CustomTree::construct_from_vector({1, 20, 300, 4000, 50000, 600000, -7000000});
    cout << "tree5:" << endl;
    cout << draw_as_text(tree5) << endl;
    cout << endl;


    const auto tree6 =
        make_tree<UsableTree<int>>(
            100,
            nullptr,
            make_tree<UsableTree<int>>(
                300,
                nullptr,
                nullptr
            )
        );
    cout << "tree6:" << endl;
    cout << draw_as_text(tree6) << endl;
    cout << endl;


    const auto tree7 = UsableTree<int>::construct_from_vector({1, 20, 300, 4000, 50000, 600000, -7000000});
    cout << "tree7:" << endl;
    cout << draw_as_text(tree7) << endl;
    cout << endl;

    assert(draw_as_text(tree7) ==
        strip_margin(R"raw(
            |       4000               |
            |   /‾‾‾    ‾‾‾‾‾\         |
            |  20          600000      |
            | /  ‾\       /      ‾\    |
            |1    300  50000   -7000000|
        )raw")
    );


    {
        cout << "finding the root:" << endl;
        int num_to_left = 0;
        const string found_label = get_label(find(tree7, [](const UsableTree<int>::TreePtr& node){ return 0; }, &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "4000");
        assert(num_to_left == 3);
    }


    {
        cout << "finding 300:" << endl;
        int num_to_left = 0;
        int i = 0;
        const string found_label = get_label(find(tree7, [i](const UsableTree<int>::TreePtr& node) mutable {
            i++;
            if (i == 1) { return -1; }
            if (i == 2) { return 1; }
            if (i == 3) { return 0; }
            assert(false);
        }, &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "300");
        assert(num_to_left == 2);
    }


    {
        cout << "finding empty spot to the right of 300:" << endl;
        int num_to_left = 0;
        int i = 0;
        const string found_label = get_label(find(tree7, [i](const UsableTree<int>::TreePtr& node) mutable {
            i++;
            if (i == 1) { return -1; }
            if (i == 2) { return 1; }
            if (i == 3) { return 1; }
            assert(false);
        }, &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "NULL TREE");
        assert(num_to_left == 3);
    }


    {
        cout << "finding index 0:" << endl;
        int num_to_left = 0;
        const string found_label = get_label(find(tree7, UsableTree<int>::index_finder(0), &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "1");
        assert(num_to_left == 0);
    }


    {
        cout << "finding index 6 from right:" << endl;
        int num_to_left = 0;
        const string found_label = get_label(find(tree7, UsableTree<int>::index_finder(6, 1), &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "1");
        assert(num_to_left == 0);
    }


    {
        cout << "finding index 0 from right:" << endl;
        int num_to_left = 0;
        const string found_label = get_label(find(tree7, UsableTree<int>::index_finder(0, 1), &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "-7000000");
        assert(num_to_left == 6);
    }


    {
        cout << "finding index 6:" << endl;
        int num_to_left = 0;
        const string found_label = get_label(find(tree7, UsableTree<int>::index_finder(6), &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "-7000000");
        assert(num_to_left == 6);
    }


    {
        cout << "finding index 2:" << endl;
        int num_to_left = 0;
        const string found_label = get_label(find(tree7, UsableTree<int>::index_finder(2), &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "300");
        assert(num_to_left == 2);
    }


    {
        cout << "finding index 3:" << endl;
        int num_to_left = 0;
        const string found_label = get_label(find(tree7, UsableTree<int>::index_finder(3), &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "4000");
        assert(num_to_left == 3);
    }


    {
        cout << "finding index 2 from right:" << endl;
        int num_to_left = 0;
        const string found_label = get_label(find(tree7, UsableTree<int>::index_finder(2, 1), &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "50000");
        assert(num_to_left == 4);
    }


    {
        cout << "finding index 8:" << endl;
        int num_to_left = 0;
        const string found_label = get_label(find(tree7, UsableTree<int>::index_finder(8), &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "NULL TREE");
        assert(num_to_left == 7);
    }


    {
        cout << "finding index -2:" << endl;
        int num_to_left = 0;
        const string found_label = get_label(find(tree7, UsableTree<int>::index_finder(-2), &num_to_left));
        cout << found_label << endl;
        cout << num_to_left << endl;
        cout << endl;
        assert(found_label == "NULL TREE");
        assert(num_to_left == 0);
    }


    const auto tree8 =
        make_tree(
            10,
            UsableTree<int>::null(),
            make_tree(
                30,
                make_tree<UsableTree<int>>(
                    20,
                    nullptr,
                    nullptr
                ),
                make_tree<UsableTree<int>>(
                    40,
                    nullptr,
                    nullptr
                )
            )
        );
    cout << "tree8:" << endl;
    cout << draw_as_text(tree8) << endl;
    cout << endl;


    const auto tree8_rotated = tree8->rotate(-1);
    cout << "tree8_rotated:" << endl;
    cout << draw_as_text(tree8_rotated) << endl;
    cout << endl;


    const auto tree8_balanced = UsableTree<int>::balance(tree8);
    cout << "tree8_balanced:" << endl;
    cout << draw_as_text(tree8_balanced) << endl;
    cout << endl;


    const auto tree8_double_rotated = tree8->double_rotate(-1);
    cout << "tree8_double_rotated:" << endl;
    cout << draw_as_text(tree8_double_rotated) << endl;
    cout << endl;


    assert(draw_as_text(tree8_balanced) == draw_as_text(tree8_rotated));


    const auto tree9 =
        make_tree(
            10,
            UsableTree<int>::null(),
            make_tree<UsableTree<int>>(
                30,
                make_tree<UsableTree<int>>(
                    20,
                    nullptr,
                    nullptr
                ),
                nullptr
            )
        );
    cout << "tree9:" << endl;
    cout << draw_as_text(tree9) << endl;
    cout << endl;


    const auto tree9_rotated = tree9->rotate(-1);
    cout << "tree9_rotated:" << endl;
    cout << draw_as_text(tree9_rotated) << endl;
    cout << endl;


    const auto tree9_balanced = UsableTree<int>::balance(tree9);
    cout << "tree9_balanced:" << endl;
    cout << draw_as_text(tree9_balanced) << endl;
    cout << endl;


    const auto tree9_double_rotated = tree9->double_rotate(-1);
    cout << "tree9_double_rotated:" << endl;
    cout << draw_as_text(tree9_double_rotated) << endl;
    cout << endl;


    assert(draw_as_text(tree9_balanced) == draw_as_text(tree9_double_rotated));


    const auto list1 = make_shared<LinkedList<int>>(1, make_shared<LinkedList<int>>(2, make_shared<LinkedList<int>>(3, nullptr)));
    cout << "list1:" << endl;
    cout << ListOps::to_string(list1) << endl;
    cout << endl;


    UsableTree<int>::TreePtr tree10 = nullptr;
    for (int i = 0; i < 10; i++) {
        tree10 = insert_or_replace(
            tree10,
            UsableTree<int>::index_finder(-1, 1), // Insert on far right (past the end).
            i,
            THROW_IF_FOUND
        );
    }
    cout << "tree10:" << endl;
    cout << draw_as_text(tree10) << endl;
    cout << endl;
    assert(is_balanced_recursively(tree10));


    UsableTree<int>::TreePtr tree11 = tree10;
    for (int i = 0; i < 3; i++) {
        tree11 = remove(
            tree11,
            [](const UsableTree<int>::TreePtr& x){ return 0; }
        );
    }
    cout << "tree11:" << endl;
    cout << draw_as_text(tree11) << endl;
    cout << endl;
    assert(is_balanced_recursively(tree11));


    cout << "Done" << endl;
    return 0;
}

// g++ -o run_tests run_tests.cpp -std=c++11 && echo && ./run_tests

#include "persistent_avl_tree.h"

using namespace std;
using namespace TreeOps;


template<typename NodeContent>
class UsableTree : public AvlTree<NodeContent, UsableTree<NodeContent>> {
    public:
        typedef AvlTree<NodeContent, UsableTree> Base;
        typedef typename Base::TreePtr TreePtr;

        using AvlTree<NodeContent, UsableTree>::AvlTree;

        // UsableTree(
        //     const NodeContent& content,
        //     const TreePtr& left,
        //     const TreePtr& right
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


    const auto tree1 = make_shared<CustomTree>(100, nullptr, nullptr);
    cout << "tree1:" << endl;
    cout << draw_as_text(tree1) << endl;
    cout << endl;


    const auto tree2 =
        make_shared<CustomTree>(
            100,
            nullptr,
            make_shared<CustomTree>(
                100,
                nullptr,
                nullptr
            )
        );
    cout << "tree2:" << endl;
    cout << draw_as_text(tree2) << endl;
    cout << endl;


    const auto tree3 =
        make_shared<CustomTree>(
            100,
            make_shared<CustomTree>(
                100,
                nullptr,
                nullptr
            ),
            make_shared<CustomTree>(
                100,
                nullptr,
                make_shared<CustomTree>(
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
        make_shared<CustomTree>(
            100,
            make_shared<CustomTree>(
                100,
                nullptr,
                nullptr
            ),
            make_shared<CustomTree>(
                100,
                make_shared<CustomTree>(
                    100,
                    nullptr,
                    nullptr
                ),
                make_shared<CustomTree>(
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
        make_shared<UsableTree<int>>(
            100,
            nullptr,
            make_shared<UsableTree<int>>(
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


    cout << "finding the root:" << endl;
    cout << get_label(UsableTree<int>::find(tree7, [](const UsableTree<int>::TreePtr& node){ return 0; })) << endl;
    cout << endl;


    const auto list1 = make_shared<LinkedList<int>>(1, make_shared<LinkedList<int>>(2, make_shared<LinkedList<int>>(3, nullptr)));
    cout << "list1:" << endl;
    cout << ListOps::to_string(list1) << endl;
    cout << endl;


    cout << "Done" << endl;
    return 0;
}

#pragma once

#include <memory>
#include <sstream>
#include <string>


/*
 * A persistent, immutable, singly-linked list. Nice and simple.
 */
template<typename NodeContent>
class LinkedList {
    public:
        typedef std::shared_ptr<LinkedList> Ptr;

        LinkedList(
            const NodeContent& content,
            const Ptr& rest
        ):
            content(content),
            rest(rest)
        {}

        const NodeContent& get_head_content() { return content; }
        const Ptr& get_rest() { return rest; }

    private:
        const NodeContent content;
        const Ptr rest;
};


namespace ListOps {

    template<typename NodeContent>
    std::string to_string(const std::shared_ptr<LinkedList<NodeContent>>& list, const std::string& prefix = "LL") {
        if (list == nullptr) {
            return "nullptr";
        }

        std::ostringstream oss;
        oss << prefix << "(" << list->get_head_content() << ", " << to_string(list->get_rest()) << ")";
        return oss.str();
    }

}

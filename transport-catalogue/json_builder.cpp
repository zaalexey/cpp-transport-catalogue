#include "json_builder.h"

namespace json {

    Builder::DictKeyContext Builder::Key(std::string key) {
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsDict() && !has_key_) {
            has_key_ = true;
            key_ = std::move(key);
            return *this;
        }
        throw std::logic_error("Incorrect place for key : "s + key);
    }

    Builder& Builder::Value(Node::Value value) {
        Node new_node = std::visit([](auto val) {
            return Node(val);
            }, value);

        if (is_empty_) {
            root_ = new_node;
            is_empty_ = false;
            return *this;
        }

        if (!nodes_stack_.empty() && nodes_stack_.back()->IsDict() && has_key_) {
            const_cast<Dict&>(nodes_stack_.back()->AsDict()).insert({ key_, new_node });
            has_key_ = false;
            return *this;
        }

        if (!nodes_stack_.empty() && nodes_stack_.back()->IsArray()) {
            const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(new_node);
            return *this;
        }

        throw std::logic_error("Incorrect place for value"s);
    }

    Builder::DictItemContext Builder::StartDict() {
        Value(Dict{});
        AddNode(Node(Dict{}));
        return *this;
    }


    Builder::ArrayItemContext Builder::StartArray() {
        Value(Array{});
        AddNode(Node(Array{}));
        return *this;
    }

    Builder& Builder::EndDict() {
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsDict() && !has_key_) {
            nodes_stack_.pop_back();
            return *this;
        }
        throw std::logic_error("Incorrect place for EndDict"s);
    }

    Builder& Builder::EndArray() {
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsArray()) {
            nodes_stack_.pop_back();
            return *this;
        }
        throw std::logic_error("Incorrect place for EndArray"s);
    }


    Node Builder::Build() {
        if (is_empty_ || !nodes_stack_.empty()) {
            throw std::logic_error("Builder state is invalid"s);
        }
        return root_;
    }

    void Builder::AddNode(const Node& value) {
        if (value.IsArray() || value.IsDict()) {
            if (nodes_stack_.empty()) {
                nodes_stack_.push_back(&root_);
                return;
            }
            if (nodes_stack_.back()->IsArray()) {
                auto p = &nodes_stack_.back()->AsArray().back();
                nodes_stack_.push_back(const_cast<Node*>(p));
                return;
            }
            if (nodes_stack_.back()->IsDict()) {
                auto p = &nodes_stack_.back()->AsDict().at(key_);
                nodes_stack_.push_back(const_cast<Node*>(p));
                return;
            }
        }
    }


    Builder::DictItemContext::DictItemContext(Builder& builder)
        : builder_(builder)
    {}

    Builder::DictKeyContext Builder::DictItemContext::Key(std::string key) {
        return builder_.Key(key);
    }

    Builder& Builder::DictItemContext::EndDict() {
        return builder_.EndDict();
    }

    Builder::ArrayItemContext::ArrayItemContext(Builder& builder)
        : builder_(builder)
    {}

    Builder::ArrayItemContext Builder::ArrayItemContext::Value(Node::Value value) {
        return ArrayItemContext(builder_.Value(value));
    }

    Builder::DictItemContext Builder::ArrayItemContext::StartDict() {
        return builder_.StartDict();
    }

    Builder::ArrayItemContext Builder::ArrayItemContext::StartArray() {
        return builder_.StartArray();
    }

    Builder& Builder::ArrayItemContext::EndArray() {
        return builder_.EndArray();
    }

    Builder::DictKeyContext::DictKeyContext(Builder& builder)
        : builder_(builder)
    {}

    Builder::DictItemContext Builder::DictKeyContext::Value(Node::Value value) {
        return DictItemContext(builder_.Value(value));
    }

    Builder::ArrayItemContext Builder::DictKeyContext::StartArray() {
        return builder_.StartArray();
    }

    Builder::DictItemContext Builder::DictKeyContext::StartDict() {
        return builder_.StartDict();
    }


}
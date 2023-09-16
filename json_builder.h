#pragma once

#include "json.h"

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

using namespace std;

namespace json {
	class Builder {
	private:
		void AddNode(const Node& value);
		Node root_;
		std::vector<Node*> nodes_stack_;
		std::string key_;
		bool is_empty_ = true;
		bool has_key_ = false;

	public:
		class DictItemContext;
		class DictKeyContext;
		class ArrayItemContext;

		DictKeyContext Key(std::string);
		Builder& Value(Node::Value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndDict();
		Builder& EndArray();
		Node Build();
	};


	class Builder::DictItemContext {
	public:
		DictItemContext(Builder& builder);

		DictKeyContext Key(std::string key);
		Builder& EndDict();

	private:
		Builder& builder_;
	};

	class Builder::ArrayItemContext {
	public:
		ArrayItemContext(Builder& builder);

		ArrayItemContext Value(Node::Value value);
		DictItemContext StartDict();
		Builder& EndArray();
		ArrayItemContext StartArray();

	private:
		Builder& builder_;
	};

	class Builder::DictKeyContext {
	public:
		DictKeyContext(Builder& builder);

		DictItemContext Value(Node::Value value);
		ArrayItemContext StartArray();
		DictItemContext StartDict();

	private:
		Builder& builder_;
	};



}
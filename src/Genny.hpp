// SdkGenny - Genny.hpp - A single file header framework for generating C++ compatible SDKs
// https://github.com/cursey/sdkgenny

#pragma once

#include <algorithm>
#include <cassert>
#include <climits>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef GENNY_INCLUDE_PARSER
#include <cstdlib>
#include <optional>

#include <tao/pegtl.hpp>
#endif

namespace genny {

class Type;
class Reference;
class Pointer;
class Array;
class Struct;
class Class;
class Enum;
class Variable;
class Function;

class Indent : public std::streambuf {
public:
    explicit Indent(std::streambuf* dest, int indent = 4) : m_dest{dest}, m_indent(indent, ' ') {}
    explicit Indent(std::ostream& dest, int indent = 4) : m_dest{dest.rdbuf()}, m_indent(indent, ' '), m_owner{&dest} {
        m_owner->rdbuf(this);
    }
    ~Indent() override {
        if (m_owner != nullptr) {
            m_owner->rdbuf(m_dest);
        }
    }

protected:
    int overflow(int ch) override {
        if (m_is_at_start_of_line && ch != '\n') {
            m_dest->sputn(m_indent.data(), m_indent.size());
        }
        m_is_at_start_of_line = ch == '\n';
        return m_dest->sputc(ch);
    }

private:
    std::streambuf* m_dest{};
    bool m_is_at_start_of_line{true};
    std::string m_indent{};
    std::ostream* m_owner{};
};

class Object {
public:
    Object() = delete;
    explicit Object(std::string_view name) : m_name{name} {}
    virtual ~Object() = default;

    const auto& name() const { return m_name; }
    auto name(std::string name) {
        m_name = std::move(name);
        return this;
    }

    const auto& metadata() const { return m_metadata; }
    auto& metadata() { return m_metadata; }

    virtual void generate_metadata(std::ostream& os) const {
        if (m_metadata.empty()) {
            return;
        }

        os << "// Metadata: ";

        for (auto&& md : m_metadata) {
            os << md;
            if (&md != &*m_metadata.rbegin()) {
                os << md << ", ";
            }
        }

        os << "\n";
    }

    template <typename T> bool is_a() const { return dynamic_cast<const T*>(this) != nullptr; }

    // Searches for an owner of the correct type.
    template <typename T> const T* owner() const {
        for (auto owner = m_owner; owner != nullptr; owner = owner->m_owner) {
            if (owner->is_a<T>()) {
                return (const T*)owner;
            }
        }

        return nullptr;
    }

    template <typename T> T* owner() { return (T*)((const Object*)this)->owner<T>(); }

    template <typename T> const T* topmost_owner() const {
        const T* topmost{};

        for (auto owner = m_owner; owner != nullptr; owner = owner->m_owner) {
            if (owner->is_a<T>()) {
                topmost = (const T*)owner;
            }
        }

        return topmost;
    }

    template <typename T> T* topmost_owner() { return (T*)((const Object*)this)->topmost_owner<T>(); }

    template <typename T> std::vector<T*> owners() const {
        std::vector<T*> owners{};

        for (auto owner = m_owner; owner != nullptr; owner = owner->m_owner) {
            if (owner->is_a<T>()) {
                owners.emplace_back((T*)owner);
            }
        }

        return owners;
    }

    template <typename T> std::vector<T*> get_all() const {
        std::vector<T*> children{};

        for (auto&& child : m_children) {
            if (child->is_a<T>()) {
                children.emplace_back((T*)child.get());
            }
        }

        return children;
    }

    template <typename T> void get_all_in_children(std::unordered_set<T*>& objects) const {
        if (is_a<T>()) {
            objects.emplace((T*)this);
        }

        for (auto&& child : m_children) {
            child->get_all_in_children(objects);
        }
    }

    template <typename T> bool has_any() const {
        return std::any_of(m_children.cbegin(), m_children.cend(), [](const auto& child) { return child->is_a<T>(); });
    }

    template <typename T> bool has_any_in_children() const {
        return std::any_of(m_children.cbegin(), m_children.cend(),
            [](const auto& child) { return child->is_a<T>() || child->has_any_in_children<T>(); });
    }

    template <typename T> bool is_child_of(T* obj) const {
        const auto o = owners<T>();
        return std::any_of(o.cbegin(), o.cend(), [obj](const auto& owner) { return owner == obj; });
    }

    template <typename T> T* add(std::unique_ptr<T> object) {
        object->m_owner = this;
        return (T*)m_children.emplace_back(std::move(object)).get();
    }

    template <typename T> T* find(std::string_view name) const {
        for (auto&& child : m_children) {
            if (child->is_a<T>() && child->m_name == name) {
                return (T*)child.get();
            }
        }

        return nullptr;
    }

    template <typename T> T* find_in_owners(std::string_view name, bool include_self) const {
        auto owner = (include_self) ? this : m_owner;

        for (; owner != nullptr; owner = owner->m_owner) {
            if (auto search = owner->find<T>(name)) {
                return search;
            }
        }

        return nullptr;
    }

    template <typename T, typename... TArgs> T* find_or_add(std::string_view name, TArgs... args) {
        if (auto search = find<T>(name)) {
            return search;
        }

        return add(std::make_unique<T>(name, args...));
    }

    template <typename T, typename... TArgs> T* find_in_owners_or_add(std::string_view name, TArgs... args) {
        if (auto search = find_in_owners<T>(name, true)) {
            return search;
        }

        return add(std::make_unique<T>(name, args...));
    }

    // Will fix up a desired name so that it's usable as a C++ identifier. Things like spaces get converted to
    // underscores, and we make sure it doesn't begin with a number. More checks could be done here in the future if
    // necessary.
    virtual std::string usable_name() const {
        std::string name{};

        const auto is_variable_or_fn = this->is_a<Variable>() || this->is_a<Function>();
        const auto is_ptr_or_ref = this->is_a<Pointer>() || this->is_a<Reference>();
        const auto is_array = this->is_a<Array>();

        for (auto&& c : m_name) {
            if (c == ' ' || c == '`' || c == '!' || c == '@' || c == '#' || c == '$' || c == '%' || c == '^' || c == '/' || c == '\\'
                || (!is_ptr_or_ref && (c == '*' || c == '&'))
                || (is_variable_or_fn && (c == '.' || c == '<' || c == '>' || c == '&'))
                || (!is_variable_or_fn && (c == '[' || c == ']'))) {
                name += '_';
            } else {
                name += c;
            } 
        }

        if (!name.empty() && isdigit(name[0])) {
            name = "_" + name;
        }

        return name;
    }

protected:
    friend class Type;
    friend class Pointer;
    friend class Namespace;
    friend class Sdk;

    Object* m_owner{};

    std::string m_name{};
    std::vector<std::unique_ptr<Object>> m_children{};
    std::vector<std::string> m_metadata{};
};

template <typename T> T* cast(const Object* object) {
    if (object->is_a<T>()) {
        return (T*)object;
    }

    return nullptr;
}

class Typename : public Object {
public:
    explicit Typename(std::string_view name) : Object{name} {}

    virtual void generate_typename_for(std::ostream& os, const Object* obj) const {
        if (auto owner_type = owner<Typename>()) {
            if (obj == nullptr || owner_type != obj->owner<Typename>()) {
                auto&& name = owner_type->name();

                if (!name.empty()) {
                    owner_type->generate_typename_for(os, obj);
                    os << "::";
                }
            }
        }

        os << usable_name();
    }
};

class Type : public Typename {
public:
    explicit Type(std::string_view name) : Typename{name} {}

    virtual void generate_variable_postamble(std::ostream& os) const {}

    virtual size_t size() const { return m_size; }
    auto size(int size) {
        m_size = size;
        return this;
    }

    Reference* ref();
    Pointer* ptr();
    Array* array_(size_t count = 0);

protected:
    size_t m_size{};
};

class Reference : public Type {
public:
    explicit Reference(std::string_view name) : Type{name} {}

    auto to() const { return m_to; }
    auto to(Type* to) {
        m_to = to;
        return this;
    }

    size_t size() const override { return sizeof(uintptr_t); }

    void generate_typename_for(std::ostream& os, const Object* obj) const override {
        m_to->generate_typename_for(os, obj);
        os << "&";
    }

protected:
    Type* m_to{};
};

inline Reference* Type::ref() {
    return m_owner->find_or_add<Reference>(name() + '&')->to(this);
}

class Pointer : public Reference {
public:
    explicit Pointer(std::string_view name) : Reference{name} {}

    auto ptr() { return m_owner->find_or_add<Pointer>(m_name + '*')->to(this); }

    void generate_typename_for(std::ostream& os, const Object* obj) const override {
        m_to->generate_typename_for(os, obj);
        os << "*";
    }
};

inline Pointer* Type::ptr() {
    return (Pointer*)m_owner->find_or_add<Pointer>(name() + '*')->to(this);
}

class Array : public Type {
public:
    explicit Array(std::string_view name) : Type{name} {}

    auto of() const { return m_of; }
    auto of(Type* of) {
        m_of = of;
        return this;
    }

    auto count() const { return m_count; }
    auto count(size_t count) {
        // Fix the name of this array type.
        if (m_of != nullptr && count != m_count) {
            const auto& base = m_of->name();
            auto first_brace = base.find_first_of('[');
            auto head = base.substr(0, first_brace);
            std::string tail{};

            if (first_brace != std::string::npos) {
                tail = base.substr(first_brace);
            }

            m_name = head + '[' + std::to_string(count) + ']' + tail;
        }

        m_count = count;

        return this;
    }

    size_t size() const override {
        if (m_of == nullptr) {
            return 0;
        }

        return m_of->size() * m_count;
    }

    void generate_typename_for(std::ostream& os, const Object* obj) const override {
        m_of->generate_typename_for(os, obj);
    }

    void generate_variable_postamble(std::ostream& os) const override {
        os << "[" << std::dec << m_count << "]";
        m_of->generate_variable_postamble(os);
    }

protected:
    Type* m_of{};
    size_t m_count{};
};

inline Array* Type::array_(size_t count) {
    return (Array*)m_owner->find_or_add<Array>(name() + "[0]")->of(this)->count(count);
}

class GenericType : public Type {
public:
    explicit GenericType(std::string_view name) : Type{name} {}

    auto template_types() const { return m_template_types; }
    auto template_type(Type* type) {
        m_template_types.emplace(type);
        return this;
    }

protected:
    std::unordered_set<Type*> m_template_types{};
};

class Variable : public Object {
public:
    explicit Variable(std::string_view name) : Object{name} {}

    auto type() const { return m_type; }
    auto type(Type* type) {
        m_type = type;
        return this;
    }

    // Helper that recurses though owners to find the correct type.
    auto type(std::string_view name) {
        m_type = find_in_owners_or_add<Type>(name);
        return this;
    }

    auto offset() const { return m_offset; }
    auto offset(uintptr_t offset) {
        m_offset = offset;
        return this;
    }

    // Sets the offset to be after the last variable in the struct.
    Variable* append();

    virtual size_t size() const {
        if (m_type == nullptr) {
            return 0;
        }

        return m_type->size();
    }

    auto end() const { return offset() + size(); }

    auto bit_size(size_t size) {
        // assert(size <= m_type->size() * CHAR_BIT);
        m_bit_size = size;
        return this;
    }
    auto bit_size() const { return m_bit_size; }

    auto bit_offset(uintptr_t offset) {
        // assert(offset < m_type->size() * CHAR_BIT);
        m_bit_offset = offset;
        return this;
    }
    auto bit_offset() const { return m_bit_offset; }

    auto is_bitfield() const { return m_bit_size != 0; }

    // Call this after append() or offset()
    Variable* bit_append();

    virtual void generate(std::ostream& os) const {
        generate_metadata(os);
        m_type->generate_typename_for(os, this);
        os << " " << usable_name();
        m_type->generate_variable_postamble(os);

        if (m_bit_size != 0) {
            os << " : " << std::dec << m_bit_size;
        }

        os << "; // 0x" << std::hex << m_offset << "\n";
    }

protected:
    Type* m_type{};
    uintptr_t m_offset{};
    size_t m_bit_size{};
    uintptr_t m_bit_offset{};
};

class Constant : public Object {
public:
    explicit Constant(std::string_view name) : Object{name} {}

    auto type() const { return m_type; }
    auto type(Type* type) {
        m_type = type;
        return this;
    }

    // Helper that recurses though owners to find the correct type.
    auto type(std::string_view name) {
        m_type = find_in_owners_or_add<Type>(name);
        return this;
    }

    const auto& value() const { return m_value; }
    auto value(std::string_view value) { 
        m_value = std::move(value);
        return this;
    }

    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
    auto real(T value) { 
        m_value = std::to_string(value);
        return this;    
    }

    template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
    auto integer(T value) {
        m_value = std::to_string(value);
        return this;
    }

    auto string(const std::string& value) { 
        m_value = "\"" + value + "\"";
        return this;
    }

    virtual void generate(std::ostream& os) const {
        os << "static constexpr ";
        generate_metadata(os);
        m_type->generate_typename_for(os, this);
        os << " " << usable_name();
        m_type->generate_variable_postamble(os);
        os << " = " << m_value << ";";
    }

protected:
    Type* m_type{};
    std::string m_value{};
};

class Parameter : public Object {
public:
    explicit Parameter(std::string_view name) : Object{name} {}

    auto type() const { return m_type; }
    auto type(Type* type) {
        m_type = type;
        return this;
    }

    virtual void generate(std::ostream& os) const {
        m_type->generate_typename_for(os, this);
        os << " " << usable_name();
    }

protected:
    Type* m_type{};
};

class Function : public Object {
public:
    explicit Function(std::string_view name) : Object{name} {}

    auto param(std::string_view name) { return find_or_add<Parameter>(name); }

    auto returns() const { return m_return_value; }
    auto returns(Type* return_value) {
        m_return_value = return_value;
        return this;
    }

    auto&& procedure() const { return m_procedure; }
    auto procedure(std::string_view procedure) {
        m_procedure = procedure;
        return this;
    }

    auto&& dependent_types() const { return m_dependent_types; }
    auto depends_on(Type* type) {
        m_dependent_types.emplace(type);
        return this;
    }

    auto&& defined() const { return m_is_defined; }
    auto defined(bool is_defined) {
        m_is_defined = is_defined;
        return this;
    }

    virtual void generate(std::ostream& os) const {
        generate_prototype(os);
        os << ";\n";
    }

    virtual void generate_source(std::ostream& os) const {
        if (m_is_defined) {
            generate_procedure(os);
        }
    }

protected:
    Type* m_return_value{};
    std::string m_procedure{};
    std::unordered_set<Type*> m_dependent_types{};
    bool m_is_defined{true};

    void generate_prototype(std::ostream& os) const {
        if (m_return_value == nullptr) {
            os << "void";
        } else {
            m_return_value->generate_typename_for(os, this);
        }

        os << " ";
        generate_prototype_internal(os);
    }

    void generate_prototype_internal(std::ostream& os) const {
        os << usable_name() << "(";

        auto is_first_param = true;

        for (auto&& param : get_all<Parameter>()) {
            if (is_first_param) {
                is_first_param = false;
            } else {
                os << ", ";
            }

            param->generate(os);
        }

        os << ")";
    }

    void generate_procedure(std::ostream& os) const {
        if (m_return_value == nullptr) {
            os << "void";
        } else {
            m_return_value->generate_typename_for(os, nullptr);
        }

        os << " ";

        std::vector<const Object*> owners{};

        for (auto o = owner<Object>(); o != nullptr; o = o->owner<Object>()) {
            owners.emplace_back(o);
        }

        std::reverse(owners.begin(), owners.end());

        for (auto&& o : owners) {
            if (o->usable_name().empty()) {
                continue;
            }

            os << o->usable_name() << "::";
        }

        generate_prototype_internal(os);

        if (m_procedure.empty()) {
            os << " {}\n";
        } else {
            os << " {\n";
            {
                Indent _{os};
                os << m_procedure;
            }
            if (m_procedure.back() != '\n') {
                os << "\n";
            }
            os << "}\n";
        }
    }
};

class VirtualFunction : public Function {
public:
    explicit VirtualFunction(std::string_view name) : Function{name} {}

    auto vtable_index() const { return m_vtable_index; }
    auto vtable_index(int vtable_index) {
        m_vtable_index = vtable_index;
        return this;
    }

    void generate(std::ostream& os) const override {
        os << "virtual ";
        generate_prototype(os);

        if (m_procedure.empty()) {
            os << " = 0;\n";
        }
    }

protected:
    int m_vtable_index{};
};

class StaticFunction : public Function {
public:
    explicit StaticFunction(std::string_view name) : Function{name} {}

    void generate(std::ostream& os) const override {
        os << "static ";
        generate_prototype(os);
        os << ";\n";
    }
};

class Enum : public Type {
public:
    explicit Enum(std::string_view name) : Type{name} {}

    auto value(std::string_view name, uint64_t value) {
        for (auto&& [val_name, val_val] : m_values) {
            if (val_name == name) {
                val_val = value;
                return this;
            }
        }

        m_values.emplace_back(name, value);
        return this;
    }

    auto type() const { return m_type; }
    auto type(Type* type) {
        m_type = type;
        return this;
    }

    auto&& values() const { return m_values; }
    auto&& values() { return m_values; }

    size_t size() const override {
        if (m_type == nullptr) {
            return sizeof(int);
        } else {
            return m_type->size();
        }
    }

    virtual void generate(std::ostream& os) const {
        os << "enum " << usable_name();
        generate_type(os);
        os << " {\n";
        generate_enums(os);
        os << "};\n";
    }

protected:
    std::vector<std::tuple<std::string, uint64_t>> m_values{};
    Type* m_type{};

    void generate_type(std::ostream& os) const {
        if (m_type != nullptr) {
            os << " : ";
            m_type->generate_typename_for(os, this);
        }
    }

    void generate_enums(std::ostream& os) const {
        Indent _{os};

        for (auto&& [name, value] : m_values) {
            os << name << " = " << value << ",\n";
        }
    }
};

class EnumClass : public Enum {
public:
    explicit EnumClass(std::string_view name) : Enum{name} {}

    void generate(std::ostream& os) const override {
        os << "enum class " << usable_name();
        generate_type(os);
        os << " {\n";
        generate_enums(os);
        os << "};\n";
    }
};

class Struct : public Type {
public:
    explicit Struct(std::string_view name) : Type{name} {}

    auto variable(std::string_view name) { return find_or_add_unique<Variable>(name); }
    auto constant(std::string_view name) { return find_or_add_unique<Constant>(name); }

    // Returns a map of bit_offset, bitfield_variable at a given offset. Optionally, it will ignore a given variable
    // while constructing the map.
    auto bitfield(uintptr_t offset, Variable* ignore = nullptr) const {
        std::map<uintptr_t, Variable*> vars{};

        for (auto&& child : m_children) {
            if (auto var = dynamic_cast<Variable*>(child.get()); var != nullptr && var != ignore) {
                if (var->offset() == offset) {
                    vars[var->bit_offset()] = var;
                }
            }
        }

        return vars;
    }

    auto struct_(std::string_view name) { return find_or_add_unique<Struct>(name); }
    auto class_(std::string_view name) { return find_or_add_unique<Class>(name); }
    auto enum_(std::string_view name) { return find_or_add_unique<Enum>(name); }
    auto enum_class(std::string_view name) { return find_or_add_unique<EnumClass>(name); }
    auto function(std::string_view name) { return find_or_add_unique<Function>(name); }
    auto virtual_function(std::string_view name) { return find_or_add_unique<VirtualFunction>(name); }
    auto static_function(std::string_view name) { return find_or_add<StaticFunction>(name); }

    auto&& parents() const { return m_parents; }
    auto parent(Struct* parent) {
        if (std::find(m_parents.begin(), m_parents.end(), parent) == m_parents.end()) {
            m_parents.emplace_back(parent);
        }

        return this;
    }

    size_t size() const override {
        size_t size = 0;

        for (auto&& parent : m_parents) {
            size += parent->size();
        }

        for (auto&& var : get_all<Variable>()) {
            auto var_end = var->end();

            if (var_end > size) {
                size = var_end;
            }
        }

        if (size == 0 && has_any<VirtualFunction>()) {
            size += sizeof(uintptr_t);
        }

        return std::max<size_t>(size, m_size);
    }
    auto size(int size) {
        m_size = size;
        return this;
    }

    virtual void generate_forward_decl(std::ostream& os) const { os << "struct " << usable_name() << ";\n"; }

    virtual void generate(std::ostream& os) const {
        generate_metadata(os);
        os << "struct " << usable_name();
        generate_inheritance(os);
        os << " {\n";
        generate_internal(os);
        os << "}; // Size: 0x" << std::hex << size() << "\n";
    }

protected:
    std::vector<Struct*> m_parents{};

    int vtable_size() const {
        auto max_index = -1;

        if (!m_parents.empty()) {
            max_index = 0;

            for (auto&& parent : m_parents) {
                if (auto parent_vtable_size = parent->vtable_size(); parent_vtable_size != -1) {
                    max_index += parent_vtable_size;
                }
            }

            if (max_index == 0) {
                max_index = -1;
            }
        }

        for (auto&& child : get_all<VirtualFunction>()) {
            max_index = std::max<int>(max_index, child->vtable_index());
        }

        return max_index + 1;
    }

    template <typename T> T* find_in_parents(std::string_view name) {
        for (auto&& parent : m_parents) {
            if (auto obj = parent->find<T>(name)) {
                return obj;
            }
        }

        return nullptr;
    }

    template <typename T, typename... TArgs> T* find_or_add_unique(std::string_view name, TArgs... args) {
        if (auto search = find<T>(name); search != nullptr) {
            return search;
        }

        std::string fixed_name{};
        auto num_collisions = 0;
        auto has_collision = false;

        do {
            has_collision = false;

            if (find_in_parents<Object>(fixed_name.empty() ? name : fixed_name) != nullptr) {
                fixed_name = name;
                fixed_name += std::to_string(num_collisions);
                ++num_collisions;
                has_collision = true;
            }
        } while (has_collision);

        if (num_collisions == 0) {
            return add(std::make_unique<T>(name, args...));
        }

        return add(std::make_unique<T>(fixed_name, args...));
    }

    void generate_inheritance(std::ostream& os) const {
        if (m_parents.empty()) {
            return;
        }

        os << " : ";

        bool is_first = true;

        for (auto&& parent : m_parents) {
            if (is_first) {
                is_first = false;
            } else {
                os << ", ";
            }

            os << "public ";
            parent->generate_typename_for(os, this);
        }
    }

    void generate_bitfield(std::ostream& os, uintptr_t offset) const {
        auto last_bit = 0;
        Type* bitfield_type{};

        for (auto&& [bit_offset, var] : bitfield(offset)) {
            if (bit_offset - last_bit > 0) {
                var->type()->generate_typename_for(os, var);
                os << " pad_bitfield_" << std::hex << offset << "_" << std::hex << last_bit << " : " << std::dec
                   << bit_offset - last_bit << ";\n";
            }

            var->generate(os);
            last_bit = bit_offset + var->bit_size();
            bitfield_type = var->type();
        }

        // Fill out the remaining space in the bitfield if necessary.
         auto num_bits = bitfield_type->size() * CHAR_BIT;

        if (last_bit != num_bits) {
            auto bit_offset = num_bits;

            bitfield_type->generate_typename_for(os, nullptr);
            os << " pad_bitfield_" << std::hex << offset << "_" << std::hex << last_bit << " : " << std::dec
               << bit_offset - last_bit << ";\n";
        }
    }

    void generate_internal(std::ostream& os) const {
        Indent _{os};

        for (auto&& child : get_all<Enum>()) {
            child->generate(os);
            os << "\n";
        }

        for (auto&& child : get_all<Struct>()) {
            child->generate(os);
            os << "\n";
        }

        for (auto&& child : get_all<Constant>()) {
            child->generate(os);
            os << "\n";
        }

        std::unordered_map<std::uintptr_t, Variable*> var_map{};

        for (auto&& var : get_all<Variable>()) {
            var_map[var->offset()] = var;
        }

        auto max_offset = size();
        size_t offset = 0;

        // Skip over the vtable.
        if (has_any<VirtualFunction>()) {
            offset = sizeof(uintptr_t);
        }

        // Start off where the parent ends.
        if (!m_parents.empty()) {
            offset = 0;

            for (auto&& parent : m_parents) {
                offset += parent->size();
            }
        }

        auto last_offset = offset;

        while (offset < max_offset) {
            if (auto search = var_map.find(offset); search != var_map.end()) {
                auto var = search->second;

                // Skip variables where the user has not given us a valid size (forgot to set a type or the type is
                // unfinished).
                if (var->size() == 0) {
                    ++offset;
                    continue;
                }

                if (offset - last_offset > 0) {
                    os << "char pad_" << std::hex << last_offset << "[0x" << std::hex << offset - last_offset << "];\n";
                }

                if (var->is_bitfield()) {
                    generate_bitfield(os, offset);
                } else {
                    var->generate(os);
                }

                offset += var->size();
                last_offset = offset;
            } else {
                ++offset;
            }
        }

        if (offset - last_offset > 0) {
            os << "char pad_" << std::hex << last_offset << "[0x" << std::hex << offset - last_offset << "];\n";
        }

        if (has_any<Function>()) {
            // Generate normal functions normally.
            for (auto&& child : get_all<Function>()) {
                if (!child->is_a<VirtualFunction>()) {
                    child->generate(os);
                }
            }
        }

        if (has_any<VirtualFunction>()) {
            std::unordered_map<int, VirtualFunction*> vtable{};

            for (auto&& child : get_all<VirtualFunction>()) {
                auto vtable_index = child->vtable_index();

                vtable[vtable_index] = child;
            }

            auto vtable_index = 0;
            auto vtbl_size = vtable_size();

            for (; vtable_index < vtbl_size; ++vtable_index) {
                if (auto search = vtable.find(vtable_index); search != vtable.end()) {
                    search->second->generate(os);
                } else {
                    // Generate a default destructor to force addition of the vtable ptr.
                    if (vtable_index == 0) {
                        os << "virtual ~" << usable_name() << "() = default;\n";
                    } else {
                        os << "virtual void virtual_function_" << std::dec << vtable_index << "() = 0;\n";
                    }
                }
            }
        }
    }
};

inline Variable* Variable::append() {
    auto struct_ = owner<Struct>();
    uintptr_t highest_offset{};
    Variable* highest_var{};

    for (auto&& var : struct_->get_all<Variable>()) {
        if (var->offset() >= highest_offset && var != this) {
            highest_offset = var->offset();
            highest_var = var;
        }
    }

    if (highest_var != nullptr) {
        // Both bitfields of the same type.
        if (is_bitfield() && highest_var->is_bitfield() && m_type == highest_var->type()) {
            auto highest_bit = 0;
            auto bf = struct_->bitfield(highest_var->offset(), this);

            for (auto&& [bit_offset, bit_var] : bf) {
                if (bit_offset >= highest_bit && bit_var != this) {
                    highest_bit = bit_offset;
                    highest_var = bit_var;
                }
            }

            auto end_bit = highest_var->bit_offset() + highest_var->bit_size();

            if (end_bit + m_bit_size <= m_type->size() * CHAR_BIT) {
                // Squeeze into the remainign bits.
                m_offset = highest_var->offset();
            } else {
                // Not enough room, so start where the previous bitfield ended.
                m_offset = highest_var->end();
            }
        } else {
            m_offset = highest_var->end();
        }
    } else if (auto parents = struct_->parents(); !parents.empty()) {
        size_t size{};

        for (auto&& parent : parents) {
            size += parent->size();
        }

        m_offset = size;
    } else {
        m_offset = 0;
    }

    return this;
}

inline Variable* Variable::bit_append() {
    auto struct_ = owner<Struct>();
    uintptr_t highest_bit{};
    Variable* highest_var{};
    auto bf = struct_->bitfield(m_offset, this);

    for (auto&& [bit_offset, bit_var] : bf) {
        if (bit_offset >= highest_bit && bit_var != this) {
            highest_bit = bit_offset;
            highest_var = bit_var;
        }
    }

    if (highest_var != nullptr) {
        auto end_bit = highest_var->bit_offset() + highest_var->bit_size();

        m_bit_offset = end_bit;
    } else {
        m_bit_offset = 0;
    }

    return this;
}

class Class : public Struct {
public:
    explicit Class(std::string_view name) : Struct{name} {}

    void generate_forward_decl(std::ostream& os) const override { os << "class " << usable_name() << ";\n"; }

    void generate(std::ostream& os) const override {
        os << "class " << usable_name();
        generate_inheritance(os);
        os << " {\n";
        os << "public:\n";
        generate_internal(os);
        os << "}; // Size: 0x" << std::hex << size() << "\n";
    }
};

class Namespace : public Typename {
public:
    explicit Namespace(std::string_view name) : Typename{name} {}

    auto type(std::string_view name) { return find_in_owners_or_add<Type>(name); }
    auto generic_type(std::string_view name) { return find_in_owners_or_add<GenericType>(name); }
    auto struct_(std::string_view name) { return find_or_add<Struct>(name); }
    auto class_(std::string_view name) { return find_or_add<Class>(name); }
    auto enum_(std::string_view name) { return find_or_add<Enum>(name); }
    auto enum_class(std::string_view name) { return find_or_add<EnumClass>(name); }
    auto namespace_(std::string_view name) { return find_or_add<Namespace>(name); }
};

class Sdk {
public:
    Sdk() = default;
    virtual ~Sdk() = default;

    auto global_ns() const { return m_global_ns.get(); }

    auto preamble(std::string_view preamble) {
        m_preamble = preamble;
        return this;
    }
    auto postamble(std::string_view postamble) {
        m_postamble = postamble;
        return this;
    }

    auto include(std::string_view header) {
        m_includes.emplace(header);
        return this;
    }
    auto include_local(std::string_view header) {
        m_local_includes.emplace(header);
        return this;
    }

    void generate(const std::filesystem::path& sdk_path) const { 
        // erase the file_list.txt
        std::filesystem::remove(sdk_path / "file_list.txt");

        generate_namespace(sdk_path, m_global_ns.get()); 
    }

    const auto& header_extension() const { return m_header_extension; }
    auto header_extension(std::string_view ext) {
        m_header_extension = ext;
        return this;
    }

    const auto& source_extension() const { return m_source_extension; }
    auto source_extension(std::string_view ext) {
        m_source_extension = ext;
        return this;
    }

protected:
    std::unique_ptr<Namespace> m_global_ns{std::make_unique<Namespace>("")};
    std::string m_preamble{};
    std::string m_postamble{};
    std::set<std::string> m_includes{};
    std::set<std::string> m_local_includes{};
    std::string m_header_extension{".hpp"};
    std::string m_source_extension{".cpp"};

    std::filesystem::path path_for_object(Object* obj) const {
        std::filesystem::path path{};
        auto owners = obj->owners<Namespace>();

        std::reverse(owners.begin(), owners.end());

        for (auto&& owner : owners) {
            if (owner->usable_name().empty()) {
                continue;
            }

            path /= owner->usable_name();
        }

        path /= obj->usable_name();

        return path;
    }

    std::filesystem::path include_path_for_object(Object* obj) const {
        auto path = path_for_object(obj);
        path += m_header_extension;
        return path;
    }

    std::filesystem::path source_path_for_object(Object* obj) const {
        auto path = path_for_object(obj);
        path += m_source_extension;
        return path;
    }

    std::filesystem::path include_path(Object* from, Object* to) const {
        auto to_path = std::filesystem::absolute(include_path_for_object(to));
        auto from_path = std::filesystem::absolute(include_path_for_object(from));
        auto rel_path = std::filesystem::relative(to_path.parent_path(), from_path.parent_path()) / to_path.filename();
        return rel_path;
    }

    template <typename T> void generate_header(const std::filesystem::path& sdk_path, T* obj) const {
        auto obj_inc_path = sdk_path / include_path_for_object(obj);
        std::ofstream file_list{sdk_path / "file_list.txt", std::ios::app};
        file_list << "\"" << obj_inc_path.string() << "\" \\\n";
        std::filesystem::create_directories(obj_inc_path.parent_path());
        std::ofstream os{obj_inc_path};

        if (!m_preamble.empty()) {
            std::istringstream sstream{m_preamble};
            std::string line{};

            while (std::getline(sstream, line)) {
                os << "// " << line << "\n";
            }
        }

        os << "#pragma once\n";

        for (auto&& include : m_includes) {
            os << "#include <" << include << ">\n";
        }

        for (auto&& include : m_local_includes) {
            os << "#include \"" << include << "\"\n";
        }

        std::unordered_set<Constant*> constants{};
        std::unordered_set<Variable*> variables{};
        std::unordered_set<Function*> functions{};
        std::unordered_set<Type*> types_to_include{};
        std::unordered_set<Struct*> structs_to_forward_decl{};
        std::function<void(Type*)> add_type = [&](Type* t) {
            if (auto ref = dynamic_cast<Reference*>(t)) {
                auto to = ref->to();

                if (auto e = dynamic_cast<Enum*>(to)) {
                    types_to_include.emplace(e);
                } else if (auto s = dynamic_cast<Struct*>(to)) {
                    structs_to_forward_decl.emplace(s);
                } else {
                    add_type(to);
                }
            } else if (auto gt = dynamic_cast<GenericType*>(t)) {
                for (auto&& tt : gt->template_types()) {
                    add_type(tt);
                }
            } else if (auto e = dynamic_cast<Enum*>(t)) {
                types_to_include.emplace(e);
            } else if (auto s = dynamic_cast<Struct*>(t)) {
                types_to_include.emplace(s);
            }
        };

        obj->get_all_in_children<Constant>(constants);
        obj->get_all_in_children<Variable>(variables);
        obj->get_all_in_children<Function>(functions);

        for (auto&& c : constants) {
            add_type(c->type());
        }

        for (auto&& var : variables) {
            add_type(var->type());
        }

        for (auto&& fn : functions) {
            for (auto&& param : fn->get_all<Parameter>()) {
                add_type(param->type());
            }
            add_type(fn->returns());
        }

        if (auto s = dynamic_cast<Struct*>(obj)) {
            for (auto&& parent : s->parents()) {
                types_to_include.emplace(parent);
            }
        }

        // Go through all the types to include and replace nested types with the types they're nested within.
        for (auto it = types_to_include.begin(); it != types_to_include.end();) {
            if (auto topmost = (*it)->topmost_owner<Struct>()) {
                it = types_to_include.erase(it);

                // Skip adding the topmost owner if it's the object we're generating a header for.
                if (topmost == (Object*)obj) {
                    continue;
                }

                if (auto&& [_, was_inserted] = types_to_include.emplace(topmost); was_inserted) {
                    it = types_to_include.begin();
                }
            } else {
                ++it;
            }
        }

        for (auto&& type : types_to_include) {
            os << "#include \"" << include_path(obj, type).string() << "\"\n";
        }

        for (auto&& type : structs_to_forward_decl) {
            // Only forward decl structs we haven't already included.
            if (types_to_include.find(type) == types_to_include.end() && !type->is_child_of(obj)) {
                auto owners = type->owners<Namespace>();

                if (owners.size() > 1) {
                    std::reverse(owners.begin(), owners.end());

                    os << "namespace ";

                    for (auto&& owner : owners) {
                        if (owner->usable_name().empty()) {
                            continue;
                        }

                        os << owner->usable_name();

                        if (owner != owners.back()) {
                            os << "::";
                        }
                    }

                    os << " {\n";
                }

                type->generate_forward_decl(os);

                if (owners.size() > 1) {
                    os << "}\n";
                }
            }
        }

        auto owners = obj->owners<Namespace>();

        if (owners.size() > 1) {
            std::reverse(owners.begin(), owners.end());

            os << "namespace ";

            for (auto&& owner : owners) {
                if (owner->usable_name().empty()) {
                    continue;
                }

                os << owner->usable_name();

                if (owner != owners.back()) {
                    os << "::";
                }
            }

            os << " {\n";
        }

        os << "#pragma pack(push, 1)\n";
        obj->generate(os);
        os << "#pragma pack(pop)\n";

        if (owners.size() > 1) {
            os << "}\n";
        }

        if (!m_postamble.empty()) {
            std::istringstream sstream{m_postamble};
            std::string line{};

            while (std::getline(sstream, line)) {
                os << "// " << line << "\n";
            }
        }
    }

    template <typename T> void generate_source(const std::filesystem::path& sdk_path, T* obj) const {
        // Skip generating a source file for an object with no functions.
        if (!obj->has_any<Function>()) {
            return;
        }

        // Skip generating a source file for an object if the functions it does have are all undefined.
        auto any_defined = false;

        for (auto&& fn : obj->get_all<Function>()) {
            if (fn->defined()) {
                any_defined = true;
                break;
            }
        }

        if (!any_defined) {
            return;
        }

        auto obj_src_path = sdk_path / source_path_for_object(obj);
        std::ofstream file_list{sdk_path / "file_list.txt", std::ios::app};
        file_list << "\"" << obj_src_path.string() << "\" \\\n";

        std::filesystem::create_directories(obj_src_path.parent_path());
        std::ofstream os{obj_src_path};

        if (!m_preamble.empty()) {
            std::istringstream sstream{m_preamble};
            std::string line{};

            while (std::getline(sstream, line)) {
                os << "// " << line << "\n";
            }
        }

        std::unordered_set<Variable*> variables{};
        std::unordered_set<Function*> functions{};
        std::unordered_set<Type*> types_to_include{};
        std::function<void(Type*)> add_type = [&](Type* t) {
            if (auto ref = dynamic_cast<Reference*>(t)) {
                auto to = ref->to();

                if (auto e = dynamic_cast<Enum*>(to)) {
                    types_to_include.emplace(e);
                } else if (auto s = dynamic_cast<Struct*>(to)) {
                    types_to_include.emplace(s);
                } else {
                    add_type(to);
                }
            } else if (auto gt = dynamic_cast<GenericType*>(t)) {
                for (auto&& tt : gt->template_types()) {
                    add_type(tt);
                }
            } else if (auto e = dynamic_cast<Enum*>(t)) {
                types_to_include.emplace(e);
            } else if (auto s = dynamic_cast<Struct*>(t)) {
                types_to_include.emplace(s);
            }
        };

        if (obj->is_a<Type>()) {
            add_type(obj);
        }

        obj->get_all_in_children<Function>(functions);

        for (auto&& fn : functions) {
            for (auto&& param : fn->get_all<Parameter>()) {
                add_type(param->type());
            }
            for (auto&& dependent : fn->dependent_types()) {
                add_type(dependent);
            }
            add_type(fn->returns());
        }

        for (auto&& type : types_to_include) {
            if (!type->is_child_of(obj)) {
                os << "#include \"" << include_path(obj, type).string() << "\"\n";
            }
        }

        for (auto&& fn : functions) {
            // Skip pure virtual functions.
            if (fn->is_a<VirtualFunction>() && fn->procedure().empty()) {
                continue;
            }

            fn->generate_source(os);
        }

        if (!m_postamble.empty()) {
            std::istringstream sstream{m_postamble};
            std::string line{};

            while (std::getline(sstream, line)) {
                os << "// " << line << "\n";
            }
        }
    }

    template <typename T> void generate(const std::filesystem::path& sdk_path, Namespace* ns) const {
        for (auto&& obj : ns->get_all<T>()) {
            generate_header(sdk_path, obj);
            generate_source(sdk_path, obj);
        }
    }

    void generate_namespace(const std::filesystem::path& sdk_path, Namespace* ns) const {
        generate<Enum>(sdk_path, ns);
        generate<Struct>(sdk_path, ns);

        for (auto&& child : ns->get_all<Namespace>()) {
            generate_namespace(sdk_path, child);
        }
    }
};

#ifdef GENNY_INCLUDE_PARSER
namespace parser {
using namespace tao::pegtl;

struct ShortComment : disable<two<'/'>, until<eolf>> {};
struct LongComment : disable<one<'/'>, one<'*'>, until<seq<one<'*'>, one<'/'>>>> {};
struct Comment : sor<ShortComment, LongComment> {};
struct Sep : sor<space, Comment> {};
struct Seps : star<Sep> {};

struct Endl : star<one<';'>> {};

struct HexNum : seq<one<'0'>, one<'x'>, plus<xdigit>> {};
struct DecNum : plus<digit> {};
struct Num : sor<HexNum, DecNum> {};

struct Metadata : plus<not_one<',', ']'>> {};
struct MetadataDecl : if_must<two<'['>, Seps, list<Metadata, one<','>, Sep>, Seps, two<']'>> {};

struct IncludeId : TAO_PEGTL_STRING("#include") {};
struct IncludePath : plus<not_one<'"'>> {};
struct IncludeDecl : if_must<IncludeId, Seps, one<'"'>, IncludePath, one<'"'>> {};

struct NsId : TAO_PEGTL_STRING("namespace") {};
struct NsName : identifier {};
struct NsNameList : list_must<NsName, one<'.'>, Sep> {};
struct NsDecl : if_must<NsId, Seps, opt<NsNameList>> {};
struct NsExpr;
struct EnumExpr;
struct StructExpr;
struct NsExprs : list<sor<Sep, NsExpr, EnumExpr, StructExpr>, Seps> {};
struct NsExpr : if_must<NsDecl, Seps, one<'{'>, Seps, opt<NsExprs>, Seps, one<'}'>, Endl> {};

struct TypeId : TAO_PEGTL_STRING("type") {};
struct TypeName : identifier {};
struct TypeSize : Num {};
struct TypeDecl : if_must<TypeId, Seps, TypeName, Seps, TypeSize, Seps, opt<MetadataDecl>, Endl> {};

struct EnumVal : Num {};
struct EnumValName : identifier {};
struct EnumValDecl : seq<EnumValName, Seps, one<'='>, Seps, EnumVal> {};
struct EnumVals : seq<list<EnumValDecl, one<','>, Sep>, opt<one<','>>> {};
struct EnumId : TAO_PEGTL_STRING("enum") {};
struct EnumClassId : TAO_PEGTL_STRING("class") {};
struct EnumName : identifier {};
struct EnumType : identifier {};
struct EnumDecl : if_must<EnumId, Seps, opt<EnumClassId>, Seps, EnumName, Seps, opt<one<':'>, Seps, EnumType>> {};
struct EnumExpr : if_must<EnumDecl, Seps, one<'{'>, Seps, EnumVals, Seps, one<'}'>, Endl> {};

struct StructId : sor<TAO_PEGTL_STRING("struct"), TAO_PEGTL_STRING("class")> {};
struct StructName : identifier {};
struct StructParentPart : identifier {};
struct StructParent : list_must<StructParentPart, one<'.'>> {};
struct StructParentList : list<StructParent, one<','>, Sep> {};
struct StructParentListDecl : seq<one<':'>, Seps, StructParentList> {};
struct StructSize : Num {};
struct StructDecl : if_must<StructId, Seps, StructName, Seps, opt<StructParentListDecl>, Seps, opt<StructSize>> {};
struct StructExpr;
struct FnDecl;
struct VarDecl;
struct StructExprs : list<sor<EnumExpr, StructExpr, FnDecl, VarDecl>, Seps> {};
struct StructExpr : if_must<StructDecl, Seps, one<'{'>, Seps, opt<StructExprs>, Seps, one<'}'>, Endl> {};

struct VarTypeNamePart : identifier {};
struct VarTypeName : list_must<VarTypeNamePart, one<'.'>> {};
struct VarTypePtr : one<'*'> {};
struct VarTypeArrayCount : Num {};
struct VarTypeArray : if_must<one<'['>, VarTypeArrayCount, one<']'>> {};
struct VarType : seq<VarTypeName, star<VarTypePtr>, star<VarTypeArray>> {};
struct VarName : identifier {};
struct VarOffset : Num {};
struct VarOffsetDecl : if_must<one<'@'>, Seps, VarOffset> {};
struct VarDelta : Num {};
struct VarDeltaDecl : if_must<one<'+'>, Seps, VarDelta> {};
struct VarOffsetDeltaDecl : sor<VarOffsetDecl, VarDeltaDecl> {};
struct VarBitSize : Num {};
struct VarBitSizeDecl : seq<one<':'>, Seps, VarBitSize> {};
struct VarDecl : seq<VarType, Seps, VarName, star<VarTypeArray>, Seps, opt<VarBitSizeDecl>, Seps,
                     opt<VarOffsetDeltaDecl>, Seps, opt<MetadataDecl>, Endl> {};

struct FnVoidId : TAO_PEGTL_STRING("void") {};
struct FnRetType : VarType {};
struct FnRet : sor<FnVoidId, FnRetType> {};
struct FnName : identifier {};
struct FnParamType : VarType {};
struct FnParamName : identifier {};
struct FnParam : seq<FnParamType, Seps, FnParamName> {};
struct FnParamList : list_must<FnParam, one<','>, Sep> {};
struct FnParams : if_must<one<'('>, Seps, opt<FnParamList>, Seps, one<')'>> {};
struct FnStaticId : TAO_PEGTL_STRING("static") {};
struct FnVirtualId : TAO_PEGTL_STRING("virtual") {};
struct FnPrefix : sor<FnStaticId, FnVirtualId> {};
struct FnDecl : seq<opt<FnPrefix>, Seps, FnRet, Seps, FnName, Seps, FnParams, Endl> {};

struct Decl : sor<IncludeDecl, TypeDecl, NsExpr, EnumExpr, StructExpr> {};
struct Grammar : until<eof, sor<eolf, Sep, Decl>> {};

struct State {
    std::filesystem::path filepath{std::filesystem::current_path()};
    std::vector<Object*> parents{};
    std::stack<int> namespace_depth{};

    std::vector<std::string> metadata_parts{};
    std::vector<std::string> metadata{};

    std::string include_path{};

    std::vector<std::string> ns_parts{};
    std::vector<std::string> ns{};

    std::string type_name{};
    int type_size{};

    std::string enum_name{};
    std::string enum_type{};
    bool enum_class{};
    std::string enum_val_name{};
    uint32_t enum_val{};
    std::vector<std::tuple<std::string, uint64_t>> enum_vals{};

    std::string struct_name{};
    std::vector<std::string> struct_parent{};
    std::vector<genny::Struct*> struct_parents{};
    std::optional<size_t> struct_size{};
    bool struct_is_class{};

    genny::Type* cur_type{};
    std::vector<std::string> var_type{};
    std::optional<size_t> var_type_array_count{};
    std::vector<size_t> var_type_array_counts{};
    std::string var_name{};
    std::optional<uintptr_t> var_offset{};
    std::optional<uintptr_t> var_delta{};
    std::optional<uintptr_t> var_bit_offset{};
    std::optional<size_t> var_bit_size{};

    genny::Type* fn_ret_type{};
    std::string fn_name{};
    bool fn_is_static{};
    bool fn_is_virtual{};

    struct Param {
        genny::Type* type{};
        std::string name{};
    };

    Param cur_param{};
    std::vector<Param> fn_params{};

    // Searches for the type identified by a vector of names.
    template <typename T> T* lookup(const std::vector<std::string>& names) {
        std::function<T*(Object*, int)> search = [&](Object* parent, int i) -> T* {
            if (names.empty() || i >= names.size()) {
                return nullptr;
            }

            const auto& name = names[i];

            // Search for the name.
            auto child = parent->find<Object>(name);

            if (child == nullptr) {
                return nullptr;
            }

            // We found the name. Is this the type we were looking for?
            if (i == names.size() - 1) {
                return dynamic_cast<T*>(child);
            }

            return search(child, ++i);
        };

        for (auto it = parents.rbegin(); it != parents.rend(); ++it) {
            if (auto type = search(*it, 0)) {
                return type;
            }
        }

        return nullptr;
    }
};

template <typename Rule> struct Action : nothing<Rule> {};

template <> struct Action<Metadata> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.metadata_parts.emplace_back(in.string_view());
    }
};

template <> struct Action<MetadataDecl> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.metadata = std::move(s.metadata_parts);
    }
};

template <> struct Action<IncludePath> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.include_path = in.string_view();
    }
};

template <> struct Action<IncludeDecl> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        auto backup_filepath = s.filepath;

        try {
            auto include_path = std::move(s.include_path);
            s.filepath = (s.filepath.has_extension() ? s.filepath.parent_path() : s.filepath) / include_path;
            file_input f{s.filepath};

            if (!parse<genny::parser::Grammar, genny::parser::Action>(f, s)) {
                throw parse_error{"Failed to parse file '" + include_path + "'", in};
            }
        } catch (const parse_error& e) {
            throw e;
        } catch (const std::exception& e) {
            throw parse_error{std::string{"Failed to include file: "} + e.what(), in};
        }

        s.filepath = backup_filepath;
    }
};

template <> struct Action<NsName> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.ns_parts.emplace_back(in.string_view());
    }
};

template <> struct Action<NsNameList> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) { s.ns = std::move(s.ns_parts); }
};

template <> struct Action<NsDecl> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        if (auto cur_ns = dynamic_cast<Namespace*>(s.parents.back())) {
            auto depth = 0;

            for (auto&& ns : s.ns) {
                cur_ns = cur_ns->namespace_(ns);
                s.parents.push_back(cur_ns);
                ++depth;
            }

            s.namespace_depth.push(depth);
            s.ns.clear();
        } else {
            throw parse_error{"Can only declare a namespace within the context of another namespace", in};
        }
    }
};

template <> struct Action<NsExpr> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        auto depth = s.namespace_depth.top();

        s.namespace_depth.pop();

        for (auto i = 0; i < depth; ++i) {
            s.parents.pop_back();
        }
    }
};

template <> struct Action<TypeSize> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.type_size = std::stoi(in.string(), nullptr, 0);
    }
};

template <> struct Action<TypeName> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.type_name = in.string_view();
    }
};

template <> struct Action<TypeDecl> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        if (auto ns = dynamic_cast<Namespace*>(s.parents.back())) {
            auto type = ns->type(s.type_name);

            type->size(s.type_size);

            if (!s.metadata.empty()) {
                type->metadata() = std::move(s.metadata);
            }

            s.type_name.clear();
            s.type_size = -1;
        } else {
            throw parse_error{"Can only declare a type within the context of a namespace", in};
        }
    }
};

template <> struct Action<EnumName> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.enum_name = in.string_view();
    }
};

template <> struct Action<EnumType> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.enum_type = in.string_view();
    }
};

template <> struct Action<EnumClassId> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) { s.enum_class = true; }
};

template <> struct Action<EnumExpr> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        Enum* enum_{};

        if (auto p = dynamic_cast<Namespace*>(s.parents.back())) {
            if (s.enum_class) {
                enum_ = p->enum_class(s.enum_name);
            } else {
                enum_ = p->enum_(s.enum_name);
            }
        } else if (auto p = dynamic_cast<Struct*>(s.parents.back())) {
            if (s.enum_class) {
                enum_ = p->enum_class(s.enum_name);
            } else {
                enum_ = p->enum_(s.enum_name);
            }
        } else {
            throw parse_error{"Cannot create an enum here. Parent must be a namespace or a struct.", in};
        }

        for (auto&& [name, val] : s.enum_vals) {
            enum_->value(name, val);
        }

        s.enum_vals.clear();
        s.enum_name.clear();
        s.enum_type.clear();
        s.enum_class = false;
    }
};

template <> struct Action<EnumVal> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.enum_val = std::stoul(in.string(), nullptr, 0);
    }
};

template <> struct Action<EnumValName> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.enum_val_name = in.string_view();
    }
};

template <> struct Action<EnumValDecl> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.enum_vals.emplace_back(s.enum_val_name, s.enum_val);
    }
};

template <> struct Action<StructId> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.struct_is_class = in.string_view() == "class";
    }
};

template <> struct Action<StructName> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.struct_name = in.string_view();
    }
};

template <> struct Action<StructParentPart> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.struct_parent.emplace_back(in.string_view());
    }
};

template <> struct Action<StructParent> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        auto parent = s.lookup<genny::Struct>(s.struct_parent);

        if (parent == nullptr) {
            throw parse_error{"Can't find parent struct type with name '" + s.struct_parent.back() + "'", in};
        }

        s.struct_parents.emplace_back(parent);
        s.struct_parent.clear();
    }
};

template <> struct Action<StructSize> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.struct_size = std::stoull(in.string(), nullptr, 0);
    }
};

template <> struct Action<StructDecl> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        Struct* struct_{};

        if (auto p = dynamic_cast<Namespace*>(s.parents.back())) {
            if (s.struct_is_class) {
                struct_ = p->class_(s.struct_name);
            } else {
                struct_ = p->struct_(s.struct_name);
            }
        } else if (auto p = dynamic_cast<Struct*>(s.parents.back())) {
            if (s.struct_is_class) {
                struct_ = p->class_(s.struct_name);
            } else {
                struct_ = p->struct_(s.struct_name);
            }
        } else {
            throw parse_error{"Structs can only be declared within the context of a namespace or another struct", in};
        }

        for (auto&& parent : s.struct_parents) {
            struct_->parent(parent);
        }

        if (s.struct_size) {
            struct_->size(*s.struct_size);
        }

        s.parents.push_back(struct_);
        s.struct_name.clear();
        s.struct_parents.clear();
        s.struct_size = std::nullopt;
        s.struct_is_class = false;
    }
};

template <> struct Action<StructExpr> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) { s.parents.pop_back(); }
};

template <> struct Action<VarTypeNamePart> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.var_type.emplace_back(in.string_view());
    }
};

template <> struct Action<VarTypeName> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.cur_type = s.lookup<Type>(s.var_type);

        if (s.cur_type == nullptr) {
            throw parse_error{"Can't find type with name '" + s.var_type.back() + "'", in};
        }

        s.var_type.clear();
        s.var_type_array_counts.clear();
    }
};

template <> struct Action<VarTypePtr> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        if (s.cur_type == nullptr) {
            throw parse_error{"The current type is null", in};
        }

        s.cur_type = s.cur_type->ptr();
    }
};

template <> struct Action<VarTypeArrayCount> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.var_type_array_count = std::stoull(in.string(), nullptr, 0);
    }
};

template <> struct Action<VarTypeArray> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        if (!s.var_type_array_count) {
            throw parse_error{"The array count is invalid", in};
        }

        s.var_type_array_counts.emplace_back(*s.var_type_array_count);
        s.var_type_array_count = std::nullopt;
    }
};

template <> struct Action<VarName> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.var_name = in.string_view();
    }
};

template <> struct Action<VarOffset> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.var_offset = std::stoull(in.string(), nullptr, 0);
    }
};

template <> struct Action<VarDelta> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.var_delta = std::stoull(in.string(), nullptr, 0);
    }
};

template <> struct Action<VarBitSize> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.var_bit_size = std::stoull(in.string(), nullptr, 0);
    }
};

template <> struct Action<VarDecl> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        if (s.cur_type == nullptr) {
            throw parse_error{"The current type is null", in};
        }

        // Reverse the order because we want to adhear to how multi-dimensional arrays are declared in C/C++.
        std::reverse(s.var_type_array_counts.begin(), s.var_type_array_counts.end());

        for (auto&& count : s.var_type_array_counts) {
            s.cur_type = s.cur_type->array_(count);
        }

        s.var_type_array_counts.clear();

        if (auto struct_ = dynamic_cast<Struct*>(s.parents.back())) {
            auto var = struct_->variable(s.var_name);

            var->type(s.cur_type);

            if (s.var_bit_size) {
                var->bit_size(*s.var_bit_size);
            }

            if (s.var_offset) {
                var->offset(*s.var_offset);
            } else {
                var->append();

                if (s.var_delta) {
                    var->offset(var->offset() + *s.var_delta);
                }
            }

            if (var->is_bitfield()) {
                var->bit_append();
            }

            if (!s.metadata.empty()) {
                var->metadata() = std::move(s.metadata);
            }

            s.var_name.clear();
            s.cur_type = nullptr;
            s.var_offset = std::nullopt;
            s.var_delta = std::nullopt;
            s.var_bit_size = std::nullopt;
            s.var_bit_offset = std::nullopt;
        } else {
            throw parse_error{"Can't declare a variable outside of a struct", in};
        }
    }
};

template <> struct Action<FnRetType> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) { s.fn_ret_type = s.cur_type; }
};

template <> struct Action<FnName> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) { s.fn_name = in.string_view(); }
};

template <> struct Action<FnParamType> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.cur_param.type = s.cur_type;
    }
};

template <> struct Action<FnParamName> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.cur_param.name = in.string_view();
    }
};

template <> struct Action<FnParam> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        s.fn_params.emplace_back(std::move(s.cur_param));
    }
};

template <> struct Action<FnStaticId> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) { s.fn_is_static = true; }
};

template <> struct Action<FnVirtualId> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) { s.fn_is_virtual = true; }
};

template <> struct Action<FnDecl> {
    template <typename ActionInput> static void apply(const ActionInput& in, State& s) {
        if (auto struct_ = dynamic_cast<Struct*>(s.parents.back())) {
            Function* fn{};

            if (s.fn_is_static) {
                fn = struct_->static_function(s.fn_name);
            } else if (s.fn_is_virtual) {
                fn = struct_->virtual_function(s.fn_name);
            } else {
                fn = struct_->function(s.fn_name);
            }

            if (s.fn_ret_type) {
                fn->returns(s.fn_ret_type);
            }

            fn->defined(false);

            for (auto&& param : s.fn_params) {
                fn->param(param.name)->type(param.type);
            }

            s.fn_name.clear();
            s.fn_ret_type = nullptr;
            s.fn_params.clear();
            s.fn_is_static = false;
            s.fn_is_virtual = false;
        } else {
            throw parse_error{"Can't declare a function outside of a struct", in};
        }
    }
};
} // namespace parser
#endif

} // namespace genny
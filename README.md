# Json 是什么

`Json`（`JavaScript Object Notation`）是一个用于数据交换的文本格式。

虽然 `Json` 源至于 `JavaScript` 语言，但它只是一种数据格式，可用于任何编程语言。现时具类似功能的格式有 `XML`、`YAML`，当中以 `Json` 的语法最为简单。

例如，一个动态网页想从服务器获得数据时，服务器从数据库查找数据，然后把数据转换成 `Json` 文本格式：


```
{
    "title": "Design Patterns",
    "subtitle": "Elements of Reusable Object-Oriented Software",
    "author": [
        "Erich Gamma",
        "Richard Helm",
        "Ralph Johnson",
        "John Vlissides"
    ],
    "year": 2009,
    "weight": 1.8,
    "hardcover": true,
    "publisher": {
        "Company": "Pearson Education",
        "Country": "India"
    },
    "website": null
}
```

网页的脚本代码就可以把此 `Json` 文本解析为内部的数据结构去使用。

从此例子可看出，`Json` 是树状结构，而 `Json` 只包含 `6` 种数据类型：

* `null`: 表示为 `null`
* `boolean`: 表示为 `true` 或 `false`
* `number`: 一般的浮点数表示方式
* `string`: 表示为 `"..."`
* `array`: 表示为 `[ ... ]`
* `object`: 表示为 `{ ... }`

我们要实现的 `Json` 库，主要是完成 `3` 个需求：

把 `Json` 文本解析为一个树状数据结构。

提供接口访问该数据结构。

把数据结构转换成 `Json` 文本。

# `MyJson` 的结构设计

`Json` 库名为 `myJson`，分为 `4` 个类：

`Json` 类：管理 `JsonValue` 类，对外的接口。

`JsonValue` 类：保存 `Json` 具体值，其值是 `6` 种数据类型中的一种

`JsonParser` 类：解析类，将字符串转化 `Json` 类存储。

`JsonException` 类：异常管理类，对不符合 `Json` 格式的字符串或不符合的类型抛出异常。

## `Json`

### 命名空间

```cpp
namespace myJson { }
```

由于库常常定义大量全局名字，比如类、函数和模板等。当一个应用程序使用来自许多不同的库时，出现一些名字冲突几乎是不可避免的。

命名空间定义以关键字 `namespace` 开始，后面跟着命名空间的名字。

命名空间名字在定义命名空间的作用域内必须是唯一的。命名空间可以定义在全局作用域内，也可以定义在另一个命名空间内。但不能定义在函数或类中。

每个命名空间都是一个作用域，定义在某个命名空间中的名字可以被该命名空间内的其它成员直接访问，也可以被这些成员内嵌作用域的任何单位访问。位于该命名空间之外的代码则必须明确指出所用的名字属于哪个命名空间。

### 枚举类型

```cpp
enum class JsonType { };
```

枚举类型将一组整形常量组织在一起，枚举属于字面值常量类型。每个枚举成员是一条常量表达式。

`C++` 包含两种枚举：限定作用域（关键字 `enum class` 或者 `enum struct`）和不限定作用域（关键字 `enum`）。

* 限定作用域的枚举类型：枚举成员的名字遵循常规的作用域准则，并且在枚举类型的作用域外是不可访问的。
* 不限定作用域的枚举类型：枚举成员的作用域与枚举类型本身的作用域相同。
* 对于限定作用域的，要使用作用域运算符，例如 `JsonType::Null`。

```cpp
enum color { red, yellow, green };
enum stoplight { red, yellow, green }; // 错误：重复定义了枚举成员
enum class peppers { red, yellow, green }; // 正确
 
color eyes = green; // 正确：不限定作用域的枚举类型的枚举成员位于有效的作用域中
peppers p = green;  // 错误：peppers的枚举成员不在有效的作用域中
                    // color::green在有效作用域中，但类型错误
 
color hair = color::red;
peppers p2 = peppers::red;
```

默认情况下，枚举值从 `0` 开始依次加 `1`，我们也可以自己制定成员的专门值，如果没有显式提供初始值，当前枚举值等于之前枚举成员的值加 `1`，各枚举常量的值可以重复。

```cpp
enum class intTypes {
    charTyp = 8, shortTyp = 16, intTyp = 16,
    longTyp =32, long_longTyp = 64
};
```

### 前向声明

```cpp
class JsonValue;
```

前向声明，因为 `Json` 类管理 `JsonValue` 类，其包含一个指向 `JsonValue` 的智能指针：`std::unique_ptr<JsonValue> _jsonValue;`，而 `JsonValue` 类已经包含 `Json` 类了（需要使用 Json 类中定义的 `Json::JsonType` 以及 `operator[]` 需要返回 `Json` 类型），所以防止相互包含，使用前向声明。此时 `JsonValue` 是不完整的类型。

### `Json` 类

```cpp
class Json final { };
```

定义 `Json` 类，`final` 表示不允许别的类继承 `Json`。

### 类型别名

```cpp
using array = std::vector<Json>;
using object = std::unordered_map<std::string, Json>;
```

用 `vector` 和 `unordered_map` 来定义 `JsonType` 里的 `Array` 和 `Object`，`Object` 为键值对，键为 `string` 类型，值为 `Json` 类型。

`using` 与 `typedef` 声明做的是同样的事情，但 `using` 看起来更加清晰：

```cpp
typedef void (*FP)(int, const std::string&); // typedef
using FP = void(*)(int, const std::string&): // using
```

`using` 可以用于模板别名，`typedef` 不可用于模板别名。

### 成员变量

```
std::unique_ptr<JsonValue> _jsonValue;
```

`_jsonValue` 指向一个 `JsonValue` 类型，`unique_ptr` 独占所指向的对象。当 `unique_ptr` 被销毁时，它所指向的对象也被销毁。

### 构造、析构函数

```cpp
Json() : Json(nullptr) { };
Json(std::nullptr_t);
Json(bool val);
Json(int val) : Json(1.0 * val) { }
Json(double val);
Json(const char* cstr) : Json(std::string(cstr)) { }
Json(const std::string& val);
Json(const array& val);
Json(const object& val);
Json(std::string&& val) noexcept;
Json(array&& val) noexcept;
Json(object&& val) noexcept;
~Json();
```

这里构造和析构函数不能在头文件中定义，是因为 `std::unique_ptr` 中需要静态检测类型的大小，但是 `JsonValue` 是一个预先声明的类型，是不完整的类型，也就没法计算，所以导致报错。

构造函数使用 `make_unique`，根据传入参数的类型，构造 `JsonValue` 类型，成员变量 `_jsonValue` 指向一个动态分配的 `JsonValue`。

因为成员变量是只能指针，所指向的对象会被自动销毁，所以析构函数可以定义为默认的。

### 拷贝（移动）构造（赋值）函数

```cpp
Json(const Json&);
Json& operator=(Json);
Json(Json&&) noexcept;
```

由于一个 `unique_ptr` 拥有它指向的对象，因此 `unique_ptr` 不支持普通的拷贝或赋值操作。获取 `rhs` 的类型，根据其类型的不同，调用 `make_unique`，在动态内存中分配一个 `JsonValue` 对象并初始化它，并且，返回指向此对象的 `unique_ptr`，换句话说，`rhs` 指向的 `JsonValue` 和 `this` 指向的 `JsonValue` 是独立的。

```cpp
Json::Json(const Json& rhs) {
    switch (rhs.getType()) {
        case JsonType::Null: {
            _jsonValue = std::make_unique<JsonValue>(nullptr);
            break;
        } case JsonType::Bool: {
            _jsonValue = std::make_unique<JsonValue>(rhs.toBool());
            break;
        } case JsonType::Number: {
            _jsonValue = std::make_unique<JsonValue>(rhs.toNumber());
            break;
        } case JsonType::String: {
            _jsonValue = std::make_unique<JsonValue>(rhs.toString());
            break;
        } case JsonType::Array: {
            _jsonValue = std::make_unique<JsonValue>(rhs.toArray());
            break;
        } case JsonType::Object: {
            _jsonValue = std::make_unique<JsonValue>(rhs.toObject());
            break;
        } default: {
            break;
        }
    }
}
```

定义 `swap` 的类通常用 `swap` 来定义它们的赋值（移动）运算符。这些运算符使用了一种名为拷贝并交换的技术。这种技术将左侧对象与右侧运算对象的一个副本进行交换：

```cpp
Json& Json::operator=(Json rhs) {
    Json temp(rhs);
    swap(temp);
    return *this;
}
```

赋值运算符的参数是值传递的，就是说实参传过来的时候要拷贝给形参，那么拷贝初始化要么是调用拷贝构造函数要么是移动构造函数，这取决于实参的类型，左值拷贝，右值移动，这样的话，就实现了单一的赋值运算符实现拷贝和移动赋值功能了。

`noexcept` 表示承诺不会抛出异常，有利于简化该函数的代码，并且编译器也能执行某些特殊的优化操作。

编译器并不会在编译时检查 `noexcept`，即便函数声明了 `noexcept`，又同时有 `throw` 语句，或者调用了可能抛出异常的操作，编译同样顺利通过。

一旦一个 `noexcept` 函数抛出了异常，程序就会调用 `terminate` 以确保遵守不在运行时抛出异常的承诺。

`noexcept` 最有用的地方是用在 `move constructor` 和 `move assignment` 上，`move` 操作如果不是 `noexcept` 的，很多情况下即使逻辑上可以 `move`，编译器也会执行 `copy`。

### swap 函数

```cpp
void swap(Json& rhs);
```
我们希望 `swap` 交换指针，而不是分配 `JsonValue` 的副本。

```cpp
void Json::swap(Json& rhs) {
    using std::swap;
    swap(_jsonValue, rhs._jsonValue);
}
```

### 类型、大小与转换

```cpp
JsonType getType() const;
std::size_t size() const;
bool isNull() const;
bool isBool() const;
bool isNumber() const;
bool isString() const;
bool isArray() const;
bool isObject() const;
bool toBool() const;
double toNumber() const;
const std::string& toString() const;
const array& toArray() const;
const object& toObject() const;
```

`getType` 返回当前 `Json` 类型。
```cpp
return _jsonValue->getType();
```

`size` 计算 `Json` 的大小，调用 `JsonValue` 的成员函数（该函数仅限 `Array` 和 `Object` 类型可调用）。

```cpp
return _jsonValue->size();
```

`isXXXX` 判断当前 `Json` 类型是否是 `Null`、`Bool`、`Number`、`String`、`Array`、`Object` 类型。

```cpp
return getType() == JsonType::XXXX;
```

`toXXXX` 将当前 `Json` 类型转换为相对应的 `bool`、`double`、`string`、`array`、`object` 类型，调用 `JsonValue` 的成员函数。

```cpp
return _jsonValue->toXXXX();
```

### operatorp[]

```cpp
Json& operator[](std::size_t);
const Json& operator[](std::size_t) const;
Json& operator[](const std::string&);
const Json& operator[](const std::string&) const;
```

对 `Array` 和 `Object` 类型取下标或关键字，分为 `const` 版本和非 `const` 版本，也是调用 `JsonValue` 的成员函数（该函数仅限 `Array` 和 `Object` 类型可调用）。

```cpp
return _jsonValue->operator[](pos);
return _jsonValue->operator[](key);
```

`const Json` 调用 `const` 版本，返回 `const Json` 类型，不可以修改，反之，普通的 `Json` 调用非 `const` 版本，返回普通的 `Json` 类型，可以修改。

### 解析函数

```cpp
static Json parse(const std::string& content, std::string& errMsg);
```

`parse` 成员函数使用 `string` 类型参数 `content` 构造 `JsonParser` 类型的变量 `p`，然后调用 `p.parse` 解析为 `Json` 类型，如果解析出错，抛出异常，错误信息保存在 `errMsg` 中，`Json` 保存为 `Null` 类型，否则，`errMsg` 为空字符串。

```cpp
Json Json::parse(const std::string& content, std::string& errMsg) {
    try {
        JsonParser p(content);
        return p.parse();
    } catch (JsonException& e) {
        errMsg = e.what();
        return Json(nullptr);
    }
}
```

`parse` 成员函数为 `static` 可以使得不需要定义一个对象就能调用该成员函数，例如：

```cpp
Json json_ = Json::parse(jsonStr, errMsg);
```

### 序列化

```cpp
std::string serializeString() const;
std::string serializeArray() const;
std::string serializeObject() const;
std::string serialize() const;
```

`serialize` 成员函数将当前 `Json` 类型中保存的 `JsonValue` 值转换为 `string` 类型，即把树形数据结构转换成 `Json` 文本。不做换行、缩进等美化处理，因此它生成的 `Json` 会是单行、无空白字符的最紧凑形式。

如果是 `Null`、`Bool` 类型，返回对应的字符串常量 `null` 或 `true（false）`。

如果是 `Number` 类型，调用 `toNumber()` 后写进 `buf` 中，用科学计数法保存 `17` 位小数，再转换成 `string`，足够把双精度浮点转换成可还原的文本（双精度完全保证的有效数字是 `15` 位，部分数值保证的有效数字是 `16` 位）。


如果是 `String` 类型，调用 `serializeString`。

如果是 `Array` 类型，调用 `serializeArray`。

如果是 `Object` 类型，调用 `serializeObject`。

```cpp
std::string Json::serialize() const  {
    switch (_jsonValue->getType()) {
        case JsonType::Null:
            return "null";
        case JsonType::Bool:
            return _jsonValue->toBool() ? "true" : "false";
        case JsonType::Number:
            char buf[32];
            sprintf(buf, "%.17g", _jsonValue->toNumber());
            return std::string(buf);
        case JsonType::String:
            return serializeString();
        case JsonType::Array:
            return serializeArray();
        default:
            return serializeObject();
    }
}
```

#### 序列化 `String`

`serializeString` 序列化字符串，对于非转义字符或非十六进制数正常输出即可。

遇到转义，替换成转义字符，例如：

读取到回车（`c == '\n'`），我们需要向字符串写入 `\n`，即 `res += "\\n"`。第一个 `\` 是第二个 `\` 的转义，如果直接写入 `\n`，字符串添加的是回车而不是 `\n`，换句话说，遇到回车我们应该输出 `\n` 而不是输出回车。

遇到控制字符，即 `c < 0x20`，将其以 `4` 位十六进制 `\uXXXX` 的形式输出。

注意到 `char` 带不带符号，是实现定义的。如果编译器定义 `char` 为带符号的话，`(unsigned char)ch >= 0x80` 的字符，都会变成负数，即对于 `ASCII` 以外的字符会出错，使用 `static_cast` 将其转型为 `unsigned char`。

十六进位输出的字母可以用大写或小写，我们这里选择了大写，所以测试时也用大写。但这个并不是必然的，输出小写（用 "`\\u%04x`"）也可以。

最后在两端加上 `""` 表示字符串。

```cpp
std::string Json::serializeString() const {
    std::string res = "\"";
    for (auto c : _jsonValue->toString()) {
        switch (c) {
            case '\"':
                res += "\\\"";
                break;
            case '\\':
                res += "\\\\";
                break;
            case '\b':
                res += "\\b";
                break;
            case '\f':
                res += "\\f";
                break;
            case '\n':
                res += "\\n";
                break;
            case '\r':
                res += "\\r";
                break;
            case '\t':
                res += "\\t";
                break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[7];
                    sprintf(buf, "\\u%04X", c);
                    res += buf;
                } else {
                    res += c;
                }
        }
    }
    return res + '\"';
}
```

#### 序列化 `Array`

```cpp
std::string Json::serializeArray() const {
    std::string res = "[ ";
    for (std::size_t i = 0; i != _jsonValue->size(); ++i) {
        if (i > 0) {
            res += ", ";
        }
        res += this->operator[](i).serialize();
    }
    return res + " ]";
}
```

对 `Array` 里每个元素序列化，中间加上 `, `，最后两端加上 `[  ]`。


#### 序列化 `Object`

```cpp
std::string Json::serializeObject() const {
    std::string res = "{ ";
    bool first = true;
    for (auto&& p : _jsonValue->toObject()) {
        if (first) {
            first = false;
        } else {
            res += ", ";
        }
        res += "\"" + p.first + "\"";
        res += ": ";
        res += p.second.serialize();
    }
    return res + " }";
}
```

对 `Object` 里每个元素序列化，格式化为 `"key": value` 的形式，中间加上 `, `，最后两端加上 `{  }`。

### 输出、比较控制符

```cpp
inline std::ostream& operator<<(std::ostream& os, const Json& json) {
    return os << json.serialize();
}

bool operator==(const Json&, const Json&);

inline bool operator!=(const Json& lhs, const Json& rhs) {
    return !(lhs == rhs);
}
```

重载输出运算符，输出其序列化后的 `string`。

重载相等、不等运算符。

```cpp
bool operator==(const Json& lhs, const Json& rhs) {
    if (lhs.getType() != rhs.getType()) {
        return false;
    }
    switch (lhs.getType()) {
        case JsonType::Null: {
            return true;
        }
        case JsonType::Bool: {
            return lhs.toBool() == rhs.toBool();
        }
        case JsonType::Number: {
            return lhs.toNumber() == rhs.toNumber();
        }
        case JsonType::String: {
            return lhs.toString() == rhs.toString();
        }
        case JsonType::Array: {
            return lhs.toArray() == rhs.toArray();
        }
        case JsonType::Object: {
            return lhs.toObject() == rhs.toObject();
        }
        default: { return false; }
    }
}
```

如果类型不等，则必不相等，如果类型相等，再转换后判断是否相等，例如，两个 `Array` 类型的 `Json` 比较，会递归调用 `operator==` 来判断两个 `Json` 是否相等。

## `JsonValue`

### 私有成员

`_val` 是一个 `std::variant`，可能的取值为 `nullptr_t`、`bool`、`double`、`std::string`、`Json::array` 或 `Json::object` 中的一个。

`std::variant` 表示一个类型安全的联合体。 `std::variant` 的一个实例在任意时刻要么保有其一个可选类型之一的值，要么在错误情况下无值。

### 构造、析构函数

```cpp
explicit JsonValue(std::nullptr_t) : _val(nullptr) { }
explicit JsonValue(bool val) : _val(val) { }
explicit JsonValue(double val) : _val(val) { }
explicit JsonValue(const std::string& val) : _val(val) { }
explicit JsonValue(const Json::array& val) : _val(val) { }
explicit JsonValue(const Json::object& val) : _val(val) { }
explicit JsonValue(std::string&& val) noexcept : _val(std::move(val)) { }
explicit JsonValue(Json::array&& val) noexcept : _val(std::move(val)) { }
explicit JsonValue(Json::object&& val) noexcept : _val(std::move(val)) { }
~JsonValue() = default;
```

构造析构函数可以在头文件中定义，`explicit` 表示禁止隐式转换。

拷贝（移动）构造（赋值）函数使用合成的。

### 类型、大小与转换

```cpp
JsonType getType() const;
size_t size() const;
std::nullptr_t toNull() const;
bool toBool() const;
double toNumber() const;
const std::string& toString() const;
const Json::array& toArray() const;
const Json::object& toObject() const;
```

`getType` 获得当前 `JsonValue` 的类型。

```cpp
JsonType JsonValue::getType() const {
    if (std::holds_alternative<std::nullptr_t>(_val)) {
        return JsonType::Null;
    } else if (std::holds_alternative<bool>(_val)) {
        return JsonType::Bool;
    }
    // ...
}
```

`std::holds_alternative<T>(v)` 可查询变体类型 `v` 是否存放了 `T` 类型的数据。

获得当前 `JsonValue` 的大小（当且仅当 `JsonValue` 为 `Array` 或 `Object` 类型时可调用）。

`size` 计算 `JsonValue` 的大小（该函数仅限 `Array` 和 `Object` 类型可调用）。

```cpp
size_t JsonValue::size() const {
    if (std::holds_alternative<Json::array>(_val)) {
        return std::get<Json::array>(_val).size();
    } else if (std::holds_alternative<Json::object>(_val)) {
        return std::get<Json::object>(_val).size();
    } else {
        throw JsonException("not a array or object");
    }
}
```

`std::get<T>(v)` 如果变体类型 `v` 存放的数据类型为 `T`，那么返回所存放的数据，否则报错。

`toXXXX` 将当前 `JsonValue` 类型转换为相对应的 `nullptr_t`、`bool`、`double`、`string`、`array`、`object` 类型。

```cpp
XXXX JsonValue::toXXXX() const {
    if (this->getType() == JsonType::XXXX) {
        return std::get<XXXX>(_val);
    } else {
        throw JsonException("not a XXXX");
    }
}
```

### operator[]

```cpp
const Json& operator[](size_t) const;
Json& operator[](size_t);
const Json& operator[](const std::string&) const;
Json& operator[](const std::string&);
```

通过下标或关键字取值，`const` 版本和非 `const` 版本。

```cpp
const Json& JsonValue::operator[](size_t pos) const {
    if (std::holds_alternative<Json::array>(_val)) {
        return std::get<Json::array>(_val)[pos];
    } else {
        throw JsonException("not a array");
    }
}
```

`return` 返回普通版本的 `Json`，通过返回类型转换为 `const` 版本。

```cpp
Json& JsonValue::operator[](size_t pos) {
    return const_cast<Json&>(static_cast<const JsonValue&>(*this)[pos]);
}
```

`return` 调用 `const` 版本的 `operator=`，并通过 `const_cast<Json&>` 转换为普通版本返回。

## `JsonParser`

```cpp
constexpr bool is1to9(char ch) { return ch >= '1' && ch <= '9'; }
constexpr bool is0to9(char ch) { return ch >= '0' && ch <= '9'; }
```

`constexpr` 限定在了编译期常量（前提是为了算出它所依赖的东西也是在编译期可以算出来的）。`const` 只保证了运行时不直接被修改（但仍然可能是个动态变量）。

其功能分别是判断 `ch` 是否是 `'0'` 到 `'9'` 和 `'1'` 到 `'9'` 之间的字符。


### 成员变量

```
const char* _start;
const char* _curr;
```

`_start` 指向处理开始部分，`_curr` 指向当前处理字符。

我们使用指针 `_curr` 来表示当前的解析字符位置。这样做有两个好处，一是代码更简单，二是在某些编译器下性能更好。如果校验成功，`Json` 的 `value` 部分由 `_curr` 至 `_start` 之间，再把 `_curr` 赋值至 `_start`。

### 构造析构函数

```cpp
explicit JsonParser(const char* cstr) : _start(cstr), _curr(cstr) { }
explicit JsonParser(const std::string& content) : _start(content.c_str()), _curr(content.c_str()) { }
~JsonParser() = default;
```

开始时，`_start` 与 `_curr` 都指向处理开始部分。

### 拷贝（赋值）构造（移动）函数

```cpp
JsonParser(const JsonParser&) = delete;
JsonParser& operator=(const JsonParser&) = delete;
```

该类不允许拷贝，拷贝（赋值）构造函数定义为删除的。

当类的类成员定义了自己的拷贝（赋值）构造函数且未定义移动构造（赋值）函数，不会使用移动版本。

### 解析

```cpp
void parseWhitespace();
Json parse();
unsigned parse4hex();
std::string encodeUTF8(unsigned u);
std::string parseRawString();
Json parseLiteral(const std::string& literal);
Json parseValue();
Json parseNumber();
Json parseString();
Json parseArray();
Json parseObject();
```

下面是 Json 的语法子集：

```
Json-text = ws value ws
ws = *(%x20 / %x09 / %x0A / %x0D)
```

当中 `%xhh` 表示以十六进制`表示的字符，`/` 是多选一，`*` 是零或多个，`()` 用于分组。

那么第一行的意思是，`Json` 文本由 `3` 部分组成，首先是空白，接着是一个值，最后是空白。

第二行告诉我们，所谓空白，是由零或多个空格符（`space U+0020`）、制表符（`tab U+0009`）、换行符（`LF U+000A`）、回车符（`CR U+000D`）所组成。

我们的解析器应能判断输入是否一个合法的 `Json`。如果输入的 `Json` 不合符这个语法，我们要产生对应的错误码，方便使用者追查问题。

**`parseWhitespace` 解析空格**

```cpp
void JsonParser::parseWhitespace() {
    while (*_curr == ' ' || *_curr == '\t' || *_curr == '\n' || *_curr == '\r') {
        ++_curr;
    }
    _start = _curr;
}
```

遇到空格则跳过，`++_curr`。

`parse` 进行 `Json` 的解析。

跳过空白---解析 `Json`---跳过空白

若一个值之后，在空白之后还有其他字符，抛出 `parse root not singular` 的异常。

```cpp
Json JsonParser::parse() {
    parseWhitespace();
    Json json = parseValue();
    parseWhitespace();
    if (*_curr) {
        throw(JsonException("parse root not singular"));
    }
    return json;
}
```

由于 `Json` 语法特别简单，我们只需检测下一个字符，便可以知道它是哪种类型的值，然后调用相关的分析函数。对于完整的 `Json` 语法，跳过空白后，只需检测当前字符：

* n ➔ null
* t ➔ true
* f ➔ false
* " ➔ string
* 0-9/- ➔ number
* [ ➔ array
* { ➔ object

**`parseValue` 解析跳过空格后的字符串**

```cpp
Json JsonParser::parseValue() {
    switch (*_curr) {
    case 'n':
        return parseLiteral("null");
    case 't':
        return parseLiteral("true");
    case 'f':
        return parseLiteral("false");
    case '\"':
        return parseString();
    case '[':
            return parseArray();
    case '{':
        return parseObject();
    case '\0':
        throw JsonException("parse expect value");
    default:
        return parseNumber();
    }
}
```

若一个 `Json` 只含有空白，抛出 `parse expect value` 的异常。

**`parseLiteral` 解析 `true`、`false`、`null`**

```
value = null / false / true
null  = "null"
false = "false"
true  = "true"
```
解析字面值类型 `null`、`false` 或 `true`，它们分别有对应的字面值（`literal`）。

```cpp
Json JsonParser::parseLiteral(const std::string& literal) {
    if (strndcmp(_curr, literal.c_str(), literal.size()) != 0) {
        throw JsonException("parse invalid value");
    }
    _curr += literal.size();
    _start = _curr;
    switch (literal[0]) {
    case 't':
        return Json(true);
    case 'f':
        return Json(false);
    default:
        return Json(nullptr);
    }
}
```

`literal` 取值为 `null`、`true`、`false` 中的一个，具体取决于解析哪个，使用 `strndcmp` 和当前字符串指针进行对比，如果没有匹配上，抛出 `parse invalid value`。

例如 `parseLiteral("true");`，但是 `_curr = ture`。

如果匹配上了，则根据匹配的不同构造 `Json` 对象返回，`_curr` 指针向后移动。

**`parseNumber` 解析数字**

```
number = [ "-" ] int [ frac ] [ exp ]
int = "0" / digit1-9 *digit
frac = "." 1*digit
exp = ("e" / "E") ["-" / "+"] 1*digit
```

负号最简单，有的话跳过便行。

```cpp
if (*_curr == '-') {
    ++_curr;
}
```

整数部分有两种合法情况，一是单个 `0`，否则是一个 `1-9` 再加上任意数量的 `digit`。对于第一种情况，我们像负号般跳过便行。对于第二种情况，第一个字符必须为 `1-9`，如果否定的就是不合法的，可立即抛出异常。然后，有多少个 `digit` 就跳过多少个。

```cpp
if (*_curr == '0') {
        ++_curr;
} else {
    if (!is1to9(*_curr)) {
        throw JsonException("parse invalid value");
    }
    while (is0to9(*++_curr)) {
        ;
    }
}
```

如果出现小数点，我们跳过该小数点，然后检查它至少应有一个 `digit`，不是 `digit` 就抛出异常。跳过首个 `digit`，我们再检查有没有 `digit`，有多少个跳过多少个。

```cpp
if (*_curr == '.') {
    if (!is0to9(*++_curr)) {
        throw JsonException("parse invalid value");
    }
    while (is0to9(*++_curr)) {
        ;
    }
}
```

最后，如果出现大小写 `e`，就表示有指数部分。跳过那个 `e` 之后，可以有一个正或负号，有的话就跳过。然后和小数的逻辑是一样的。

```cpp
if (*_curr == 'E' || *_curr == 'e') {
    ++_curr;
    if (*_curr == '-' || *_curr == '+') {
        ++_curr;
    }
    if (!is0to9(*_curr)) {
        throw JsonException("parse invalid value");
    }
    while (is0to9(*++_curr)) {
        ;
    }
}
```

为了简单起见，`myJson` 将使用标准库的 `strtod()` 来进行转换。`strtod()` 可转换 `Json` 所要求的格式，但问题是，一些 `Json` 不容许的格式，`strtod()` 也可转换，所以我们需要自行做格式校验。

```cpp
double val = strtod(_start, nullptr);
```

数字过大的处理。

```cpp
if (fabs(val) == HUGE_VAL) {
    throw JsonException("parse number too big");
}
```

```cpp
Json JsonParser::parseNumber() {
    if (*_curr == '-') {
        ++_curr;
    }
    if (*_curr == '0') {
        ++_curr;
    } else {
        if (!is1to9(*_curr)) {
            throw JsonException("parse invalid value");
        }
        while (is0to9(*++_curr)) {
            ;
        }
    }
    if (*_curr == '.') {
        if (!is0to9(*++_curr)) {
            throw JsonException("parse invalid value");
        }
        while (is0to9(*++_curr)) {
            ;
        }
    }
    if (*_curr == 'E' || *_curr == 'e') {
        ++_curr;
        if (*_curr == '-' || *_curr == '+') {
            ++_curr;
        }
        if (!is0to9(*_curr)) {
            throw JsonException("parse invalid value");
        }
        while (is0to9(*++_curr)) {
            ;
        }
    }
    double val = strtod(_start, nullptr);
    if (fabs(val) == HUGE_VAL) {
        throw JsonException("parse number too big");
    }
    _start = _curr;
    return Json(val);
}
```

**`parseRawString` 解析字符串**

`Json` 的字符串语法以双引号把字符括起来，如 `"Hello"`。但字符串采用了双引号作分隔，那么怎样可以在字符串中插入一个双引号？ 把 `a"b` 写成 `"a"b"` 肯定不行，都不知道那里是字符串的结束了。因此，我们需要引入转义字符，`C++` 和 `Json` 都使用 `\`（反斜线）作为转义字符，那么 " 在字符串中就表示为 `\"`，`a"b` 的 `Json` 字符串则写成 `"a\"b"`。如以下的字符串语法所示，`Json` 共支持 `9` 种转义序列：

```
string = quotation-mark *char quotation-mark
char = unescaped /
   escape (
       %x22 /          ; "    quotation mark  U+0022
       %x5C /          ; \    reverse solidus U+005C
       %x2F /          ; /    solidus         U+002F
       %x62 /          ; b    backspace       U+0008
       %x66 /          ; f    form feed       U+000C
       %x6E /          ; n    line feed       U+000A
       %x72 /          ; r    carriage return U+000D
       %x74 /          ; t    tab             U+0009
       %x75 4HEXDIG )  ; uXXXX                U+XXXX
escape = %x5C          ; \
quotation-mark = %x22  ; "
unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
```

简单翻译一下，`Json` 字符串是由前后两个双引号夹着零至多个字符。字符分为无转义字符或转义序列。转义序列有 `9` 种，都是以反斜线开始，如常见的 `\n` 代表换行符。比较特殊的是 `\uXXXX`，当中 `XXXX` 为十六进制的 `UTF-16` 编码。

在 `C++` 语言中，字符串一般表示为空结尾字符串，即以空字符（`'\0'`）代表字符串的结束。然而，`Json` 字符串是允许含有空字符的，例如这个 `Json` `"Hello\u0000World"` 就是单个字符串，解析后为 `11` 个字符。如果纯粹使用空结尾字符串来表示 `Json` 解析后的结果，就没法处理空字符。

如果遇到 `"` 表示字符串解析结束，返回解析到的 `string`。如果遇到 `\0` 表示缺少了 `"`。

转义序列的解析很直观，对其他不合法的字符 抛出 `parse invalid string escape` 异常。

```cpp
while (true) {
    switch (*++_curr) {
        case '\"':
            _start = ++_curr;
            return str;
        case '\0':
            throw JsonException("parse miss quotation mark");
        case '\\':
            switch (*++_curr) {
                case '\"':
                    str.push_back('\"');
                    break;
                case '\\':
                    str.push_back('\\');
                    break;
                case '/':
                    str.push_back('/');
                    break;
                case 'b':
                    str.push_back('\b');
                    break;
                case 'f':
                    str.push_back('\f');
                    break;
                case 'n':
                    str.push_back('\n');
                    break;
                case 't':
                    str.push_back('\t');
                    break;
                case 'r':
                    str.push_back('\r');
                    break;
                default:
                    throw (JsonException("parse invalid string escape"));
            } break;
    }
}
```

上面已解决不合法转义，余下部分是要从语法中知道哪些是不合法字符：

```
unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
```

当中空缺的 `%x22` 是双引号，`%x5C` 是反斜线，都已经处理。所以不合法的字符是 `%x00` 至 `%x1F`。我们简单地在 `default` 里处理：

```cpp
default:
    if (static_cast<unsigned char>(*_curr) < 0x20) {
        throw(JsonException("parse invalid string char"));
    }
    str.push_back(*_curr);
```

注意到 `char` 带不带符号，是实现定义的。如果编译器定义 `char` 为带符号的话，`(unsigned char)ch >= 0x80` 的字符，都会变成负数，即对于 `ASCII` 以外的字符会出错，使用 `static_cast` 将其转型为 `unsigned char`。

由于 `UTF-8` 的普及性，大部分的 `Json` 也通常会以 `UTF-8` 存储。我们的 `Json` 库也会只支持 `UTF-8`。

对于非转义的字符，只要它们不少于 `32`（`0 ~ 31` 是不合法的编码单元），我们可以直接复制至结果。我们假设输入是以合法 `UTF-8` 编码。

而对于 `Json` 字符串中的 `\uXXXX` 是以十六进制表示码点 `U+0000` 至 `U+FFFF`，我们需要：

* 解析 `4` 位十六进制整数为码点；
* 由于字符串是以 `UTF-8` 存储，我们要把这个码点编码成 `UTF-8`。

码点：

一个字符集一般可以用一张或多张由多个行和多个列所构成的二维表来表示。

二维表中行与列相交的点，称之为码点，也称之为码位；每个码点分配一个唯一的编号，称之为码点值或码点编号，除开某些特殊区域（比如代理区、专用区）的非字符码点和保留码点，每个码点唯一对应于一个字符。

因此，除开非字符码点和保留码点，码点值（即码点编号）通常来说就是其所对应的字符的编号，所以码点值有时也可以直接称之为字符编号。

目前 `Unicode` 标准中，将字符按照一定的类别划分到 `0~16` 这 `17` 个平面中，每个平面中拥有 `2 ^ 16 = 65536` 个码点，因此，目前 `Unicode` 字符集所拥有的码点总数，也就是 `Unicode` 的编号空间为 `17 * 65536 = 1114112`。

`U+0000` 至 `U+FFFF` 这组 `Unicode` 字符称为基本多文种平面，还有另外 `16` 个平面。那么 `BMP` 以外的字符，`Json` 会使用代理对（`surrogate pair`）表示 `\uXXXX\uYYYY`。在 `BMP` 中，保留了 `2048` 个代理码点。如果第一个码点是 `U+D800` 至 `U+DBFF`，我们便知道它的代码对的高代理项（`high surrogate`），之后应该伴随一个 `U+DC00` 至 `U+DFFF` 的低代理项（`low surrogate`）。然后，我们用下列公式把代理对 (`H`, `L`) 变换成真实的码点：

```
codepoint = 0x10000 + (H − 0xD800) × 0x400 + (L − 0xDC00)
```

举个例子，高音谱号字符 `𝄞` → `U+1D11E` 不是 `BMP` 之内的字符。在 `Json` 中可写成转义序列 `\uD834\uDD1E`，我们解析第一个 `\uD834` 得到码点 `U+D834`，我们发现它是 `U+D800` 至 `U+DBFF` 内的码点，所以它是高代理项。然后我们解析下一个转义序列 `\uDD1E` 得到码点 `U+DD1E`，它在 `U+DC00` 至 `U+DFFF` 之内，是合法的低代理项。我们计算其码点：

```
H = 0xD834, L = 0xDD1E
codepoint = 0x10000 + (H − 0xD800) × 0x400 + (L − 0xDC00)
          = 0x10000 + (0xD834 - 0xD800) × 0x400 + (0xDD1E − 0xDC00)
          = 0x10000 + 0x34 × 0x400 + 0x11E
          = 0x10000 + 0xD000 + 0x11E
          = 0x1D11E
```

这样就得出这转义序列的码点，然后我们再把它编码成 `UTF-8`。如果只有高代理项而欠缺低代理项，或是低代理项不在合法码点范围，抛出 `parse invalid unicode surrogate` 错误。如果 `\u` 后不是 `4` 位十六进位数字，则抛出 `parse invalid unicode hex` 错误。

我们的 `Json` 库也只支持 `UTF-8`，我们需要把码点编码成 `UTF-8`。这里简单介绍一下 `UTF-8` 的编码方式。

`UTF-8` 的编码单元为 `8` 位（`1` 字节），每个码点编码成 `1` 至 `4` 个字节。它的编码方式很简单，按照码点的范围，把码点的二进位分拆成 `1` 至最多 `4` 个字节：

| 码点范围 | 码点位数 | 字节1 | 字节2 | 字节3 | 字节4 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| U+0000 ~ U+007F | 7 | 0xxxxxxx |   |   |   |
| U+0080 ~ U+07FF | 11 | 110xxxxx | 10xxxxxx |   |   |
| U+0800 ~ U+FFFF | 16 | 1110xxxx | 10xxxxxx | 10xxxxxx |   |
| U+0800 ~ U+FFFF | 21 | 10xxxxxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |

这个编码方法的好处之一是，码点范围 `U+0000 ~ U+007F` 编码为一个字节，与 `ASCII` 编码兼容。这范围的 `Unicode` 码点也是和 `ASCII` 字符相同的。因此，一个 `ASCII` 文本也是一个 `UTF-8` 文本。

举一个例子解析多字节的情况，欧元符号 € → U+20AC：

* `U+20AC` 在 `U+0800 ~ U+FFFF` 的范围内，应编码成 `3` 个字节。
* U+20AC 的二进位为 `10000010101100`
* `3` 个字节的情况我们要 `16` 位的码点，所以在前面补两个 `0`，成为 `0010000010101100`
* 按上表把二进位分成 `3` 组：`0010`, `000010`, `101100`
* 加上每个字节的前缀：`11100010`, `10000010`, `10101100`
* 用十六进位表示即：`0xE2`, `0x82`, `0xAC`

```cpp
case 'u': {
unsigned u1 = parse4hex();
if (u1 >= 0xd800 && u1 <= 0xdbff) {
    if (*++_curr != '\\') {
        throw JsonException("parse invalid unicode surrogate");
    }
    if (*++_curr != 'u') {
        throw JsonException("parse invalid unicode surrogate");
    }
    unsigned u2 = parse4hex();
    if (u2 < 0xdc00 || u2 > 0xdfff) {
        throw(JsonException("parse invalid unicode surrogate"));
    }
    u1 = (((u1 - 0xd800) << 10) | (u2 - 0xdc00)) + 0x10000;
}
str += encodeUTF8(u1);
} break;
```

遇到 `\u` 转义时，调用 `parse4hex()` 解析 `4` 位十六进数字，这个函数在成功时返回解析后的十进制，失败抛出 `parse invalid unicode hex`。最后，把码点编码成 `UTF-8`。遇到高代理项，就需要把低代理项 `\uxxxx` 也解析进来，然后用这两个项去计算出码点。

**`parse4hex` 转换 `4` 位十六进制字符串**

```cpp
unsigned JsonParser::parse4hex() {
    unsigned u = 0;
    for (int i = 0; i != 4; ++i) {
        char ch = *++_curr;
        u <<= 4;
        if (ch >= '0' && ch <= '9') {
            u |= ch - '0';
        } else if (ch >= 'A' && ch <= 'F') {
            u |= ch - ('A' - 10);
        } else if (ch >= 'a' && ch <= 'f') {
            u |= ch - ('a' - 10);
        } else {
            throw JsonException("parse invalid unicode hex");
        }
    }
    return u;
}
```

**`encodeUTF8` 编码 `UTF-8`**

这个函数只需要根据那个 `UTF-8` 编码表就可以实现：

```cpp
std::string JsonParser::encodeUTF8(unsigned u) {
    std::string utf8;
    if (u <= 0x7F) {
        utf8.push_back(static_cast<char>(u & 0xff));
    } else if (u <= 0x7FF) {
        utf8.push_back(static_cast<char>(0xc0 | ((u >> 6) & 0xff)));
        utf8.push_back(static_cast<char>(0x80 | (u & 0x3f)));
    } else if (u <= 0xFFFF) {
        utf8.push_back(static_cast<char>(0xe0 | ((u >> 12) & 0xff)));
        utf8.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3f)));
        utf8.push_back(static_cast<char>(0x80 | (u & 0x3f)));
    } else {
        assert(u <= 0x10FFFF);
        utf8.push_back(static_cast<char>(0xf0 | ((u >> 18) & 0xff)));
        utf8.push_back(static_cast<char>(0x80 | ((u >> 12) & 0x3f)));
        utf8.push_back(static_cast<char>(0x80 | ((u >> 6) & 0x3f)));
        utf8.push_back(static_cast<char>(0x80 | (u & 0x3f)));
    }
    return utf8;
}
```

举个具体例子，字符串为 `"\u4E1C"`，解析后字符串是 `"东"`，当遇到 `\u` 后，调用 `parseRawString` 解析 `4E1C`，`parse4hex` 将其转换为十进制的 `19996`，即十六进制的 `4E1C`。

`encodeUTF8` 将其编码，其范围在 `U+0800 ~ U+FFFF` 之间，应编码成 `3` 个字节。

`4E1C` 的二进位为 `0100111000011100`，按表把二进位分成 `3` 组：`0100`, `111000`, `011100`。

加上每个字节的前缀：`11100100`, `10111000`, `10011100`，用十六进位表示即：`0xE4`, `0xB8`, `0x9C`，用十进制表示为 `228`, `184`, `156`。

也就是说，`string` 中 `push` 进去的是 `228`, `184`, `156`，但是由于 `string` 中存储的是 `char` 类型，所以实际中存储的是 `-28`, `-72`, `-100`。这也是为什么之前需要把 `char` 类型转换为 `unsigned char` 类型。并且两位十六进制（一字节）表示范围为 `0 ~ 255`，恰好是 `unsigned char` 的范围。

可以简单的实验证明：

```cpp
string str;
str.push_back(228);
str.push_back(184);
str.push_back(156);
cout << str << endl; // 东

for (char c : str) {
    // -28 -72 -100
    cout << (int)c << " ";
}
cout << endl;

for (unsigned char c : str) {
    // 228 184 156
    cout << (int)c << " ";
}
cout << endl;
```

**`parseString` 是对 `parseRawString` 的封装**

```cpp
return Json(parseRawString());
```

`parseRawString` 返回字符串，封装为 `Json` 类型返回。

**`parseArray` 解析数组**

```jon
array = %x5B ws [ value *( ws %x2C ws value ) ] ws %x5D
```

当中，`%x5B` 是左中括号 `[`，`%x2C` 是逗号 `,`，`%x5D` 是右中括号 `]` ，`ws` 是空白字符。一个数组可以包含零至多个值，以逗号分隔，例如 `[]`、`[1,2,true]`、`[[1,2],[3,4],"abc"]` 都是合法的数组。但注意 Json 不接受末端额外的逗号，例如 `[1,2,]` 是不合法的（许多编程语言如 `C/C++`、`Javascript`、`Java`、`C#` 都容许数组初始值包含末端逗号）。

```cpp
Json JsonParser::parseArray() {
    Json::array arr;
    ++_curr;
    parseWhitespace();
    if (*_curr == ']') {
        _start = ++_curr;
        return Json(arr);
    }
    while (true) {
        parseWhitespace();
        arr.push_back(parseValue());
        parseWhitespace();
        if (*_curr == ',') {
            ++_curr;
        } else if (*_curr == ']') {
            _start = ++_curr;
            return(Json(arr));
        } else {
            throw(JsonException("parse miss comma or square bracket"));
        }
    }
}
```

跳过空白后如果遇到 `]` 表明数组为空，返回空数组即可。否则，循环处理：

跳过空白-->解析并加入数组-->跳过空白-->遇到 `,` 跳过、 遇到 `]` 解析结束，将其以 `Json` 的类型返回，否则，抛出 `parse miss comma or square bracket` 异常。

**`parseObject` 解析对象**

`Json` 对象和 `Json` 数组非常相似，区别包括 `Json` 对象以花括号 `{}` 包裹表示，另外 `Json` 对象由对象成员组成，而 `Json` 数组由 `Json` 值组成。所谓对象成员，就是键值对，键必须为 `Json` 字符串，然后值是任何 `Json` 值，中间以冒号 `:` 分隔。完整语法如下：

```
member = string ws %x3A ws value
object = %x7B ws [ member *( ws %x2C ws member ) ] ws %x7D
```

```cpp
Json JsonParser::parseObject() {
    Json::object obj;
    ++_curr;
    parseWhitespace();
    if (*_curr == '}') {
        _start = ++_curr;
        return Json(obj);
    }
    while (true) {
        parseWhitespace();
        if (*_curr != '\"') {
            throw(JsonException("parse miss key"));
        }
        std::string key = parseRawString();
        parseWhitespace();
        if (*_curr++ != ':') {
            throw(JsonException("parse miss colon"));
        }
        parseWhitespace();
        Json val = parseValue();
        obj.insert({key, val});
        parseWhitespace();
        if (*_curr == ',') {
            ++_curr;
        } else if (*_curr == '}') {
            _start = ++_curr;
            return Json(obj);
        } else {
            throw JsonException("parse miss comma or curly bracket");
        }
    }
}
```

## `JsonException` 

```cpp
class JsonException final : public std::logic_error {
public:
    explicit JsonException(const std::string& errMsg) : logic_error(errMsg) { }
};
```

我们通常定义自己的异常类来表示特定于应用程序的问题，`JsonException` 类表示可能在逻辑上出现问题，比如对非数组（或对象）的 `Json` 取下标（或关键字）。或解析过程中发现不符合 `Json` 的结构。

如果出现以上错误，抛出 `JsonException` 异常。

struct X{};


X f()
X *f()
const X &f()
shared_ptr<X> f()
unique_ptr<X> f()
optional<X> f()

f(X)
f(X&)
f(X*)
f(const X&)
f(X&&)
f(shared_ptr<X>)
f(unique_ptr<X>)
f(optional<X>)

make_lower(string &x)

process(const string &request)

process(string &&request)

write_log(string &&msg)

resize(size_t new_size)

size_t size()


// Bad
get_endpoint(string &host, int &port);

// Better
tuple<string, int> get_endpoint();

// Good
struct endpoint{...};
endpoint get_endpoint();

// Not movable?
unique_ptr<endpoint> get_endpoint();

// Shared ownership?
shared_ptr<endpoint> get_endpoint();

class UserGroup{
    vector<User> members_;
    public:
    // Never!
    vector<User> &get_members(){
        return members_;
    }
    // Ok!
    const vector<User> &get_members() const {
        return members;
    }
    void add_member(const User &user);
    void remove_member(const User &user);
};

class User{
    string name_;
    public:
    // A. Default
    void set_name(const string &name){ name_ = name; }

    // B. Support rvalues (optimization)
    void set_name1(string &&name){ name_ = name; }

    // C. Less code, extra destructor
    void set_name2(string name){ name_ = std::move(name); }
};

auto u = User{};
auto name = string{"Georgy"};
u.set_name(name);      // A. The value is copied
u.set_name("Georgy");  // B. The value is moved

u.set_name2("Georgy"); // C. Same as B
u.set_name2(name);     // C. Same as A, extra destructor call


constexpr int fac(int n)
{
    int x = 1;
    for (int i = 2; i <= n; ++i) x *= i;
    return x;
}

// Evaluated at compile time
constexpr auto f5 = fac(5);


class X {
    void change_things(){
        if (precondition())
            throw exception("0"); // Ok, state unchanged

        change_first_thing(); // Throws? Ok

        change_second_thing(); // Throws? GOTCHA!

        // Do we need to wrap change_second_thing in try-catch
        // to revert changes from change_first_thing?
    }
};

// x must be nonnegative 
double sqrt(double x){
    assert(x >= 0);
    ...
    constexpr double ypsilon = 0.000001d;
    assert(abs(result*result - x) < ypsilon);
    return result;
}

auto value = 42;
auto result = sqrt(value);
assert (abs(result*result) - value < ypsilon);

void f()
{
    char buffer[MAX];
    // ...
    memset(buffer, 0, MAX);
    assert(buffer[0] == 0);
}


struct point {
    int x;
    int y;
};

class date {
    private:
    int month;
    int day;
    int year;
};




    try
    {
        // Initialize boost.asio
        boost::asio::io_context io_context;
        auto work = boost::asio::make_work_guard(io_context);
        std::thread thread{[&io_context](){ io_context.run(); }};

        if (!run_server)
            throw std::runtime_error("Not running in server mode");

        // Make sure that the thread is joined, even in case of exception
        helpers::scope_guard guard{ [&thread, &io_context](){
            if (thread.joinable())
                thread.join();
        }};

        // Run the server
        daytime::server server {io_context, port};
        thread.join();
    }
    catch (std::runtime_error &err)
    {
        // Run in client mode...
    }



enum class error_code : int {
    success = 0,
    unknown_error,
    file_not_found,
    access_denied,
    ...
};

// A common use
error_code open_file(string path, file &result);

// A somewhat better implementation
std::pair<error_code, std::optional<file>> open_file(string path);

// Usage:
auto result = open_file("file.txt");
if (error_code::success == result.first){
    auto &file = *result.second;
    //...
}
else {
    // Handle the error (optional is empty!)
}

class date {
public:
    // All special functions are generated
private:
    int year_ = 1970;
    int month_ = 1;
    int day_ = 1;
};


class file {
public:
    bool is_valid() const {
        return handle_ && handle_ != INVALID_HANDLE_VALUE;
    }
};

class file {
public:
    explicit file(const wstring &name);
};

bool has_unsaved_changes(const file &f);

auto file_name = L"example.txt"s;
file f {file_name}; // Can't use assignment
//...
// Compiler error: can't convert from wstring to file
if (has_unsaved_changes(file_name)){
    // ...
}

file f1 = L"example.txt"s;
file f2 = { L"example.txt"s, L"alias"s };


class file {
public:
    explicit file(const wstring &name)
        : handle_{ ::CreateFileW(...) } 
        { };

    ~file() noexcept { 
        if (handle_ != INVALID_HANDLE_VALUE)
            if (!::CloseHandle(handle_))
                std::terminate();
    }
private:
    HANDLE handle_;
};


file a{L"example.txt"};
file b = a; // CloseHandle is called twice

class file {
public:
    explicit file(const wstring &name)
        : handle_{ ::CreateFileW(...) } { };

    // Copy constructor
    file(const file &other) { *this = other; }

    // Copy assignment
    file &operator=(const file &other){
        // Handle self-assignment
        if (other.handle_ == handle_)
            return *this;
        // Clean-up own resources
        if (handle_ != INVALID_HANDLE_VALUE)
            if (!::CloseHandle(handle_))
                throw std::exception("Could not close the handle during copy");
        // Actual copy
        if (other.handle_ != INVALID_HANDLE_VALUE)
            handle_ = ::DuplicateHandle(other.handle_);
        return *this;
    }

    ~file() noexcept { 
        if (handle_ != INVALID_HANDLE_VALUE)
            if (!::CloseHandle(handle_))
                std::terminate();
    }
private:
    HANDLE handle_;
};

class file {
public:
    file() = default;
    explicit file(const wstring &name)
        : handle_{ ::CreateFileW(...) } 
        { }

    file(file &&other) { *this = std::move(other); }
    file &operator=(file &&other){
        // Handle self-assignment
        if (other.handle_ == handle_)
            return *this;

        // Clean-up own resources
        if (handle_ != INVALID_HANDLE_VALUE)
            if (!::CloseHandle(handle_))
                throw std::exception("Could not close the handle during copy");

        // No deep copy: this now manages the resource
        handle_ = other.handle_;
        // Original no longer manages the resource,
        // but still can be used
        other.handle_ = INVALID_HANDLE_VALUE;
    }

    ~file() noexcept{ /*... (same as before) */}
private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
};

// Possible thanks to move constructor
vector<file> files;
files.push_back(file{"hello.txt"});

class file {
public:
    explicit file(const wstring &name)
        : handle_{ ::CreateFileW(...) } 
        { };

    file(file &&other) = delete;
    file(const file &other) = delete;
    file &operator=(const file &other) = delete;
    file &operator=(file &&other) = delete;

    ~file() noexcept { 
        if (handle_ != INVALID_HANDLE_VALUE)
            ::CloseHandle(handle_);
    }
private:
    HANDLE handle_;
};


object a = new object{};
object b = *a;

// Copy should copy!
assert (a == b);

// A copy is independent!
delete a;
assert (b.is_valid());


object a = new object{};
//...modify a
object b = std::move(*a);

// Move leaves object in a valid state
assert (b.is_valid());
assert (a->is_valid());
assert (a != b);
// The new object is independent
delete a;
assert (b.is_valid());



class file {
public:
    static pair<shared_ptr<file>, int> create_file(const wstring &name){
        auto handle = ::CreateFileW(...);
        if (handle == INVALID_HANDLE_VALUE){
            return make_pair(shared_ptr<file>(nullptr), ::GetLastError());
        }
        return make_pair(make_shared<file>(handle), 0);
    }

private:
    file(HANDLE h)
        : handle_{h} 
        { };
    HANDLE handle_;
};


class text_file: public file {
    public:
    //...
};

void do_something(file &f){
    // Slicing: duplicate is of type file
    auto duplicate = f; 
}

text_file f("file.txt");
do_someting(f);


class file{
    public:
    virtual file *clone() const {
        return new file(*this);
    }

    protected:
    file(const file &other) {
        // ... same as before
    }
};

class text_file: public file {
    public:
    file *clone() const override {
        return new text_file(*this);
    }

    protected:
    text_file(const text_file &other)
    : file(other)
    {
        // Do text_file specific copy
    }
};

do_something(file &f){
    auto duplicate = unique_ptr<file>(f.clone());
    // If f was text_file, then duplicate is text_file!
}


std::unique_ptr<server> create_server(){
    return make_unique<daytime_server>();
}

std::unique_ptr<network::server> create_server(io_context &io_context, short port){

    // Recommended: use make_unique instead of new!
    return std::make_unique<daytime::daytime_server> (io_context, port);

    // This is equivalent to:
    return std::unique_ptr<daytime::daytime_server>(new daytime_server(io_context, port));

    // You can wrap an existing pointer in unique_ptr too:
    auto server = new daytime_server(io_context, port);
    return std::unique_ptr<daytime::daytime_server>(server);
}

// Function assumes ownership of server
void run_server(std::unique_ptr<network::server> server){
    // Server will be deleted when it goes out of scope
}

int main(int argc, char *argv[]){

    auto server = make_server(argc, argv, io_context);
    server->start();
    run_server(std::move(server))
    assert(server.get() == nullptr);
    
    return 0;
}

class scene{
public:
    class rectangle{};
    rectangle get_rectangle() const;
};

scene::rectangle scene::get_rectangle(){

}

class scene{
public:
    class rectangle{};
    auto get_rectangle() -> rectangle const;
};

auto scene::get_rectangle() -> rectangle {

}


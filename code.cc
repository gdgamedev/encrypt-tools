#include <napi.h>
#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <sstream>

using namespace Napi;
using namespace std;

const int ERR_FNE = 1;
const int ERR_WKEY = 2;

void emptyCallback(const CallbackInfo& info) {}

wstring string2wstring(string input) {
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(input);
}

string wstring2string(wstring input) {
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(input);
}

wstring encrypt(wstring input, wstring key) {
    wstring result;
    int text_size = input.size();
    int key_size = key.size();
    for (int text_i = 0; text_i < text_size; text_i++) {
        int result_char = input[text_i];
        for (int key_i = 0; key_i < key_size; key_i++) {
            result_char += key[key_i] + key_i + text_i;
        }
        result += (unsigned char)result_char;
    }
    return result;
}

string encrypt(string input, string key) {
    string result;
    int text_size = input.size();
    int key_size = key.size();
    for (int text_i = 0; text_i < text_size; text_i++) {
        int result_char = (wchar_t)input[text_i];
        for (int key_i = 0; key_i < key_size; key_i++) {
            result_char += (wchar_t)key[key_i] + key_i + text_i;
        }
        result += (unsigned char)result_char;
    }
    return result;
}

wstring decrypt(wstring input, wstring key) {
    wstring result;
    int text_size = input.size();
    int key_size = key.size();
    for (int text_i = 0; text_i < text_size; text_i++) {
        int result_char = input[text_i];
        for (int key_i = 0; key_i < key_size; key_i++) {
            result_char -= key[key_i] + key_i + text_i;
        }
        result += (unsigned char)result_char;
    }
    return result;
}

string decrypt(string input, string key) {
    string result;
    int text_size = input.size();
    int key_size = key.size();
    for (int text_i = 0; text_i < text_size; text_i++) {
        int result_char = (wchar_t)input[text_i];
        for (int key_i = 0; key_i < key_size; key_i++) {
            result_char -= (wchar_t)key[key_i] + key_i + text_i;
        }
        result += (unsigned char)result_char;
    }
    return result;
}

void clear_file(string path) {
    ofstream file(path.c_str(), ofstream::out | ofstream::trunc);
    file.close();
}

bool test_key(string path, wstring key) {
    wifstream file(path.c_str(), wifstream::binary);
    wstring f_key;
    f_key.resize(key.size(), ' ');
    wchar_t* k_begin = &*f_key.begin();
    file.read(k_begin, key.size());
    f_key = decrypt(f_key, key);
    if (f_key.compare(key) == 0) return true;
    return false;
}

class encryptFile : public AsyncProgressWorker<int> {
    public:
        encryptFile(
            Function& callback,
            string& inputPath,
            wstring& key,
            string& outputPath,
            Promise::Deferred& deferred
        ) : AsyncProgressWorker(callback),
            inputPath(inputPath),
            key(key),
            outputPath(outputPath),
            deferred(deferred) {}
        
        ~encryptFile() {}

        void Execute(const ExecutionProgress& progress) {
            ifstream file_in(inputPath.c_str());

            if (!file_in.good()) {
                errStatus = ERR_FNE;
                SetError("a");
                return;
            }

            string data(
                (istreambuf_iterator<char>(file_in)),
                 istreambuf_iterator<char>()
            );

            file_in.close();

            ofstream file_out(outputPath.c_str());

            f_chars = data.length();
            p_chars = 0;

            file_out << wstring2string(encrypt(key, key));

            for (size_t ch = 0; ch < f_chars; ch ++) {
                string en_char = encrypt(string(1, data[ch]), wstring2string(key));
                file_out << en_char;
                ++p_chars;
                progress.Send(&p_chars, 1);
            }
        }

        void OnOK() {
            HandleScope scope(Env());
            deferred.Resolve({Env().Undefined()});
        }

        void OnProgress(const int* d, size_t s) {
            HandleScope scope(Env());
            Callback().Call({
                Number::New(Env(), p_chars),
                Number::New(Env(), f_chars)
            });
        }

        void OnError(const Error&) {
            HandleScope scope(Env());
            deferred.Reject({
                Number::New(Env(), errStatus)
            });
        }

    private:
        int errStatus = 0;
        Promise::Deferred deferred;
        int p_chars, f_chars;
        string inputPath, outputPath;
        wstring key;
};

class decryptFile : public AsyncProgressWorker<int> {
    public:
        decryptFile(
            Function& callback,
            string& inputPath,
            wstring& key,
            string& outputPath,
            Promise::Deferred& deferred
        ) : AsyncProgressWorker(callback),
            inputPath(inputPath),
            key(key),
            outputPath(outputPath),
            deferred(deferred) {}
        
        ~decryptFile() {}

        void Execute(const ExecutionProgress& progress) {
            ifstream file_in(inputPath.c_str());

            if (!file_in.good()) {
                errStatus = ERR_FNE;
                SetError("a");
                return;
            }

            if (!test_key(inputPath, key)) {
                errStatus = ERR_WKEY;
                SetError("a");
                return;
            }

            file_in.seekg(key.size());

            string data(
                (istreambuf_iterator<char>(file_in)),
                 istreambuf_iterator<char>()
            );

            file_in.close();

            ofstream file_out(outputPath.c_str());

            f_chars = data.length();
            p_chars = 0;

            for (size_t ch = 0; ch < f_chars; ch ++) {
                string de_char = decrypt(string(1, data[ch]), wstring2string(key));
                file_out << de_char;
                ++p_chars;
                progress.Send(&p_chars, 1);
            }
        }

        void OnOK() {
            HandleScope scope(Env());
            deferred.Resolve({Env().Undefined()});
        }

        void OnProgress(const int*, size_t) {
            HandleScope scope(Env());
            Callback().Call({
                Number::New(Env(), p_chars),
                Number::New(Env(), f_chars)
            });
        }

        void OnError(const Error&) {
            HandleScope scope(Env());
            deferred.Reject({
                Number::New(Env(), errStatus)
            });
        }

    private:
        int errStatus = 0;
        Promise::Deferred deferred;
        int p_chars, f_chars;
        string inputPath, outputPath;
        wstring key;
};

class writeFile : public AsyncProgressWorker<int> {
    public:
        writeFile(
            Function& callback,
            string& outputPath,
            string& data,
            wstring& key,
            Promise::Deferred& deferred
        ) : AsyncProgressWorker(callback),
            data(data),
            key(key),
            outputPath(outputPath),
            deferred(deferred) {}
        
        ~writeFile() {}

        void Execute(const ExecutionProgress& progress) {
            ofstream file_out(outputPath.c_str());

            f_chars = data.length();
            p_chars = 0;

            file_out << wstring2string(encrypt(key, key));

            for (size_t ch = 0; ch < f_chars; ch ++) {
                string en_char = encrypt(string(1, data[ch]), wstring2string(key));
                file_out << en_char;
                ++p_chars;
                progress.Send(&p_chars, 1);
            }
        }

        void OnOK() {
            HandleScope scope(Env());
            deferred.Resolve({Env().Undefined()});
        }

        void OnProgress(const int* d, size_t s) {
            HandleScope scope(Env());
            Callback().Call({
                Number::New(Env(), p_chars),
                Number::New(Env(), f_chars)
            });
        }

        void OnError(const Error&) {
            HandleScope scope(Env());
            deferred.Reject({
                Number::New(Env(), errStatus)
            });
        }

    private:
        int errStatus = 0;
        Promise::Deferred deferred;
        int p_chars, f_chars;
        string outputPath, data;
        wstring key;
};

class readFile : public AsyncProgressWorker<int> {
    public:
        readFile(
            Function& callback,
            string& inputPath,
            wstring& key,
            Promise::Deferred& deferred
        ) : AsyncProgressWorker(callback),
            key(key),
            inputPath(inputPath),
            deferred(deferred) {}
        
        ~readFile() {}

        void Execute(const ExecutionProgress& progress) {
            ifstream file_in(inputPath.c_str());

            if (!file_in.good()) {
                errStatus = ERR_FNE;
                SetError("a");
                return;
            }

            if (!test_key(inputPath, key)) {
                errStatus = ERR_WKEY;
                SetError("a");
                return;
            }

            file_in.seekg(key.size());

            string data(
                (istreambuf_iterator<char>(file_in)),
                 istreambuf_iterator<char>()
            );

            file_in.close();

            f_chars = data.length();
            p_chars = 0;

            for (size_t ch = 0; ch < f_chars; ch ++) {
                string de_char = decrypt(string(1, data[ch]), wstring2string(key));
                result += de_char;
                ++p_chars;
                progress.Send(&p_chars, 1);
            }
        }

        void OnOK() {
            HandleScope scope(Env());
            deferred.Resolve({
                String::New(Env(), result)
            });
        }

        void OnProgress(const int* d, size_t s) {
            HandleScope scope(Env());
            Callback().Call({
                Number::New(Env(), p_chars),
                Number::New(Env(), f_chars)
            });
        }

        void OnError(const Error&) {
            HandleScope scope(Env());
            deferred.Reject({
                Number::New(Env(), errStatus)
            });
        }
        
    private:
        int errStatus = 0;
        Promise::Deferred deferred;
        int p_chars, f_chars;
        string result, inputPath;
        wstring key;
};

String encryptWrapper(const CallbackInfo& info) {
    Env env = info.Env();
    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
        TypeError::New(env, "A input (String) and a key (String) is required!").ThrowAsJavaScriptException();
    }

    wstring input = string2wstring(info[0].As<String>().Utf8Value());
    wstring key = string2wstring(info[1].As<String>().Utf8Value());

    string enc = wstring2string(encrypt(input, key));

    return String::New(env, enc);
}

String decryptWrapper(const CallbackInfo& info) {
    Env env = info.Env();
    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
        TypeError::New(env, "A input (String) and a key (String) is required!").ThrowAsJavaScriptException();
    }

    wstring input = string2wstring(info[0].As<String>().Utf8Value());
    wstring key = string2wstring(info[1].As<String>().Utf8Value());

    string enc = wstring2string(decrypt(input, key));

    return String::New(env, enc);
}

Promise encryptFileWrapper(const CallbackInfo& info) {
    Env env = info.Env();
    if (info.Length() < 3 || !info[0].IsString() || !info[1].IsString() || !info[2].IsString()) {
        TypeError::New(env, "A input path (String), a key (String) and a output path (String) is required!").ThrowAsJavaScriptException();
    }

    string inputPath = info[0].As<String>().Utf8Value();
    wstring key = string2wstring(info[1].As<String>().Utf8Value());
    string outputPath = info[2].As<String>().Utf8Value();
    Function cb;
    if (info.Length() < 4) {
        cb = Function::New(env, emptyCallback);
    } else {
        cb = info[3].As<Function>();
    }
    Promise::Deferred def = Promise::Deferred::New(env);

    encryptFile* wk = new encryptFile(cb, inputPath, key, outputPath, def);
    wk->Queue();

    return def.Promise();
}

Promise decryptFileWrapper(const CallbackInfo& info) {
    Env env = info.Env();
    if (info.Length() < 3 || !info[0].IsString() || !info[1].IsString() || !info[2].IsString()) {
        TypeError::New(env, "A input path (String), a key (String) and a output path (String) is required!").ThrowAsJavaScriptException();
    }

    string inputPath = info[0].As<String>().Utf8Value();
    wstring key = string2wstring(info[1].As<String>().Utf8Value());
    string outputPath = info[2].As<String>().Utf8Value();
    Function cb;
    if (info.Length() < 4) {
        cb = Function::New(env, emptyCallback);
    } else {
        cb = info[3].As<Function>();
    }
    Promise::Deferred def = Promise::Deferred::New(env);

    decryptFile* wk = new decryptFile(cb, inputPath, key, outputPath, def);
    wk->Queue();

    return def.Promise();
}

Promise writeFileWrapper(const CallbackInfo& info) {
    Env env = info.Env();
    if (info.Length() < 3 || !info[0].IsString() || !info[1].IsString() || !info[2].IsString()) {
        TypeError::New(env, "A output path (String), a data (string) and a key (String) is required!").ThrowAsJavaScriptException();
    }

    string outputPath = info[0].As<String>().Utf8Value();
    string data = info[1].As<String>().Utf8Value();
    wstring key = string2wstring(info[2].As<String>().Utf8Value());
    Function cb;
    if (info.Length() < 4) {
        cb = Function::New(env, emptyCallback);
    } else {
        cb = info[3].As<Function>();
    }
    Promise::Deferred def = Promise::Deferred::New(env);

    writeFile* wk = new writeFile(cb, outputPath, data, key, def);
    wk->Queue();

    return def.Promise();
}

Promise readFileWrapper(const CallbackInfo& info) {
    Env env = info.Env();
    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
        TypeError::New(env, "A input path (String) and a key (string) is required!").ThrowAsJavaScriptException();
    }

    string inputPath = info[0].As<String>().Utf8Value();
    wstring key = string2wstring(info[1].As<String>().Utf8Value());
    Function cb;

    if (info.Length() < 3) {
        cb = Function::New(env, emptyCallback);
    } else {
        cb = info[2].As<Function>();
    }

    Promise::Deferred def = Promise::Deferred::New(env);

    readFile* wk = new readFile(cb, inputPath, key, def);
    wk->Queue();

    return def.Promise();
}

Object Init(Env env, Object exports) {
    exports.Set(
        "encrypt",
        Function::New(env, encryptWrapper)
    );
    exports.Set(
        "decrypt",
        Function::New(env, decryptWrapper)
    );
    exports.Set(
        "encryptFile",
        Function::New(env, encryptFileWrapper)
    );
    exports.Set(
        "decryptFile",
        Function::New(env, decryptFileWrapper)
    );
    exports.Set(
        "writeFile",
        Function::New(env, writeFileWrapper)
    );
    exports.Set(
        "readFile",
        Function::New(env, readFileWrapper)
    );
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
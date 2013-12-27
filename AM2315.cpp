#include <node.h>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <bcm2835.h>

using namespace v8;

// Forward declaration. Usually, you do this in a header file.
Handle<Value> AM2315(const Arguments& args);
void AM2315Work(uv_work_t* req);
void AM2315After(uv_work_t* req);

int address;
int func;
int start;
int num;
int i, j;
double rhtfactor;
double t, rh;


// We use a struct to store information about the asynchronous "work request".
struct Baton {
    // This handle holds the callback function we'll call after the work request
    // has been completed in a threadpool thread. It's persistent so that V8
    // doesn't garbage collect it away while our request waits to be processed.
    // This means that we'll have to dispose of it later ourselves.
    Persistent<Function> callback;

    // Tracking errors that happened in the worker function. You can use any
    // variables you want. E.g. in some cases, it might be useful to report
    // an error number.
    bool error;
    std::string error_message;

    // Custom data you can pass through.
    double temp;
    double humid;
};

// This is the function called directly from JavaScript land. It creates a
// work request object and schedules it for execution.
Handle<Value> AM2315(const Arguments& args) {
    HandleScope scope;

    if (!args[0]->IsFunction()) {
        return ThrowException(Exception::TypeError(
            String::New("First argument must be a callback function")));
    }
    // There's no ToFunction(), use a Cast instead.
    Local<Function> callback = Local<Function>::Cast(args[0]);

    // The baton holds our custom status information for this asynchronous call,
    // like the callback function we want to call when returning to the main
    // thread and the status information.
    Baton* baton = new Baton();
    baton->error = false;
    baton->callback = Persistent<Function>::New(callback);

    // This creates the work request struct.
    uv_work_t *req = new uv_work_t();
    req->data = baton;



    // Initialize I2C Comunication
    address = 0x5c;//92

    bcm2835_init();
    bcm2835_i2c_set_baudrate(100000);
    bcm2835_i2c_begin();
    bcm2835_i2c_setSlaveAddress(address);

    // Schedule our work request with libuv. Here you can specify the functions
    // that should be executed in the threadpool and back in the main thread
    // after the threadpool function completed.
    int status = uv_queue_work(uv_default_loop(), req, AM2315Work,
                               (uv_after_work_cb)AM2315After);
    assert(status == 0);

    return Undefined();
}

// This function is executed in another thread at some point after it has been
// scheduled. IT MUST NOT USE ANY V8 FUNCTIONALITY. Otherwise your extension
// will crash randomly and you'll have a lot of fun debugging.
// If you want to use parameters passed into the original call, you have to
// convert them to PODs or some other fancy method.
void AM2315Work(uv_work_t* req) {
    Baton* baton = static_cast<Baton*>(req->data);
    func=3;
    start=0;
    num=4;
    rhtfactor = 0.1;

    //Function start
    char writebuf[3] = {func, start, num};
    char readbuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned short c, crc;
    int delay = 158000;

    for(i=0;i<2;i++)
    {
        bcm2835_i2c_write(&writebuf[0], 3);
    };

    usleep(delay);
    bcm2835_i2c_read(&readbuf[0],8);

    crc = (readbuf[7]<<8)+readbuf[6];

    if(readbuf[1]!=writebuf[2] || readbuf[0]!=writebuf[0])
    {
        baton->error_message = "Failed AM2315 read";
        baton->error = true;
    }
    else
    {
        rh = rhtfactor * (double)(readbuf[3]+(readbuf[2]<<8));
        t = rhtfactor * (double)(readbuf[5]+(readbuf[4]<<8));
        t = (t*9/5)+32;
        c = 0xFFFF;
        for(int l=0;l<6;l++)
        {
            c^=readbuf[l];
            for(int i=0;i<8;i++)
            {
                if(c & 0x01)
                {
                    c>>=1;
                    c^=0xA001;
                }
                else
                {
                    c>>=1;
                };
            };
        };

        if(crc!=c)
        {
            baton->error_message = "CRC check failed";
            baton->error = true;
        };

        baton->temp = t;
        baton->humid = rh;

    };

    // If the work we do fails, set baton->error_message to the error string
    // and baton->error to true.
}

// This function is executed in the main V8/JavaScript thread. That means it's
// safe to use V8 functions again. Don't forget the HandleScope!
void AM2315After(uv_work_t* req) {
    HandleScope scope;
    Baton* baton = static_cast<Baton*>(req->data);

    if (baton->error) {
        Local<Value> err = Exception::Error(String::New(baton->error_message.c_str()));

        // Prepare the parameters for the callback function.
        const unsigned argc = 1;
        Local<Value> argv[argc] = { err };

        // Wrap the callback function call in a TryCatch so that we can call
        // node's FatalException afterwards. This makes it possible to catch
        // the exception from JavaScript land using the
        // process.on('uncaughtException') event.
        TryCatch try_catch;
        baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);
        if (try_catch.HasCaught()) {
            node::FatalException(try_catch);
        }
    } else {
        // In case the operation succeeded, convention is to pass null as the
        // first argument before the result arguments.
        // In case you produced more complex data, this is the place to convert
        // your plain C++ data structures into JavaScript/V8 data structures.
        const unsigned argc = 2;

        v8::Local<v8::Array> dataArray = v8::Array::New(2);

        dataArray->Set(0, Number::New(baton->temp));
        dataArray->Set(1, Number::New(baton->humid));

        Local<Value> argv[argc] = {
            Local<Value>::New(Null()),
            dataArray
        };

        // Wrap the callback function call in a TryCatch so that we can call
        // node's FatalException afterwards. This makes it possible to catch
        // the exception from JavaScript land using the
        // process.on('uncaughtException') event.
        TryCatch try_catch;
        baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);
        if (try_catch.HasCaught()) {
            node::FatalException(try_catch);
        }
    }

    // The callback is a permanent handle, so we have to dispose of it manually.
    baton->callback.Dispose();

    // We also created the baton and the work_req struct with new, so we have to
    // manually delete both.
    delete baton;
    delete req;
}

void Init(Handle<Object> target) {
    target->Set(String::NewSymbol("read"),
        FunctionTemplate::New(AM2315)->GetFunction());
}

NODE_MODULE(AM2315, Init);

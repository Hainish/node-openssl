#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <v8.h>
#include <node.h>
#include <openssl/aes.h>
#include <node_buffer.h>
#include <map>
#include <utility>

using namespace v8;
using namespace node;
using namespace std;

class OpenSSL : ObjectWrap {
	public:
		static void Initialize(Handle<Object> target);
		static Persistent<Function> js_conditioner;
		static void SetJSConditioner(Persistent<Function> constructor);

	protected:
		static Persistent<FunctionTemplate> constructor_template;

		OpenSSL();
		~OpenSSL();

		static Handle<Value> New(const Arguments& args);
		static Handle<Value> OpenSSLTest(const Arguments& args);
};

Persistent<FunctionTemplate> OpenSSL::constructor_template;

Persistent<Function> OpenSSL::js_conditioner;

void OpenSSL::SetJSConditioner(Persistent<Function> constructor) {
	js_conditioner = constructor;
}

void OpenSSL::Initialize(v8::Handle<v8::Object> target) {
	HandleScope scope;
	
	Local<FunctionTemplate> t = FunctionTemplate::New(New);
	constructor_template = Persistent<FunctionTemplate>::New(t);

	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("OpenSSL"));

        NODE_SET_PROTOTYPE_METHOD(constructor_template, "openssltest", OpenSSLTest);

	target->Set(String::NewSymbol("OpenSSL"), constructor_template->GetFunction());
}

OpenSSL::OpenSSL () : ObjectWrap ()
{
}

OpenSSL::~OpenSSL ()
{
}

Handle<Value>
OpenSSL::New(const Arguments& args)
{
	if(!args.IsConstructCall()) {
		int len = args.Length();
		Handle<Value>* newArgs = new Handle<Value>[len];
		for(int i = 0; i < len; i++) {
			newArgs[i] = args[i];
		}
		Handle<Value> newInst = constructor_template->GetFunction()->NewInstance(len, newArgs);
		delete[] newArgs;
		return newInst;
	}
	HandleScope scope;
	return scope.Close(args.This());
}


struct ctr_state {
    unsigned char ivec[16];  /* ivec[0..7] is the IV, ivec[8..15] is the big-endian counter */
    unsigned int num;
    unsigned char ecount[16];
};

int init_ctr(struct ctr_state *state)
{
    /* aes_ctr128_encrypt requires 'num' and 'ecount' set to zero on the
     * first call. */
    state->num = 0;
    memset(state->ecount, 0, 16);

    /* Initialise counter in 'ivec' to 0 */
    memset(state->ivec, 0, 16);
}

Handle<Value>
OpenSSL::OpenSSLTest(const Arguments& args)
{
  HandleScope scope;
  
  // Get passed in values into openssl unsigned char format
  Local<Object> key_buff = args[0]->ToObject();
  // Debug if keylen != 128
  char* key_buff_data = Buffer::Data(key_buff);
  unsigned char ckey[16];
  memcpy(ckey, key_buff_data, 16);

  Local<Object> input_buff = args[1]->ToObject();
  char* input_buff_data = Buffer::Data(input_buff);
  int input_length = Buffer::Length(input_buff);
  unsigned char input[input_length];
  memcpy(input, input_buff_data, input_length);

  // Initialize the counter and output
  struct ctr_state state;
  unsigned char output[input_length];
  AES_KEY key;

  init_ctr(&state);

  // Set encryption key, and encrypt
  AES_set_encrypt_key(ckey, 128, &key);
  AES_ctr128_encrypt(input, output, input_length, &key, state.ivec, state.ecount, &state.num);

  // Copy the ciphertext into a buffer object
  Buffer *outbuff = Buffer::New(input_length);
  memcpy(Buffer::Data(outbuff), output, input_length);

  // Get the buffer constructor from the global context
  Local<Object> globalObj = Context::GetCurrent()->Global();
  Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
  Handle<Value> constructorArgs[3] = { outbuff->handle_, Integer::New(input_length), Integer::New(0) };

  // Build the js buffer
  Local<Object> output_buffer = bufferConstructor->NewInstance(3, constructorArgs);
  
  return scope.Close(output_buffer);
}

static Handle<Value>
SetJSConditioner(const Arguments& args)
{
	HandleScope scope;

	OpenSSL::SetJSConditioner(Persistent<Function>::New(Local<Function>::Cast(args[0])));

	return Undefined();
}

extern "C" void
init (Handle<Object> target)
{
	HandleScope scope;

	OpenSSL::Initialize(target);
	NODE_SET_METHOD(target, "setJSConditioner", SetJSConditioner);
}

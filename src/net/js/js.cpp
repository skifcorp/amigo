

#include "tag.h"
#include "amigofs.h"
#include "modiface.h"

#include <v8/v8.h>

using namespace v8;



#include <iostream>
#include <string>
#include <fstream>
#include <cassert>

using std::cout;
using std::string;
using std::fstream;



#include "monotone_timer.hpp"
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
//#include <boost/assert.hpp>

using boost::asio::io_service;
using boost::asio::monotone_timer;
using boost::posix_time::milliseconds;
using boost::bind;




string convertString(Local<Value> v)
{
  String::Utf8Value utf8_value(v);
  return std::move(*utf8_value);
}
/*
void stop(io_service & ios, monotone_timer & t)
{
  cout << "stoppping js...";
  t.cancel();
  ios.stop();
  cout << " stopped\n";
}*/

template <class T>
T & unwrapObject(Handle<Object> obj)
{
  Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
  void * ptr = field->Value();
  return *static_cast<T*>(ptr);
}

Handle<Value> tagValue( const Arguments& args )
{
  HandleScope handle_scope;
    
  if ( args.Length() < 1 ) {
    cerr <<__func__ << " args count\n"; return Handle<Value>();
  }
    
  if (!args[0]->IsString()) {
    cerr << __func__<< " bad type 0\n"; return Handle<Value>();
  }
  string tag_name(convertString(args[0]));
//  cout << "tag_name: "<<tag_name<<"\n";
  
  tags_t & tags = unwrapObject<tags_t>(args.This());

  value_t val = 0.0f;
  Tag * t =  tags[tag_name];
  if (!t) {
    cerr << __func__<<": cant find tag with name: "<<tag_name; return Handle<Value>();
  }
  
  if ( t->lookup("value", val) ) {
    return Number::New( static_cast<float>(val) );
  }

  cerr << __func__<<": " << "property value NOT FOUND tag_name "<< tag_name<<" id: "<<t->id()<<  "\n";
   
  return Handle<Value>();
}

Handle<Value> setTagValue( const Arguments& args )
{
  HandleScope handle_scope;
  
  if ( args.Length() < 3 ) {
    cout<< __func__<<": " << "setTagCount(): args count\n"; return Handle<Value>();
  }
    
  if (!args[0]->IsString()) {
    cout<< __func__<<": " << "bad type 0\n"; return Handle<Value>();
  }

  if (!args[1]->IsNumber()) {
    cout<< __func__<<": " << "bad type 1\n"; return Handle<Value>();
  }

  if (!args[2]->IsNumber()) {
    cout<< __func__<<": " << "bad type 2\n"; return Handle<Value>();
  }

  string tag_name(convertString(args[0]));
  float val = static_cast<float>(args[1]->NumberValue());
  EventSubscriber ev_type = static_cast<EventSubscriber>(args[2]->NumberValue());
  
  tags_t & tags = unwrapObject<tags_t>(args.This());
  Tag * t = tags[tag_name];
  if (!t) {
    cout<< __func__<<": " << "cant find tag with name: "<<tag_name; return Handle<Value>();
  }

//cout << "setValue: "<<val<<"\n";

  t->setProp("value", value_t(val), ev_type );

  return Handle<Value>();
}

Handle<Value> containsTag( const Arguments& args )
{
  HandleScope handle_scope;
  
  if ( args.Length() < 1 ) {
    cout<< __func__<<": " << "args count\n"; return Handle<Value>();
  }
    
  if (!args[0]->IsString()) {
    cout<< __func__<<": " << "bad type 0\n"; return Handle<Value>();
  }

  string tag_name(convertString(args[0]));

  tags_t & tags = unwrapObject<tags_t>(args.This());
  Tag * t = tags[tag_name];
  
  return handle_scope.Close(Boolean::New(t != nullptr));
}


Handle<Value> setReliability( const Arguments& args )
{
  HandleScope handle_scope;
  
  if ( args.Length() < 2 ) {
    cout<< __func__<<": " << "args count\n"; return Handle<Value>();
  }
    
  if (!args[0]->IsString()) {
    cout<< __func__<<": " << "bad type 0\n"; return Handle<Value>();
  }

  if (!args[1]->IsInt32()) {
    cout<< __func__<<": " << "bad type 1\n"; return Handle<Value>();
  }

  string tag_name(convertString(args[0]));
  short rel = args[1]->ToInt32()->Value();
  
  tags_t & tags = unwrapObject<tags_t>(args.This());
  Tag * t = tags[tag_name];
  
  if (!t) {
    cout<< __func__<<": " << "cant find tag with name: "<<tag_name<<"\n"; return Handle<Value>();
  }
  
  t->setProp("rel", rel, EventReader);
  
  return Handle<Value>();
}

Handle<FunctionTemplate> wrapMethod(InvocationCallback inv)
{
  HandleScope handle_scope;
  Handle<FunctionTemplate> func = FunctionTemplate::New();
  func->SetCallHandler(inv);
  
  return handle_scope.Close(func);
}

template <class Fun>
Handle<ObjectTemplate> makeTemplate(Handle<ObjectTemplate> obj, const string& nam, Fun fun)
{
  HandleScope handle_scope;
  obj->Set(nam.c_str(), wrapMethod(fun));
  return handle_scope.Close(obj);
}

template <class Fun, class... Args>
Handle<ObjectTemplate> makeTemplate(Handle<ObjectTemplate> obj, const string& nam, Fun fun, Args ... args)
{
  HandleScope handle_scope;
  obj->Set(nam.c_str(), wrapMethod(fun));
  makeTemplate(obj, args...);
  return handle_scope.Close(obj);
}

template <class T, class Fun, class... Args>
Handle<Object> wrapObject(T & o, const string& nam, Fun fun, Args ... args)
{
  HandleScope handle_scope;
  
  Handle<ObjectTemplate> obj = ObjectTemplate::New();
  obj->SetInternalFieldCount(1);
  Handle<ObjectTemplate> raw_template = makeTemplate(obj, nam, fun, args...);
  
//  Handle<ObjectTemplate> raw_template = makeTagsTemplate();
  
  Persistent<ObjectTemplate> tags_template = Persistent<ObjectTemplate>::New(raw_template);
  Handle<ObjectTemplate> templ = tags_template;
  
  Handle<Object> result = templ->NewInstance();
  Handle<External> obj_ptr = External::New(&o);
  result->SetInternalField(0, obj_ptr);
  
  return handle_scope.Close(result);
}


Handle<Value> tagChildrenNames( const Arguments& args )
{
  HandleScope handle_scope;
  
  if ( args.Length() < 1 ) {
    cout<< __func__<<": "  << "args count\n"; return Handle<Value>();
  }
    
  if (!args[0]->IsString()) {
    cout<< __func__<<": "  << "bad type 0\n"; return Handle<Value>();
  }

  tags_t & tags = unwrapObject<tags_t>(args.This());

  string tag_name(convertString(args[0]));
  
  Tag * t = tags[tag_name];
  if (!t) {
    cerr<< __func__<<": " <<"Bad tag name: "<<tag_name<<"\n"; return Handle<Value>();
  }
  
  t = t->firstChild();
  Handle<Array> ret = Array::New();
  int counter = 0;
  while (t) {
    ret->Set(counter++, String::New(t->fullName().c_str()));
    t = t->nextSibling();
  }
  
  return handle_scope.Close(ret); //try to remove handle_scope
}

Handle<Value> incTagLink( const Arguments& args )
{
  HandleScope handle_scope;
  
  if ( args.Length() < 1 ) {
    cout<< __func__<<": "  << "args count\n"; return Handle<Value>();
  }
    
  if (!args[0]->IsString()) {
    cout<< __func__<<": "  << "bad type 0\n"; return Handle<Value>();
  }

  string tag_name(convertString(args[0]));

  tags_t & tags = unwrapObject<tags_t>(args.This());
  Tag * t = tags[tag_name];
  
  if (!t) {
    cout<< __func__<<": "  << "cant find tag with name: "<<tag_name<<"\n"; return Handle<Value>();
  }
  
  t->incLink();

  return Handle<Value>();
}

Handle<Value> decTagLink( const Arguments& args )
{
  HandleScope handle_scope;
  
  if ( args.Length() < 1 ) {
    cout<< __func__<<": "  << "args count\n"; return Handle<Value>();
  }
    
  if (!args[0]->IsString()) {
    cout<< __func__<<": "  << "bad type 0\n"; return Handle<Value>();
  }

  string tag_name(convertString(args[0]));

  tags_t & tags = unwrapObject<tags_t>(args.This());
  Tag * t = tags[tag_name];
  
  if (!t) {
    cout<< __func__<<": "  << "cant find tag with name: "<<tag_name<<"\n"; return Handle<Value>();
  }
  
  t->decLink();

  return Handle<Value>();
}



void installTags(tags_t & tags, Persistent<Context>& context)
{
  HandleScope handle_scope;
  Handle<Object> tags_obj = wrapObject(tags, "tagValue", tagValue, "setTagValue", setTagValue, "tagChildrenNames", tagChildrenNames, 
                                               "contains", containsTag, "setReliability", setReliability, "incLink", incTagLink, "decLink", decTagLink ) ;
  context->Global()->Set(String::New("tags"), tags_obj);
}

Handle<Value> waitTimer( const Arguments& args )
{
  HandleScope handle_scope;
  
  monotone_timer & timer = unwrapObject<monotone_timer>(args.This());

  boost::system::error_code err_code;
  auto wild_func = [&err_code](const boost::system::error_code & err) {
    err_code = err;
  }; 
  timer.async_wait(wild_func);
//  timer.wait();
  auto ret = timer.get_io_service().run_one();
  
  if ( err_code || ret == 0 )  {
    return handle_scope.Close(Boolean::New(false));
  }
  
  return handle_scope.Close(Boolean::New(true));
}

Handle<Value> expiresFromNow( const Arguments& args )
{
  HandleScope handle_scope;
  if (args.Length() < 1) {
    cout << "bad params count!: "<<args.Length()<<"\n"; return Handle<Value>();
  }
  
  if (!args[0]->IsInt32()) {
    cout << "bad params 1 type: \n"; return Handle<Value>();
  }

  monotone_timer & timer = unwrapObject<monotone_timer>(args.This());
  
  timer.expires_from_now(milliseconds(args[0]->ToInt32()->Value()));
  
  return Handle<Value>();
}


void installTimer(monotone_timer& timer, Persistent<Context>& context)
{
  HandleScope handle_scope;
  Handle<Object> obj = wrapObject(timer, "waitTimer", waitTimer, "expiresFromNow", expiresFromNow) ;
  context->Global()->Set(String::New("timer"), obj);
}

Handle<Value> Write(const Arguments& args) {
  for (int i = 0; i < args.Length(); i++) {
    HandleScope handle_scope;
    if (i != 0) {
      printf(" ");
    }
    v8::String::Utf8Value str(args[i]);
    int n = fwrite(*str, sizeof(**str), str.length(), stdout);
    if (n != str.length()) {
      printf("Error in fwrite\n");
      exit(1);
    }
  }
  return Undefined();
}


Handle<Value> Print(const Arguments& args) {
  Handle<Value> val = Write(args);
  printf("\n");
  return val;
}


string readScriptText(const string& proj_path)
{
  fstream stream;
  
  stream.open ( proj_path + a::fs::slash + "script.js", fstream::in );
  string data;
  getline(stream, data, '\0' );

  stream.close();
  
  return std::move(data);
}

const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::TryCatch* try_catch) {
  v8::HandleScope handle_scope;
  v8::String::Utf8Value exception(try_catch->Exception());
  const char* exception_string = ToCString(exception);
  v8::Handle<v8::Message> message = try_catch->Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    printf("%s\n", exception_string);
  } 
  else {
    // Print (filename):(line number): (message).
    v8::String::Utf8Value filename(message->GetScriptResourceName());
    const char* filename_string = ToCString(filename);
    int linenum = message->GetLineNumber();
    printf("%s:%i: %s\n", filename_string, linenum, exception_string);
    // Print line of source code.
    v8::String::Utf8Value sourceline(message->GetSourceLine());
    const char* sourceline_string = ToCString(sourceline);
    printf("%s\n", sourceline_string);
    // Print wavy underline (GetUnderline is deprecated).
    int start = message->GetStartColumn();
    for (int i = 0; i < start; i++) {
      printf(" ");
    }
    int end = message->GetEndColumn();
    for (int i = start; i < end; i++) {
      printf("^");
    }
    printf("\n");
    
    v8::String::Utf8Value stack_trace(try_catch->StackTrace());
    if (stack_trace.length() > 0) {
      const char* stack_trace_string = ToCString(stack_trace);
      cerr<<stack_trace_string<<"\n";
    }
  }
}

class TestStatic
{
public:
  TestStatic():a(4)
  {
    cout<<"static created\n";
  }


  ~TestStatic()
  {
    cout<<"static destructed: a:"<<a<<"\n";
  }
  
  void printA()
  {
    cout << "a:::: "<<a<<"\n";
  }

private:
  int a;
};

class JsIface : public ModIface 
{
public:
  
  static TestStatic tt;
  
  JsIface(tags_t & t):tags(t), work_(service), timer(service)
  {
    cout<<"JIFace created\n";
    tt.printA();
  }
  
  ~JsIface()
  {
  }
  
  virtual void init()
  {
  }
  
  virtual void start()
  {  
    HandleScope handle_scope;
  
    Handle<ObjectTemplate> global_template = ObjectTemplate::New();
    global_template->Set(String::New("print"), FunctionTemplate::New(Print));

    Persistent<Context> context = Context::New(nullptr, global_template);
    Context::Scope context_scope(context);
    installTags(tags, context);

//    io_service service;
//    io_service::work w(service);
  
    monotone_timer timer(service);
    installTimer(timer, context);
  
    Handle<String> source = String::New( readScriptText(tags.projectFolder()).c_str()  );

    do {
      Handle<Script> script;
      { 
        TryCatch try_catch;
        cout << "before compile\n";
        script = Script::Compile(source);
        if ( script.IsEmpty() ) {
          ReportException(&try_catch);
          break;
        }
      }
      {
        TryCatch try_catch;
        cout << "before run\n";
        script->Run();
        cout << "script finished!!!!\n";
        if ( try_catch.HasCaught() ) {
          ReportException(&try_catch);
          break;
       }
     }
    } while (false);
  
    context.Dispose();
  }
  
  virtual void stop()
  {
    cout << "stoppping js...";
    timer.cancel();
    service.stop();
    cout << " stopped\n";
  }
  

private:
  tags_t & tags;
  io_service service;
  io_service::work work_;

  monotone_timer timer;
  
};

TestStatic JsIface::tt;

extern "C" shared_ptr<ModIface> executeModule( tags_t& tags )
{
  return shared_ptr<ModIface>(new JsIface(tags));
#if 0
  HandleScope handle_scope;
  
  Handle<ObjectTemplate> global_template = ObjectTemplate::New();
  global_template->Set(String::New("print"), FunctionTemplate::New(Print));

  Persistent<Context> context = Context::New(nullptr, global_template);
  Context::Scope context_scope(context);
  installTags(tags, context);


  io_service service;
  io_service::work w(service);
  
  monotone_timer timer(service);
  installTimer(timer, context);
  
  f = bind(stop, std::ref(service), std::ref(timer));

  Handle<String> source = String::New( readScriptText(tags.projectFolder()).c_str()  );

  do {
    Handle<Script> script;
    { 
      TryCatch try_catch;
      cout << "before compile\n";
      script = Script::Compile(source);
      if ( script.IsEmpty() ) {
        ReportException(&try_catch);
        break;
      }
    }
    {
      TryCatch try_catch;
      cout << "before run\n";
      script->Run();
      cout << "script finished!!!!\n";
      if ( try_catch.HasCaught() ) {
        ReportException(&try_catch);
        break;
     }
    }  
  } while (false);
  
  context.Dispose();
#endif

}

%module(directors="1") openpad
%{
#include "../PlayPhone/openpad.h"
%}

%feature("directors");

%include "std_string.i"
%include "std_vector.i"
#include <string>
#include <vector>

%typemap(jni) std::string *INOUT, std::string &INOUT %{jobjectArray%}
%typemap(jtype) std::string *INOUT, std::string &INOUT "java.lang.String[]"
%typemap(jstype) std::string *INOUT, std::string &INOUT "java.lang.String[]"
%typemap(javain) std::string *INOUT, std::string &INOUT "$javainput"

%typemap(in) std::string *INOUT (std::string strTemp ), std::string &INOUT (std::string strTemp ) {
  if (!$input) {
    SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
    return $null;
  }
  if (JCALL1(GetArrayLength, jenv, $input) == 0) {
    SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
    return $null;
  }

  jobject oInput = JCALL2(GetObjectArrayElement, jenv, $input, 0); 
  if ( NULL != oInput ) {
    jstring sInput = static_cast<jstring>( oInput );

    const char * $1_pstr = (const char *)jenv->GetStringUTFChars(sInput, 0); 
    if (!$1_pstr) return $null;
    strTemp.assign( $1_pstr );
    jenv->ReleaseStringUTFChars( sInput, $1_pstr);  
  }

  $1 = &strTemp;
}

%typemap(freearg) std::string *INOUT, std::string &INOUT ""

%typemap(argout) std::string *INOUT, std::string &INOUT
{ 
  jstring jStrTemp = jenv->NewStringUTF( strTemp$argnum.c_str() );
  JCALL3(SetObjectArrayElement, jenv, $input, 0, jStrTemp ); 
}

%include "../PlayPhone/openpad.h"
%include "../PlayPhone/Server.h"
%include "../PlayPhone/Request.h"
//%include "../PlayPhone/ServerHandler.h"

namespace openpad {
    class ServerHandler{
    public:
        Server* serv;
        
        virtual void onStart();
        %apply std::string &INOUT { std::string & why };
        virtual bool canJoin(Client* cli, string& why);
        virtual string getName();
        virtual string getDesc();
        virtual string getIconFilePath();
        virtual int getFilledSlots();
        virtual int getOpenSlots();
        virtual void onJoin(Client* cli);
        virtual PadConfig getDefaultControls();
        virtual void onDisconnect(Client* cli);
        virtual void onPadUpdate(Client* cli, PadUpdateObject update);
    private:
    };
    
}
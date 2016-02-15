#include "jexception.h"
#include "tools.h"


JException::JException ( java::lang::Throwable * t ):throwable(t)
{
}

string JException::getMessage()
{
  return jstring_to_string(throwable->getMessage());
}
  
string JException::getLocalizedMessage()
{
  return jstring_to_string(throwable->getLocalizedMessage());
}
  
string JException::toString()
{
  return jstring_to_string(throwable->toString());
}
  
void JException::printStackTrace()
{
  throwable->printStackTrace();
}


#include "stream.h"

namespace mu {

  // return a string describing this stream and all of its inputs.
  std::string Stream::inspect(int level) {
    std::stringstream ss;
    inspectIndent(ss, level); 
    ss << "#<" << getClassName() << " " << this << ">" << std::endl;
    inspectIndent(ss, level+1); 
    // wtf?
    // ss << "getStart() = " << ((getStart()==kIndefinite)?"kIndefinite":getStart()) << std::endl;
    if (getStart() == kIndefinite) {
      ss << "getStart() = " << "kIndefinite" << std::endl;
    } else {
      ss << "getStart() = " << getStart() << std::endl;
    }      
    inspectIndent(ss, level+1); 
    // ss << "getStart() = " << ((getEnd()==kIndefinite)?"kIndefinite":getEnd()) << std::endl;
    if (getEnd() == kIndefinite) {
      ss << "getEnd() = " << "kIndefinite" << std::endl;
    } else {
      ss << "getEnd() = " << getEnd() << std::endl;
    }      
    inspectAux(ss, level+1);
    return ss.str();
  }

  void Stream::inspectIndent(std::stringstream& ss, int level) {
    for (int i=0; i<level; i++) { ss << "| "; }
  }

  void Stream::inspectAux(std::stringstream& ss, int level) {
  }
  
}

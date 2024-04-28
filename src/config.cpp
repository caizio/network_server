#include "config.h"

namespace caizi{

std::ostream& operator<<(std::ostream & out, const ConfigVarBase &cvb){
    out << cvb.getName() << ":" << cvb.getDesccription();
    return out;
}
}
#ifndef AFLCLASS_H
#define AFLCLASS_H

#ifndef __VECTOR__
# include <vector>
#endif
#ifndef __STRING__
# include <string>
#endif

class AFLClass
{
public:
    AFLClass();
    std::vector< unsigned int> GetFecActive();
    bool SetFecActive(int module, std::string & FeeServerName );


private:



};

#endif // AFLCLASS_H

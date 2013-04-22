#ifndef __LOCKGUARD_HPP
#define __LOCKGUARD_HPP
namespace dcs {
namespace fee {

class Mutex
{
public:
  Mutex();
  ~Mutex();

  int Lock();
  int Unlock();

private:
  void* fpMutex;
};

class LockGuard
{
public:
  LockGuard(Mutex& m);
  ~LockGuard();

private:
  Mutex& fMutex;
};

}
}
#endif //__LOCKGUARD_HPP

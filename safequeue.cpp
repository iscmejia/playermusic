#ifndef SAFE_QUEUE
#define SAFE_QUEUE

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>


// A threadsafe-queue.
template <class T>
class SafeQueue
{
public:
  SafeQueue(void)
    : q()
    , m()
    , c()
  {}

  ~SafeQueue(void)
  {}

  // Add an element to the queue.
  void enqueue(T t)
  {
    std::lock_guard<std::mutex> lock(m);
    q.push(t);
    c.notify_one();
  }

  // Get the "front"-element.
  // If the queue is empty, wait till a element is avaiable.
  T dequeue(void)
  {
    std::unique_lock<std::mutex> lock(m);
    while(q.empty())
    {
      // release lock as long as the wait and reaquire it afterwards.
      c.wait(lock);
    }
    T val = q.front();
    q.pop();
    return val;
  }

private:
  std::queue<T> q;
  mutable std::mutex m;
  std::condition_variable c;
};

class iteratorQueue{
  SafeQueue<std::string>& iter;
  int index;

  public: iteratorQueue(SafeQueue<std::string>& sq) : iter(sq), index(0) {}
  std::string operator++(int) { // Prefix
    //require(index < iter.top,NULL);
    return iter.dequeue();
  }

};
#endif

/*int main() {
  //string v[5]={"Cancion 1","Cancion 2","Cancion 3","Cancion 4","Cancion 5"};
  SafeQueue<std::string> sq;
  for(int i = 0; i < 5; i++){
    char palabra[64]="Texto Variado";
    //letras = std::to_string(20-i);
    //printf("%s\n",v[i]);
    sq.enqueue(palabra);
  }
    
  // Traverse with an iterator:
  iteratorQueue iter(sq);
  for(int j = 0; j < 5; j++)
    printf("%s\n",iter++);
}*/
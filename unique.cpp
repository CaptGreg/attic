#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>

using namespace std;

class Timer {
  decltype(std::chrono::high_resolution_clock::now()) start, stop;
public:
  void Start() { start = std::chrono::high_resolution_clock::now(); }
  void Stop()  { stop  = std::chrono::high_resolution_clock::now(); }
  uint64_t millisecs()
  {
   typedef std::chrono::duration<int,std::milli> millisecs_t ;
    millisecs_t duration_get( std::chrono::duration_cast<millisecs_t>(stop-start) ) ;
    return duration_get.count();
  }
  uint64_t microsecs()
  {
   typedef std::chrono::duration<int,std::micro> microsecs_t ;
    microsecs_t duration_get( std::chrono::duration_cast<microsecs_t>(stop-start) ) ;
    return duration_get.count();
  }
  uint64_t nanosecs()
  {
   typedef std::chrono::duration<int,std::nano> nanosecs_t ;
    nanosecs_t duration_get( std::chrono::duration_cast<nanosecs_t>(stop-start) ) ;
    return duration_get.count();
  }
};


// algo f1 to f5 are from the comments to 
// https://stackoverflow.com/questions/1041620/whats-the-most-efficient-way-to-erase-duplicates-and-sort-a-vector
// (converted to function template by GB)

// timings for a 100 million element vector initiallized with values 0-99999
// RemoveDuplicatesInVector 20.241 s.   <-- set<T> values;
// RemoveDuplicatesInVector 0.55858 s.  <-- unorderedset<T> values;
// algo 1  .715575 sec.
// algo 2 1.953870 sec.
// algo 3 2.213034 sec.
// algo 4  .093701 sec.
// algo 5  .067934 sec.

// timings for a 1<<29 element vector initiallized with values 0-99999
// algo 1  40.904 s.
// algo 2 110.114 s.
// algo 3 118.927 s.
// algo 4   4.265 s.
// algo 5   2.754 s.


template<typename T>
void algo_f1 (vector<T>& vec) // sort + unique
{
  sort( vec.begin(), vec.end() );
  vec.erase( unique( vec.begin(), vec.end() ), vec.end() );
}

template<typename T>
void algo_f2 (vector<T>& vec) // Convert to set (using a constructor)
{
  set<int> s( vec.begin(), vec.end() );
  vec.assign( s.begin(), s.end() );
}

template<typename T>
void algo_f3 (vector<T>& vec) // Convert to set (manually)
{
  set<int> s;
  for (int i : vec)
    s.insert(i);
  vec.assign( s.begin(), s.end() );
}

template<typename T>
void algo_f4 (vector<T>& vec) // Convert to unordered_set (using a constructor)
{
  unordered_set<int> s( vec.begin(), vec.end() );
  vec.assign( s.begin(), s.end() );
  sort( vec.begin(), vec.end() );
}

template<typename T>
void algo_f5 (vector<T>& vec) // Convert to unordered_set (manually) - tied with RemoveDuplicatesInVector
{
  unordered_set<int> s;
  for (int i : vec)
    s.insert(i);
  vec.assign( s.begin(), s.end() );
  sort( vec.begin(), vec.end() );
}

template <class T>
void RemoveDuplicatesInVector(std::vector<T> & vec) // tied with algo_f5
{
    // set<T> values;
    unordered_set<T> values;
    /*
      Sets are containers that store unique elements following a specific order.

      In a set, the value of an element also identifies it (the value is itself the key, of type T), 
      and each value must be unique. The value of the elements in a set cannot be modified once in 
      the container (the elements are always const), but they can be inserted or removed from the container.

      Internally, the elements in a set are always sorted following a specific strict weak ordering 
      criterion indicated by its internal comparison object (of type Compare).

      set containers are generally slower than unordered_set containers to access individual elements 
      by their key, but they allow the direct iteration on subsets based on their order.

      Sets are typically implemented as binary search trees.

      set::insert --- The single element versions (1) return a pair, with its member pair::first set 
      to an iterator pointing to either the newly inserted element or to the equivalent element already 
      in the set. The pair::second element in the pair is set to true if a new element was inserted or 
      false if an equivalent element already existed.
    */
    vec.erase(std::remove_if(vec.begin(), vec.end(), [&values](const T & value) { return !values.insert(value).second; }), vec.end());
}

int main() 
{
    // remove duplicate elements --- same as algo_f1: 1. sort 2. erase(unique(...))
    std::vector<int> v{1,2,3,1,2,3,3,4,5,4,5,6,7};
    std::sort(v.begin(), v.end()); // 1 1 2 2 3 3 3 4 4 5 5 6 7 
      // auto last = std::unique(v.begin(), v.end());
      // v now holds {1 2 3 4 5 6 7 x x x x x x}, where 'x' is indeterminate
      // v.erase(last, v.end()); 

    // OR IN ONE LINE:
    v.erase(std::unique(v.begin(), v.end()), v.end()); 

    cout << "sort-unique: "; 
    for (int i : v)
      std::cout << i << " ";
    std::cout << "\n";

    cout << "w: ";
    std::vector<int> w{1,2,3,1,2,3,3,4,5,4,5,6,7};
    for (int i : w)
      std::cout << i << " ";
    std::cout << "\n";

    RemoveDuplicatesInVector(w);
    for (int i : w)
      std::cout << i << " ";
    std::cout << "\n";


    vector<int> master, copy;

    // master.resize(1<<29);  // 1/2 billion
    master.resize(100'000'000);
    for(auto& e : master)
      e = rand() % 100000;

    Timer t;

    copy = master;
    t.Start();
    RemoveDuplicatesInVector(copy);
    t.Stop();
    cout << setw(30) << "RemoveDuplicatesInVector " << 1e-6*t.microsecs() << " s. " << master.size() << " in " << copy.size() << " out\n";

    copy = master;
    t.Start();
    algo_f1(copy);
    t.Stop();
    cout << setw(30) <<  "algo 1 " << 1e-6*t.microsecs() << " s. " << master.size() << " in " << copy.size() << " out\n";

    copy = master;
    t.Start();
    algo_f2(copy);
    t.Stop();
    cout << setw(30) << "algo 2 " << 1e-6*t.microsecs() << " s. " << master.size() << " in " << copy.size() << " out\n";

    copy = master;
    t.Start();
    algo_f3(copy);
    t.Stop();
    cout << setw(30) <<  "algo 3 " << 1e-6*t.microsecs() << " s. " << master.size() << " in " << copy.size() << " out\n";

    copy = master;
    t.Start();
    algo_f4(copy);
    t.Stop();
    cout << setw(30) <<  "algo 4 " << 1e-6*t.microsecs() << " s. " << master.size() << " in " << copy.size() << " out\n";

    copy = master;
    t.Start();
    algo_f5(copy);
    t.Stop();
    cout << setw(30) << "algo 5 " << 1e-6*t.microsecs() << " s. " << master.size() << " in " << copy.size() << " out\n";
}

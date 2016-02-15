#ifndef __AMIGO_FS_H_
#define __AMIGO_FS_H_

#include <string>
#include <list>

#include <boost/filesystem.hpp>

namespace a
{
  namespace fs {
    using std::string;
    
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) 
      const string slash(1, '/');
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) 
      const string slash(1, '\\');
#else
#error "slash for platform not implemented"
#endif

//    const string slash(1, boost::filesystem::slash<boost::filesystem::path>::value);

    using namespace boost::filesystem;

    namespace impl {
      inline path bin_path ()
      {
        return initial_path();
      }
      inline path lib_path ()
      {
        return initial_path().parent_path() / "lib";
      }
    }

    typedef std::list<string> file_paths;
  
    inline string bin_path ()
    {
      return impl::bin_path().string();
    }
  
    inline string lib_path()
    {
      return impl::lib_path().string();
    }
    inline string net_modules_path()
    {
      return lib_path() + slash + "net";
    }
    inline string input_modules_path()
    {
      return net_modules_path() + slash + "drivers" + slash + "input";
    }

    inline string output_modules_path()
    {
      return net_modules_path() + slash + "drivers" + slash + "output";
    }
    
    inline string etc_path()
    {
      return (initial_path().parent_path() / "etc" ).string();
    }
    
    inline file_paths modules ()
    {
      path p = impl::lib_path();
      file_paths paths;
      directory_iterator end;
      
      for (directory_iterator iter(p); iter != end; ++iter  ) {
        if (is_regular_file(iter->status()) )
          paths.push_back ( (*iter).path().string() );
      }
      return paths;
    }
    
    template <class S>
    inline S lib_name(const S& n)
    {
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) 
      return "lib" + n + ".so";
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) 
      return "lib" + n + ".dll";
#else
#error "lib_name for platform not implemented"
#endif
    }
    
  }
};

#endif

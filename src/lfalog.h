#ifndef LFALOG_H
#define LFALOG_H

/**
 * Log system.
 * Macros to do cout<< style logging
 *
 * Same syntax on all platforms, different implementations.
 *
 * Good to use everywhere since stdio (cout) and error (cerr) do not work on some platforms.
 * It does not do wide characters
 *
 * Define LFALOG (-DLFALOG) in build to get logging
 *
 */




#ifdef ANDROID
#include <android/log.h>
#include <iostream>
#include <string>
#include <sstream>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Lfa", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Lfa", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Lfa", __VA_ARGS__))

/*
 * Note:
 * if(bla)
 *   {
 *   LOGERR("foo"); //If no enclosing '{' '}' --will not compile. (given the 'else' below)
 *   }
 * else
 *  {
 *  }
 *
 */
#define LOGERR(arg)    { std::ostringstream oss; oss<<"("<<__FUNCTION__<<") "<<arg<< std::endl; LOGE(oss.str().c_str());}void(0)
#define LOGSTD(arg)    { std::ostringstream oss; oss<<"("<<__FUNCTION__<<") "<<arg<< std::endl; LOGI(oss.str().c_str());}void(0)
#define LOGWRN(arg)    { std::ostringstream oss; oss<<"("<<__FUNCTION__<<") "<<arg<< std::endl; LOGW(oss.str().c_str());}void(0)



#else

#include <iostream>
#include <string>
#include <sstream>

#ifdef __FUNCTION__
#define LOGERR(arg) (std::cout<<"E: ""("<<__FUNCTION__<<") \a"<<arg<<std::endl<<std::flush)
#define LOGSTD(arg) (std::cout<<"   "<<arg<<std::endl<<std::flush)
#define LOGWRN(arg) (std::cout<<"W: ""("<<__FUNCTION__<<") "<<arg<<std::endl<<std::flush)
#else

#define LOGERR(arg) (std::cout<<"E: "<<arg<<std::endl<<std::flush)
#define LOGSTD(arg) (std::cout<<"   "<<arg<<std::endl<<std::flush)
#define LOGWRN(arg) (std::cout<<"W: "<<arg<<std::endl<<std::flush)

#endif

#endif




//If no logging , don't do anything:
#ifndef LFALOG

/*
Log errors no matter what:
Please do not abuse this -only log true errors.
#undef LOGERR
#define LOGERR(arg) void(0)
*/
#undef LOGSTD
#define LOGSTD(arg) void(0)
#undef LOGWRN
#define LOGWRN(arg) void(0)

#undef LOGI
#define LOGI(...) void(0)

#undef LOGW
#define LOGW(...) void(0)

#undef LOGE
#define LOGE(...) void(0)
#endif



//Use to log when profiling and such:
#ifdef LFATRACE
#define LOGT(arg) (std::cout<<"T: ""("<<__FUNCTION__<<") "<<arg<<std::endl<<std::flush)
#else
#define LOGT(...) void(0)
#endif

#endif //LFALOG_H


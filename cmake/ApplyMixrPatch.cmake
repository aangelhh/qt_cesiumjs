if(NOT DEFINED MIXR_SOURCE_DIR)
    message(FATAL_ERROR "MIXR_SOURCE_DIR is required")
endif()

function(replace_or_validate variable_name original replacement description)
    set(content "${${variable_name}}")
    string(FIND "${content}" "${replacement}" replacement_position)
    if(NOT replacement_position EQUAL -1)
        return()
    endif()

    string(FIND "${content}" "${original}" original_position)
    if(original_position EQUAL -1)
        message(FATAL_ERROR
            "Pinned MIXR source no longer matches the ${description} anchor")
    endif()

    string(REPLACE "${original}" "${replacement}" content "${content}")
    set(${variable_name} "${content}" PARENT_SCOPE)
endfunction()

set(thread_source_path
    "${MIXR_SOURCE_DIR}/src/base/threads/platform/IThread_linux.cpp")
file(READ "${thread_source_path}" thread_source)

replace_or_validate(thread_source
    [=[#include <signal.h>
#include <iostream>]=]
    [=[#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>]=]
    "POSIX includes")

replace_or_validate(thread_source
    [=[int IThread::getNumProcessors()
{
   int num{};

   cpu_set_t mask;
   int rtn{sched_getaffinity(0, sizeof(cpu_set_t), &mask)};
   if (rtn == 0) {
      // we're checking the number of bits that are set in 'mask'
      for (int cpu = 0; cpu < MAX_CPUS; cpu++) {
         if ( CPU_ISSET(cpu, &mask) != 0 ) num++;
      }
   }

   return num;
}]=]
    [=[int IThread::getNumProcessors()
{
#if defined(__APPLE__)
   const long count{sysconf(_SC_NPROCESSORS_ONLN)};
   return count > 0 ? static_cast<int>(count) : 1;
#else
   int num{};

   cpu_set_t mask;
   int rtn{sched_getaffinity(0, sizeof(cpu_set_t), &mask)};
   if (rtn == 0) {
      // we're checking the number of bits that are set in 'mask'
      for (int cpu = 0; cpu < MAX_CPUS; cpu++) {
         if ( CPU_ISSET(cpu, &mask) != 0 ) num++;
      }
   }

   return num;
#endif
}]=]
    "processor-count implementation")

replace_or_validate(thread_source
    [=[   pthread_t* thread{new pthread_t};
   pthread_create(thread, &attr, staticThreadFunc, this);]=]
    [=[   pthread_t* thread{new pthread_t};
   const int status{pthread_create(thread, &attr, staticThreadFunc, this)};
   pthread_attr_destroy(&attr);]=]
    "pthread creation")

replace_or_validate(thread_source
    [=[   theThread = thread;

   return (theThread != nullptr);]=]
    [=[   if (status == 0) theThread = thread;
   else delete thread;

   return (theThread != nullptr);]=]
    "pthread result handling")

replace_or_validate(thread_source
    [=[      pthread_t* thread{static_cast<pthread_t*>(theThread)};
      pthread_kill(*thread, SIGKILL);
      theThread = nullptr;]=]
    [=[      pthread_t* thread{static_cast<pthread_t*>(theThread)};
#if defined(__APPLE__)
      pthread_cancel(*thread);
      pthread_join(*thread, nullptr);
#else
      pthread_kill(*thread, SIGKILL);
#endif
      delete thread;
      theThread = nullptr;]=]
    "pthread termination")

file(WRITE "${thread_source_path}" "${thread_source}")

set(makedefs_path "${MIXR_SOURCE_DIR}/src/makedefs")
file(READ "${makedefs_path}" makedefs)
replace_or_validate(makedefs
    [=[CPPFLAGS += -Wno-unused-but-set-variable

# ---]=]
    [=[CPPFLAGS += -Wno-unused-but-set-variable

# Apple Clang treats the legacy Flex-generated 'register' declarations as an
# error in C++17. Keep the generated scanner buildable until it is regenerated.
ifeq ($(shell uname -s),Darwin)
CPPFLAGS += -Wno-register
endif

# ---]=]
    "Apple Clang warning flags")
file(WRITE "${makedefs_path}" "${makedefs}")

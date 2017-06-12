//
// Created by Thoa Kim on 6/10/17.
//

#include <errno.h>
#ifndef VIDEOKIT_UTILS_H
#define VIDEOKIT_UTILS_H
#define IP_FILE "/Users/anonymousjp/Desktop/test.mp4"

#ifdef CRAZY_DEBUG
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: [__func__] " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOL printf("LOL Debugger\n");
#define LOGI(name,val) printf("Log - %s: %d\n",name,val)
#define LOGS(name,val) printf("Log - %s: %s\n",name,val)
#else
#define debug(M, ...)
#define LOL
#define LOGI(name,val)
#define LOGS(name,val)
#endif
#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define LOGERR(M, ...) fprintf(stderr, "[ERROR - line:%d] in FILE[%s] at FUNC[%s]:" M "\n", __LINE__, __FILE__, __func__, clean_errno(), ##__VA_ARGS__)
#define LOGWARN(M, ...) fprintf(stderr, "[WARN - line:%d] in FILE[%s] at FUNC[%s]:" M "\n",__LINE__, __FILE__, __func__, clean_errno(), ##__VA_ARGS__)
#define LOGINFO(M, ...) fprintf(stderr, "[INFO - line:%d] in FILE[%s] at FUNC[%s]:" M "\n", __LINE__, __FILE__, __func__, clean_errno(), ##__VA_ARGS__)
#define check(A, M, ...) if(!(A)) { LOGERR(M, ##__VA_ARGS__);}
#define sentinel(M, ...) LOGERR(M, ##__VA_ARGS__);
#define check_mem(A) check(A, "Memory allocation error.")
#endif //VIDEOKIT_UTILS_H

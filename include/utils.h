//
// Created by Thoa Kim on 6/10/17.
//

#ifndef VIDEOKIT_UTILS_H
#define VIDEOKIT_UTILS_H
#define IP_FILE "/Users/anonymousjp/Desktop/test.mp4"

#ifdef CRAZY_DEBUG
#define LOL printf("LOL Debugger\n");
#define LOGI(name,val) printf("Log - %s: %d\n",name,val)
#define LOGS(name,val) printf("Log - %s: %s\n",name,val)
#else
#define LOL
#define LOGI(name,val)
#define LOGS(name,val)
#endif

#endif //VIDEOKIT_UTILS_H

#include "bits/stdc++.h"
using namespace std;

int main()
{
    char cmd[100] = "aaa          ";
    char *p;
    p = strchr(cmd,' ');
    while(*p++ == ' ');
    printf("%c",*p);
    return 0;
}

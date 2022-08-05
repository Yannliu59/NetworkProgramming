#include "bits/stdc++.h"
using namespace std;

void usage()
{
//    创建信号量 -c
    fprintf(stderr, "semtool -c\n");
//    删除	  -d
    fprintf(stderr, "semtool -d\n");
//    获得值    -g
    fprintf(stderr, "semtool -g\n");
//    设置值    -s <val>
    fprintf(stderr, "semtool -s <val>\n");
//    p操作	 -p
    fprintf(stderr, "semtool -p\n");
//    v操作     -v
    fprintf(stderr, "semtool -v\n");
//    查看权限   -f
    fprintf(stderr, "semtool -f\n");
//    更改权限   -m <mode>
    fprintf(stderr, "semtool -m <mode>\n");
}


int main()
{
    usage();
    return 0;
}

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

typedef struct
{
    char name[20];
    unsigned long total;
    char name2[20];
    unsigned long free;
    unsigned long avail;
}MEM_OCCUPY;

typedef struct          //定义一个cpu occupy的结构体
{
    char name[20];      //定义一个char类型的数组名name有20个元素
    unsigned int user; //定义一个无符号的int类型的user
    unsigned int nice; //定义一个无符号的int类型的nice
    unsigned int system;//定义一个无符号的int类型的system
    unsigned int idle; //定义一个无符号的int类型的idle
}CPU_OCCUPY;

class System_Info
{
public:
    System_Info();
private:
    static int  cal_cpuoccupy (CPU_OCCUPY *o, CPU_OCCUPY *n) ;
    static void get_cpuoccupy (CPU_OCCUPY *cpust);
public:
    static int Get_CPU_Info();
    static int Get_GPU_Info();
    static int Get_RAM_Info();

    static void Get_Mem_Occupy (MEM_OCCUPY *mem);
};

#endif // SYSTEM_INFO_H

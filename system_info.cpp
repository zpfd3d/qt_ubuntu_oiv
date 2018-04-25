#include "system_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void System_Info::Get_Mem_Occupy(MEM_OCCUPY *mem)
{
    FILE *fd;
    char buff[256];
    MEM_OCCUPY *m;
    m=mem;

    fd = fopen ("/proc/meminfo", "r");
    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%s %lu %s", m->name, &m->total, m->name2);
    //printf("%s %u %s\n", m->name, m->total, m->name2);
    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%s %lu %s", m->name, &m->free, m->name2);
    //printf("%s %u %s\n", m->name, m->free, m->name2);
    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%s %lu %s", m->name, &m->avail, m->name2);
    fclose(fd);
}

int System_Info::Get_CPU_Info()
{/*
    system("top -n 1 | grep Cpu > cpu.txt");
    FILE *fd;
    char buf[255] = {0};
    float sysOcc = 0,usrOcc = 0;
    fd = fopen ("cpu.txt", "r");
    fgets (buf, sizeof(buf), fd);
   // printf("%s sysocc:%lf usrOcc:%lf\n", buf,sysOcc,usrOcc);
    sscanf (buf, "%s %f %s %f",buf,sysOcc,buf,usrOcc);
  //  printf("%s sysocc:%lf usrOcc:%lf\n", buf,sysOcc,usrOcc);
    cpu_occ = sysOcc + usrOcc;
    fclose(fd);//*/
    int cpu;
    CPU_OCCUPY cpu_stat1;
    CPU_OCCUPY cpu_stat2;
    //第一次获取cpu使用情况
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1);
    //sleep(2);

    //第二次获取cpu使用情况
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2);

    //计算cpu使用率
    cpu = cal_cpuoccupy ((CPU_OCCUPY *)&cpu_stat1, (CPU_OCCUPY *)&cpu_stat2);
    return cpu/100;
}

int System_Info::Get_GPU_Info()
{
    return 0;
}

int System_Info::Get_RAM_Info()
{
    MEM_OCCUPY mem_stat;
    System_Info::Get_Mem_Occupy((MEM_OCCUPY *)&mem_stat);
    //  float occ = (float)(mem_stat.total-mem_stat.free)/(float)mem_stat.total;
    float occ = 1.0 - (float)(mem_stat.avail)/(float)mem_stat.total;
    return (int)(occ*100);
}

int System_Info::cal_cpuoccupy (CPU_OCCUPY *o, CPU_OCCUPY *n)
{
    unsigned long od, nd;
    unsigned long id, sd;
    int cpu_use = 0;

    od = (unsigned long) (o->user + o->nice + o->system +o->idle);//第一次(用户+优先级+系统+空闲)的时间再赋给od
    nd = (unsigned long) (n->user + n->nice + n->system +n->idle);//第二次(用户+优先级+系统+空闲)的时间再赋给od

    id = (unsigned long) (n->user - o->user);    //用户第一次和第二次的时间之差再赋给id
    sd = (unsigned long) (n->system - o->system);//系统第一次和第二次的时间之差再赋给sd
    if((nd-od) != 0)
        cpu_use = (int)((sd+id)*10000)/(nd-od); //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
    else cpu_use = 0;
    //printf("cpu: %u\n",cpu_use);
    return cpu_use;
}

void System_Info::get_cpuoccupy (CPU_OCCUPY *cpust) //对无类型get函数含有一个形参结构体类弄的指针O
{
    FILE *fd;
    int n;
    char buff[256];
    CPU_OCCUPY *cpu_occupy;
    cpu_occupy=cpust;

    fd = fopen ("/proc/stat", "r");
    fgets (buff, sizeof(buff), fd);

    sscanf (buff, "%s %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle);

    fclose(fd);
}

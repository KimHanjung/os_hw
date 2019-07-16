#include <iostream>
#include <stdio.h>
#include <cstring>
#include <string>
#include <stack>
#include <queue>
#include "lru_stack.h"
#include <cmath>

using namespace std;

//initialize global variable
int binary[6] = {1, 2, 4, 8, 16, 32};
int p_memory[32];
int p_size[32];
int process[5][64] = {-1,};
int p_index[5] = {0,};
int page_fault = 0, id_index = 0, opt_count = 0, lru_count = 0, pra, inst, process_num;
int ids[320][2];
int reference[320][10];
int counting[320][2];
int **optlist;
stack <int> stk;
queue <int> que;
l_stack *lstk; // stack data structure of lru algorithm

//inittialize global function
void paging(string s);
void alloc(int pid, int alloc_id, int page_num);
bool buddy(int alloc_id, int page_num);
void unbuddy(int alloc_id);
void fifo(int pid, int alloc_id, int page_num);
void lru(int pid, int alloc_id, int page_num);
void sample_lru(int pid, int alloc_id, int page_num);
void lfu(int pid, int alloc_id, int page_num);
void mfu(int pid, int alloc_id, int page_num);
void optimal();
void optimal_set(int pid, int func, int alloc_id, int page_num, int opt_count);
int check(int alloc_id);
int find_id(int alloc_id);
void print_out(int p, int func, int alloc_id, int page_num);
bool is_blank(int start, int size);


int main(void)
{
    string s;
    lstk = new l_stack();
    
    memset(p_memory, -1, sizeof(int)*32);
    memset(process, -1, sizeof(int) * 5 * 64);
    memset(reference, 0, sizeof(int) * 320 * 10);
    memset(counting, 0, sizeof(int) * 320 * 2);

    cin >> pra >> process_num >> inst;


    optlist = new int*[inst];

    for(int i=0; i<inst; i++) optlist[i] = new int[4];
    for(int i=0; i<32; i++) p_size[i] = 32;
    
    getline(cin, s, '\n');
    for(int i=0; i<inst; i++)
    {
        getline(cin, s, '\n');
        paging(s);
    }

    if(pra == 5) optimal();
    printf("page fault = %d\n", page_fault);
}

//paging function
void paging(string s)
{
    lru_count++;
    size_t pos = 0;
    string temp;
    int input[4];
    int index = 0;
    int size;
    while ((pos = s.find(char(9))) != std::string::npos)
    {
            temp = s.substr(0, pos);
            s.erase(0, pos + 1);
            input[index++] = atoi(temp.c_str());
    }
    input[index++] = atoi(s.c_str());

    
    if(pra == 5)
    {
        optimal_set(input[0], input[1], input[2], input[3], opt_count++);
    }
    else if(input[1] == 1)
    {
        counting[id_index][0] = input[2];
        reference[id_index][0] = input[2];
        ids[id_index][0] = input[2];
        ids[id_index++][1] = input[3];
        size = input[3];
        alloc(input[0], input[2], input[3]);

        print_out(input[0], input[1], input[2], size);
    }
    else
    {
        size = find_id(input[2]);
        if(pra == 0)
            fifo(input[0], input[2], size);
        else if(pra == 1)
            lru(input[0], input[2], size);
        else if(pra == 2)
            sample_lru(input[0], input[2], size);
        else if(pra == 3)
            lfu(input[0], input[2], size);
        else if(pra == 4)
            mfu(input[0], input[2], size);
        
        print_out(input[0], input[1], input[2], size);
    }
    if(lru_count==8) lru_count = 0;
}

//allocation function
void alloc(int pid, int alloc_id, int page_num)
{
    int index;

    for(int i = 0; i < page_num; i++)
    {
        index = p_index[pid]++;
        process[pid][index] = alloc_id;
    }   
}

//allocate the memory to physical memory by buddy system
bool buddy(int alloc_id, int page_num)
{
    if(check(alloc_id)) 
    {
        if(pra == 1) lstk->push(alloc_id);
        return true;
    }

    int size;
    for(int i=0; i<5; i++)
    {
        if(page_num<=binary[i+1])
        {
            size = binary[i+1];
            break;
        }
    }
    if(page_num==1) size = 1;

    int gap = 0, origin = size;;
    int temp, start;
    temp = size;
    while(temp<64)
    {
        gap = 0;
        while(gap * temp <32)
        {
            if(is_blank(gap*temp, temp))
            {
                if(p_size[gap*temp] == temp)
                {
                    if(pra == 0) que.push(alloc_id);
                    else if(pra == 1) lstk->push(alloc_id);

                    for(int i=0; i<size; i++)
                        p_memory[gap * temp + i] = alloc_id;
                    
                    for(int i=0; i<size; i++)
                        p_size[gap * temp + i] = 0;

                    start = gap*temp;
                    while(temp>size)
                    {
                        for(int i=0; i<temp; i++) 
                            if(p_size[start + i]!=0) p_size[start + i] = temp/2;
                        temp /= 2;
                    }
                    
                    page_fault++;
                    return true;
                }
            }
            gap++;
        }
        temp *= 2;
    }

    return false;
}

//deallocating
void unbuddy(int alloc_id)
{
    int index, page_num;

    for(int i=0; i<32; i++)
    {
        if(p_memory[i]==alloc_id) 
        {
            index = i; 
            break;
        }
    }

    for(int i=0; i<32; i++)
        if(p_memory[i]==alloc_id) p_memory[i] = -1;

    for(int i=0; i<id_index; i++) 
        if(ids[i][0] == alloc_id) page_num = ids[i][1];

    int size, mate;
    for(int i=0; i<5; i++)
    {
        if(page_num<=binary[i+1])
        {
            size = binary[i+1];
            break;
        }
    }
    if(page_num==1) size = 1;
    if(size == 32)
        for(int i=0; i<32; i++) p_size[i] = 32;
    while(size<32)
    {
        mate = ((index % (2 * size)) == 0) ? index + size : index - size;
        if(p_size[mate] != size)
        {
            for(int i=0; i < size; i++) p_size[index + i] = size;
            break;
        }
        else
        {
            index = (mate < index) ? mate : index;
            size *= 2;
            for(int i=0; i< size; i++) p_size[index + i] = size;
        }
    }
    
}

//fifo algorithm
void fifo(int pid, int alloc_id, int page_num)
{
    int victim;
    while(!buddy(alloc_id, page_num))
    {
        victim = que.front();
        que.pop();
        unbuddy(victim);
    }
}

//LRU algorithm
void lru(int pid, int alloc_id, int page_num)
{
    int victim;
    while(!buddy(alloc_id, page_num))
    {
        victim = lstk->pop();
        unbuddy(victim);
    }
}

//Sampled LRU algorithm
void sample_lru(int pid, int alloc_id, int page_num)
{
    int min;
    int tmp, victim;
    int value[320];

    for(int i=0; i<id_index; i++)
    {
        tmp = 0;
        for(int j=1;j<9;j++) tmp += pow(2, 8-j) * reference[i][j];
        value[i] = tmp;
    }

    while(!buddy(alloc_id, page_num))
    {
        min = 2048;
        victim = INT32_MAX;
        
        for(int i=0; i<id_index; i++)
        {
            if((check(reference[i][0]))&&((value[i]<min) || ((value[i]==min)&&(reference[i][0] < victim))))
            {
                min = value[i];
                victim = reference[i][0];
            }
        }
        for(int i=0; i<id_index; i++)
        {
            if(reference[i][0] == victim)
                for(int j=1; j<10; j++) reference[i][j] = 0;
        }
        unbuddy(victim);
    }

    for(int i=0; i<id_index; i++)
    {
        if(reference[i][0]==alloc_id) reference[i][9] = 1;
        if(lru_count==8)
        {
            for(int j=8; j>1; j--) reference[i][j] = reference[i][j-1];
            reference[i][1] = reference[i][9];
            reference[i][9] = 0;
        }
    }
}

//LFU algorithm
void lfu(int pid, int alloc_id, int page_num)
{
    int victim;
    int min;

    while(!buddy(alloc_id, page_num))
    {
        min  = INT32_MAX;
        victim = INT32_MAX;
        for(int i=0; i<id_index; i++)
        {
            if((check((counting[i][0]))) && ((counting[i][1] < min)||((counting[i][1]==min)&& (counting[i][0]<victim))))
            {
                min = counting[i][1];
                victim = counting[i][0];
            }
        }
        for(int i=0; i<id_index; i++)
            if(counting[i][0] ==  victim) counting[i][1] = 0;
        unbuddy(victim);
    }

    for(int i=0; i<id_index; i++)
        if(counting[i][0] == alloc_id) counting[i][1]++;
}

//MFU algorithm
void mfu(int pid, int alloc_id, int page_num)
{
    int victim;
    int max;
    while(!buddy(alloc_id, page_num))
    {
        max  = -1;
        victim = INT32_MAX;
        for(int i=0; i<id_index; i++)
        {
            if((check((counting[i][0])))&&((counting[i][1] > max)||((counting[i][1] == max)&&(counting[i][0] < victim))))
            {
                max = counting[i][1];
                victim = counting[i][0];
            }
        }
        for(int i=0; i<id_index; i++)
            if(counting[i][0] ==  victim) counting[i][1] = 0;
        unbuddy(victim);
    }

    for(int i=0; i<id_index; i++)
        if(counting[i][0] == alloc_id) counting[i][1]++;
}

//Optimal algorithm
void optimal()
{  
    int target;
    int victim;
    int max;

    for(int i=0; i<inst; i++)
    {
        if(optlist[i][1] == 1) 
        {
            counting[id_index][0] = optlist[i][2];
            ids[id_index][0] = optlist[i][2];
            ids[id_index++][1] = optlist[i][3];
            alloc(optlist[i][0], optlist[i][2], optlist[i][3]);
            print_out(optlist[i][0], optlist[i][1], optlist[i][2], optlist[i][3]);
        }
        else
        {
            target = optlist[i][2];
            optlist[i][3] = find_id(optlist[i][2]);
            for(int j=i; j<inst; j++)
            {
                for(int k = 0; k<id_index; k++)
                    if((optlist[j][2] == counting[k][0]) && (counting[k][1] == 0)) 
                        counting[k][1] = j;
            }
            while(!buddy(target, optlist[i][3]))
            {
                max = -1;
                for(int j=0; j<id_index; j++)
                {
                    if((counting[j][1] > max) && check(counting[j][0]))
                    {
                        victim = counting[j][0];
                        max = counting[j][1];
                    }
                }
                max = 0;
                for(int j=0; j<id_index; j++)
                {
                    if(max==0)
                    {
                        if((counting[j][1] == 0) && check(counting[j][0])) 
                        {
                            victim = counting[j][0];
                            max++;
                        }
                    }
                    else
                        if((counting[j][1] == 0) && check(counting[j][0]) && (counting[j][0]<victim)) victim = counting[j][0];
                }
                unbuddy(victim);
            }
            for(int j=0; j<id_index; j++) counting[j][1] = 0;
            print_out(optlist[i][0], optlist[i][1], optlist[i][2], optlist[i][3]);
        }
    }   
}

//setting for Optimal algorithm
void optimal_set(int pid, int func, int alloc_id, int page_num, int opt_count)
{
    optlist[opt_count][0] = pid;
    optlist[opt_count][1] = func;
    optlist[opt_count][2] = alloc_id;
    optlist[opt_count][3] = page_num;
}

//check the page is in the physical memory
int check(int alloc_id)
{
    for(int i=0; i<32; i++)
    {
        if(p_memory[i]==alloc_id)
            return 1;
    }
    return 0;
}

//find the size of alloc_id
int find_id(int alloc_id)
{
    for(int i=0; i<id_index; i++)
    {
        if(ids[i][0] == alloc_id)
            return ids[i][1];
    }
}

//print the result
void print_out(int p, int func, int alloc_id, int page_num)
{
    string s = "|", s1;
    string alloc_func = (func==1) ? "ALLOCATION" : "ACCESS";
    printf("* Input : Pid [%d] Function [%s] Alloc ID [%d] Page Num[%d]\n", p, alloc_func.c_str(), alloc_id, page_num);

    for(int i=0; i<8; i++)
    {
        for(int j=0; j<4; j++)
        {
            if(p_memory[i*4 + j]==-1) s += '-';
            else  s += to_string(p_memory[i*4 + j]);
        }
        s += '|';
    }
    printf("%-30s", ">> Physical Memory : "); 
    printf("%s", s.c_str());
    printf("\n");

    for(int pid=0; pid<process_num; pid++)
    {
        s = '|';
        s1 = '|';
        for(int j=0; j<16; j++)
        {
            for(int k=0; k<4; k++)
            {
                if(process[pid][j*4 + k]==-1)
                {
                    s += '-';
                    s1 += '-';
                }
                else
                {
                    s += to_string(process[pid][j*4 + k]);
                    s1 += char(check(process[pid][j*4 + k]) + '0');
                }
            }
            s += '|';
            s1 += '|';
        }
        printf(">> pid(%d) %-20s",pid, "Page Table(AID) : "); printf("%s", s.c_str());
        printf("\n");
        printf(">> pid(%d) %-20s",pid, "Page Table(Valid) : "); printf("%s", s1.c_str());
        printf("\n");
    }
    printf("\n");
}

//check the physical memory is empty
bool is_blank(int start, int size)
{
    if(start + size>32) return false;
    for(int i=0; i<size; i++)
        if(p_memory[start + i] != -1) return false;
    
    return true;
}
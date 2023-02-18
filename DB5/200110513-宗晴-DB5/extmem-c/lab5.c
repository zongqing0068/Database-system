#include <stdlib.h>
#include <stdio.h>
#include "extmem.h"

// 存储每个磁盘块的内容
typedef struct Blk
{
	int X[7];
    int Y[7];
    int addr;
}Blk;

// 初始化buf
int init_buf(Buffer *buf)
{
    if (!initBuffer(520, 64, buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    return 0;
}

// 初始化blk
void init_blk(Blk *blk)
{
	for(int i=0; i<7; i++){
		blk -> X[i] = -1;
		blk -> Y[i] = -1;
	}
	blk -> addr = -1;
}

// 解析每个磁盘块的内容（7个元组和1个后继磁盘块地址）
void read_blk(unsigned char *blk_d, Blk *blk)
{
	char str[5];
	int i;
	for (i = 0; i < 7; i++) // 读取7个元组
    {

        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk_d + i*8 + k);
        }
        blk->X[i] = atoi(str);
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk_d + i*8 + 4 + k);
        }
        blk->Y[i] = atoi(str);
    }
    for (int k = 0; k < 4; k++)  // 读取后继磁盘块地址
    {
        str[k] = *(blk_d + i*8 + k);
    }
    blk->addr = atoi(str);
}

// 封装初始化块、读取块、解析块、释放块的代码
Blk read_blk_data(int blk_id, Buffer *buf)
{
    unsigned char *blk_d;  // 用于存储从磁盘中读出的blk
    Blk blk;  // 用于存储从磁盘中读出的blk中的数据
	init_blk(&blk);  // 初始化块
    if ((blk_d = readBlockFromDisk(blk_id, buf)) == NULL)  // 读取块
    {
        perror("Reading Block Failed!\n");
    }
    read_blk(blk_d, &blk);  // 解析块
    freeBlockInBuffer(blk_d, buf);  // 释放块
    return blk;
}

// 将某个磁盘块的内容写回buffer（7个元组和1个后继磁盘块地址）
void write_blk(unsigned char *blk_d, Blk blk)
{
	char str[5];
	int i;
	for (i = 0; i < 7; i++)  // 写7个元组
    {
		itoa(blk.X[i],str,10);
        for (int k = 0; k < 4; k++)
        {
            *(blk_d + i*8 + k) = str[k];
        }
        itoa(blk.Y[i],str,10);
        for (int k = 0; k < 4; k++)
        {
            *(blk_d + i*8 + 4 + k) = str[k];
        }
    }
	itoa(blk.addr,str,10);
    for (int k = 0; k < 4; k++)  // 写后继磁盘块地址
    {
        *(blk_d + i*8 + k) = str[k];
	}
}

// 封装解析块、写回块、清空块、释放块的代码
void write_ans_blk(Blk *ans, Buffer *buf, int blk_id)
{
    ans->addr = blk_id + 1;
    unsigned char *ans_d;  // 用于存储待写入磁盘的结果
    ans_d = getNewBlockInBuffer(buf);  // 获取缓冲区中的空闲块
    write_blk(ans_d, *ans);  // 解析结果块
    if (writeBlockToDisk(ans_d, blk_id, buf) != 0)  // 写回磁盘
    {
        perror("Writing Block Failed!\n");
        return;
    }
    init_blk(ans);  // 将中间结果块清空
    freeBlockInBuffer(ans_d, buf);  // 释放buf中的结果块
}

void task1_linearSearch(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("基于线性搜索的关系选择算法 S.C=128\n");
    printf("-------------------------------------------------\n");

    init_buf(buf);
    Blk blk;  // 用于存储从磁盘中读出的blk中的数据
	Blk ans;  // 用于存储结果
	init_blk(&ans);
	int count = 0;	// 计算S.C=128的元组个数
	int blk_id_begin = 100;  // 输出结果起始块号
	int blk_num = 0;  // 输出结果块数


	// 查找S.C=128的所有元组
	for(int blk_id = 17; blk_id <= 48; blk_id++){
		blk = read_blk_data(blk_id, buf);
		printf("读入数据块%d\n", blk_id);
		for(int i = 0; i <= 7; i++){
			if(blk.X[i]==128){
				ans.X[count % 7] = blk.X[i];
				ans.Y[count % 7] = blk.Y[i];
				printf("(X=%d, Y=%d)\n", ans.X[count], ans.Y[count]);
				count++;
				if(count % 7 == 0){
                    write_ans_blk(&ans, buf, blk_id_begin + blk_num);
                    blk_num++;
                }
			}
		}
	}
	if(count != 0 && count % 7 != 0){
        write_ans_blk(&ans, buf, blk_id_begin + blk_num);
        blk_num++;
	}
	if(count == 0){
        printf("未查找到该元组！\n");
        return;
	}

	printf("注：结果写入磁盘：[%d, %d]\n\n", blk_id_begin, blk_id_begin + blk_num - 1);
	printf("满足选择条件的元组一共%d个。\n\n", count);
	printf("IO读写一共%ld次。\n\n\n", buf->numIO);
	return;
}

// 若元组(X1, Y1)大于(X2, Y2)，则返回1；否则返回0
int larger(int X1, int Y1, int X2, int Y2)
{
    if(X1 > X2) return 1;
    else if(X1 == X2 && Y1 > Y2) return 1;
    else return 0;
}

// 交换blk1中第i个元组和blk2中第j的元组
void swap(Blk *blk1, int i, Blk *blk2, int j)
{
    int tempx, tempy;
    tempx = blk1->X[i];
    tempy = blk1->Y[i];
    blk1->X[i] = blk2->X[j];
    blk1->Y[i] = blk2->Y[j];
    blk2->X[j] = tempx;
    blk2->Y[j] = tempy;
}

/* 对关系的子集合进行排序，按第一个属性从小到大然后第二个属性从小到大排序
   begin为待排序关系的起始块号，
   len为待排序关系的块数，
   out_begin为存储按子集合排好序的关系的起始块号。
*/
void task2_stage1(Buffer *buf, int begin, int len, int out_begin)
{
    int set_blk_num = 8;  // 每个子集合8块blk
    Blk blk[set_blk_num];  // 用于存储从磁盘中读出的blk中的数据
    int count = 0;  // 已排序块数
    while(len > count){
        // 读取每个子集合的数据
        int i = 0;
        while(len > count && i < set_blk_num){
            blk[i] = read_blk_data(begin + count, buf);
            count++;
            i++;
        }
        // 冒泡排序
        for(int j = 0; j < 7 * i; j++){
            for(int k = j + 1; k < 7 * i; k++){
                if(larger(blk[j/7].X[j%7], blk[j/7].Y[j%7], blk[k/7].X[k%7], blk[k/7].Y[k%7]))
                    swap(&blk[j/7], j%7, &blk[k/7], k%7);
            }
        }
        for(int j = 0; j < i; j++)
            write_ans_blk(&blk[j], buf, out_begin + count - i + j);
    }
    printf("第一阶段：划分子集合并在子集合内排序，中间结果写入磁盘[%d, %d]\n", out_begin, out_begin + len - 1);
    return;
}

// 获取最小的元组及其所在路数
int get_min(int X[], int Y[], int *minX, int *minY, int way_num, int not_finished[])
{
    *minX = 1000;
    *minY = 1000;
    int way_id;
    for (int i = 0; i < way_num ; i ++ ) {
        // 当前路还未被读完，且当前路的元组更小
        if (not_finished[i] && larger(*minX, *minY, X[i], Y[i])) {
            *minX = X[i];
            *minY = Y[i];
            way_id = i;
        }
    }
    return way_id;
}

/* 对子集合进行归并排序，按第一个属性从小到大然后第二个属性从小到大排序
   begin为待排序关系的起始块号，
   len为待排序关系的块数，
   out_begin为排好序的关系的起始块号。
*/
void task2_stage2(Buffer *buf, int begin, int len, int out_begin)
{
    int set_blk_num = 8;  // 每个子集合8块blk
    int way_num = len/set_blk_num;  // way_num路归并排序
    Blk blk[way_num];  // 用于存储从磁盘中读出的blk中的数据
    int count[way_num];  // 记录各路目前载入的块数
    int out_count = 0;  // 记录已输出的块数
    int pointer[way_num];  // 记录当前块读到的位置
    Blk ans;  // 输出块
    init_blk(&ans);
    int ans_pointer = 0;  // 输出块指针
    int not_finished[way_num];  // 判断某路是否已经处理完毕
	int i;
	// 读取各路的第一块
	for (i = 0; i < way_num; i++){
        blk[i] = read_blk_data(begin + i * set_blk_num, buf);
        count[i] = 1;
        pointer[i] = 0;
        not_finished[i] = 1;
	}
	int flag = 1;  // 标志是否所有路都读完
	while(flag){
		for (i = 0; i < way_num; i++){
            //某路的块读完了，需要读入新的块
			if(pointer[i] == 7){
				if (count[i] < set_blk_num){
                    blk[i] = read_blk_data(begin + i * set_blk_num + count[i], buf);
					pointer[i] = 0;
				}
				count[i]++;
			}
            // 判断某路是否已经处理完毕
			if(count[i] > set_blk_num)
				not_finished[i] = 0;
		}

        int X[way_num], Y[way_num];
        int minX, minY, way_id;
        int cur_not_finished = 1;  // 判断当前轮是否结束（即是否有块读完）
        while(cur_not_finished){
            for(i = 0; i < way_num; i++){
                if(not_finished[i]){
                    X[i] = blk[i].X[pointer[i]];
                    Y[i] = blk[i].Y[pointer[i]];
                }

            }
            way_id = get_min(X, Y, &minX, &minY, way_num, not_finished);
            ans.X[ans_pointer] = minX;
            ans.Y[ans_pointer] = minY;

            // 有一块读完
            if(++pointer[way_id] == 7){
                cur_not_finished = 0;
                if(count[way_id] == set_blk_num)  not_finished[way_id] = 0;
            }

            // 输出块写完
            if(++ans_pointer == 7){
                write_ans_blk(&ans, buf, out_begin + out_count);
                out_count++;
                ans_pointer = 0;
            }
        }

        // 判断是否所有子集合都已处理完毕
        flag = 0;
        for(i = 0; i < way_num; i++){
            if(not_finished[i]) flag = 1;
        }
	}
	printf("第二阶段：各子集合间归并排序，最终结果写入磁盘[%d, %d]\n\n", out_begin, out_begin + len - 1);
	return;
}

void task2_tpmms(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("两阶段多路归并排序算法，将R和S分别排序\n");
    printf("-------------------------------------------------\n");

    init_buf(buf);
    // 关系R
    printf("---------------关系R---------------\n");
    // 第一阶段，对R的子集合(2个子集合*每个子集合8块)进行排序
    // 中间结果存储在blk 201-216中
    task2_stage1(buf, 1, 16, 201);
    // 第二阶段，子集合之间进行2路归并排序，最终结果存储在blk 301-316中
    task2_stage2(buf, 201, 16, 301);

    // 关系S
    printf("---------------关系S---------------\n");
    // 第一阶段，对S的子集合(4个子集合*每个子集合8块)进行排序，
    // 中间结果存储在blk 217-248中
    task2_stage1(buf, 17, 32, 217);
    // 第二阶段，子集合之间进行4路归并排序，最终结果存储在blk 317-348中
    task2_stage2(buf, 217, 32, 317);

    printf("-----------------------------------\n");
    printf("IO读写一共%ld次。\n\n\n", buf->numIO);
    return;
}

/* 对排好序的关系建立索引，对每个关系的第一个属性的不同值建立索引。
   begin为关系的起始块号，
   len为关系的块数，
   out_begin为建立好索引的关系的起始块号。
*/
int build_index(Buffer *buf, int begin, int len, int out_begin)
{
    init_buf(buf);
    Blk blk;  // 用于存储从磁盘中读出的blk中的数据
    Blk out;  // 输出索引块
    init_blk(&out);
    int out_pointer = 0;  // 输出索引块指针
    int last_X, cur_X;  // 上一个索引X，当前X
    last_X = cur_X = -1;
    int out_count = 0;  // 输出索引块数

    for(int i = begin; i < begin + len; i++){  // 读取每块
        blk = read_blk_data(i, buf);
        for(int j = 0; j < 7; j++){  // 读取每个元组
            cur_X = blk.X[j];
            if(cur_X != last_X){
                out.X[out_pointer] = cur_X;
                out.Y[out_pointer] = i;  // 块号
                out_pointer++;
                last_X = cur_X;
                // 输出索引块写满需要写回
                if(out_pointer == 7){
                    write_ans_blk(&out, buf, out_begin + out_count);
                    out_count++;
                    out_pointer = 0;
                }
            }
        }
    }
    // 最后一块输出索引块需要写回
    if(out_pointer < 7){
        write_ans_blk(&out, buf, out_begin + out_count);
        out_count++;
    }
    printf("建立索引，索引文件写入磁盘[%d, %d]\n", out_begin, out_begin + out_count - 1);
    printf("IO读写一共%ld次。\n\n", buf->numIO);
    return out_count;
}

/* 利用索引文件选出S.C=128的元组
   idx_begin为索引文件的起始块号，
   idx_len为索引文件的块数，
   ans_begin为输出结果的起始块号，
   c为待查找的数值，
   end为关系的结束块号。
*/
void index_select(Buffer *buf, int idx_begin, int idx_len, int ans_begin, int c, int end)
{
    init_buf(buf);
    Blk blk;  // 用于存储从磁盘中读出的blk中的数据
    Blk ans;  // 查找结果
    init_blk(&ans);
    int ans_pointer = 0;  // 结果块指针
    int cur_blk_id;  // 当前索引指向的块号
    int ans_blk_count = 0;  // 结果块数
    int ans_count = 0;  // 结果元组数
    int find = 0;  // 是否查找到

    int i, j;
    // 读取每块索引块
    for(i = idx_begin; i < idx_begin + idx_len; i++){
        blk = read_blk_data(i, buf);
        printf("读入索引块%d\n", i);
        for(j = 0; j < 7; j++){  // 读取每个元组
            if(blk.X[j] == c){
                cur_blk_id = blk.Y[j];
                find = 1;
                break;
            }
        }
        if(find) break;
    }
    if(!find){
        printf("未查找到该元组!\n");
        return;
    }

    int is_finished = 0;
    for(i = cur_blk_id; i <= end; i++){
        blk = read_blk_data(i, buf);
        printf("读入数据块%d\n", i);
        for(j = 0; j < 7; j++){
            if(blk.X[j] == c){
                ans.X[ans_pointer] = blk.X[j];
                ans.Y[ans_pointer] = blk.Y[j];
                printf("(X = %d, Y = %d)\n", blk.X[j], blk.Y[j]);
                ans_pointer++;
                ans_count++;
                // 结果块写满需要写回
                if(ans_pointer == 7){
                    write_ans_blk(&ans, buf, ans_begin + ans_blk_count);
                    ans_blk_count++;
                    ans_pointer = 0;
                }
            }
            if(blk.X[j] > c){
                is_finished = 1;
                break;
            }
        }
        if(is_finished) break;
    }
    // 最后一块结果块需要写回
    if(ans_pointer < 7){
        write_ans_blk(&ans, buf, ans_begin + ans_blk_count);
        ans_blk_count++;
    }
    printf("注：结果写入磁盘：[%d, %d]\n\n", ans_begin, ans_begin + ans_blk_count - 1);
    printf("满足选择条件的元组一共%d个\n\n", ans_count);
    printf("IO读写一共%ld次\n\n", buf->numIO);

    return;
}

void task3_indexSelect(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("基于索引的关系选择算法 S.C=128\n");
    printf("-------------------------------------------------\n");

    printf("---------------关系R---------------\n");
    // 对关系R建立索引，索引文件存储在blk 401-406中
    int r_len = build_index(buf, 301, 16, 401);

    printf("---------------关系S---------------\n");
    // 对关系S建立索引，索引文件存储在blk 407-412中
    int s_len = build_index(buf, 317, 32, 401 + r_len);

    printf("---------查找S.C=128的元组----------\n");
    index_select(buf, 401 + r_len, s_len, 120, 128, 348);

    return;
}

/* 判断是否读完，若未读完则读入新块
   count为关系指针，指向该关系遍历到的元组个数
   not_end标志是否有关系已遍历完
   blk_id_start表示关系的开始块数
   blk_id_end表示关系的结束块数
   blk用于存储从磁盘中读出的blk中的数据
 */
void check_and_read(Buffer *buf, int *count, int *not_end, int blk_id_start, int blk_id_end, Blk *blk)
{
    (*count)++;
    if(*count % 7 == 0) {
        if(blk_id_start + *count / 7 > blk_id_end) *not_end = 0;
        else *blk = read_blk_data(blk_id_start + *count / 7, buf);
    }
}

void task4_sortMergeJoin(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("基于排序的连接算法\n");
    printf("-------------------------------------------------\n");
    init_buf(buf);

    Blk blk_R[6], blk_S;  // 用于存储从磁盘中读出的blk中的数据
    Blk ans;  // 连接结果
    init_blk(&ans);
    int blk_id_R_start, blk_id_S_start, blk_id_R_end, blk_id_S_end;
    blk_id_R_start = 301;  // 关系R（有序）的起始块号
    blk_id_R_end = 316;  // 关系R（有序）的结束块号
    blk_id_S_start = 317;  // 关系S（有序）的起始块号
    blk_id_S_end = 348;  // 关系S（有序）的结束块号
    /* 遍历过程对于关系R需要两个指针，对于关系S需要一个指针，
    表示当前是该关系的第几个元组 */
    int count_R, count_R_new, count_S;
    count_R = count_R_new = count_S = 0;
    int pointer_ans = 0;  // 结果块指针
    int not_end = 1;  // 标志是否有关系已遍历完
    int count_ans = 0;  // 连接次数
    int count_ans_blk = 0;  // 结果块数
    int ans_begin = 501;  // 结果的起始块号
    int last_C = 0;

    blk_R[0] = read_blk_data(blk_id_R_start, buf);
    blk_S = read_blk_data(blk_id_S_start, buf);

    while(1){
        // 遍历两个关系，直到S.C = R.A
        if(blk_S.X[count_S % 7] != last_C) {
            while(not_end && blk_S.X[count_S % 7] != blk_R[0].X[count_R % 7]){
                while(not_end && blk_S.X[count_S % 7] < blk_R[0].X[count_R % 7])
                    check_and_read(buf, &count_S, &not_end, blk_id_S_start, blk_id_S_end, &blk_S);
                while(not_end && blk_S.X[count_S % 7] > blk_R[0].X[count_R % 7])
                    check_and_read(buf, &count_R, &not_end, blk_id_R_start, blk_id_R_end, &blk_R[0]);
            }
            if(!not_end) break;

            // 查找所有和S.C相等的R.A
            while(blk_R[0].X[count_R % 7] == blk_R[(count_R % 7 + count_R_new) / 7].X[(count_R + count_R_new) % 7]){
                count_R_new++;
                if((count_R + count_R_new) % 7 == 0) {
                    if(blk_id_R_start + (count_R + count_R_new) / 7 > blk_id_R_end) break;
                    else blk_R[(count_R % 7 + count_R_new) / 7] = read_blk_data (blk_id_R_start + (count_R + count_R_new) / 7, buf);
                }
            }
        }
        last_C = blk_S.X[count_S % 7];

        // 将S的当前元组和所有查找到的R中的元组连接并写回
        for(int i = 0; i < count_R_new; i++){
            ans.X[pointer_ans] = blk_S.X[count_S % 7];
            ans.Y[pointer_ans] = blk_S.Y[count_S % 7];
            pointer_ans++;
            ans.X[pointer_ans] = blk_R[(count_R % 7 + i) / 7].X[(count_R + i) % 7];
            ans.Y[pointer_ans] = blk_R[(count_R % 7 + i) / 7].Y[(count_R + i) % 7];
            pointer_ans++;
            count_ans++;
            if(pointer_ans == 6){  // 结果块写满需要写回
                write_ans_blk(&ans, buf, ans_begin + count_ans_blk);
                count_ans_blk++;
                pointer_ans = 0;
            }
        }
        check_and_read(buf, &count_S, &not_end, blk_id_S_start, blk_id_S_end, &blk_S);
        if(blk_S.X[count_S % 7] != last_C) count_R_new = 0;
    }
    // 最后一块也需要写回
    if(pointer_ans < 6){
        write_ans_blk(&ans, buf, ans_begin + count_ans_blk);
        count_ans_blk++;
    }
    printf("注：结果写入磁盘：[%d, %d]\n\n", ans_begin, ans_begin + count_ans_blk - 1);
    printf("总共连接%d次\n\n", count_ans);

    return;
}

// 若元组(X1, Y1)等于(X2, Y2)，则返回1；否则返回0
int equal(int X1, int Y1, int X2, int Y2)
{
    if(X1 == X2 && Y1 == Y2) return 1;
    else return 0;
}

/* 写入结果，并判断是否需要写回
   ans是结果块，blk中存储该关系的当前块，count计数当前元组是关系中的第几个元组，
   pointer_ans是结果块指针，count_ans是结果计数器，ans_begin是结果起始块号，
   count_ans_blk是结果块数计数器
   */
void write_and_check(Buffer *buf, Blk *ans, Blk *blk, int count, int *pointer_ans, int *count_ans, int ans_begin, int *count_ans_blk)
{
    ans->X[*pointer_ans] = blk->X[count % 7];
    ans->Y[*pointer_ans] = blk->Y[count % 7];
    (*count_ans)++;
    (*pointer_ans)++;
    if(*pointer_ans == 7){  // 结果块写满需要写回
        write_ans_blk(ans, buf, ans_begin + *count_ans_blk);
        (*count_ans_blk)++;
        *pointer_ans = 0;
    }
}

void task5_sortUnion(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("基于排序的集合并算法\n");
    printf("-------------------------------------------------\n");
    init_buf(buf);

    Blk blk_R, blk_S;  // 用于存储从磁盘中读出的blk中的数据
    Blk ans;  // 集合操作结果
    init_blk(&ans);
    int blk_id_R_start, blk_id_S_start, blk_id_R_end, blk_id_S_end;
    blk_id_R_start = 301;  // 关系R（有序）的起始块号
    blk_id_R_end = 316;  // 关系R（有序）的结束块号
    blk_id_S_start = 317;  // 关系S（有序）的起始块号
    blk_id_S_end = 348;  // 关系S（有序）的结束块号
    // 遍历过程对于关系R和S需要一个指针，表示当前是该关系的第几个元组
    int count_R, count_S;
    count_R = count_S = 0;
    int pointer_ans = 0;  // 结果块指针
    int not_end_R, not_end_S;
    not_end_R = not_end_S = 1;
    int count_ans = 0;  // 元组个数
    int count_ans_blk = 0;  // 结果块数
    int ans_begin = 701;  // 结果的起始块号

    blk_R = read_blk_data(blk_id_R_start, buf);
    blk_S = read_blk_data(blk_id_S_start, buf);

    while(1){
        while((not_end_R || not_end_S) && !equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            // 关系S未被读完，且R中的元组大于S中的元组，写入S中的元组并读入新的
            while(not_end_S && larger(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
                write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            }
            // 关系R未被读完，且S中的元组大于R中的元组，写入R中的元组并读入新的
            while(not_end_R && larger(blk_S.X[count_S % 7], blk_S.Y[count_S % 7], blk_R.X[count_R % 7], blk_R.Y[count_R % 7])){
                write_and_check(buf, &ans, &blk_R, count_R, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
            }
            // 只剩S未被读完
            if(not_end_S && !not_end_R){
                while(not_end_S){
                    write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                    check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
                }
                break;
            }
            // 只剩R未被读完
            if(not_end_R && !not_end_S){
                while(not_end_R){
                    write_and_check(buf, &ans, &blk_R, count_R, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                    check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
                }
                break;
            }
        }
        // 当S中的元组等于R中的元组，写入S中的元组，且R和S均读入新的
        if(equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
            check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
        }
        if(!not_end_R && !not_end_S) break;
    }
    // 最后一块也需要写回
    if(pointer_ans < 6){
        write_ans_blk(&ans, buf, ans_begin + count_ans_blk);
        count_ans_blk++;
    }
    printf("注：结果写入磁盘：[%d, %d]\n\n", ans_begin, ans_begin + count_ans_blk - 1);
    printf("R和S的并集有%d个元组\n\n", count_ans);
    printf("IO读写一共%ld次\n\n", buf->numIO);

    return;
}

void task5_sortIntersection(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("基于排序的集合交算法\n");
    printf("-------------------------------------------------\n");
    init_buf(buf);

    Blk blk_R, blk_S;  // 用于存储从磁盘中读出的blk中的数据
    Blk ans;  // 集合操作结果
    init_blk(&ans);
    int blk_id_R_start, blk_id_S_start, blk_id_R_end, blk_id_S_end;
    blk_id_R_start = 301;  // 关系R（有序）的起始块号
    blk_id_R_end = 316;  // 关系R（有序）的结束块号
    blk_id_S_start = 317;  // 关系S（有序）的起始块号
    blk_id_S_end = 348;  // 关系S（有序）的结束块号
    // 遍历过程对于关系R和S需要一个指针，表示当前是该关系的第几个元组
    int count_R, count_S;
    count_R = count_S = 0;
    int pointer_ans = 0;  // 结果块指针
    int not_end_R, not_end_S;
    not_end_R = not_end_S = 1;
    int count_ans = 0;  // 元组个数
    int count_ans_blk = 0;  // 结果块数
    int ans_begin = 801;  // 结果的起始块号

    blk_R = read_blk_data(blk_id_R_start, buf);
    blk_S = read_blk_data(blk_id_S_start, buf);

    while(1){
        while((not_end_R || not_end_S) && !equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            // 关系S未被读完，且R中的元组大于S中的元组，读入新的S中的元组
            while(not_end_S && larger(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7]))
                check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            // 关系R未被读完，且S中的元组大于R中的元组，读入新的R中的元组
            while(not_end_R && larger(blk_S.X[count_S % 7], blk_S.Y[count_S % 7], blk_R.X[count_R % 7], blk_R.Y[count_R % 7]))
                check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
            // 只剩S未被读完
            if(not_end_S && !not_end_R) {
                not_end_S = 0;
                break;
            }
            // 只剩R未被读完
            if(not_end_R && !not_end_S) {
                not_end_R = 0;
                break;
            }
        }
        // 当S中的元组等于R中的元组，写入S中的元组，且R和S均读入新的
        if(equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            printf("(X=%d, Y=%d)\n", blk_R.X[count_R % 7], blk_R.Y[count_R % 7]);
            write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
            check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
        }
        if(!not_end_R && !not_end_S) break;
    }
    // 最后一块也需要写回
    if(pointer_ans < 7){
        write_ans_blk(&ans, buf, ans_begin + count_ans_blk);
        count_ans_blk++;
    }
    printf("注：结果写入磁盘：[%d, %d]\n\n", ans_begin, ans_begin + count_ans_blk - 1);
    printf("R和S的交集有%d个元组\n\n", count_ans);
    printf("IO读写一共%ld次\n\n", buf->numIO);

    return;
}

void task5_sortMinus(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("基于排序的集合差算法\n");
    printf("-------------------------------------------------\n");
    init_buf(buf);

    Blk blk_R, blk_S;  // 用于存储从磁盘中读出的blk中的数据
    Blk ans;  // 集合操作结果
    init_blk(&ans);
    int blk_id_R_start, blk_id_S_start, blk_id_R_end, blk_id_S_end;
    blk_id_R_start = 301;  // 关系R（有序）的起始块号
    blk_id_R_end = 316;  // 关系R（有序）的结束块号
    blk_id_S_start = 317;  // 关系S（有序）的起始块号
    blk_id_S_end = 348;  // 关系S（有序）的结束块号
    // 遍历过程对于关系R和S需要一个指针，表示当前是该关系的第几个元组
    int count_R, count_S;
    count_R = count_S = 0;
    int pointer_ans = 0;  // 结果块指针
    int not_end_R, not_end_S;
    not_end_R = not_end_S = 1;
    int count_ans = 0;  // 元组个数
    int count_ans_blk = 0;  // 结果块数
    int ans_begin = 901;  // 结果的起始块号

    blk_R = read_blk_data(blk_id_R_start, buf);
    blk_S = read_blk_data(blk_id_S_start, buf);

    while(1){
        while((not_end_R || not_end_S) && !equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            // 关系S未被读完，且R中的元组大于S中的元组，写入S中的元组并读入新的
            while(not_end_S && larger(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
                write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            }
            // 关系R未被读完，且S中的元组大于R中的元组，读入新的R中的元组
            while(not_end_R && larger(blk_S.X[count_S % 7], blk_S.Y[count_S % 7], blk_R.X[count_R % 7], blk_R.Y[count_R % 7]))
                check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
            // 只剩S未被读完
            if(not_end_S && !not_end_R) {
                while(not_end_S){
                    write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                    check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
                }
                break;
            }
            // 只剩R未被读完
            if(not_end_R && !not_end_S) {
                not_end_R = 0;
                break;
            }
        }
        // 当S中的元组等于R中的元组，R和S均读入新的
        if(equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
        }
        if(!not_end_R && !not_end_S) break;
    }
    // 最后一块也需要写回
    if(pointer_ans < 7){
        write_ans_blk(&ans, buf, ans_begin + count_ans_blk);
        count_ans_blk++;
    }
    printf("注：结果写入磁盘：[%d, %d]\n\n", ans_begin, ans_begin + count_ans_blk - 1);
    printf("S和R的差集(S-R)有%d个元组\n\n", count_ans);
    printf("IO读写一共%ld次\n\n", buf->numIO);

    return;
}

int main(int argc, char **argv)
{
    Buffer buf; /* A buffer */

    // 任务一
    task1_linearSearch(&buf);

    // 任务二
    task2_tpmms(&buf);

    // 任务三
    task3_indexSelect(&buf);

    // 任务四
    task4_sortMergeJoin(&buf);

    // 任务五
    task5_sortUnion(&buf);  // 并
    task5_sortIntersection(&buf);  // 交
    task5_sortMinus(&buf);  // 差
}

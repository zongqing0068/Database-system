#include <stdlib.h>
#include <stdio.h>
#include "extmem.h"

// �洢ÿ�����̿������
typedef struct Blk
{
	int X[7];
    int Y[7];
    int addr;
}Blk;

// ��ʼ��buf
int init_buf(Buffer *buf)
{
    if (!initBuffer(520, 64, buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    return 0;
}

// ��ʼ��blk
void init_blk(Blk *blk)
{
	for(int i=0; i<7; i++){
		blk -> X[i] = -1;
		blk -> Y[i] = -1;
	}
	blk -> addr = -1;
}

// ����ÿ�����̿�����ݣ�7��Ԫ���1����̴��̿��ַ��
void read_blk(unsigned char *blk_d, Blk *blk)
{
	char str[5];
	int i;
	for (i = 0; i < 7; i++) // ��ȡ7��Ԫ��
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
    for (int k = 0; k < 4; k++)  // ��ȡ��̴��̿��ַ
    {
        str[k] = *(blk_d + i*8 + k);
    }
    blk->addr = atoi(str);
}

// ��װ��ʼ���顢��ȡ�顢�����顢�ͷſ�Ĵ���
Blk read_blk_data(int blk_id, Buffer *buf)
{
    unsigned char *blk_d;  // ���ڴ洢�Ӵ����ж�����blk
    Blk blk;  // ���ڴ洢�Ӵ����ж�����blk�е�����
	init_blk(&blk);  // ��ʼ����
    if ((blk_d = readBlockFromDisk(blk_id, buf)) == NULL)  // ��ȡ��
    {
        perror("Reading Block Failed!\n");
    }
    read_blk(blk_d, &blk);  // ������
    freeBlockInBuffer(blk_d, buf);  // �ͷſ�
    return blk;
}

// ��ĳ�����̿������д��buffer��7��Ԫ���1����̴��̿��ַ��
void write_blk(unsigned char *blk_d, Blk blk)
{
	char str[5];
	int i;
	for (i = 0; i < 7; i++)  // д7��Ԫ��
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
    for (int k = 0; k < 4; k++)  // д��̴��̿��ַ
    {
        *(blk_d + i*8 + k) = str[k];
	}
}

// ��װ�����顢д�ؿ顢��տ顢�ͷſ�Ĵ���
void write_ans_blk(Blk *ans, Buffer *buf, int blk_id)
{
    ans->addr = blk_id + 1;
    unsigned char *ans_d;  // ���ڴ洢��д����̵Ľ��
    ans_d = getNewBlockInBuffer(buf);  // ��ȡ�������еĿ��п�
    write_blk(ans_d, *ans);  // ���������
    if (writeBlockToDisk(ans_d, blk_id, buf) != 0)  // д�ش���
    {
        perror("Writing Block Failed!\n");
        return;
    }
    init_blk(ans);  // ���м��������
    freeBlockInBuffer(ans_d, buf);  // �ͷ�buf�еĽ����
}

void task1_linearSearch(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("�������������Ĺ�ϵѡ���㷨 S.C=128\n");
    printf("-------------------------------------------------\n");

    init_buf(buf);
    Blk blk;  // ���ڴ洢�Ӵ����ж�����blk�е�����
	Blk ans;  // ���ڴ洢���
	init_blk(&ans);
	int count = 0;	// ����S.C=128��Ԫ�����
	int blk_id_begin = 100;  // ��������ʼ���
	int blk_num = 0;  // ����������


	// ����S.C=128������Ԫ��
	for(int blk_id = 17; blk_id <= 48; blk_id++){
		blk = read_blk_data(blk_id, buf);
		printf("�������ݿ�%d\n", blk_id);
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
        printf("δ���ҵ���Ԫ�飡\n");
        return;
	}

	printf("ע�����д����̣�[%d, %d]\n\n", blk_id_begin, blk_id_begin + blk_num - 1);
	printf("����ѡ��������Ԫ��һ��%d����\n\n", count);
	printf("IO��дһ��%ld�Ρ�\n\n\n", buf->numIO);
	return;
}

// ��Ԫ��(X1, Y1)����(X2, Y2)���򷵻�1�����򷵻�0
int larger(int X1, int Y1, int X2, int Y2)
{
    if(X1 > X2) return 1;
    else if(X1 == X2 && Y1 > Y2) return 1;
    else return 0;
}

// ����blk1�е�i��Ԫ���blk2�е�j��Ԫ��
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

/* �Թ�ϵ���Ӽ��Ͻ������򣬰���һ�����Դ�С����Ȼ��ڶ������Դ�С��������
   beginΪ�������ϵ����ʼ��ţ�
   lenΪ�������ϵ�Ŀ�����
   out_beginΪ�洢���Ӽ����ź���Ĺ�ϵ����ʼ��š�
*/
void task2_stage1(Buffer *buf, int begin, int len, int out_begin)
{
    int set_blk_num = 8;  // ÿ���Ӽ���8��blk
    Blk blk[set_blk_num];  // ���ڴ洢�Ӵ����ж�����blk�е�����
    int count = 0;  // ���������
    while(len > count){
        // ��ȡÿ���Ӽ��ϵ�����
        int i = 0;
        while(len > count && i < set_blk_num){
            blk[i] = read_blk_data(begin + count, buf);
            count++;
            i++;
        }
        // ð������
        for(int j = 0; j < 7 * i; j++){
            for(int k = j + 1; k < 7 * i; k++){
                if(larger(blk[j/7].X[j%7], blk[j/7].Y[j%7], blk[k/7].X[k%7], blk[k/7].Y[k%7]))
                    swap(&blk[j/7], j%7, &blk[k/7], k%7);
            }
        }
        for(int j = 0; j < i; j++)
            write_ans_blk(&blk[j], buf, out_begin + count - i + j);
    }
    printf("��һ�׶Σ������Ӽ��ϲ����Ӽ����������м���д�����[%d, %d]\n", out_begin, out_begin + len - 1);
    return;
}

// ��ȡ��С��Ԫ�鼰������·��
int get_min(int X[], int Y[], int *minX, int *minY, int way_num, int not_finished[])
{
    *minX = 1000;
    *minY = 1000;
    int way_id;
    for (int i = 0; i < way_num ; i ++ ) {
        // ��ǰ·��δ�����꣬�ҵ�ǰ·��Ԫ���С
        if (not_finished[i] && larger(*minX, *minY, X[i], Y[i])) {
            *minX = X[i];
            *minY = Y[i];
            way_id = i;
        }
    }
    return way_id;
}

/* ���Ӽ��Ͻ��й鲢���򣬰���һ�����Դ�С����Ȼ��ڶ������Դ�С��������
   beginΪ�������ϵ����ʼ��ţ�
   lenΪ�������ϵ�Ŀ�����
   out_beginΪ�ź���Ĺ�ϵ����ʼ��š�
*/
void task2_stage2(Buffer *buf, int begin, int len, int out_begin)
{
    int set_blk_num = 8;  // ÿ���Ӽ���8��blk
    int way_num = len/set_blk_num;  // way_num·�鲢����
    Blk blk[way_num];  // ���ڴ洢�Ӵ����ж�����blk�е�����
    int count[way_num];  // ��¼��·Ŀǰ����Ŀ���
    int out_count = 0;  // ��¼������Ŀ���
    int pointer[way_num];  // ��¼��ǰ�������λ��
    Blk ans;  // �����
    init_blk(&ans);
    int ans_pointer = 0;  // �����ָ��
    int not_finished[way_num];  // �ж�ĳ·�Ƿ��Ѿ��������
	int i;
	// ��ȡ��·�ĵ�һ��
	for (i = 0; i < way_num; i++){
        blk[i] = read_blk_data(begin + i * set_blk_num, buf);
        count[i] = 1;
        pointer[i] = 0;
        not_finished[i] = 1;
	}
	int flag = 1;  // ��־�Ƿ�����·������
	while(flag){
		for (i = 0; i < way_num; i++){
            //ĳ·�Ŀ�����ˣ���Ҫ�����µĿ�
			if(pointer[i] == 7){
				if (count[i] < set_blk_num){
                    blk[i] = read_blk_data(begin + i * set_blk_num + count[i], buf);
					pointer[i] = 0;
				}
				count[i]++;
			}
            // �ж�ĳ·�Ƿ��Ѿ��������
			if(count[i] > set_blk_num)
				not_finished[i] = 0;
		}

        int X[way_num], Y[way_num];
        int minX, minY, way_id;
        int cur_not_finished = 1;  // �жϵ�ǰ���Ƿ���������Ƿ��п���꣩
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

            // ��һ�����
            if(++pointer[way_id] == 7){
                cur_not_finished = 0;
                if(count[way_id] == set_blk_num)  not_finished[way_id] = 0;
            }

            // �����д��
            if(++ans_pointer == 7){
                write_ans_blk(&ans, buf, out_begin + out_count);
                out_count++;
                ans_pointer = 0;
            }
        }

        // �ж��Ƿ������Ӽ��϶��Ѵ������
        flag = 0;
        for(i = 0; i < way_num; i++){
            if(not_finished[i]) flag = 1;
        }
	}
	printf("�ڶ��׶Σ����Ӽ��ϼ�鲢�������ս��д�����[%d, %d]\n\n", out_begin, out_begin + len - 1);
	return;
}

void task2_tpmms(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("���׶ζ�·�鲢�����㷨����R��S�ֱ�����\n");
    printf("-------------------------------------------------\n");

    init_buf(buf);
    // ��ϵR
    printf("---------------��ϵR---------------\n");
    // ��һ�׶Σ���R���Ӽ���(2���Ӽ���*ÿ���Ӽ���8��)��������
    // �м����洢��blk 201-216��
    task2_stage1(buf, 1, 16, 201);
    // �ڶ��׶Σ��Ӽ���֮�����2·�鲢�������ս���洢��blk 301-316��
    task2_stage2(buf, 201, 16, 301);

    // ��ϵS
    printf("---------------��ϵS---------------\n");
    // ��һ�׶Σ���S���Ӽ���(4���Ӽ���*ÿ���Ӽ���8��)��������
    // �м����洢��blk 217-248��
    task2_stage1(buf, 17, 32, 217);
    // �ڶ��׶Σ��Ӽ���֮�����4·�鲢�������ս���洢��blk 317-348��
    task2_stage2(buf, 217, 32, 317);

    printf("-----------------------------------\n");
    printf("IO��дһ��%ld�Ρ�\n\n\n", buf->numIO);
    return;
}

/* ���ź���Ĺ�ϵ������������ÿ����ϵ�ĵ�һ�����ԵĲ�ֵͬ����������
   beginΪ��ϵ����ʼ��ţ�
   lenΪ��ϵ�Ŀ�����
   out_beginΪ�����������Ĺ�ϵ����ʼ��š�
*/
int build_index(Buffer *buf, int begin, int len, int out_begin)
{
    init_buf(buf);
    Blk blk;  // ���ڴ洢�Ӵ����ж�����blk�е�����
    Blk out;  // ���������
    init_blk(&out);
    int out_pointer = 0;  // ���������ָ��
    int last_X, cur_X;  // ��һ������X����ǰX
    last_X = cur_X = -1;
    int out_count = 0;  // �����������

    for(int i = begin; i < begin + len; i++){  // ��ȡÿ��
        blk = read_blk_data(i, buf);
        for(int j = 0; j < 7; j++){  // ��ȡÿ��Ԫ��
            cur_X = blk.X[j];
            if(cur_X != last_X){
                out.X[out_pointer] = cur_X;
                out.Y[out_pointer] = i;  // ���
                out_pointer++;
                last_X = cur_X;
                // ���������д����Ҫд��
                if(out_pointer == 7){
                    write_ans_blk(&out, buf, out_begin + out_count);
                    out_count++;
                    out_pointer = 0;
                }
            }
        }
    }
    // ���һ�������������Ҫд��
    if(out_pointer < 7){
        write_ans_blk(&out, buf, out_begin + out_count);
        out_count++;
    }
    printf("���������������ļ�д�����[%d, %d]\n", out_begin, out_begin + out_count - 1);
    printf("IO��дһ��%ld�Ρ�\n\n", buf->numIO);
    return out_count;
}

/* ���������ļ�ѡ��S.C=128��Ԫ��
   idx_beginΪ�����ļ�����ʼ��ţ�
   idx_lenΪ�����ļ��Ŀ�����
   ans_beginΪ����������ʼ��ţ�
   cΪ�����ҵ���ֵ��
   endΪ��ϵ�Ľ�����š�
*/
void index_select(Buffer *buf, int idx_begin, int idx_len, int ans_begin, int c, int end)
{
    init_buf(buf);
    Blk blk;  // ���ڴ洢�Ӵ����ж�����blk�е�����
    Blk ans;  // ���ҽ��
    init_blk(&ans);
    int ans_pointer = 0;  // �����ָ��
    int cur_blk_id;  // ��ǰ����ָ��Ŀ��
    int ans_blk_count = 0;  // �������
    int ans_count = 0;  // ���Ԫ����
    int find = 0;  // �Ƿ���ҵ�

    int i, j;
    // ��ȡÿ��������
    for(i = idx_begin; i < idx_begin + idx_len; i++){
        blk = read_blk_data(i, buf);
        printf("����������%d\n", i);
        for(j = 0; j < 7; j++){  // ��ȡÿ��Ԫ��
            if(blk.X[j] == c){
                cur_blk_id = blk.Y[j];
                find = 1;
                break;
            }
        }
        if(find) break;
    }
    if(!find){
        printf("δ���ҵ���Ԫ��!\n");
        return;
    }

    int is_finished = 0;
    for(i = cur_blk_id; i <= end; i++){
        blk = read_blk_data(i, buf);
        printf("�������ݿ�%d\n", i);
        for(j = 0; j < 7; j++){
            if(blk.X[j] == c){
                ans.X[ans_pointer] = blk.X[j];
                ans.Y[ans_pointer] = blk.Y[j];
                printf("(X = %d, Y = %d)\n", blk.X[j], blk.Y[j]);
                ans_pointer++;
                ans_count++;
                // �����д����Ҫд��
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
    // ���һ��������Ҫд��
    if(ans_pointer < 7){
        write_ans_blk(&ans, buf, ans_begin + ans_blk_count);
        ans_blk_count++;
    }
    printf("ע�����д����̣�[%d, %d]\n\n", ans_begin, ans_begin + ans_blk_count - 1);
    printf("����ѡ��������Ԫ��һ��%d��\n\n", ans_count);
    printf("IO��дһ��%ld��\n\n", buf->numIO);

    return;
}

void task3_indexSelect(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("���������Ĺ�ϵѡ���㷨 S.C=128\n");
    printf("-------------------------------------------------\n");

    printf("---------------��ϵR---------------\n");
    // �Թ�ϵR���������������ļ��洢��blk 401-406��
    int r_len = build_index(buf, 301, 16, 401);

    printf("---------------��ϵS---------------\n");
    // �Թ�ϵS���������������ļ��洢��blk 407-412��
    int s_len = build_index(buf, 317, 32, 401 + r_len);

    printf("---------����S.C=128��Ԫ��----------\n");
    index_select(buf, 401 + r_len, s_len, 120, 128, 348);

    return;
}

/* �ж��Ƿ���꣬��δ����������¿�
   countΪ��ϵָ�룬ָ��ù�ϵ��������Ԫ�����
   not_end��־�Ƿ��й�ϵ�ѱ�����
   blk_id_start��ʾ��ϵ�Ŀ�ʼ����
   blk_id_end��ʾ��ϵ�Ľ�������
   blk���ڴ洢�Ӵ����ж�����blk�е�����
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
    printf("��������������㷨\n");
    printf("-------------------------------------------------\n");
    init_buf(buf);

    Blk blk_R[6], blk_S;  // ���ڴ洢�Ӵ����ж�����blk�е�����
    Blk ans;  // ���ӽ��
    init_blk(&ans);
    int blk_id_R_start, blk_id_S_start, blk_id_R_end, blk_id_S_end;
    blk_id_R_start = 301;  // ��ϵR�����򣩵���ʼ���
    blk_id_R_end = 316;  // ��ϵR�����򣩵Ľ������
    blk_id_S_start = 317;  // ��ϵS�����򣩵���ʼ���
    blk_id_S_end = 348;  // ��ϵS�����򣩵Ľ������
    /* �������̶��ڹ�ϵR��Ҫ����ָ�룬���ڹ�ϵS��Ҫһ��ָ�룬
    ��ʾ��ǰ�Ǹù�ϵ�ĵڼ���Ԫ�� */
    int count_R, count_R_new, count_S;
    count_R = count_R_new = count_S = 0;
    int pointer_ans = 0;  // �����ָ��
    int not_end = 1;  // ��־�Ƿ��й�ϵ�ѱ�����
    int count_ans = 0;  // ���Ӵ���
    int count_ans_blk = 0;  // �������
    int ans_begin = 501;  // �������ʼ���
    int last_C = 0;

    blk_R[0] = read_blk_data(blk_id_R_start, buf);
    blk_S = read_blk_data(blk_id_S_start, buf);

    while(1){
        // ����������ϵ��ֱ��S.C = R.A
        if(blk_S.X[count_S % 7] != last_C) {
            while(not_end && blk_S.X[count_S % 7] != blk_R[0].X[count_R % 7]){
                while(not_end && blk_S.X[count_S % 7] < blk_R[0].X[count_R % 7])
                    check_and_read(buf, &count_S, &not_end, blk_id_S_start, blk_id_S_end, &blk_S);
                while(not_end && blk_S.X[count_S % 7] > blk_R[0].X[count_R % 7])
                    check_and_read(buf, &count_R, &not_end, blk_id_R_start, blk_id_R_end, &blk_R[0]);
            }
            if(!not_end) break;

            // �������к�S.C��ȵ�R.A
            while(blk_R[0].X[count_R % 7] == blk_R[(count_R % 7 + count_R_new) / 7].X[(count_R + count_R_new) % 7]){
                count_R_new++;
                if((count_R + count_R_new) % 7 == 0) {
                    if(blk_id_R_start + (count_R + count_R_new) / 7 > blk_id_R_end) break;
                    else blk_R[(count_R % 7 + count_R_new) / 7] = read_blk_data (blk_id_R_start + (count_R + count_R_new) / 7, buf);
                }
            }
        }
        last_C = blk_S.X[count_S % 7];

        // ��S�ĵ�ǰԪ������в��ҵ���R�е�Ԫ�����Ӳ�д��
        for(int i = 0; i < count_R_new; i++){
            ans.X[pointer_ans] = blk_S.X[count_S % 7];
            ans.Y[pointer_ans] = blk_S.Y[count_S % 7];
            pointer_ans++;
            ans.X[pointer_ans] = blk_R[(count_R % 7 + i) / 7].X[(count_R + i) % 7];
            ans.Y[pointer_ans] = blk_R[(count_R % 7 + i) / 7].Y[(count_R + i) % 7];
            pointer_ans++;
            count_ans++;
            if(pointer_ans == 6){  // �����д����Ҫд��
                write_ans_blk(&ans, buf, ans_begin + count_ans_blk);
                count_ans_blk++;
                pointer_ans = 0;
            }
        }
        check_and_read(buf, &count_S, &not_end, blk_id_S_start, blk_id_S_end, &blk_S);
        if(blk_S.X[count_S % 7] != last_C) count_R_new = 0;
    }
    // ���һ��Ҳ��Ҫд��
    if(pointer_ans < 6){
        write_ans_blk(&ans, buf, ans_begin + count_ans_blk);
        count_ans_blk++;
    }
    printf("ע�����д����̣�[%d, %d]\n\n", ans_begin, ans_begin + count_ans_blk - 1);
    printf("�ܹ�����%d��\n\n", count_ans);

    return;
}

// ��Ԫ��(X1, Y1)����(X2, Y2)���򷵻�1�����򷵻�0
int equal(int X1, int Y1, int X2, int Y2)
{
    if(X1 == X2 && Y1 == Y2) return 1;
    else return 0;
}

/* д���������ж��Ƿ���Ҫд��
   ans�ǽ���飬blk�д洢�ù�ϵ�ĵ�ǰ�飬count������ǰԪ���ǹ�ϵ�еĵڼ���Ԫ�飬
   pointer_ans�ǽ����ָ�룬count_ans�ǽ����������ans_begin�ǽ����ʼ��ţ�
   count_ans_blk�ǽ������������
   */
void write_and_check(Buffer *buf, Blk *ans, Blk *blk, int count, int *pointer_ans, int *count_ans, int ans_begin, int *count_ans_blk)
{
    ans->X[*pointer_ans] = blk->X[count % 7];
    ans->Y[*pointer_ans] = blk->Y[count % 7];
    (*count_ans)++;
    (*pointer_ans)++;
    if(*pointer_ans == 7){  // �����д����Ҫд��
        write_ans_blk(ans, buf, ans_begin + *count_ans_blk);
        (*count_ans_blk)++;
        *pointer_ans = 0;
    }
}

void task5_sortUnion(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("��������ļ��ϲ��㷨\n");
    printf("-------------------------------------------------\n");
    init_buf(buf);

    Blk blk_R, blk_S;  // ���ڴ洢�Ӵ����ж�����blk�е�����
    Blk ans;  // ���ϲ������
    init_blk(&ans);
    int blk_id_R_start, blk_id_S_start, blk_id_R_end, blk_id_S_end;
    blk_id_R_start = 301;  // ��ϵR�����򣩵���ʼ���
    blk_id_R_end = 316;  // ��ϵR�����򣩵Ľ������
    blk_id_S_start = 317;  // ��ϵS�����򣩵���ʼ���
    blk_id_S_end = 348;  // ��ϵS�����򣩵Ľ������
    // �������̶��ڹ�ϵR��S��Ҫһ��ָ�룬��ʾ��ǰ�Ǹù�ϵ�ĵڼ���Ԫ��
    int count_R, count_S;
    count_R = count_S = 0;
    int pointer_ans = 0;  // �����ָ��
    int not_end_R, not_end_S;
    not_end_R = not_end_S = 1;
    int count_ans = 0;  // Ԫ�����
    int count_ans_blk = 0;  // �������
    int ans_begin = 701;  // �������ʼ���

    blk_R = read_blk_data(blk_id_R_start, buf);
    blk_S = read_blk_data(blk_id_S_start, buf);

    while(1){
        while((not_end_R || not_end_S) && !equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            // ��ϵSδ�����꣬��R�е�Ԫ�����S�е�Ԫ�飬д��S�е�Ԫ�鲢�����µ�
            while(not_end_S && larger(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
                write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            }
            // ��ϵRδ�����꣬��S�е�Ԫ�����R�е�Ԫ�飬д��R�е�Ԫ�鲢�����µ�
            while(not_end_R && larger(blk_S.X[count_S % 7], blk_S.Y[count_S % 7], blk_R.X[count_R % 7], blk_R.Y[count_R % 7])){
                write_and_check(buf, &ans, &blk_R, count_R, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
            }
            // ֻʣSδ������
            if(not_end_S && !not_end_R){
                while(not_end_S){
                    write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                    check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
                }
                break;
            }
            // ֻʣRδ������
            if(not_end_R && !not_end_S){
                while(not_end_R){
                    write_and_check(buf, &ans, &blk_R, count_R, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                    check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
                }
                break;
            }
        }
        // ��S�е�Ԫ�����R�е�Ԫ�飬д��S�е�Ԫ�飬��R��S�������µ�
        if(equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
            check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
        }
        if(!not_end_R && !not_end_S) break;
    }
    // ���һ��Ҳ��Ҫд��
    if(pointer_ans < 6){
        write_ans_blk(&ans, buf, ans_begin + count_ans_blk);
        count_ans_blk++;
    }
    printf("ע�����д����̣�[%d, %d]\n\n", ans_begin, ans_begin + count_ans_blk - 1);
    printf("R��S�Ĳ�����%d��Ԫ��\n\n", count_ans);
    printf("IO��дһ��%ld��\n\n", buf->numIO);

    return;
}

void task5_sortIntersection(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("��������ļ��Ͻ��㷨\n");
    printf("-------------------------------------------------\n");
    init_buf(buf);

    Blk blk_R, blk_S;  // ���ڴ洢�Ӵ����ж�����blk�е�����
    Blk ans;  // ���ϲ������
    init_blk(&ans);
    int blk_id_R_start, blk_id_S_start, blk_id_R_end, blk_id_S_end;
    blk_id_R_start = 301;  // ��ϵR�����򣩵���ʼ���
    blk_id_R_end = 316;  // ��ϵR�����򣩵Ľ������
    blk_id_S_start = 317;  // ��ϵS�����򣩵���ʼ���
    blk_id_S_end = 348;  // ��ϵS�����򣩵Ľ������
    // �������̶��ڹ�ϵR��S��Ҫһ��ָ�룬��ʾ��ǰ�Ǹù�ϵ�ĵڼ���Ԫ��
    int count_R, count_S;
    count_R = count_S = 0;
    int pointer_ans = 0;  // �����ָ��
    int not_end_R, not_end_S;
    not_end_R = not_end_S = 1;
    int count_ans = 0;  // Ԫ�����
    int count_ans_blk = 0;  // �������
    int ans_begin = 801;  // �������ʼ���

    blk_R = read_blk_data(blk_id_R_start, buf);
    blk_S = read_blk_data(blk_id_S_start, buf);

    while(1){
        while((not_end_R || not_end_S) && !equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            // ��ϵSδ�����꣬��R�е�Ԫ�����S�е�Ԫ�飬�����µ�S�е�Ԫ��
            while(not_end_S && larger(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7]))
                check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            // ��ϵRδ�����꣬��S�е�Ԫ�����R�е�Ԫ�飬�����µ�R�е�Ԫ��
            while(not_end_R && larger(blk_S.X[count_S % 7], blk_S.Y[count_S % 7], blk_R.X[count_R % 7], blk_R.Y[count_R % 7]))
                check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
            // ֻʣSδ������
            if(not_end_S && !not_end_R) {
                not_end_S = 0;
                break;
            }
            // ֻʣRδ������
            if(not_end_R && !not_end_S) {
                not_end_R = 0;
                break;
            }
        }
        // ��S�е�Ԫ�����R�е�Ԫ�飬д��S�е�Ԫ�飬��R��S�������µ�
        if(equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            printf("(X=%d, Y=%d)\n", blk_R.X[count_R % 7], blk_R.Y[count_R % 7]);
            write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
            check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
        }
        if(!not_end_R && !not_end_S) break;
    }
    // ���һ��Ҳ��Ҫд��
    if(pointer_ans < 7){
        write_ans_blk(&ans, buf, ans_begin + count_ans_blk);
        count_ans_blk++;
    }
    printf("ע�����д����̣�[%d, %d]\n\n", ans_begin, ans_begin + count_ans_blk - 1);
    printf("R��S�Ľ�����%d��Ԫ��\n\n", count_ans);
    printf("IO��дһ��%ld��\n\n", buf->numIO);

    return;
}

void task5_sortMinus(Buffer *buf)
{
    printf("-------------------------------------------------\n");
    printf("��������ļ��ϲ��㷨\n");
    printf("-------------------------------------------------\n");
    init_buf(buf);

    Blk blk_R, blk_S;  // ���ڴ洢�Ӵ����ж�����blk�е�����
    Blk ans;  // ���ϲ������
    init_blk(&ans);
    int blk_id_R_start, blk_id_S_start, blk_id_R_end, blk_id_S_end;
    blk_id_R_start = 301;  // ��ϵR�����򣩵���ʼ���
    blk_id_R_end = 316;  // ��ϵR�����򣩵Ľ������
    blk_id_S_start = 317;  // ��ϵS�����򣩵���ʼ���
    blk_id_S_end = 348;  // ��ϵS�����򣩵Ľ������
    // �������̶��ڹ�ϵR��S��Ҫһ��ָ�룬��ʾ��ǰ�Ǹù�ϵ�ĵڼ���Ԫ��
    int count_R, count_S;
    count_R = count_S = 0;
    int pointer_ans = 0;  // �����ָ��
    int not_end_R, not_end_S;
    not_end_R = not_end_S = 1;
    int count_ans = 0;  // Ԫ�����
    int count_ans_blk = 0;  // �������
    int ans_begin = 901;  // �������ʼ���

    blk_R = read_blk_data(blk_id_R_start, buf);
    blk_S = read_blk_data(blk_id_S_start, buf);

    while(1){
        while((not_end_R || not_end_S) && !equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            // ��ϵSδ�����꣬��R�е�Ԫ�����S�е�Ԫ�飬д��S�е�Ԫ�鲢�����µ�
            while(not_end_S && larger(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
                write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            }
            // ��ϵRδ�����꣬��S�е�Ԫ�����R�е�Ԫ�飬�����µ�R�е�Ԫ��
            while(not_end_R && larger(blk_S.X[count_S % 7], blk_S.Y[count_S % 7], blk_R.X[count_R % 7], blk_R.Y[count_R % 7]))
                check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
            // ֻʣSδ������
            if(not_end_S && !not_end_R) {
                while(not_end_S){
                    write_and_check(buf, &ans, &blk_S, count_S, &pointer_ans, &count_ans, ans_begin, &count_ans_blk);
                    check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
                }
                break;
            }
            // ֻʣRδ������
            if(not_end_R && !not_end_S) {
                not_end_R = 0;
                break;
            }
        }
        // ��S�е�Ԫ�����R�е�Ԫ�飬R��S�������µ�
        if(equal(blk_R.X[count_R % 7], blk_R.Y[count_R % 7], blk_S.X[count_S % 7], blk_S.Y[count_S % 7])){
            check_and_read(buf, &count_S, &not_end_S, blk_id_S_start, blk_id_S_end, &blk_S);
            check_and_read(buf, &count_R, &not_end_R, blk_id_R_start, blk_id_R_end, &blk_R);
        }
        if(!not_end_R && !not_end_S) break;
    }
    // ���һ��Ҳ��Ҫд��
    if(pointer_ans < 7){
        write_ans_blk(&ans, buf, ans_begin + count_ans_blk);
        count_ans_blk++;
    }
    printf("ע�����д����̣�[%d, %d]\n\n", ans_begin, ans_begin + count_ans_blk - 1);
    printf("S��R�Ĳ(S-R)��%d��Ԫ��\n\n", count_ans);
    printf("IO��дһ��%ld��\n\n", buf->numIO);

    return;
}

int main(int argc, char **argv)
{
    Buffer buf; /* A buffer */

    // ����һ
    task1_linearSearch(&buf);

    // �����
    task2_tpmms(&buf);

    // ������
    task3_indexSelect(&buf);

    // ������
    task4_sortMergeJoin(&buf);

    // ������
    task5_sortUnion(&buf);  // ��
    task5_sortIntersection(&buf);  // ��
    task5_sortMinus(&buf);  // ��
}

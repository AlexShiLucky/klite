/* 
* ����: �����ʱ��
*       �ɼ򻯶�ʱ����Ĵ����д
* ������<kerndev@foxmail.com>
*/
#include "kernel.h"
#include "timer.h"

struct timer
{
	int id;
	int elapse;
	void(*callback)(int);
	kmutex_t mutex;
	kthread_t thread;
};

static uint32_t timer_stk_size;
static uint32_t timer_thread_prio;

//��ʱ���߳�
//ѭ�����ö�ʱ���ص�����
static void timer_thread_entry(void* arg)
{
	struct timer* tmr;
	tmr = (struct timer*)arg;
	while(1)
	{
		kthread_sleep(tmr->elapse);
		kmutex_lock(tmr->mutex);
		tmr->callback(tmr->id);
		kmutex_unlock(tmr->mutex);
	}
}

//��ʼ����ʱ��ģ��
//���ö�ʱ���̵߳Ķ�ջ��С���߳����ȼ�
void timer_init(uint32_t stk_size, int prio)
{
	timer_stk_size = stk_size;
	timer_thread_prio = prio;
}

//������ʱ��
timer_t timer_create(int id, int elapse, void(*callback)(int))
{
	struct timer* tmr;
	tmr = kmem_alloc(sizeof(struct timer));
	if(tmr != NULL)
	{
		tmr->id = id;
		tmr->elapse = elapse;
		tmr->callback = callback;
		tmr->mutex = kmutex_create();
		tmr->thread = kthread_create(timer_thread_entry, tmr, timer_stk_size);
		kthread_setprio(tmr->thread, timer_thread_prio);
	}
	return tmr;
}

//���ٶ�ʱ��
void timer_destroy(timer_t timer)
{
	struct timer* tmr;
	tmr = (struct timer*)timer;
	kmutex_lock(tmr->mutex);
	kthread_destroy(tmr->thread);
	kmutex_unlock(tmr->mutex);
	kmutex_destroy(tmr->mutex);
	kmem_free(tmr);
}

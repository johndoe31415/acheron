#include <linux/module.h>
#include <linux/spinlock.h>

static int Statistics[10] = {0};
static spinlock_t acheron_StatisticLock = SPIN_LOCK_UNLOCKED;
module_param_array(Statistics, int, NULL, 0444);

void acheron_Statistic_Increment(int Index) {
	spin_lock(&acheron_StatisticLock);
	Statistics[Index]++;
	spin_unlock(&acheron_StatisticLock);
}

void acheron_Statistic_Set(int Index, int Value) {
	spin_lock(&acheron_StatisticLock);
	Statistics[Index] = Value;
	spin_unlock(&acheron_StatisticLock);
}


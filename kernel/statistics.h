#define STATISTIC_INIT							0
#define STATISTIC_CONNECT_CNT					1
#define STATISTIC_CONNECT_ACCEPT_CNT			2
#define STATISTIC_CONNECT_REJECT_CNT			3
#define STATISTIC_BIND_CNT						4
#define STATISTIC_BIND_ACCEPT_CNT				5
#define STATISTIC_BIND_REJECT_CNT				6
#define STATISTIC_REJECT_BUFFER_FULL_CNT		7
#define STATISTIC_ACCEPT_NO_IP_CNT				8
#define STATISTIC_ACCEPT_UNKNOWN_PROTO_CNT		9

void acheron_Statistic_Increment(int Index);
void acheron_Statistic_Set(int Index, int Value);

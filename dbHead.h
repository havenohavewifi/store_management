
#include"file.h"
#include"err.h"

#define DBMS_DAT "dbms.dat"

struct dbSysHead
{
	struct SysDesc desc;
	struct buffSpace buff;

	unsigned long *bitMap;	
	FILE *fpdesc;
};


//	bit.cpp
int setBit( unsigned long *num, int pos, int setValue );
int getBit(unsigned long num, int pos);

/*******************	buffManage.cpp   *******************/
// ��ѯҳquery��ȫ��ҳ�ţ��Ƿ��ڻ�������
int queryPage( struct dbSysHead *head, long query );
// ��pageNo��ȫ��ҳ�ţ�ҳ�滻���������±�ΪmapNo��ҳ
int replacePage( struct dbSysHead *head, int mapNo, long pageNo );
// ������ҳ������㷨����ѡ��LRU����FIFO
int scheBuff( struct dbSysHead *head );
// ������ҳ����ȣ�LRU
int LRU(struct dbSysHead *head);
// ������ҳ����ȣ�FIFO
int FIFO(struct dbSysHead *head);
// �򻺳���������queryҳ�棬�������ڻ������е��±�
int reqPage( struct dbSysHead *head, long query );


//	diskOpt.cpp
long allocatePage( struct dbSysHead *head,int reqPageNum );//����������ҳ
int initSegment(struct Segment *newSegment, long startPage, long pageNum, long fd, long preAddr, long nextAddr);//��ʼ����
long creatFileSpace(struct dbSysHead *head);//Ϊһ���ļ������ռ�
int recyOnePage(struct dbSysHead *head,long pageNo );//����ҳ�Ż���һ��ҳ
int recyPage(struct dbSysHead *head, long segmentAddr);//���մ�ĳһ����ͷ��ʼ�����е�ҳ
int recyFileSpace(struct dbSysHead *head, long fid);//�����ļ��ռ�
int queryFileID( struct dbSysHead *head, long fid );//��ѯһ���ļ���ʶ�Ƿ���ϵͳ��
int getSegmentValue(struct dbSysHead *head, long segmentAddr, struct Segment *newSegment);//���һ���ε�����
int writeSegmentValue(struct dbSysHead *head, long segmentAddr, struct Segment *newSegment);//������д������
long getLastSegmentAddr(struct dbSysHead *head, long fid);//�õ����һ���εĶ�ͷ
int extendFileSpace(struct dbSysHead *head, long fid, int extendPage);//��չ�οռ�

//	err.cpp
int isAvail(void *p,char *funcName,int type);

//	fileOpt.cpp
long mapPage( struct dbSysHead *head, long fid, long num );
int readInPage( struct dbSysHead *head, long pgID,long pos,long length, void *des );
int writeInPage( struct dbSysHead *head, long pgID, long pos, long length, void *des );
int rdFile( struct dbSysHead *head, long fid, long pos, long length, void *des);
int wtFile( struct dbSysHead *head, long fid, long pos, long length, void *des);

//	init.cpp
int initSys(struct dbSysHead *head);
int creaSysHead();

//	show.cpp
int showDesc(struct dbSysHead *head);
int showFileDesc( struct dbSysHead *head );
int showiNode( struct dbSysHead *head, long fid );
int showBitMap( struct dbSysHead *head, long start, long end );

//	update.cpp
int sysUpdate( struct dbSysHead *head );

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
// 查询页query（全局页号）是否在缓冲区中
int queryPage( struct dbSysHead *head, long query );
// 用pageNo（全局页号）页替换缓冲区中下标为mapNo的页
int replacePage( struct dbSysHead *head, int mapNo, long pageNo );
// 缓冲区页面调度算法，可选择LRU或者FIFO
int scheBuff( struct dbSysHead *head );
// 缓冲区页面调度：LRU
int LRU(struct dbSysHead *head);
// 缓冲区页面调度：FIFO
int FIFO(struct dbSysHead *head);
// 向缓冲区中请求query页面，返回其在缓冲区中的下标
int reqPage( struct dbSysHead *head, long query );


//	diskOpt.cpp
long allocatePage( struct dbSysHead *head,int reqPageNum );//分配连续的页
int initSegment(struct Segment *newSegment, long startPage, long pageNum, long fd, long preAddr, long nextAddr);//初始化段
long creatFileSpace(struct dbSysHead *head);//为一个文件创建空间
int recyOnePage(struct dbSysHead *head,long pageNo );//根据页号回收一个页
int recyPage(struct dbSysHead *head, long segmentAddr);//回收从某一个段头开始的所有的页
int recyFileSpace(struct dbSysHead *head, long fid);//回收文件空间
int queryFileID( struct dbSysHead *head, long fid );//查询一个文件标识是否在系统中
int getSegmentValue(struct dbSysHead *head, long segmentAddr, struct Segment *newSegment);//获得一个段的内容
int writeSegmentValue(struct dbSysHead *head, long segmentAddr, struct Segment *newSegment);//将内容写到段中
long getLastSegmentAddr(struct dbSysHead *head, long fid);//得到最后一个段的段头
int extendFileSpace(struct dbSysHead *head, long fid, int extendPage);//扩展段空间

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